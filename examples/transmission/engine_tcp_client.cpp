/**
 * This example shows how to define a TCP Client CoAP reliable engine.
 * The engine is where all the CoAP-te features are aggregate. All the
 * transaction complexity, resources management and others are hidden from
 * the user.
 *
 * Thinking that different environments have different needs, it was develop
 * a configurable engine. More about how to configure it to your needs below.
 *
 * This examples shows how to send a ping signal, wait for a response, and then
 * make a simple request, and print the response (if any).
 *
 * To configure a CoAP-te Reliable client engine, you must choose:
 * * Connection/Endpoint type: TCP or Websocket (not implemented), IPv4 or IPv6
 * * Transaction list type (if any)
 */

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
 * Connection port
 */
#define COAP_PORT		CoAP::default_port		//5683

/**
 * Choosing the endpoint type
 *
 * Uncomment the following line to use IPv4 configuration
 */
//#define USE_ENDPOINT_IPV4

#ifdef USE_ENDPOINT_IPV4
/**
 * Endpoint IPv4 defined and connect address
 */
using endpoint_t = CoAP::Port::POSIX::endpoint_ipv4;
#define HOST_ADDR		"127.0.0.1"				//localhost
#else
/**
 * Endpoint IPv6 defined and connect address
 */
using endpoint_t = CoAP::Port::POSIX::endpoint_ipv6;
#define HOST_ADDR		"::1"					//localhost
#endif

/**
 * Reliable connection MUST exchange CSM signaling message when connect.
 * This is our client configuration:
 * * Max message size = 1152 (default)
 * * Accept block wise transfer
 */
static constexpr const CoAP::Transmission::Reliable::csm_configure csm = {
		.max_message_size = CoAP::Transmission::Reliable::default_max_message_size,
		.block_wise_transfer = true
};

/**
 * Connection type we are going to use
 */
using connection = CoAP::Port::POSIX::tcp_client<endpoint_t>;		///< TCP client socket definition

/**
 * Definiting a transaction.
 *
 * To use with engine, transactions MUST use internal storage, i.e.,
 * MaxPacketSize > 0 (first parameter). You should use the same size of the
 * max message size that you support (defined at the CSM parameters above).
 *
 * The second parameter is the transaction callback signature
 */
using transaction_t = CoAP::Transmission::Reliable::transaction<
		connection::handler,
		csm.max_message_size,
		CoAP::Transmission::Reliable::transaction_cb>;

/**
 * Choosing transaction list type.
 *
 * Transaction are essential when using unreliable connections, as
 * the CoAP layer is response to retransmit any unanswered (ack) response.
 *
 * To reliable connection they are still need to associate a request with a response
 * (by the token). A transaction list holds the request until receives a response (or
 * timeout). Transaction also holds a specific callback function that will be called
 * if a response arrive, or a error (cancel, timeout...) occur.
 *
 * If you don't need this feature, you can disable it using CoAP::disable
 * as transaction list (comment both define statements below).
 */
/**
 * (Un)comment the following lines the select transaction list
 */
//#define USE_TRANSACTION_LIST_DEFAULT
//#define USE_TRANSACTION_LIST_VECTOR

#ifdef USE_TRANSACTION_LIST_VECTOR
/**
 * As trasaction_list_vector uses std::vector (i.e., dynamic allocation) as
 * internal container. As dynamic allocation is a feature not used in all CoAP-te
 * we choose to keep it apart from the default implementation. So, you must
 * explicitly included it
 */
#include "transmission/reliable/containers/transaction_list_vector.hpp"

/**
 * This transaction list implementation can hold a unlimited number
 * of transaction. It's more suitable to devices where memory usage
 * is not limited (servers).
 *
 * As template parameter, just the (1) transaction type.
 */
using transaction_list_t =
		CoAP::Transmission::Reliable::transaction_list_vector<
			transaction_t		/* (1) transaction type */
		>;
#elif defined(USE_TRANSACTION_LIST_DEFAULT)
/**
 * This is the default implementation, and recommended to constrained devices.
 * It uses a simple array to hold the transaction. The template parameters are:
 * (1) The transaction type it will hold, defined above;
 * (2) The number of transactions to hold simultaneously.
 */
using transaction_list_t =
		CoAP::Transmission::Reliable::transaction_list<
			transaction_t,	/* (1) transaction type */
			4>;				/* (2) number of transaction */
#else
/**
 * This will disable transactions. Any responses received must
 * be handled at the 'default callback'.
 */
using transaction_list_t = CoAP::disable;
#endif /* USE_TRANSACTION_LIST_VECTOR */

/**
 * Any request received must be associated to a specific resource.
 *
 * If you are not intended to receive any request (like a client), you can
 * use CoAP::disable and any income request will generate a
 * response error.
 *
 * If you want to know how define a resource, check the 'engine_tcp_server'.
 */
using resource = CoAP::disable;

/**
 * Engine: the pale blue dot
 *
 * Here we are going to show how to define a engine, and how each
 * parameters impacts:
 *-------
 * (1) Connection type: RFC 8323 defines CoAP to reliable connections. Here you can
 * use any type of connection that satisfy the CoAP-te Reliable connection
 * requirements.  More about CoAP-te reliable connection port at 'tcp_client/tcp_server'
 * example. At this example we are using TCP Client sockets.
 *--------
 * (2) CSM signal configuration. This configuration will be exchanged at connection
 *--------
 * (3) Transaction List type: a transaction list, as defined above.
 *--------
 * (4) Default callback type: the callback function type to call. When this is called?
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
 * std::function<void(socket, CoAP::Message::message const&,void*)>;
 *
 * And bind values, uses lambdas... and so on.
 * ------
 * (5) resource type, as defined above;
 *------
 * So that it, that's us... CoAP-te...
 */
using engine = CoAP::Transmission::Reliable::engine_client<
		connection,			///< (1) TCP client socket definition
		csm,									///< (2) CSM paramenter configuration
		transaction_list_t,						///< (3) Trasaction list as defined above
		CoAP::Transmission::Reliable::default_cb<CoAP::Port::POSIX::tcp_server<endpoint_t>::handler>,	
												///< (4) Default callback signature function
		resource>;								///< (5) Resource definition (disabled)

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

//Response flag to check if response was received
static bool response_flag = false;
//Pong flag to check if pong message was received
static bool pong_flag = false;

/**
 * This default callback response to signal response
 */
void default_callback(engine::socket,
		CoAP::Message::Reliable::message const* response,
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

	CoAP::Debug::print_message_string(*response);

	//Check if is a signal response
	if(CoAP::Message::is_signaling(response->mcode))
	{
		//Check if is a pong
		if(response->mcode == CoAP::Message::code::pong)
			pong_flag = true;
		return;
	}
	response_flag = true;
}

/**
 * Request callback (signature defined at transaction)
 */
void request_cb(void const* trans, CoAP::Message::Reliable::message const* response, void*) noexcept
{
	debug(example_mod, "Request callback called...");

	auto const* t = static_cast<engine::transaction_t const*>(trans);
	status(example_mod, "Status: %s", CoAP::Debug::transaction_status_string(t->status()));
	if(response)
	{
		status(example_mod, "Response received!");
		CoAP::Debug::print_message_string(*response);
	}
	else
	{
		/**
		 * Function timeout or transaction was cancelled
		 */
		status(example_mod, "Response NOT received");
	}
	/**
	 * Setting flag that response was received
	 */
	response_flag = true;
}

/**
 * Main function
 */
int main()
{
	debug(example_mod, "TCP client engine example...");

	/**
	* Window/Linux: Initialize random number generator
	* Windows: initialize winsock library
	*/
	CoAP::init();

	CoAP::Error ec;

	debug(example_mod, "Initating the engine...");

	/**
	 * Instantiating a CoAP engine
	 */
	engine coap_engine;

	/**
	 * Instantiating a endpoint that we are going to connect
	 */
	engine::endpoint ep{HOST_ADDR, COAP_PORT, ec};
	if(ec) exit_error(ec);

	/**
	 * Setting the default callback function to track signal
	 * response
	 */
	coap_engine.default_cb(default_callback);

	/**
	 * Connection to the server
	 *
	 * This call will block until connect or receive a error
	 */
	coap_engine.open(ep, ec);
	if(ec) exit_error(ec, "connect");

	/**
	 * First we are going to send a ping signal to the server
	 */
	/**
	 * Making the ping message
	 */
	engine::request<CoAP::Message::code::ping> req_ping;

	/**
	 * Sending...
	 */
	coap_engine.send(req_ping, ec);
	if(ec) exit_error(ec, "ping");
	status(example_mod, "Ping message sent...");
	/**
	 * Waiting for the pong response
	 */
	status(example_mod, "Waiting pong signal...");

	while(!pong_flag && coap_engine(ec));
	if(ec) exit_error(ec, "pong");

	debug(example_mod, "Constructing the request message...");

//	CoAP::Message::Option::node path_op1{CoAP::Message::Option::code::uri_path, "time"};
	CoAP::Message::Option::node path_op1{CoAP::Message::Option::code::uri_path, ".well-known"};
	CoAP::Message::Option::node path_op2{CoAP::Message::Option::code::uri_path, "core"};

	/**
	 * We are going to create a request to be sent. Request are a lot
	 * like factory, but you also must defined a callback function.
	 */
	engine::request<> request;
	request.code(CoAP::Message::code::get)
			.token("token")
			.add_option(path_op1)
			.add_option(path_op2)
			.callback(request_cb/*, data */);

	debug(example_mod, "Sending the request...");
	/**
	 * Sending the request.
	 *
	 * The second parameter defines that this request will never timeout... Until
	 * the connection exist, we are going to wait for a response
	 */
	coap_engine.send(request, CoAP::Transmission::Reliable::no_expiration, ec);
	if(ec) exit_error(ec, "send");

	debug(example_mod, "Message sended");

	debug(example_mod, "Initiating engine loop");

	/**
	 * This loop will run until receive a successful response.
	 *
	 * The call to coap_engine() will continually monitor the socket response and check
	 * all the transaction timers. If engine is set to server profile it will monitor any
	 * receiving request and deliver the request to the appropriate resource.
	 */
	while(!response_flag && coap_engine(ec))
	{
		/**
		 * Your code, some code, who knows...? (or whom?)
		 */
	}
	if(ec) exit_error(ec, "run");

	return EXIT_SUCCESS;
}
