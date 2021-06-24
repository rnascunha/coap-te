/**
 * This example shows how to define a CoAP::engine. The engine
 * is where all the CoAP-te features are aggregate. All the transaction
 * complexity, resources management and others are hidden from the user.
 *
 * Thinking that different environments have different needs, it was develop
 * a configurable engine. More about how to configure it to your needs below.
 *
 * This examples show the use in client mode, so we are going to make a
 * simple request, and print the response (if any).
 */

#include "coap-te/log.hpp"				//Log header
#include "coap-te.hpp"			//Convenient header
#include "coap-te-debug.hpp"	//Convenient debug header

/**
 * Uncommenting this line the select transaction_list_vector as Tranasction List
 *
 * More about Transaction List below
 */
#define USE_TRANSACTION_LIST_VECTOR

using namespace CoAP::Log;

#define COAP_PORT		CoAP::default_port		//5683
#define HOST_ADDR		"127.0.0.1"				//Address

/**
 * Log module
 */
static constexpr module example_mod = {
		/*.name = */"EXAMPLE",
		/*.max_level = */CoAP::Log::type::debug
};

/**
 * Transmission configuration settings
 * https://tools.ietf.org/html/rfc7252#section-4.8
 */
static constexpr const CoAP::Transmission::configure tconfigure = {
	/*.ack_timeout_seconds 			= */2.0,	//ACK_TIMEOUT
	/*.ack_random_factor 			= */1.5,	//ACK_RANDOM_FACTOR
	/*.max_restransmission 			= */4,		//MAX_RETRANSMIT
};

/**
 * Definition of transaction. All transaction parameters are described
 * at raw_transaction example.
 *
 * To use with engine, transactions MUST use internal storage, i.e.,
 * MaxPacketSize > 0 (first parameter)
 */
using transaction_t = CoAP::Transmission::transaction<
		512,
		CoAP::Transmission::transaction_cb,
		CoAP::Port::POSIX::endpoint_ipv4>;

/**
 * Transaction List are what holds all the transactions inside the
 * engine. You can also implement your own transaction list, if needed.
 *
 * Every time a request of type confirmable is made, a transaction
 * slot is occupied at the transaction list, and is only released
 * when cancelled, timeout or successfully receives a response.
 *
 * There are two implementations, as follows.
 */
#ifdef USE_TRANSACTION_LIST_VECTOR
/**
 * As trasaction_list_vector uses std::vector (i.e., dynamic allocation) as
 * internal container, you must explicitly included it
 */
#include "coap-te/transmission/transaction_list_vector.hpp"

/**
 * This transaction list implementation can hold a unlimited number
 * of transaction. It's more suitable to devices where memory usage
 * is not limited (servers).
 *
 * As template parameter, just the (1) transaction type.
 */
using transaction_list_t =
		CoAP::Transmission::transaction_list_vector<
			transaction_t		/* (1) transaction type */
		>;
#else /* USE_TRANSACTION_LIST_VECTOR */
/**
 * This is the default implementation, and recommended to constrained devices.
 * It uses a simple array to hold the transaction. The template parameters are:
 * (1) The transaction type it will hold, defined above;
 * (2) The number of transactions to hold simultaneously.
 */
using transaction_list_t =
		CoAP::Transmission::transaction_list<
			transaction_t,	/* (1) transaction type */
			4>;				/* (2) number of transaction */
#endif /* USE_TRANSACTION_LIST_VECTOR */

/**
 * Any request made with CoAP must be associated to a specific resource.
 *
 * If you are not intended to receive any request (like a client), you can
 * use CoAP::disable (i.e. "void*") and any income request will generate a
 * response error (this example could disable resource as we are just going
 * to make a request, but we keeped here as reference).
 *
 * But if you expect to act like a server, you must provide a resource type
 * to CoAP-te engine. Above is shown the default resource definition:
 *----------
 * (1) the callback function type that it will be called to any successful request.
 * The function signature must be:
 *
 * void(*)(Message::message const&, engine::response& , void*) = CoAP::Resource::callback<Endpoint>;
 * Where the parameters are: request, response and engine.
 *
 * You could also use:
 *
 * std::function<void(Message::message const&, engine::response& , void*)>
 *
 * And bind values, uses lambdas... and so on.
 * --------
 * (2) enable/disable resource description, as defined at RFC6690 (you can save some
 * bytes (set to false) if this is not necessary).
 */
using resource = CoAP::Resource::resource<
		CoAP::Resource::callback<CoAP::Port::POSIX::endpoint_ipv4>,	///< (1) resource callback
		true>;												///< (2) enable/disable description

/**
 * Engine: the pale blue dot
 *
 * Here we are going to show how to define a engine, and how each
 * parameters impacts:
 *-------
 * (1) Connection type: CoAP was develop to work on top of UDP sockets, nevertheless,
 * here you can use any type of connection that satisfy the CoAP-te connection
 * requirements. This means a endpoint type, and a send/receive function. More about
 * CoAP-te port at 'udp_socket' example. At this example we are using UDP sockets.
 *--------
 * (2) Message ID generator type: CoAP-te already defines a default MessageID generator,
 * but you can define your own. Check 'message_id' at 'message/message_id'.
 *--------
 * (3) Transaction List type: a transaction list, as defined above.
 *--------
 * (4) Default callback type: the callback function type to call. When this is called?
 * * When receiving a response from a non-confirmable request;
 * * When receiving a ack response from a transaction that timeout;
 * * When receiving response of separate type. The transaction was that holded the request
 * was cleared when receiving the empty ack, and the response will be sent in the future
 * with a new Message ID.
 *
 * The callback signature is:
 *
 * void(*)(Endpoint const&, CoAP::Message::message const&,void*) = CoAP::Transmission::default_cb<Endponint>;
 * Where the paremeters are: response enpoint, response, engine.
 *
 * You could use:
 *
 * std::function<void(Endpoint const&, CoAP::Message::message const&,void*)>;
 *
 * And bind values, uses lambdas... and so on.
 * ------
 *
 * So that it, that's us... CoAP-te...
 */
using engine = CoAP::Transmission::engine<
		CoAP::Port::POSIX::udp<CoAP::Port::POSIX::endpoint_ipv4>,	/* (1) socket type */
		CoAP::Message::message_id,				/* (2) message id generator type */
		transaction_list_t,						/* (3) transaction list */
		CoAP::Transmission::default_cb<			/* (4) default callback type */
			CoAP::Port::POSIX::endpoint_ipv4>,
		resource								/* (5) resource callback */
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

//Response flag to check if response was received
static bool response_flag = false;

/**
 * This default callback response to non-confirmable messages,
 * async responses or timed out transaction
 */
void default_callback(engine::endpoint const& ep,
		CoAP::Message::message const* response,
		void* engine_ptr) noexcept
{
	debug(example_mod, "default cb called");
	CoAP::Debug::print_message(*response);

	/**
	 * If we are receiving a response in a confirmable message (probably from a
	 * separate response), we MUST sent a empty ack response.
	 *
	 * This function will make all necessary checks and sent the appropriate response.
	 * It returns 'true' if it was a confirmable response, and false otherwise. The
	 * last parameter you can check if the sent call was successful.
	 *
	 * Highly recommended to always call this function if you expect a separated response
	 */
	CoAP::Error ec;
	if(CoAP::Transmission::send_async_ack(*static_cast<engine*>(engine_ptr), ep, *response, ec))
	{
		debug(example_mod, "Confirmable response received");
		if(ec) exit_error(ec, "send async");
	}

	/**
	 * Setting flag that response was received
	 */
	response_flag = true;
}

/**
 * Request callback (signature defined at transaction)
 */
void request_cb(void const* trans, CoAP::Message::message const* response, void*) noexcept
{
	debug(example_mod, "Request callback called...");

	auto const* t = static_cast<engine::transaction_t const*>(trans);
	status(example_mod, "Status: %s", CoAP::Debug::transaction_status_string(t->status()));
	if(response)
	{
		status(example_mod, "Response received!");
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
		status(example_mod, "Response NOT received");
	}
	/**
	 * Setting flag that response was received
	 */
	response_flag = true;
}

int main()
{
	debug(example_mod, "Init engine code...");
	
	/**
	* Window/Linux: Initialize random number generator
	* Windows: initialize winsock library
	*/
	CoAP::init();

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
	 *
	 * The first and second parameters are mandatory: connection and
	 * message_id generator. The third is the transaction parameters
	 * as defined at https://tools.ietf.org/html/rfc7252#section-4.8.
	 * If you don't provide this value, the default values will be used.
	 *
	 * Also, transaction parameters can be defined peer request, if
	 * you need.
	 */
	engine coap_engine(std::move(conn),
			CoAP::Message::message_id((unsigned)CoAP::time()),
			tconfigure);

	/**
	 * Setting the default callback function the untracked
	 * responses.
	 */
	coap_engine.default_cb(default_callback);

	debug(example_mod, "Constructing the request message...");

//	CoAP::Message::Option::node path_op1{CoAP::Message::Option::code::uri_path, "time"};
	CoAP::Message::Option::node path_op1{CoAP::Message::Option::code::uri_path, ".well-known"};
	CoAP::Message::Option::node path_op2{CoAP::Message::Option::code::uri_path, "core"};

	/**
	 * Instantiating a endpoint that the request will be sent.
	 *
	 * CoAP::socket::endpoint == engine::endpoint
	 */
	engine::endpoint ep{HOST_ADDR, COAP_PORT, ec};
	if(ec) exit_error(ec);

	/**
	 * We are going to create a request to be sent. Request are a lot
	 * like factory. But you also must defined a endpoint where the request
	 * will be sent, and, optionally, a callback function.
	 */
	engine::request request(ep);
	request.header(CoAP::Message::type::confirmable, CoAP::Message::code::get)
			.token("token")
			.add_option(path_op1)
			.add_option(path_op2)
			.callback(request_cb/*, data */);

	debug(example_mod, "Sending the request...");
	/**
	 * Sending the request. We could also externally provide a message id,
	 * or set the transaction configuration parameters. Check other ways on
	 * how to send request at 'transmission/engine'.
	 */
	coap_engine.send(request, ec);
	if(ec) exit_error(ec, "send");

	debug(example_mod, "Message sended");

	debug(example_mod, "Initiating engine loop");
	/**
	 * This loop will run until receive a successful response or the transaciton
	 * timeout.
	 *
	 * The call to coap_engine() will continually monitor the socket response, check
	 * all the transaction timers and make all necessary retransmissions. If engine is
	 * set to server profile it will monitor any receiving request and deliver the
	 * request to the appropriate resource.
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
