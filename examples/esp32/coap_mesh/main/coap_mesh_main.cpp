/**
 * This examples show how to set up a ESP32 Mesh network using CoAP as
 * application layer.
 *
 * You must first intilialize the mesh network with the parameters of your envirioment,
 * at `idf.py menuconfig > Example Mesh/CoAP Configuration`. There you must set:
 *
 * * SSID/Password of your local router;
 * * Server IP/port that you receive data from network;
 * * Number of transaction/packet size to configure the CoAP engine;
 *
 * If everthing was set up correctly, all nodes will create a mesh network, with
 * CoAP engine on top processing requests. The root node will also open a UDP connection
 * and will forward any packet (as a forward proxy) from the mesh network to the IP
 * network, and vice-versa.
 *
 * To know to which node forward the packet when comming from the IP network, the
 * proxy will read the URI-Host option from the CoAP packet. If the option is not set,
 * or is a invalid value, the proxy will respond with "4.12 Precondition Failed" error
 * code response. The option value must be of format "xx:xx:xx:xx:xx:xx" (where 'x' is
 * a hexadecimal value), refering to the mac of the destination node.
 *
 * When forwarding a packet from the mesh network to the IP network, the proxy will
 * automatically add the host MAC to the CoAP packet as a URI-Host option.
 *
 * The proxying is made by the 'proxy_forward_udp_mesh' function.
 *
 * To test this network we suggest the use of the 'coap-client' tool that is build
 * with CoAP-te (when built with -DWITH_EXAMPLES=1 parameter). To use:
 *
 * ./coap_client [-N] -h=<xx:xx:xx:xx:xx:xx> -p=<local_bind> coap://<root_ip>:<root_port>/<resource_path>?<query>
 *
 * Where:
 * * -N: if present, sent request as non-confirmable, otherwise sent as confirmable;
 * * -h=<xx:xx:xx:xx:xx:xx>: adds the uri-host option to the packet (the argument must
 * be the MAC of the destination node);
 * * -p=<local_bind>: the local port that will receive the request (must be the same of
 * configured "Server Port" at `idf.py menuconfig`);
 * * <uri>: the full uri of your request, with the IP (received by DHCP) and port
 * (configured at `idf.py menuconfig`, Root Server Port), and resource path.
 *
 * A request example:
 *
 * $ ./coap_client -N -h=30:ae:a4:47:20 -p=5683 coap://192.168.0.11:5683/time
 *
 *--------------------------------------------------------------------
 * Resource Descrition
 *
 * We are going to create a resource tree as presented below.
 *
 * 	l0:			          	   root
 * 			____________________|_______________________________________________
 * 			|		 |			          |				|		   |			|
 * 	l1:	   time   sensors	 	      actuators	 	  dynamic  separate	   .well-known
 *				____|_______		______|_____								|
 *			   |     |      |       |     |     |							  core
 *	l2:		 temp  light humidity gpio0 gpio1 gpio2
 *
 * We are going to create appropriate response to each of the resources, using
 * different strategies. As the focus of the example is the use of the CoAP-te library,
 * we just simulating the read of sensors and enabling/disabling some external device
 * with GPIOs.
 *
 * A overview of the resources:
 * -------
 * *\/ (root): GET method. It will print the resource tree.
 *
 * * time: GET method only. Returns the current time.
 * -------
 * * sensors[temp/light/humidity]: GET method only. Returns random values simulating
 * a read sensor. We are going to use the same callback function to any of the sensors.
 * -------
 * * actuators[gpio[0/1/2]]: GET and PUT methods defined. GET will retrieve the current
 * port value, and PUT will set this value (must provide query of type ?value=<0|1>).
 * -------
 * * dynamic: GET and POST methods. GET method will retrieve than sons list of the resource.
 * POST method will create the resource sons as request (query ?dyn=<1|2|3>). The created
 * sons (dyn1 and/or dyn2 and/or dyn3) have GET and DELETE method. GET retrieves the sons name,
 * and DELETE deletes the son.
 * ------
 * * separate: GET method only. Sends a separate response within a random time.
 * ------
 * * .well-known/core: provide resource information as defined at RFC6690
 * ------
 * For brevity, all resources return data as strings (content_format::text_plain).
 */

#include <cstdio>

#include "log.hpp"				//Log header
#include "coap-te.hpp"			//Convenient header
#include "coap-te-debug.hpp"	//Convenient debug header

#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "esp_event.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_system.h"

#include "esp_mesh.h"
#include "esp_mesh_internal.h"

#define TAG				"MESH"
#define RX_SIZE          (CONFIG_BUFFER_TRANSACTION)

#define CONFIG_MESH_AP_PASSWD		"MAP_PASSWD"

static const uint8_t MESH_ID[6] = { 0x77, 0x77, 0x77, 0x77, 0x77, 0x77};

 static bool coap_engine_started = false;

/**
 * Receving buffer of proxy
 */
static uint8_t rx_buf[RX_SIZE];
static esp_netif_t *netif_sta = NULL;

#if CONFIG_ENDPOINT_TYPE == 0
using endpoint = CoAP::Port::POSIX::endpoint_ipv4;
#else
using endpoint = CoAP::Port::POSIX::endpoint_ipv6;
#endif

/**
 * UDP socket type that will forward packtes from networks
 */
using udp_conn = CoAP::Port::POSIX::udp<endpoint>;

CoAP::Error ecp;
/**
 * Server endpoint where all the internal packets will be forward
 */
udp_conn::endpoint ep_server{CONFIG_SERVER_ADDR, CONFIG_SERVER_PORT, ecp};

/**
 * The mesh socket, implemented to support CoAP-te library.
 *
 * The template parameter defines the type of service used
 * <https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/network/esp_mesh.html#_CPPv410mesh_tos_t>
 * By default it's MESH_TOS_P2P
 */
using mesh_socket = typename CoAP::Port::ESP_Mesh::socket<>;

/**
 * Engine definition. Check 'raw_engine' example for a full
 * description os the options.
 *
 * Here we are using server profile, and defining a resource callback
 * function. Server profile allow to add resources. We are also disabling
 * the default callback.
 */
using engine = CoAP::Transmission::engine<
		mesh_socket,
		CoAP::Message::message_id,
		CoAP::Transmission::transaction_list<
			CoAP::Transmission::transaction<
				CONFIG_BUFFER_TRANSACTION,
				CoAP::Transmission::transaction_cb,
				mesh_socket::endpoint>,
			CONFIG_TRANSACTION_NUM>,
		CoAP::disable,		//default callback disabled
		CoAP::Resource::resource<
			CoAP::Resource::callback<mesh_socket::endpoint>,
			true
		>
	>;

/**
 * CoAP-te engine instantiation
 *
 * We prefer to instantiate the engine globally to not be limited by the
 * app_main stack size.
 *
 * If you prefer to instantiate locally at the main task (or any other task), depending
 * on the packet size/transaction number you choose, it can overflow the task memory.
 * Keep a eye on it.
 *
 * The main task memory size can be changed at:
 * > 'idf.py menuconfig' > 'Component config' > 'Common ESP-related' > 'Main task size'
 */
engine coap_engine{mesh_socket{},
					CoAP::Message::message_id{(unsigned)CoAP::random_generator()}};

/**
 * CoAP-te log namespace
 */
using namespace CoAP::Log;

/**
 * Example log module
 */
static constexpr module example_mod = {
		.name = "EXAMPLE",
		.max_level = CoAP::Log::type::debug
};

/**
 * Response callback declaration
 *
 * Detailed explanation of the callbacks below 'main'
 */
static void get_root_handler(engine::message const& request,
								engine::response& response, void*) noexcept;
static void get_time_handler(engine::message const& request,
								engine::response& response, void*) noexcept;
static void get_sensor_handler(engine::message const& request,
								engine::response& response, void*) noexcept;
template<int GPIONum>
static void get_actuator_handler(engine::message const& request,
								engine::response& response, void*) noexcept;
template<int GPIONum>
static void put_actuator_handler(engine::message const& request,
								engine::response& response, void*) noexcept;
static void get_dynamic_list_handler(engine::message const& request,
								engine::response& response, void*) noexcept;
static void post_dynamic_handler(engine::message const& request,
								engine::response& response, void*) noexcept;
static void get_dynamic_handler(engine::message const& request,
								engine::response& response, void*) noexcept;
static void delete_dynamic_handler(engine::message const& request,
								engine::response& response, void*) noexcept;
static void get_separate_handler(engine::message const& request,
								engine::response& response, void*) noexcept;
static void get_discovery_handler(engine::message const& request,
								engine::response& response, void*) noexcept;
/**
 * This function will add all CoAP resource defined and run CoAP-te engine.
 *
 * It's initialized when the node join's the mesh network.
 */
void coap_te_engine(void*)
{
	/**
	 * Setting the root callback
	 *
	 * There is also post, put and delete function to register a callback
	 */
	coap_engine.root().get(get_root_handler);

	/**
	 * Resource instantiation
	 *
	 * Each resource must provide a path name, and a callback function to
	 * the method it support. (GET/POST/PUT/DELETE)
	 */
	engine::resource_node	res_time{"time", "title='time of device'", get_time_handler},
							res_sensors{"sensors"},
								res_sensor_temp{"temp", get_sensor_handler},
								res_sensor_light{"light", get_sensor_handler},
								res_sensor_humidity{"humidity", get_sensor_handler},
							res_actuators{"actuators"},
										/* path     get                     post     put */
								res_gpio0{"gpio0", get_actuator_handler<0>, nullptr, put_actuator_handler<0>},
								res_gpio1{"gpio1", get_actuator_handler<1>, nullptr, put_actuator_handler<1>},
								res_gpio2{"gpio2", get_actuator_handler<2>, nullptr, put_actuator_handler<2>},
										/* path			get							post			put */
							res_dynamic{"dynamic", get_dynamic_list_handler, post_dynamic_handler, nullptr},
							res_separate{"separate", get_separate_handler},
							res_well_known{".well-known"},
								res_core{"core", get_discovery_handler};

	debug(example_mod, "Adding resources...");
	/**
	 * Adding resource to the tree
	 *
	 * There is no checking if a resource is added cyclicly.
	 */
	res_sensors.add_child(res_sensor_temp,
							res_sensor_light,
							res_sensor_humidity);
	res_actuators.add_child(res_gpio0,
							res_gpio1,
							res_gpio2);
	res_well_known.add_child(res_core);
	coap_engine.root_node().add_child(
			res_time,
			res_sensors,
			res_actuators,
			res_dynamic,
			res_separate,
			res_well_known);
	coap_engine.root_node().add_child(res_time);
	coap_engine.root_node().add_child(res_time);

	/**
	 * Initializing
	 */
	status(example_mod, "CoAP engine initialize");
	CoAP::Error ec;
	while(coap_engine.run<50>(ec)){}

	coap_engine_started = false;
	vTaskDelete(NULL);
}


#ifdef JUST_EXAMPLE_NEVER_RUN
/**
 * Callback function of transaction
 */
void request_cb(void const* trans, CoAP::Message::message const* response, void*) noexcept
{
	CoAP::Log::debug("Request callback called...");

	auto const* t = static_cast<engine::transaction_t const*>(trans);
	CoAP::Log::status("Status: %s", CoAP::Debug::transaction_status_string(t->status()));
	if(response)
	{
		char ep_addr[20];
		std::printf("Response received! %s\n", t->endpoint().address(ep_addr, 20));
		CoAP::Debug::print_message_string(*response);

		/**
		 * Checking if we received a empty acknowledgment. This means that we
		 * are going to receive our response in a separated message, so we can
		 * not go out of the check loop at main.
		 */
		if(response->mtype == CoAP::Message::type::acknowledgment &&
			response->mcode == CoAP::Message::code::empty)
		{
			return;
		}
	}
	else
	{
		/**
		 * Function timeout or transaction was cancelled
		 */
		CoAP::Log::status("Response NOT received");
	}
}

/**
 * Example on how to initiate a request to a external network
 */
void coap_send_main(void*)
{
	/**
	 * Initialing a dummy endpoint
	 *
	 * Must be any value different from all 0's (the address doesn't matter as
	 * it always forward to the same address);
	 */
	mesh_addr_t addr{};
	addr.mip.port = htons(5683);
	engine::endpoint ep{addr, CoAP::Port::ESP_Mesh::endpoint_type::to_external};

	while(coap_engine_started)
	{
		CoAP::Error ec;
		CoAP::Message::Option::node path_time{CoAP::Message::Option::code::uri_path, "time"};

		engine::request req{ep};
		req.header(CoAP::Message::type::nonconfirmable, CoAP::Message::code::get)
					.token("token")
					.add_option(path_time)
					.callback(request_cb/*, data */);

		coap_engine.send(req, ec);
		if(ec)
		{
			ESP_LOGE(TAG, "ERROR sending time request [%d/%s]...", ec.value(), ec.message());
		}
		else
			ESP_LOGI(TAG, "Sending time request...");
		vTaskDelay(120 * 1000 / portTICK_RATE_MS);
	}
	vTaskDelete(NULL);
}
#endif /* JUST_EXAMPLE_NEVER_RUN */

/**
 * This function will route the packets from the IP network to the
 * mesh network.
 *
 * It's only initialized if the node is the root of the mesh tree network.
 *
 * It will open a UDP connection and bind to the port configured.
 */
void coap_forward_proxy(void*)
{
	debug(example_mod, "Proxy forwaring task init");

	/**
	 * Opening UDP socket
	 */
	udp_conn conn{};
	conn.open(ecp);
	if(ecp)
	{
		ESP_LOGE(TAG, "Error opening socket [%d/%s]", ecp.value(), ecp.message());
		/**
		 * As we could not open a socket to forward the packets, we are giving up
		 * from being root!
		 */
		esp_mesh_waive_root(nullptr, MESH_VOTE_REASON_ROOT_INITIATED);
		vTaskDelete(NULL);
		return;
	}

	/**
	 * Bind socket to the configured port
	 */
	udp_conn::endpoint bind_ep{CONFIG_ROOT_SERVER_PORT};
	conn.bind(bind_ep, ecp);
	if(ecp)
	{
		ESP_LOGE(TAG, "Error binding [%d/%s]", ecp.value(), ecp.message());
	}

	/**
	 * We are going to work as forward proxy as long as we are root
	 */
	while(esp_mesh_is_root())
	{
		/**
		 * `proxy_forward_udp_mesh` will deal with all complexity to forward
		 * packets between networks.
		 *
		 * The template parameter is the block reading time to each network, in milliseconds.
		 */
		CoAP::Error ec;
		CoAP::Port::ESP_Mesh::proxy_forward_udp_mesh<50>(
				conn,				///< UDP connection
				ep_server,			///< Endpoint of IP networ to forward
				coap_engine,		///< CoAP engine
				rx_buf,				///< Buffer to be used to receive packtes
				RX_SIZE,			///< Buffer size
				ec);				///< Error report
		if(ec)
		{
			ESP_LOGE(TAG, "Error proxy [%d/%s]", ec.value(), ec.message());
		}
	}
	debug(example_mod, "Proxy forwaring task end");

	esp_mesh_post_toDS_state(false);
	conn.close();
	vTaskDelete(NULL);
}

esp_err_t coap_mesh_start(void)
{
    if (!coap_engine_started)
    {
    	coap_engine_started = true;

        xTaskCreate(coap_te_engine, "MPRX", 3072, NULL, 5, NULL);
#ifdef JUST_EXAMPLE_NEVER_RUN
        xTaskCreate(coap_send_main, "MPTX", 3072, NULL, 5, NULL);
#endif /* JUST_EXAMPLE_NEVER_RUN */
    }
    return ESP_OK;
}

/**
 * IP event handler.
 *
 * When receive a IP (i.e, is root), initalize proxy forward task
 */
void ip_event_handler(void *arg, esp_event_base_t event_base,
                      int32_t event_id, void *event_data)
{
    ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;
    ESP_LOGI(TAG, "<IP_EVENT_STA_GOT_IP>IP:" IPSTR, IP2STR(&event->ip_info.ip));
	esp_mesh_post_toDS_state(true);
	xTaskCreate(coap_forward_proxy, "root", 3072, NULL, 5, NULL);
}

/**
 * Mesh event handler.
 */
void mesh_event_handler(void *arg, esp_event_base_t event_base,
                        int32_t event_id, void *event_data)
{
    switch (event_id) {
		break;
		case MESH_EVENT_PARENT_CONNECTED: {
			/**
			 * Initialize tasks and DHCP if root
			 */
			if (esp_mesh_is_root()) {
				esp_netif_dhcpc_start(netif_sta);
			}
			coap_mesh_start();
		}
		break;
		case MESH_EVENT_TODS_STATE: {
			mesh_event_toDS_state_t *toDs_state = (mesh_event_toDS_state_t *)event_data;
			ESP_LOGI(TAG, "<MESH_EVENT_TODS_REACHABLE>state:%d", *toDs_state);
		}
		break;
		default:
			break;
    }
}

extern "C" void app_main(void)
{
	nvs_flash_init();
	/**
	 * ESP32 mesh initialization
	 */
	/*  tcpip initialization */
	esp_netif_init();
	/*  event initialization */
	esp_event_loop_create_default();
	/*  create network interfaces for mesh (only station instance saved for further manipulation, soft AP instance ignored */
	esp_netif_create_default_wifi_mesh_netifs(&netif_sta, NULL);
	/*  wifi initialization */
	wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();
	esp_wifi_init(&config);
	esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &ip_event_handler, NULL);
	esp_wifi_set_storage(WIFI_STORAGE_FLASH);
	esp_wifi_start();
	/*  mesh initialization */
	esp_mesh_init();
	esp_event_handler_register(MESH_EVENT, ESP_EVENT_ANY_ID, &mesh_event_handler, NULL);
	/*  set mesh topology */
	esp_mesh_set_topology(MESH_TOPO_TREE);
	/*  set mesh max layer according to the topology */
	esp_mesh_set_max_layer(6);
	esp_mesh_set_vote_percentage(1);
	esp_mesh_set_xon_qsize(128);
	/* Disable mesh PS function */
	esp_mesh_disable_ps();
	esp_mesh_set_ap_assoc_expire(10);

	mesh_cfg_t cfg = {};
	cfg.crypto_funcs = &g_wifi_default_mesh_crypto_funcs;
	esp_mesh_allow_root_conflicts(false);
	/* mesh ID */
	memcpy((uint8_t *) &cfg.mesh_id, MESH_ID, 6);
	/* router */
	cfg.channel = 0;
	cfg.router.ssid_len = strlen(CONFIG_MESH_ROUTER_SSID);
	memcpy((uint8_t *) &cfg.router.ssid, CONFIG_MESH_ROUTER_SSID, cfg.router.ssid_len);
	memcpy((uint8_t *) &cfg.router.password, CONFIG_MESH_ROUTER_PASSWD,
		   strlen(CONFIG_MESH_ROUTER_PASSWD));
	/* mesh softAP */
	esp_mesh_set_ap_authmode(WIFI_AUTH_WPA2_PSK);
	cfg.mesh_ap.max_connection = 6;
	memcpy((uint8_t *) &cfg.mesh_ap.password, CONFIG_MESH_AP_PASSWD,
		   strlen(CONFIG_MESH_AP_PASSWD));
	esp_mesh_set_config(&cfg);
	/* mesh start */
	esp_mesh_start();

	ESP_LOGI(TAG, "mesh starts successfully, heap:%d, %s<%d>%s, ps:%d\n",  esp_get_minimum_free_heap_size(),
	esp_mesh_is_root_fixed() ? "root fixed" : "root not fixed",
	esp_mesh_get_topology(), esp_mesh_get_topology() ? "(chain)":"(tree)", esp_mesh_is_ps_enabled());
}

/**
 * Resource callback definition
 *
 * The resource callback signature is:
 * void(*)(engine::message const&, engine::response&, void*) noexcept;
 *
 * You could use
 * std::function<void(engine::message const&, engine::response&, void*) noexcept>
 *
 * as a more flexible type.
 */

/**
 * \/ (root)	[GET]
 *
 * Just a warming up example. It prints the resource tree and respond
 * a OK.
 */
static void get_root_handler(engine::message const&,
								engine::response& response, void* engine_void) noexcept
{
	debug(example_mod, "Called get root handler");

	/**
	 * The third parameter is the engine...
	 */
	engine* eng = static_cast<engine*>(engine_void);

	CoAP::Debug::print_resource_branch(eng->root_node().node());

	response
		.code(CoAP::Message::code::content)
		.payload("OK")
		.serialize();
}

/**
 * \/time [GET]
 *
 * Retrieve local time in epoch format.
 */
static void get_time_handler(engine::message const&,
								engine::response& response, void*) noexcept
{
	debug(example_mod, "Called get time handler");

	/**
	 * creating option content_format
	 */
	CoAP::Message::content_format format = CoAP::Message::content_format::text_plain;
	CoAP::Message::Option::node content{format};

	char time[15];
	std::snprintf(time, 15, "%llu", (long long unsigned)CoAP::time());

	/**
	 * Making response (always call serialize)
	 */
	response
			.code(CoAP::Message::code::content)
			.add_option(content)
			.payload(time)
			.serialize();
}

/**
 * \/sensors/[temp/light/humidity] [GET]
 *
 * Retrieve sensor value read (a random number here).
 *
 * Same callback to all the sensors
 */
static void get_sensor_handler(engine::message const& request,
								engine::response& response, void*) noexcept
{
	debug(example_mod, "Called get sensor handler");

	/**
	 * As we are using the same callback to all sensors, get what sensor by
	 * inspecting the last path
	 * (we could archive this searching the path at the resource list)
	 */
	CoAP::Message::Option::Parser parser(request);
	CoAP::Message::Option::option const *opt, *path_opt = nullptr;
	while((opt = parser.next()))
	{
		if(opt->ocode == CoAP::Message::Option::code::uri_path)
			path_opt = opt;
	}

	/**
	 * Checking if path was found (need to check? Let's keep it safe)
	 */
	if(!path_opt)
	{
		/**
		 * Path not found (some error for sure. Always call serialize)
		 */
		response.
				code(CoAP::Message::code::bad_request)
				.serialize();
		return;
	}

	/**
	 * Making the payload
	 */
	char buffer[20];
	snprintf(buffer, 20, "%.*s: %u", path_opt->length, static_cast<char const*>(path_opt->value),
										CoAP::random_generator() % 100);
	CoAP::Message::content_format format = CoAP::Message::content_format::text_plain;
	CoAP::Message::Option::node content{format};

	/**
	 * Sending response (always call serialize)
	 */
	response
			.code(CoAP::Message::code::content)
			.add_option(content)
			.payload(buffer)
			.serialize();
}

/**
 * Simulate the gpio ports of a uC
 */
static bool gpios[3] = {false, false, false};

/**
 * \/actuators/[gpio0/gpio1/gpio2] GET
 *
 * Retrieve the port status.
 *
 * As we are creating this function as template, we are going to instantiate
 * one function to each GPIO port. We don't need to found out to which port
 * as the sensor callback.
 */
template<int GPIONum>
static void get_actuator_handler(engine::message const&,
								engine::response& response, void*) noexcept
{
	debug(example_mod, "Called get actuator handler");

	/**
	 * Option
	 */
	CoAP::Message::content_format format = CoAP::Message::content_format::text_plain;
	CoAP::Message::Option::node content{format};

	/**
	 * Payload
	 */
	char buffer[2];
	snprintf(buffer, 2, "%u", gpios[GPIONum]);
	response
		.code(CoAP::Message::code::content)
		.add_option(content)
		.payload(buffer)
		.serialize();
}

/**
 * \/actuators/[gpio0/gpio1/gpio2] PUT
 *
 * Set the GPIO port value.
 *
 * The port level we are going to set need to be sent as a query value (?value=<0|1>).
 * 0 means low level, not 0 (=1) high level (:-p). If the user doesn't send a query with
 * key 'value', we are going to report a error.
 */
template<int GPIONum>
static void put_actuator_handler(engine::message const& request,
								engine::response& response, void*) noexcept
{
	debug(example_mod, "Called put actuator handler");

	/**
	 * Querying value (?value=<0|1>)
	 */
	const void* value = nullptr;
	unsigned length;
	if(!CoAP::Message::query_by_key(request, "value", &value, length) || !length)
	{
		/**
		 * query key 'value' not found.
		 */
		response
			.code(CoAP::Message::code::bad_request)
			.payload("value not set")
			.serialize();
		return;
	}

	/**
	 * Setting value
	 */
	char v = *static_cast<char const*>(value);
	gpios[GPIONum] = v != '0' ? true : false;

	/**
	 * Payload
	 */
	char buf[2];
	snprintf(buf, 2, "%c", gpios[GPIONum] + '0');

	response
		.code(CoAP::Message::code::changed)
		.payload(buf)
		.serialize();
}

/**
 * \/dynamic	[GET]
 *
 * The dynamic resources lets us create sons to it (dyn1/dyn2/dyn3). The GET
 * methods retrieves the list of resources created.
 */
static void get_dynamic_list_handler(engine::message const& request,
								engine::response& response, void* engine_void) noexcept
{
	debug(example_mod, "Called get dynamic list handler");

	/**
	 * The third parameter is the engine...
	 */
	engine* eng = static_cast<engine*>(engine_void);

	/**
	 * Searching our node
	 */
	engine::resource_node* node = eng->root_node().search_node(request);
	if(!node)
	{
		/* Couldn't find */
		response
			.code(CoAP::Message::code::internal_server_error)
			.payload("didn't find node")
			.serialize();
	}

	/**
	 * packet size count with header, token and any other
	 * options, this should be considered... we are not...
	 */
	char buffer[engine::packet_size] = "";
	int len = 0;
	unsigned i = 0;
	engine::resource_node* child;

	/**
	 * Payload. Creating the sons resource list
	 */
	while((child = (*node)[i++]))
	{
		len += snprintf(buffer + len, engine::packet_size - len, "%s ", child->value().path());
	}
	CoAP::Message::content_format format = CoAP::Message::content_format::text_plain;
	CoAP::Message::Option::node content{format};

	/**
	 * Sending response (always call serialize)
	 */
	response
			.code(CoAP::Message::code::content)
			.add_option(content)
			.payload(buffer)
			.serialize();
}

/**
 * The dynamic sons.
 *
 * This could be (really) create dynamically (malloc/new) but as we are avoiding
 * any dynamic memory allocation, we are just going to add/remove from the tree.
 *
 * All sons define a GET and a DELETE method.
 */
static engine::resource_node dynamics[] = {
		/* path		get					post	put			delete	*/
		{"dyn1", get_dynamic_handler, nullptr, nullptr, delete_dynamic_handler},
		{"dyn2", get_dynamic_handler, nullptr, nullptr, delete_dynamic_handler},
		{"dyn3", get_dynamic_handler, nullptr, nullptr, delete_dynamic_handler}
};

/**
 * \/dynamic	[POST]
 *
 * Creates the dynamic son. Which dynamic son to create must be passed by a query
 * (?dyn=<1|2|3>). Not setting the query or a query invalid option will report a error.
 */
static void post_dynamic_handler(engine::message const& request,
								engine::response& response, void* engine_void) noexcept
{
	debug(example_mod, "Called post dynamic handler");

	engine* eng = static_cast<engine*>(engine_void);

	/**
	 * Searching this node
	 *
	 * Well, we are not checking if the node was found. Should we?
	 */
	engine::resource_node* node = eng->root_node().search_node(request);

	/**
	 * Querying which node to create (?dyn=<1|2|3>)
	 */
	const void* value = nullptr;
	unsigned length;
	if(!CoAP::Message::query_by_key(request, "dyn", &value, length) || !length)
	{
		/**
		 * Query 'dyn' not set
		 */
		response
			.code(CoAP::Message::code::bad_request)
			.payload("dyn not set")
			.serialize();
		return;
	}

	/**
	 * Checking if value is valid
	 */
	char v = *static_cast<char const*>(value);
	if(v < '1' || v > '3')
	{
		/* not valid :-( */
		response
			.code(CoAP::Message::code::bad_option)
			.payload("dyn=<1|2|3>")
			.serialize();
		return;
	}

	/**
	 * Adding new resource
	 *
	 * If it fails when trying to add, means that the child
	 * already existed
	 */
	int index = v - '0' - 1;
	if(!node->add_child(dynamics[index]))
	{
		/* child already added */
		response
			.code(CoAP::Message::code::precondition_failed)
			.payload("already create")
			.serialize();
		return;
	}

	/**
	 * Sending added information back (yes, the option and payload are
	 * redundant... just showing off)
	 */
	CoAP::Message::Option::node location{CoAP::Message::Option::code::location_path, dynamics[index].value().path()};
	response
		.code(CoAP::Message::code::created)
		.add_option(location)
		.payload(dynamics[index].value().path())
		.serialize();

	/**
	 * Printing the brach tree
	 */
	CoAP::Debug::print_resource_branch(*node);
}

/**
 * \/dynamic/dyn[1|2|3]	[GET]
 *
 * This call is made from one of the created sons. Just retrieve the
 * its own path.
 */
static void get_dynamic_handler(engine::message const& request,
		engine::response& response, void* engine_void) noexcept
{
	debug(example_mod, "Called get dynamic handler");

	engine* eng = static_cast<engine*>(engine_void);
	/**
	 * Searching the node
	 */
	engine::resource_node* node = eng->root_node().search_node(request);

	/**
	 * Sending response
	 */
	response
		.code(CoAP::Message::code::content)
		.payload(node->value().path())
		.serialize();
}

/**
 * \/dynamic/dyn[1|2|3]	[DELETE]
 *
 * Delete the node (remove from the resource tree).
 */
static void delete_dynamic_handler(engine::message const& request,
		engine::response& response, void* engine_void) noexcept
{
	debug(example_mod, "Called delete dynamic handler");

	engine* eng = static_cast<engine*>(engine_void);

	/**
	 * Removing. It returns it own node
	 */
	engine::resource_node* node = eng->root_node().remove_node(request);

	/**
	 * Checking if found the node and remove correctly
	 */
	if(!node)
	{
		/**
		 * It didn't...
		 */
		response
			.code(CoAP::Message::code::internal_server_error)
			.payload("node search error")
			.serialize();
		return;
	}

	/**
	 * Sending successful response
	 */
	response
		.code(CoAP::Message::code::deleted)
		.payload(node->value().path())
		.serialize();
}

/**
 * Callback function of the separeted resquest
 *
 * This function just report if the request was arrived to the client.
 */
void cb(void const* trans, engine::message const* r, void*) noexcept
{
	auto const* t = static_cast<engine::transaction_t const*>(trans);
	status(example_mod, "Status: %s", CoAP::Debug::transaction_status_string(t->status()));
	if(r)
	{
		status(example_mod, "Response received!");
		CoAP::Debug::print_message(*r);
	}
	else
	{
		status(example_mod, "Response NOT received");
	}
}

/**
 * Type that will hold the values necessary to send the async
 * response
 */
struct async_data{
	engine* engine_ptr;
	engine::async_response data;

	void set(engine* eng,
			engine::message const& request,
		engine::response& response) noexcept
	{
		engine_ptr = eng;
		data.set(request, response);
	}
};

/**
 * Instantiating the async_data that will hold the
 * separated values.
 *
 * We are making a very naive implementation, as in real environment with
 * concurrent requests, this global instance would be corrupted
 */
static async_data a_data;

/**
 * Function "calculating" the separeted response
 *
 * This function will be called in a new task. As separated response
 * should not block the response, a new task must be started (you must
 * not block the response).
 *
 * Here we are going to wait for a random period of seconds and then send
 * the response.
 */
static void separated_response(void* ptr)
{
	debug(example_mod, "Async response");
	async_data* data = static_cast<async_data*>(ptr);

	char print_buf[46];
	status(example_mod, "ep=%s:%u, type=%s, token[%lu]=%.*s",
						data->data.ep.address(print_buf),
						data->data.ep.port(),
						CoAP::Debug::type_string(data->data.type),
						data->data.token_len, data->data.token_len, data->data.token);

	/**
	 * Waiting time simulating sleep
	 */
	CoAP::time_t wait = 5 + (CoAP::random_generator() % 6);
	debug(example_mod, "Waiting for: %lu", wait);
	/**
	 * Sleeping beauty
	 */
	vTaskDelay(pdMS_TO_TICKS(wait * 1000));

	/**
	 * Option
	 */
	CoAP::Message::content_format format = CoAP::Message::content_format::text_plain;
	CoAP::Message::Option::node content{format};

	/**
	 * Payload
	 */
	std::snprintf(print_buf, 20, "%u", CoAP::random_generator() % 100);

	/**
	 * The response is going to be sent in a request (irony...). The request is constructed
	 * with all the correct values, passing the async_response data, just set
	 * the desirable response code.
	 */
	engine::request req{data->data, CoAP::Message::code::content};

	/**
	 * Constructing the request.
	 */
	req
		.add_option(content)
		.callback(cb)
		.payload(print_buf);

	status(example_mod, "Sending payload=[%s]...", print_buf);
	CoAP::Error ec;

	/**
	 * Sending the message
	 */
	data->engine_ptr->send(req, ec);
	if(ec)
	{
		error(example_mod, ec, "send");
	}

	vTaskDelete(NULL);
}

/**
 * \/separate [GET]
 *
 * Making a separated response
 */
static void get_separate_handler(engine::message const& request,
		engine::response& response, void* engine_void) noexcept
{
	debug(example_mod, "Called get separate handler");

	/**
	 * To make a separated response we must first respond with a
	 * empty ack.
	 */
	response.serialize_empty_ack();

	/**
	 * This call will save all the need values to make
	 * send the separated response
	 */
	a_data.set(static_cast<engine*>(engine_void), request, response);

	/**
	 * Open a new thread to simulate a long read. Never block this function
	 * because is called synchronously with engine processing
	 */
	xTaskCreate(separated_response, "separated_response", 2048, &a_data, 5, NULL);
}

/**
 * \/well-known/core [GET]
 *
 * Respond to request with resource information as defined at RFC6690
 */
static void get_discovery_handler(engine::message const&,
								engine::response& response, void* eng_ptr) noexcept
{
	char buffer[512];
	CoAP::Error ec;
	engine* eng = static_cast<engine*>(eng_ptr);

	/**
	 * Constructing link format resource information
	 */
	std::size_t size = CoAP::Resource::discovery(eng->root_node().node(),
			buffer, 512, ec);

	/**
	 * Checking error
	 */
	if(ec)
	{
		response
			.code(CoAP::Message::code::internal_server_error)
			.payload(ec.message())
			.serialize();
		return;
	}

	/**
	 * Setting content type as link format
	 */
	CoAP::Message::content_format format = CoAP::Message::content_format::application_link_format;
	CoAP::Message::Option::node content{format};

	/**
	 * Sending response
	 */
	response
		.code(CoAP::Message::code::content)
		.add_option(content)
		.payload(buffer, size)
		.serialize();

	CoAP::Error ecs = response.error();
	if(ecs)
	{
		error(example_mod, ecs, "core serialize");
		response
			.code(CoAP::Message::code::internal_server_error)
			.payload(ecs.message())
			.serialize();
	}
}
