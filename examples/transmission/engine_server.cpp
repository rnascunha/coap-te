/**
 * This examples shows how to use engines in a server mode.
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
 * different strategies. We are simulating a embedded device connected to
 * some sensors and enabling/disabling some external device with GPIOs.
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
#include <chrono>
#include <thread>		//thread

#include "log.hpp"				//Log header
#include "coap-te.hpp"			//Convenient header
#include "coap-te-debug.hpp"	//Convenient debug header

/**
 * Uncomment to use transaction_list_vector (see raw_engine example)
 */
//#define USE_TRANSACTION_LIST_VECTOR

/**
 * There is two methods to add resources: add_child and add_branch. Both
 * can be used interchangeable, but here a presented separated just to
 * show each concept.
 *
 * Comment the line to use add_branch
 */
#define USE_RESOURCE_ADD_CHILD

using namespace CoAP::Log;

#define COAP_PORT		CoAP::default_port		//5683
#define BUFFER_LEN		512						//Buffer size
#define HOST_ADDR		"127.0.0.1"				//Address

#ifdef USE_TRANSACTION_LIST_VECTOR
/**
 * Must include header to use transaction list
 */
#include "transmission/transaction_list_vector.hpp"
#else
/**
 * Default transaction list have limited transaction capacity
 */
#define TRANSACT_NUM	4
#endif /* USE_TRANSACTION_LIST_VECTOR */

/**
 * Example log module
 */
static constexpr module example_mod = {
		.name = "EXAMPLE",
		.max_level = CoAP::Log::type::debug
};

/**
 * Engine definition. Check 'raw_engine' example for a full
 * description os the options.
 *
 * Here we are using server profile, and defining a resource callback
 * function. Server profile allow to add resources. We are also disabling
 * the default callback.
 */
using engine = CoAP::Transmission::engine<
		CoAP::Port::POSIX::udp<CoAP::Port::POSIX::endpoint_ipv4>,
		CoAP::Message::message_id,
#ifdef USE_TRANSACTION_LIST_VECTOR
		CoAP::Transmission::transaction_list_vector<
			CoAP::Transmission::transaction<
				BUFFER_LEN,
				CoAP::Transmission::transaction_cb,
				CoAP::socket::endpoint>
		>,
#else /* USE_TRANSACTION_LIST_VECTOR */
		CoAP::Transmission::transaction_list<
			CoAP::Transmission::transaction<
				BUFFER_LEN,
				CoAP::Transmission::transaction_cb,
				CoAP::Port::POSIX::endpoint_ipv4>,
			TRANSACT_NUM>,
#endif /* USE_TRANSACTION_LIST_VECTOR */
		CoAP::disable,		//default callback disabled
		CoAP::Resource::resource<
			CoAP::Resource::callback<CoAP::Port::POSIX::endpoint_ipv4>,
			true
		>
	>;

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
 * Auxiliary function
 */
static void exit_error(CoAP::Error& ec, const char* what = nullptr)
{
	error(example_mod, ec, what);
	exit(EXIT_FAILURE);
}

int main()
{
	/**
	 * At Linux, does nothing. At Windows initiate winsock
	 */
	CoAP::Port::POSIX::init();
	
	CoAP::Error ec;
	/**
	 * Socket
	 */
	//Initiating the endpoint to bind
	engine::endpoint ep{HOST_ADDR, COAP_PORT, ec};
	if(ec) exit_error(ec, "endpoint");

	engine::connection socket;

	socket.open(ec);
	if(ec) exit_error(ec, "Error trying to open socket...");
	socket.bind(ep, ec);
	if(ec) exit_error(ec, "Error trying to bind socket...");

	debug(example_mod, "Socket opened and binded...");

	debug(example_mod, "Initiating server engine...");
	/**
	 * Initiating CoAP engine
	 */
	engine coap_engine(std::move(socket),
			CoAP::Message::message_id(CoAP::time()));

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
#ifdef USE_RESOURCE_ADD_CHILD
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
#else
	/*
	 * Using add_branch
	 *
	 * Make a path from the calling resource to the last resource, adding
	 * every resource as children of the precedent
	 */
	coap_engine.root_node().add_branch(res_time);

	coap_engine.root_node().add_branch(res_sensors, res_sensor_temp);
	coap_engine.root_node().add_branch(res_sensors, res_sensor_light);
	coap_engine.root_node().add_branch(res_sensors, res_sensor_humidity);

	coap_engine.root_node().add_branch(res_actuators, res_gpio0);
	coap_engine.root_node().add_branch(res_actuators, res_gpio1);
	coap_engine.root_node().add_branch(res_actuators, res_gpio2);
	coap_engine.root_node().add_branch(res_dynamic);
	coap_engine.root_node().add_branch(res_separate);
	coap_engine.root_node().add_branch(res_well_known, res_core);
#endif

	/**
	 * "Printing" resource tree
	 */
	std::printf("l: path: [children]:\n");
	std::printf("--------------------\n");
	CoAP::Debug::print_resource_branch(coap_engine.root_node().node());

	debug(example_mod, "Initiating CoAP engine loop...");
	//CoAP engine loop.
	while(coap_engine(ec));

	return EXIT_SUCCESS;
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
static void get_time_handler(engine::message const& request,
								engine::response& response, void*) noexcept
{
	debug(example_mod, "Called get time handler");

	/**
	 * creating option content_format
	 */
	CoAP::Message::content_format format = CoAP::Message::content_format::text_plain;
	CoAP::Message::Option::node content{format};

	char time[15];
	std::snprintf(time, 15, "%llu", CoAP::time());

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
	CoAP::Message::Option::Parser<CoAP::Message::Option::code> parser(request);
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
static void get_actuator_handler(engine::message const& request,
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
 * Function "calculating" the separeted response
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
static void separated_response(engine* eng, engine::async_response data)
{
	status(example_mod, "Separate response...");
	char print_buf[20];
	status(example_mod, "ep=%s:%u, type=%s, token[%lu]=%.*s",
						data.ep.address(print_buf),
						data.ep.port(),
						CoAP::Debug::type_string(data.type),
						data.token_len, data.token_len, data.token);

	/**
	 * Wainting time simulating sleep
	 */
	CoAP::time_t wait = 5 + (CoAP::random_generator() % 6);
	debug(example_mod, "Wainting for: %lu", wait);
	/**
	 * Sleeping beauty
	 */
	std::this_thread::sleep_for(std::chrono::seconds(wait));

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
	engine::request req{data, CoAP::Message::code::content};

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
	eng->send(req, ec);
	if(ec)
	{
		error(example_mod, ec, "send");
	}
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
	engine::async_response adata{request, response};

	/**
	 * Open a new thread to simulate a long read. Never block this function
	 * because is called synchronously with engine processing
	 */
	std::thread{separated_response, static_cast<engine*>(engine_void), adata}.detach();
}

/**
 * \/well-known/core [GET]
 *
 * Respond to request with resource information as defined at RFC6690
 */
static void get_discovery_handler(engine::message const& request,
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
}
