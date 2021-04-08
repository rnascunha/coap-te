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
 */

#include "log.hpp"				//Log header
#include "coap-te.hpp"			//Convenient header
#include "coap-te-debug.hpp"	//Convenient debug header

/**
 * Uncommenting this line the select transaction_list_vector as Tranasction List
 *
 * More about Transaction List below
 */
//#define USE_TRANSACTION_LIST_VECTOR

using namespace CoAP::Log;

#define COAP_PORT		CoAP::default_port		//5683
#define HOST_ADDR		"127.0.0.1"				//Address

/**
 * Log module
 */
static constexpr module example_mod = {
		.name = "EXAMPLE",
		.max_level = CoAP::Log::type::debug
};

/**
 * Reliable connection MUST exachange CSM signaling message when connect.
 * This is our client configuration:
 * * Max message size = 1152 (default)
 * * Accept block wise transfer
 */
static constexpr const CoAP::Transmission::Reliable::csm_configure csm = {
		.max_message_size = CoAP::Transmission::Reliable::default_max_message_size,
		.block_wise_transfer = true
};

using connection_list_t = CoAP::Transmission::Reliable::connection_list<
		CoAP::Transmission::Reliable::Connection, 5>;

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
		csm.max_message_size,
		CoAP::Transmission::Reliable::transaction_cb>;

/**
 * Transaction List are what holds all the transactions inside the
 * engine. You can also implement your own transaction list, if needed.
 *
 * Every time you make a request, a transaction
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
#include "transmission/reliable/transaction_list_vector.hpp"

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
#else /* USE_TRANSACTION_LIST_VECTOR */
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
 * void(*)(Message const&, Response& , void*) =
 * 		CoAP::Resource::callback_reliable<CoAP::Message::Reliable::message,
 * 										CoAP::Transmission::Reliable::Response>;
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
		CoAP::Resource::callback_reliable<
			CoAP::Message::Reliable::message,
			CoAP::Transmission::Reliable::Response
		>,		///< (1) resource callback
		true>;												///< (2) enable/disable description

/**
 * Engine: the pale blue dot
 *
 * Here we are going to show how to define a engine, and how each
 * parameters impacts:
 *-------
 * (1) Connection type: RFC 8323 defines CoAP to reliable connections. Here you can
 * use any type of connection that satisfy the CoAP-te Reliable connection
 * requirements. This means a endpoint type, and a send/receive function. More about
 * CoAP-te reliable connection port at 'tcp_client/tcp_server' example. At this example
 * we are using TCP sockets.
 *--------
 * (2) CSM signal configuration. This configuration will be exchanged at connection
 *--------
 * (3) Transaction List type: a transaction list, as defined above.
 *--------
 * (4) Default callback type: the callback function type to call. When this is called?
 * * When receiving a signal code response;
 * * When receiving a response from a transaction that timeout or just wasn't expected;
 *
 * The callback signature is:
 *
 * void(*)(int socket, CoAP::Message::Reliable::message const&,void*) = CoAP::Transmission::Reliable::default_cb;
 * Where the paremeters are: socket, response, engine.
 *
 * You could use:
 *
 * std::function<void(socket, CoAP::Message::message const&,void*)>;
 *
 * And bind values, uses lambdas... and so on.
 * ------
 *
 * So that it, that's us... CoAP-te...
 */
using engine = CoAP::Transmission::Reliable::engine_server<
		CoAP::Port::POSIX::tcp_server<			///< (1) TCP client socket definition
			CoAP::Port::POSIX::endpoint_ipv4
		>,
		csm,									///< (2) CSM paramenter configuration
		connection_list_t,
		CoAP::disable,
//		transaction_list_t,						///< (3) Trasaction list as defined above
		CoAP::Transmission::Reliable::default_cb,	///< (4) Default callback signature function
		resource>;								///< (5) Resource definition

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
 * This default callback response to signal response
 */
void default_callback(int socket,
		CoAP::Message::Reliable::message const& response,
		void* engine_ptr) noexcept
{
	debug(example_mod, "default cb called");
	CoAP::Debug::print_message(response);
	debug(example_mod, "OUT default cb called");
}

static void get_discovery_handler(CoAP::Message::Reliable::message const& request,
					engine::response& response, void*) noexcept;

int main()
{
	debug(example_mod, "Init engine code...");

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
	 * Setting the default callback function the untracked
	 * responses or signal response.
	 */
	coap_engine.default_cb(default_callback);

	/**
	 * Resource
	 */
	engine::resource_node res_well_known{".well-known"},
								res_core{"core", get_discovery_handler};
	coap_engine.root_node().add_branch(res_well_known, res_core);

	/**
	 * Connection to the server
	 */
	coap_engine.open(ep, ec);
	if(ec) exit_error(ec, "open");

	debug(example_mod, "Initiating engine loop");
	/**
	 * This loop will run until receive a successful response.
	 *
	 * The call to coap_engine() will continually monitor the socket response and check
	 * all the transaction timers. If engine is set to server profile it will monitor any
	 * receiving request and deliver the request to the appropriate resource.
	 */
	while(coap_engine.run(ec))
	{
		/**
		 * Your code, some code, who knows...? (or whom?)
		 */
	}
	if(ec) exit_error(ec, "run");

	return EXIT_SUCCESS;
}

/**
 * \/well-known/core [GET]
 *
 * Respond to request with resource information as defined at RFC6690
 */
static void get_discovery_handler(CoAP::Message::Reliable::message const& request,
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
