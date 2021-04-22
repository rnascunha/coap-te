/**
 * This examples shows how to use reliable connection engines in a server mode.
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
 * port value, and PUT will set its value (must provide query of type ?value=<0|1>).
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
 *
 * ------
 *
 * To configure a reliable CoAP-te server engine you must define:
 * * Connection/Endpoint type: TCP or Websocket (not implemented), IPv4 or IPv6
 * * Transaction list type (if any)
 * * Connection list type: holds all connection and CSM signal configuration.
 */

#include "example_init.hpp"

#include "log.hpp"				//Log header
#include "coap-te.hpp"			//Convenient header
#include "coap-te-debug.hpp"	//Convenient debug header

/**
 * Logging namespace
 */
using namespace CoAP::Log;

/**
 * Log module
 */
static constexpr module example_mod = {
		.name = "EXAMPLE",
		.max_level = CoAP::Log::type::debug
};

/**
 * Defining the endpoint
 */
#if CONFIG_ENDPOINT_TYPE == 1
/**
 * IPv6 definitions
 */
using endpoint = CoAP::Port::POSIX::endpoint_ipv6;
#define BIND_ADDR			IN6ADDR_ANY_INIT
#else
/**
 * IPv4 definitions
 */
using endpoint = CoAP::Port::POSIX::endpoint_ipv4;
#define BIND_ADDR			INADDR_ANY
#endif /* USE_IPV6 */

/**
 *
 */
#define BUFFER_LEN			CONFIG_BUFFER_TRANSACTION
#define CONN_PORT			CONFIG_SERVER_PORT

/**
 * Connection type we are going to use
 */
using connection = CoAP::Port::POSIX::tcp_server<endpoint>;			///< TCP server socket definition

/**
 * Reliable connection MUST exchange CSM signaling message when connect.
 * This is our client configuration:
 * * Max message size = 1152 (default)
 * * Accept block wise transfer
 */
static constexpr const CoAP::Transmission::Reliable::csm_configure csm = {
		.max_message_size = BUFFER_LEN,
		.block_wise_transfer = true
};

/**
 * Choosing a connection list type
 *
 * As specified above, a connection holds the socket and its CSM
 * signaling information. If you don't need this information, i.e.,
 * just respond incoming request and ignore any CSM information you
 * can disable the use of connection list using CoAP::disable.
 */
/**
 * (Un)comment the define statements above to change the connection list
 * implementation.
 */
#define USE_CONNECTION_LIST_DEFAULT

#if defined(USE_CONNECTION_LIST_DEFAULT)
/**
 * This is the default implementation, and recommended to constrained devices.
 * It uses a simple array to hold the connections. The template parameters are:
 * (1) The connection type it will hold, defined above;
 * (2) The number of connections to hold simultaneously.
 *
 * If it receives a incoming connection and no free slots available, it will
 * send a abort signal message with a payload "max connection reached"
 */
using connection_list_t =
		CoAP::Transmission::Reliable::connection_list<
			CoAP::Transmission::Reliable::Connection<connection::handler>,	/* (1) transaction type */
			4>;				/* (2) number of transaction */
#else
/**
 * This will disable connection list
 */
using connection_list_t = CoAP::disable;
#endif /* USE_CONNECTION_LIST_DEFAULT */

/**
 * To a server, a transaction MAY not be need. As the retransmission is
 * handled by the underlying layer (TCP), and if you will only respond
 * incoming request, you can disable it (as we are going to do here).
 *
 * But if you expect to make request, is advisable to enable it. Check
 * 'engine_tcp_example' to examine transaction and transaction list options.
 */
using transaction_list_t = CoAP::disable;

/**
 * Any request received must be associated to a specific resource.
 *
 * If you are not intended to receive any request (like a client), you can
 * use CoAP::disable and any income request will generate a
 * response error. As we are implementing a server, we must define it.
 *----------
 * (1) the callback function type that it will be called to any successful request.
 * The function signature must be:
 *
 * void(*)(Message const&, Response& , void*) =
 * 		CoAP::Resource::callback_reliable<CoAP::Message::Reliable::message,
 * 										CoAP::Transmission::Reliable::Response>;
 * Where the parameters are: request, response and engine.
 *
 * You could also use:
 *
 * std::function<void(Message::Relaible::message const&, engine::response& , void*)>
 *
 * And bind values, uses lambdas... and so on.
 * --------
 * (2) enable/disable resource description, as defined at RFC6690 (you can save some
 * bytes (set to false) if this is not necessary).
 */
using resource = CoAP::Resource::resource<
		CoAP::Resource::callback_reliable<
			CoAP::Message::Reliable::message,
			CoAP::Transmission::Reliable::Response<connection::handler>
		>,									///< (1) resource callback
		true>;								///< (2) enable/disable description

/**
 * Engine: the pale blue dot
 *
 * Here we are going to show how to define a engine, and how each
 * parameters impacts:
 *-------
 * (1) Connection type: RFC 8323 defines CoAP to reliable connections. Here you can
 * use any type of connection that satisfy the CoAP-te Reliable connection
 * requirements.  More about CoAP-te reliable connection port at 'tcp_client/tcp_server'
 * example. At this example we are using TCP Server sockets.
 *--------
 * (2) CSM signal configuration. This configuration will be exchanged at connection
 *--------
 * (3) Connection List type: a connection list, as defined above.
 *--------
 * (4) Transaction List type: a transaction list, as defined above. (disabled)
 *--------
 * (5) Default callback type: the callback function type to call. When this is called?
 * * When receiving a signal code response;
 * * When receiving a response from a transaction that timeout or just wasn't expected;
 * * When you disable transactions. Any response will call this functions;
 *
 * The callback signature is:
 *
 * void(*)(socket, CoAP::Message::Reliable::message const&,void*) =
 * 												CoAP::Transmission::Reliable::default_cb;
 * Where the parameters are: socket, response, engine.
 *
 * You could use:
 *
 * std::function<void(socket, CoAP::Message::Reliable::message const&,void*)>;
 *
 * And bind values, uses lambdas... and so on.
 * ------
 * (6) resource type, as defined above;
 *------
 * So that it, that's us... CoAP-te...
 */
using engine = CoAP::Transmission::Reliable::engine_server<
		connection,			///< (1) TCP server socket definition
		csm,									///< (2) CSM paramenter configuration
		connection_list_t,						///< (3) Connection list defined above
		transaction_list_t,						///< (4) Trasaction list as defined above (disabled)
		CoAP::Transmission::Reliable::default_cb<connection::handler>,
												///< (4) Default callback signature function
		resource>;								///< (5) Resource definition

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
 * This default callback response to signal response
 */
void default_callback(engine::socket,
		engine::message const* response,
		void*) noexcept
{
	debug(example_mod, "default cb called");

	/**
	 * Checking response. If null means that the socket was closed
	 */
	if(!response)
	{
		error(example_mod, "Socket closed");
		return;
	}

	CoAP::Debug::print_message(*response);
}

/**
 * Auxiliary function
 */
static void exit_error(CoAP::Error& ec, const char* what = nullptr)
{
	error(example_mod, ec, what);
	while(true) vTaskDelay(portMAX_DELAY);
}


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
engine coap_engine;

extern "C" void app_main(void)
{
	/**
	 * This is a very naive implementation of the WIFI / TCP/IP stack initializaition.
	 * It will BLOCK until connect, or get stuck if fail... Should not be used in
	 * production code
	 */
	wifi_stack_init();

	CoAP::Error ec;

	/**
	 * Instantiating a endpoint that we are going to bind
	 */
	engine::endpoint ep{BIND_ADDR, CONN_PORT};

	/**
	 * Setting the default callback function the untracked
	 * responses or signal response.
	 */
	coap_engine.default_cb(default_callback);

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
	engine::resource_node	res_time{"time", get_time_handler},
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
	/**
	 * Using add_child
	 *
	 * Add all as children of the calling resource
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

	/**
	 * Open a socket and binding endpoint.
	 */
	coap_engine.open(ep, ec);
	if(ec) exit_error(ec, "open");

	/**
	 * "Printing" resource tree
	 */
	std::printf("l: path: [children]:\n");
	std::printf("--------------------\n");
	CoAP::Debug::print_resource_branch(coap_engine.root_node().node());

	debug(example_mod, "Initiating CoAP engine loop...");

	/**
	 * This code will run indefinitely
	 *
	 * The template parameter is the block time in milliseconds:
	 * 0 - No block (default);
	 * -1 - block indefinitely (this could be used here)
	 */
	while(coap_engine.run<1000>(ec))
	{
		vTaskDelay(pdMS_TO_TICKS(50));
	}

	if(ec) exit_error(ec, "run");

	while(true) vTaskDelay(portMAX_DELAY);
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
 * Simulate the GPIO ports of a uC
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
 * Function "calculating" the separated response
 *
 * This function will be called in a new thread. As separated response
 * should not block the response, a new thread (or task depending of your
 * environment) must be open (you must not block the response).
 *
 * Here we are going to wait for a random period of seconds and then send
 * the response.
 *
 * As the aync_response was created inside the response callback, we are
 * passing by value.
 */
static void separated_response(void* ptr)
{
	status(example_mod, "Separate response...");
	async_data* data = static_cast<async_data*>(ptr);

	status(example_mod, "token[%lu]=%.*s",
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
	char print_buf[20];
	std::snprintf(print_buf, 20, "%u", CoAP::random_generator() % 100);

	/**
	 * The response is going to be sent in a request (irony...). The request is constructed
	 * with all the correct values, passing the async_response data, just set
	 * the desirable response code.
	 */
	engine::request<> req{data->data, CoAP::Message::code::content};

	/**
	 * Constructing the request.
	 */
	req
		.add_option(content)
		.payload(print_buf);

	status(example_mod, "Sending payload=[%s]...", print_buf);
	CoAP::Error ec;

	/**
	 * Sending the message
	 *
	 * As this is a response from a previously request, we MUST NOT
	 * use a transaction! There will be no response from the client or
	 * ack (at the CoAP layer level), so a transaction would be stucked
	 * forever!
	 */
	data->engine_ptr->send(req,
			CoAP::Transmission::Reliable::no_transaction,
			ec);
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
	 * If we were using a unreliable connection (UDP), we should
	 * send a empty ack. As TCP takes care of any acknowledgment,
	 * this is not necessary (and... a invalid call).
	 */
//	response.serialize_empty_ack();		//Error INVALID call!

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
	std::size_t size = CoAP::Resource::discovery(eng->root_node().node(), buffer, 512, ec);

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
}
