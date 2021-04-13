/**
 * Here we are going to implement a client CoAP device that will make 3
 * request and register to observe 3 different servers resources: 'time',
 * 'type' and 'sensor'. A full description of this resources can be seen
 * at 'server_observer' example.
 *
 * This example is to be run with 'server_observer'.
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
 * Engine definition
 *
 * To know how to define a engine, see 'raw_engine'/'server_engine' examples
 */
using engine = CoAP::Transmission::engine<
		CoAP::Port::POSIX::udp<endpoint>,			/* socket type */
		CoAP::Message::message_id,					/* message id generator type */
		CoAP::Transmission::transaction_list<		/* transaction list */
				CoAP::Transmission::transaction<	/* transaction type */
						512,								/* transaction packet size */
						CoAP::Transmission::transaction_cb,	/* transaction callback type */
						endpoint>,							/* transaction endpoint type */
				4>									/* number of transaction */,
		CoAP::Transmission::default_cb<				/* default callback type */
			endpoint>,
		CoAP::disable								/* disable resource */
	>;

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
 */
static CoAP::Observe::observe<endpoint> time_obs;
static CoAP::Observe::observe<endpoint> type_obs;
static CoAP::Observe::observe<endpoint> sensor_obs;

/**
 * As observable are sent asynchronously, not associated with
 * a specific request, they will call the default callback engine
 * function.
 *
 * Notification can be sent of type confirmable or non-confirmable.
 * Confirmable notification must sent back a ack, but this is already
 * handled by the CoAP-te engine.
 */
void default_callback(engine::endpoint const& ep,
		CoAP::Message::message const& response,
		void* engine_ptr) noexcept
{
	debug(example_mod, "default cb called");
	CoAP::Debug::print_message_string(response);

	/**
	 * Checking if the response is associated to any of the observable variable
	 */
	if(time_obs.check(ep, response))		//Is from time resource?
	{
		status(example_mod, "time: %.*s",
				static_cast<int>(response.payload_len), static_cast<const char*>(response.payload));
	}
	else if(type_obs.check(ep, response)) 		//Is from type resource?
	{
		status(example_mod, "typed: %c", *static_cast<const char*>(response.payload));
	}
	else if(sensor_obs.check(ep, response))		//Is from sensor resource?
	{
		status(example_mod, "sensor: %.*s",
				static_cast<int>(response.payload_len), static_cast<const char*>(response.payload));
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
				time_obs.set(t->endpoint(), *response);
			}
			/**
			 * Is type resource request?
			 */
			else if(std::memcmp(response->token, type_token, response->token_len) == 0)
			{
				type_obs.set(t->endpoint(), *response);
			}
			/**
			 * Is sensor resource request?
			 */
			else if(std::memcmp(response->token, sensor_token, response->token_len) == 0)
			{
				sensor_obs.set(t->endpoint(), *response);
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

	CoAP::Error ec;

	/**
	 * Socket
	 */
	engine::connection conn;

	conn.open(ec);
	if(ec) exit_error(ec, "Error trying to open socket...");

	debug(example_mod, "Socket opened...");

	debug(example_mod, "Initating the engine...");

	/**
	 * After successfully opened the socket, we are going to
	 * instantiate the engine.
	 */
	engine coap_engine(std::move(conn),
			CoAP::Message::message_id(CoAP::time()));

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
	 * Instantiating a endpoint that the request will be sent.
	 */
	engine::endpoint ep{HOST_ADDR, COAP_PORT, ec};
	if(ec) exit_error(ec);

	/**
	 * Instatiation a request
	 */
	engine::request request(ep);

	/**
	 * Setting the time resource request and sending
	 */
	CoAP::Message::Option::node path_op1{CoAP::Message::Option::code::uri_path, "time"};
	request.header(CoAP::Message::type::confirmable, CoAP::Message::code::get)
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

	request.header(CoAP::Message::type::confirmable, CoAP::Message::code::get)
				.token(type_token)
				.add_option(obs_op)
				.add_option(path_op2)
				.callback(request_cb/*, data */);

	coap_engine.send(request, ec);
	if(ec) exit_error(ec, "send");
	debug(example_mod, "Message type observe sended");

	/**
	 * Setting the sensor resource request and sending
	 *
	 * This resource you can specify the frequency that you want to
	 * receive the data, using the query option (15s in this example).
	 */
	request.reset();

	CoAP::Message::Option::node path_op3{CoAP::Message::Option::code::uri_path, "sensor"};
	CoAP::Message::Option::node query_op{CoAP::Message::Option::code::uri_query, "freq=15"};

	request.header(CoAP::Message::type::confirmable, CoAP::Message::code::get)
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
	 * This loop will run until receive a successful response or the transaction
	 * timeout.
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
