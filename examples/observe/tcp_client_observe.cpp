/**
 * Here we are going to implement a TCP client CoAP device that will make 3
 * request and register to observe 3 different servers resources: 'time',
 * 'type' and 'sensor'. A full description of this resources can be seen
 * at 'tcp_server_observer' example.
 *
 * This example is to be run with 'tcp_server_observer'.
 */

#include "log.hpp"				//Log header
#include "coap-te.hpp"			//Convenient header
#include "coap-te-debug.hpp"	//Convenient debug header

#include <cstring>

/**
 * Log namespace
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
 * Servers endpoint definitions
 */
#define COAP_PORT		CoAP::default_port		//5683
#define HOST_ADDR		"127.0.0.1"				//Address
/**
 * Endpoint type
 */
using endpoint = CoAP::Port::POSIX::endpoint_ipv4;

/**
 * CSM signal configuration
 */
static constexpr const CoAP::Transmission::Reliable::csm_configure csm = {
		.max_message_size = CoAP::Transmission::Reliable::default_max_message_size,
		.block_wise_transfer = true
};

/**
 * Client Reliable Engine definition
 *
 * To know how to define a engine, see 'engine_tcp_client'/'engine_tcp_server'
 * examples
 */
using engine = CoAP::Transmission::Reliable::engine_client<
		CoAP::Port::POSIX::tcp_client<			///< TCP client socket definition
			endpoint>,
		csm,									///< CSM paramenter configuration
		CoAP::Transmission::Reliable::transaction_list<	///< transaction list type
			CoAP::Transmission::Reliable::transaction<	///< trasnaction type
				csm.max_message_size,
				CoAP::Transmission::Reliable::transaction_cb>,
			4>,											///< transaction list size
		CoAP::Transmission::Reliable::default_cb<CoAP::Port::POSIX::tcp_client<endpoint>::handler>,	///< Default callback signature function
		CoAP::disable>;								///< Disable resource

/**
 *
 *
 */

/**
 * Auxiliary function
 */
void exit_error(CoAP::Error& ec, const char* what = nullptr)
{
	error(example_mod, ec, what);
	exit(EXIT_FAILURE);
}

/**
 * Every observable message must be associated with a token
 *
 * Here we are defining one token to each resource we are going
 * to observe.
 */
static constexpr const char* time_token = "time";		//time resource
static constexpr const char* type_token = "type";		//type resource
static constexpr const char* sensor_token = "sensor";	//sensor resource

/**
 * This variables will hold the observable information so we can
 * check if the message is from what resource
 *
 * The second template parameter is to enable/disable the storage of
 * order parameters. As we are using a reliable connection type (TCP)
 * that garantees do order of the packets received, we don't need it.
 * We are going to disable setting it to false.
 */
static CoAP::Observe::observe<engine::socket, false> time_obs;
static CoAP::Observe::observe<engine::socket, false> type_obs;
static CoAP::Observe::observe<engine::socket, false> sensor_obs;

/**
 * As observable are sent asynchronously, not associated with
 * a specific request, they will call the default callback engine
 * function.
 */
void default_callback(engine::socket ep,
		engine::message const* response,
		void* engine_ptr) noexcept
{
	debug(example_mod, "default cb called");

	if(!response)
	{
		status(example_mod, "Socket closed!");
		return;
	}

	CoAP::Debug::print_message_string(*response);

	/**
	 * Checking if the response is associated to any of the observable variable
	 */
	if(time_obs.check(ep, *response))		//Is from time resource?
	{
		status(example_mod, "time: %.*s",
				static_cast<int>(response->payload_len), static_cast<const char*>(response->payload));
	}
	else if(type_obs.check(ep, *response)) 		//Is from type resource?
	{
		status(example_mod, "typed: %c", *static_cast<const char*>(response->payload));
	}
	else if(sensor_obs.check(ep, *response))		//Is from sensor resource?
	{
		status(example_mod, "sensor: %.*s",
				static_cast<int>(response->payload_len), static_cast<const char*>(response->payload));
	}
	else
		warning(example_mod, "Response is not from a observable request...");
}

/**
 * To start receiving response from observable resources, we must ask to the server to
 * register.
 *
 * This callback will be called when receive a response from the server, and we
 * are going to check if we were registered to the specific resource we asked for.
 */
void request_cb(void const* trans, engine::message const* response, void*) noexcept
{
	debug(example_mod, "Request callback called...");

	auto const* t = static_cast<engine::transaction_t const*>(trans);
	if(response)
	{
		status(example_mod, "Response received!");
		CoAP::Debug::print_message_string(*response);

		/**
		 * Checking if there is a observe option at response.
		 *
		 * If there is, the registration was successful
		 */
		CoAP::Message::Option::option opt;
		if(CoAP::Message::Option::get_option(*response, opt, CoAP::Message::Option::code::observe))
		{
			/**
			 * There is...
			 *
			 * Every response with the observable option has a value associated for synchronization
			 * purpose.
			 */
			unsigned value = CoAP::Message::Option::parse_unsigned(opt);
			status(example_mod, "Observe resource register [%.*s/%u]",
					response->token_len, response->token, value);

			/**
			 * As we are using the same callback to all the request, we must check to which
			 * request this is associated.
			 */
			/**
			 * Is time resource request?
			 */
			if(std::memcmp(response->token, time_token, response->token_len) == 0)
			{
				time_obs.set(t->socket(), *response);
			}
			/**
			 * Is type resource request?
			 */
			else if(std::memcmp(response->token, type_token, response->token_len) == 0)
			{
				type_obs.set(t->socket(), *response);
			}
			/**
			 * Is sensor resource request?
			 */
			else if(std::memcmp(response->token, sensor_token, response->token_len) == 0)
			{
				sensor_obs.set(t->socket(), *response);
			}
			else
			{
				status(example_mod, "Observable resource not identified");
			}
		}
	}
	else
	{
		/**
		 * Function timeout or transaction was cancelled
		 */
		status(example_mod, "Response NOT received");
	}
}

int main()
{
	debug(example_mod, "Init Observe Client code...");
	
	/**
	 * At Linux, does nothing. At Windows initiate winsock
	 */
	CoAP::Port::POSIX::init();

	CoAP::Error ec;

	/**
	 * Instantiating a endpoint of server we are going to connect
	 */
	engine::endpoint ep{HOST_ADDR, COAP_PORT, ec};
	if(ec) exit_error(ec);

	debug(example_mod, "Initating the engine...");
	/**
	 * Instantiate the engine.
	 */
	engine coap_engine;

	/**
	 * Connection to server
	 */
	coap_engine.open(ep, ec);
	if(ec) exit_error(ec, "Error trying to connect to socket...");

	debug(example_mod, "Socket opened...");

	/**
	 * Setting the default callback function. All notification will
	 * be handled here
	 */
	coap_engine.default_cb(default_callback);

	debug(example_mod, "Constructing the request message...");

	/**
	 * To register to each to the server resource, we must send a observe option
	 * with 0 ( = CoAP::Observe::reg).
	 */
	unsigned regi = CoAP::Observe::reg;
	CoAP::Message::Option::node obs_op{CoAP::Message::Option::code::observe, regi};

	/**
	 * Instatiation a request
	 */
	engine::request<> request;

	/**
	 * Setting the time resource request and sending
	 */
	CoAP::Message::Option::node path_op1{CoAP::Message::Option::code::uri_path, "time"};
	request
		.code(CoAP::Message::code::get)
		.token(time_token)
		.add_option(obs_op)
		.add_option(path_op1)
		.callback(request_cb/*, data */);

	coap_engine.send(request, ec);
	if(ec) exit_error(ec, "send");
	debug(example_mod, "Message time observe sended");

	/**
	 * Setting the type resource request and sending
	 */
	CoAP::Message::Option::node path_op2{CoAP::Message::Option::code::uri_path, "type"};

	request.reset();

	request
		.code(CoAP::Message::code::get)
		.token(type_token)
		.add_option(obs_op)
		.add_option(path_op2)
		.callback(request_cb/*, data */);

	coap_engine.send(request, ec);
	if(ec) exit_error(ec, "send");
	debug(example_mod, "Message type observe sended [%zu]");

	/**
	 * Setting the sensor resource request and sending
	 *
	 * This resource you can specify the frequency that you want to
	 * receive the data, using the query option (15s in this example).
	 */
	request.reset();

	CoAP::Message::Option::node path_op3{CoAP::Message::Option::code::uri_path, "sensor"};
	CoAP::Message::Option::node query_op{CoAP::Message::Option::code::uri_query, "freq=15"};

	request
		.code(CoAP::Message::code::get)
		.token(sensor_token)
		.add_option(obs_op)
		.add_option(path_op3)
		.add_option(query_op)
		.callback(request_cb/*, data */);

	coap_engine.send(request, ec);
	if(ec) exit_error(ec, "send");
	debug(example_mod, "Message sensor observe sended");

	debug(example_mod, "Initiating engine loop");
	/**
	 * This loop will run indefinitely
	 */
	while(coap_engine(ec))
	{
		/**
		 * Your code, some code, who knows...? (or whom?)
		 */
	}
	if(ec) exit_error(ec, "run");

	return EXIT_SUCCESS;
}
