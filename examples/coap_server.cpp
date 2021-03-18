#include <ctime>
#include <cstdio>
#include <thread>

#include "coap-te.hpp"
#include "coap-te-debug.hpp"

#define USE_RESOURCE_ADD_CHILD

/**
 * 	l0:			          	   root
 * 			____________________|__________________________________
 * 			|		 |			          |				|		   |
 * 	l1:	   time   sensors	 	      actuators	 	  dynamic  separate
 *				____|_______		______|_____
 *			   |     |      |       |     |     |
 *	l2:		 temp  light humidity gpio0 gpio1 gpio2
 */

using namespace CoAP;
using namespace CoAP::Log;

#define COAP_PORT		CoAP::default_port		//5683
#define TRANSACT_NUM	4
#define BUFFER_LEN		512
#define HOST_ADDR		"127.0.0.1"

static constexpr module main_mod = {
		.name = "MAIN",
		.max_level = CoAP::Log::type::debug
};

using engine = CoAP::Transmission::engine<
		CoAP::Transmission::profile::server,
		CoAP::socket,
		CoAP::Message::message_id,
		CoAP::Transmission::transaction_list<
			CoAP::Transmission::transaction<
				512,
				CoAP::Transmission::transaction_cb,
				CoAP::socket::endpoint>,
			TRANSACT_NUM>,
		void*,		//default callback disabled
		CoAP::Resource::callback<CoAP::socket::endpoint>
	>;

/**
 * Response callback declaration
 */
static void get_time_handler(Message::message const& request,
								engine::response& response, void*) noexcept;
static void get_sensor_handler(CoAP::Message::message const& request,
								engine::response& response, void*) noexcept;
template<int GPIONum>
static void get_actuator_handler(CoAP::Message::message const& request,
								engine::response& response, void*) noexcept;
template<int GPIONum>
static void put_actuator_handler(CoAP::Message::message const& request,
								engine::response& response, void*) noexcept;
static void get_dynamic_list_handler(CoAP::Message::message const& request,
								engine::response& response, void*) noexcept;
static void post_dynamic_handler(CoAP::Message::message const& request,
								engine::response& response, void*) noexcept;
static void delete_dynamic_handler(CoAP::Message::message const& request,
								engine::response& response, void*) noexcept;
static void get_separate_handler(CoAP::Message::message const& request,
								engine::response& response, void*) noexcept;

static void exit_error(CoAP::Error& ec, const char* what = nullptr)
{
	error(main_mod, ec, what);
	exit(EXIT_FAILURE);
}

int main()
{
	Error ec;

	/**
	 * Socket
	 */
	engine::endpoint ep{HOST_ADDR, COAP_PORT, ec};
	if(ec) exit_error(ec);
	CoAP::socket socket;

	socket.open(ec);
	if(ec) exit_error(ec, "Error trying to open socket...");
	socket.bind(ep, ec);
	if(ec) exit_error(ec, "Error trying to bind socket...");

	debug(main_mod, "Socket opened...");

	/**
	 * Initiating CoAP engine
	 */
	engine coap_engine(std::move(socket),
			CoAP::Message::message_id(CoAP::time()));

	status(main_mod, "engine has: %u", engine::has_default_callback);
	/**
	 * Resource instantiation
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
							res_separate{"separate", get_separate_handler};

	/**
	 * Adding resource to the tree
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
	coap_engine.root_node().add_child(
			res_time,
			res_sensors,
			res_actuators,
			res_dynamic,
			res_separate);
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
#endif

	/**
	 * "Printing" resource tree
	 */
	std::printf("l: path: [children]:\n");
	std::printf("--------------------\n");
	CoAP::Debug::print_resource_branch(coap_engine.root_node().node());

	while(coap_engine(ec));

	return EXIT_SUCCESS;
}

/**
 * Resource callback definition
 */
static void get_time_handler(Message::message const& request,
								engine::response& response, void*) noexcept
{
	debug(main_mod, "Called get time handler");

	Message::content_format format = Message::content_format::text_plain;
	Message::Option::node content{format};

	char time[15];
	std::snprintf(time, 15, "%lu", CoAP::time());

	response
			.code(Message::code::content)
			.add_option(content)
			.payload(time)
			.serialize();
}

static void get_sensor_handler(CoAP::Message::message const& request,
								engine::response& response, void*) noexcept
{
	debug(main_mod, "Called get sensor handler");

	/**
	 * As we are using the same call back to all sensors, get what sensor by
	 * inspecting the last path
	 */
	Message::Option_Parser parser(request);
	Message::Option::option const *opt, *path_opt = nullptr;
	while((opt = parser.next()))
	{
		if(opt->ocode == Message::Option::code::uri_path)
			path_opt = opt;
	}

	/**
	 * Checking if path was found (need to check? Let's keep it safe from changes)
	 */
	if(!path_opt)
	{
		response.
				code(Message::code::bad_request)
				.serialize();
		return;
	}

	char buffer[20];
	snprintf(buffer, 20, "%.*s: %u", path_opt->length, static_cast<char const*>(path_opt->value),
										CoAP::random_generator() % 100);
	Message::content_format format = Message::content_format::text_plain;
	Message::Option::node content{format};

	response
			.code(Message::code::content)
			.add_option(content)
			.payload(buffer)
			.serialize();
}

/**
 * Simulate the gpio ports of a uC
 */
static bool gpios[3] = {false, false, false};

template<int GPIONum>
static void get_actuator_handler(CoAP::Message::message const& request,
								engine::response& response, void*) noexcept
{
	debug(main_mod, "Called get actuator handler");

	/**
	 * Option
	 */
	Message::content_format format = Message::content_format::text_plain;
	Message::Option::node content{format};

	/**
	 * Payload
	 */
	char buffer[2];
	snprintf(buffer, 2, "%u", gpios[GPIONum]);
	response
		.code(Message::code::content)
		.add_option(content)
		.payload(buffer)
		.serialize();
}

template<int GPIONum>
static void put_actuator_handler(CoAP::Message::message const& request,
								engine::response& response, void*) noexcept
{
	debug(main_mod, "Called put actuator handler");

	/**
	 * Querying value (?value=<0|1>)
	 */
	const void* value = nullptr;
	unsigned length;
	if(!Message::query_by_key(request, "value", &value, length) || !length)
	{
		response
			.code(Message::code::bad_request)
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
		.code(Message::code::changed)
		.payload(buf)
		.serialize();
}

static void get_dynamic_list_handler(CoAP::Message::message const& request,
								engine::response& response, void* engine_void) noexcept
{
	debug(main_mod, "Called get sensor handler");

	/**
	 * The third parameter is the engine...
	 */
	engine* eng = static_cast<engine*>(engine_void);
	engine::resource_node* node = eng->root_node().search_node(request);
	if(!node)
	{
		/* Couldn't find */
		response
			.code(Message::code::internal_server_error)
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
	while((child = (*node)[i++]))
	{
		len += snprintf(buffer + len, engine::packet_size - len, "%s ", child->value().path());
	}
	Message::content_format format = Message::content_format::text_plain;
	Message::Option::node content{format};

	response
			.code(Message::code::content)
			.add_option(content)
			.payload(buffer)
			.serialize();
}

static void get_dynamic_handler(CoAP::Message::message const& request,
		engine::response& response, void* engine_void) noexcept
{
	engine* eng = static_cast<engine*>(engine_void);
	engine::resource_node* node = eng->root_node().search_node(request);

	response
		.code(Message::code::content)
		.payload(node->value().path())
		.serialize();
}

static engine::resource_node dynamics[] = {
		/* path		get					post	put			delete	*/
		{"dyn1", get_dynamic_handler, nullptr, nullptr, delete_dynamic_handler},
		{"dyn2", get_dynamic_handler, nullptr, nullptr, delete_dynamic_handler},
		{"dyn3", get_dynamic_handler, nullptr, nullptr, delete_dynamic_handler}
};

static void post_dynamic_handler(CoAP::Message::message const& request,
								engine::response& response, void* engine_void) noexcept
{
	engine* eng = static_cast<engine*>(engine_void);
	engine::resource_node* node = eng->root_node().search_node(request);

	/**
	 * Querying which node to create (?dyn=<1|2|3>)
	 */
	const void* value = nullptr;
	unsigned length;
	if(!Message::query_by_key(request, "dyn", &value, length) || !length)
	{
		response
			.code(Message::code::bad_request)
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
			.code(Message::code::bad_option)
			.payload("dyn=<1|2|3>")
			.serialize();
		return;
	}

	/**
	 * Adding new resource
	 */
	int index = v - '0' - 1;
	if(!node->add_child(dynamics[index]))
	{
		/* child already added */
		response
			.code(Message::code::precondition_failed)
			.payload("already create")
			.serialize();
		return;
	}

	/**
	 * Sending added information back (yes, it's duplicated... just showing off)
	 */
	Message::Option::node location{Message::Option::code::location_path, dynamics[index].value().path()};
	response
		.code(Message::code::created)
		.add_option(location)
		.payload(dynamics[index].value().path())
		.serialize();

	CoAP::Debug::print_resource_branch(*node);
}

static void delete_dynamic_handler(CoAP::Message::message const& request,
		engine::response& response, void* engine_void) noexcept
{
	engine* eng = static_cast<engine*>(engine_void);
	engine::resource_node* node = eng->root_node().remove_node(request);

	if(!node)
	{
		response
			.code(Message::code::internal_server_error)
			.payload("node search error")
			.serialize();
		return;
	}

	response
		.code(Message::code::deleted)
		.payload(node->value().path())
		.serialize();
}

void cb(void const* trans, CoAP::Message::message const* r, void*)
{
	auto const* t = static_cast<engine::transaction_t const*>(trans);
	status(main_mod, "Status: %s", CoAP::Debug::transaction_status_string(t->status()));
	if(r)
	{
		status(main_mod, "Response received!");
		CoAP::Debug::print_message(*r);
	}
	else
	{
		status(main_mod, "Response NOT received");
	}
}

static void separated_response(engine* eng, engine::async_response data)
{
	char print_buf[20];
	status(main_mod, "Separate response...");
	status(main_mod, "ep=%s:%u, type=%s, token[%lu]=%.*s",
						data.ep.address(print_buf),
						data.ep.port(),
						CoAP::Debug::type_string(data.type),
						data.token_len, data.token_len, data.token);

	/**
	 * Wainting time simulating sleep
	 */
	CoAP::time_t wait = 5 + (CoAP::random_generator() % 6);
	status(main_mod, "Wainting for: %lu", wait);
	std::this_thread::sleep_for(std::chrono::seconds(wait));


	Message::content_format format = Message::content_format::text_plain;
	Message::Option::node content{format};

	std::snprintf(print_buf, 20, "%u", CoAP::random_generator() % 100);

	/**
	 * The response is going to be sent in a request (irony...). You MUST
	 * set the response code.
	 */
	engine::request req{data, Message::code::content};

	req
		.add_option(content)
		.callback(cb)
		.payload(print_buf);

	status(main_mod, "Sending payload=[%s]...", print_buf);
	CoAP::Error ec;
	eng->send(req, ec);
	if(ec)
	{
		error(main_mod, ec, "send");
	}
}

static void get_separate_handler(CoAP::Message::message const& request,
		engine::response& response, void* engine_void) noexcept
{
	response.serialize_empty_ack();
	engine::async_response adata{request, response};

	/**
	 * Open a new thread to simulate a long read. Never block this function
	 * because is called synchronously with engine processing
	 */
	std::thread{separated_response, static_cast<engine*>(engine_void), adata}.detach();
}
