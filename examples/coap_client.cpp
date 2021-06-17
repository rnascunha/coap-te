#include <cstdio>
#include <cstdlib>	//std::strtod
#include <cstring>	//std::strcmp
#include <csignal>
#include <type_traits>

#include "arguments.hpp"

#include "log.hpp"
#include "coap-te.hpp"
#include "coap-te-debug.hpp"

using namespace CoAP::Log;

#define BUFFER_LEN		512

void error_message(const char* message)
{
	color<type::error>();
	printf("%s", message);
	nl_rs();
}

void exit_error(const char* message)
{
	error_message(message);
	exit(EXIT_FAILURE);
}

void print_usage(const char* name)
{
	std::printf(R"(How to use:
	%s [-N] [-T=<token>] [-c=<get|post|put|delete|fetch|patch|ipatch>] 
		[-a=<ack_timeout>] [-R=<retransmist_count>] [-r=<ack_random_factor>]
		[-h=<host>] [-f=<payload>] 
		'<coap|coap+tcp>://<host>:<port>/<path>?<query>'
	-N send non-confirmable message
	-T=<token> set token
	-c=<method> request method. Can be: get|post|put|delete|fetch|patch|ipatch. Default: get
	-a=<ack_timeout> time to wait for first ack, before retransmit (float value). Default: 2.0s
	-R=<retransmit_count> how many retransmit attempt will be made (interger value). Default 4
	-r=<ack_random_factor> factor to calculate retransmit time (float value). Default: 1.5
	-h=<host> adds option 'uri-host' with value <host>
	-p=<bind_port> bind local address to port
	-f=<payload> payload of request (just string for now)
)", name);
	std::printf("Examples:\n");
	std::printf("\t%s coap://[::1]:5683/path/to/resource?query=key\n", name);
	std::printf("\t%s 'coap+tcp://127.0.0.1:5683?value1=key&value2'\n", name);
	//Not supported
//	std::printf("\t%s 'coaps://127.0.0.1:5683?value1=key&value2'\n", name);
//	std::printf("\t%s 'coaps+tcp://127.0.0.1:5683?value1=key&value2'\n", name);
//	std::printf("\t%s 'coap+ws://127.0.0.1:5683?value1=key&value2'\n", name);
//	std::printf("\t%s 'coaps+ws://127.0.0.1:5683?value1=key&value2'\n", name);
}

using endpoint = CoAP::Port::POSIX::endpoint_ip;

using engine_udp = CoAP::Transmission::engine<
		CoAP::Port::POSIX::udp<endpoint>,	/* (1) socket type */
		CoAP::Message::message_id,			/* (2) message id generator type */
		CoAP::Transmission::transaction_list<
			CoAP::Transmission::transaction<
				512,
				CoAP::Transmission::transaction_cb,
				endpoint>,					/* (1) transaction type */
			4>,								/* (3) transaction list */
		CoAP::Transmission::default_cb<		/* (4) default callback type */
			endpoint>,
		CoAP::disable						/* (5) resource callback */
	>;

static constexpr const CoAP::Transmission::Reliable::csm_configure csm = {
		/*.max_message_size = */CoAP::Transmission::Reliable::default_max_message_size,
		/*.block_wise_transfer = */true
};

using connection = CoAP::Port::POSIX::tcp_client<endpoint>;

using engine_tcp = CoAP::Transmission::Reliable::engine_client<
		connection,			///< (1) TCP client socket definition
		csm,							///< (2) CSM paramenter configuration
		CoAP::Transmission::Reliable::transaction_list<
			CoAP::Transmission::Reliable::transaction<
				connection::handler,
				csm.max_message_size,
				CoAP::Transmission::Reliable::transaction_cb>,	/* (1) transaction type */
			4>,				/* (2) number of transaction */
		CoAP::Transmission::Reliable::default_cb<
			connection::handler
		>,									///< (4) Default callback signature function
		CoAP::disable						///< (5) Resource definition (disabled)
	>;

static volatile bool response_flag = false;

/**
 * Signal handling
 */
void signal_handler(int)
{
	error_message("Aborted");
	response_flag = true;
}

/**
 * This default callback response to non-confirmable messages,
 * async responses or timed
 */
void default_callback(engine_udp::endpoint const& ep,
		CoAP::Message::message const* response,
		void* engine_ptr) noexcept
{
	debug("default cb called");
	CoAP::Debug::print_message_string(*response);

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
	if(CoAP::Transmission::send_async_ack(*static_cast<engine_udp*>(engine_ptr), ep, *response, ec))
	{
		debug("Confirmable response received");
		if(ec) exit_error("send async");
	}

	/**
	 * Setting flag that response was received
	 */
	response_flag = true;
}

/**
 * Request callback (signature defined at transaction)
 */
void request_udp_cb(void const* trans, CoAP::Message::message const* response, void*) noexcept
{
	debug("Request callback called...");

	auto const* t = static_cast<engine_udp::transaction_t const*>(trans);
	status("Status: %s", CoAP::Debug::transaction_status_string(t->status()));
	if(response)
	{
		status("Response received!");
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
		status("Response NOT received");
	}
	/**
	 * Setting flag that response was received
	 */
	response_flag = true;
}

/**
 * Request callback (signature defined at transaction)
 */
void request_tcp_cb(void const* trans, CoAP::Message::Reliable::message const* response, void*) noexcept
{
	debug("Request callback called...");

	auto const* t = static_cast<engine_tcp::transaction_t const*>(trans);
	status("Status: %s", CoAP::Debug::transaction_status_string(t->status()));
	if(response)
	{
		status("Response received!");
		CoAP::Debug::print_message_string(*response);

		/**
		 * Checking if we received a empty acknowledgment. This means that we
		 * are going to receive our response in a separated message, so we can
		 * not go out of the check loop at main.
		 */
//		if(response->mcode == CoAP::Message::code::empty)
//		{
//			return;
//		}
	}
	else
	{
		/**
		 * Function timeout or transaction was cancelled
		 */
		status("Response NOT received");
	}
	/**
	 * Setting flag that response was received
	 */
	response_flag = true;
}

void run_udp(endpoint const& ep,
		uint16_t port,
		CoAP::Transmission::configure tconfig,
		CoAP::Message::type mtype, CoAP::Message::code mcode,
		const void* token, std::size_t token_len,
		CoAP::Message::Option::List& list_op,
		const void* payload, std::size_t payload_len) noexcept
{
	std::printf("Running UDP\n");

	CoAP::Error ec;
	engine_udp::connection conn;

	conn.open(ep.family(), ec);
	if(ec)
	{
		error(ec, "open");
		exit_error("Open error");
	}

	if(port)
	{
		endpoint bind_ep{ep.family(), port};
		conn.bind(bind_ep, ec);
		if(ec)
		{
			error(ec, "bind");
			exit_error("Bind error");
		}
	}

	engine_udp eng(std::move(conn), CoAP::Message::message_id{});

	eng.default_cb(default_callback);

	engine_udp::request req(ep);
	req.header(mtype, mcode, token, token_len)
			.add_option(*list_op.head())
			.payload(payload, payload_len)
			.callback(request_udp_cb);

	auto size = eng.send(req, tconfig, ec);
	if(ec)
	{
		error(ec, "send");
		exit_error("send");
	}

	std::printf("Sended %zu\n", size);

	while(eng.run<50>(ec) && !response_flag){}

	if(ec)
	{
		error(ec, "run");
		exit_error("run");
	}

	eng.get_connection().close();
}

void run_tcp(endpoint& ep,
		uint16_t port,
		CoAP::Message::code mcode,
		const void* token, std::size_t token_len,
		CoAP::Message::Option::List& list_op,
		const void* payload, std::size_t payload_len
		)  noexcept
{
	std::printf("Running TCP\n");

	engine_tcp eng;

	CoAP::Error ec;
	eng.open(ep, ec);
	if(ec)
	{
		error(ec, "open");
		exit_error("Open error");
	}

	if(port)
	{
		endpoint bind_ep{ep.family(), port};
		eng.get_connection().bind(bind_ep, ec);
		if(ec)
		{
			error(ec, "bind");
			exit_error("Bind error");
		}
	}

	engine_tcp::request<> req;
	req.code(mcode)
		.token(token, token_len)
		.add_option(*list_op.head())
		.payload(payload, payload_len)
		.callback(request_tcp_cb);

	auto size = eng.send(req, ec);
	if(ec)
	{
		error(ec, "serialize");
		exit_error("serialize");
	}

	std::printf("Sended %zu\n", size);

	while(eng.run<50>(ec) && !response_flag){}

	if(ec)
	{
		error(ec, "run");
		exit_error("run");
	}

	eng.get_connection().close();
}

int main(int argc, char** argv)
{
	if(argc < 2)
	{
		error_message("Arguments error");
		print_usage(argv[0]);
		return EXIT_FAILURE;
	}

	std::signal(SIGINT, signal_handler);

	char* uri_str = nullptr,
		*token = nullptr,
		*method_str = nullptr,
		*payload_str = nullptr,
		*ack_timeout_str = nullptr,
		*retransmit_str = nullptr,
		*ack_factor_str = nullptr,
		*local_port_str = nullptr,
		*host_str = nullptr;
	std::size_t token_len = 0, payload_len = 0;
	std::uint16_t port = 0;
	CoAP::Message::type mtype = CoAP::Message::type::confirmable;
	CoAP::Message::code mcode = CoAP::Message::code::get;
	CoAP::Message::Option::node	host_op;
	CoAP::Transmission::configure tconfig = {
			/*.ack_timeout_seconds 			= */2.0,	//ACK_TIMEOUT
			/*.ack_random_factor 			= */1.5,	//ACK_RANDOM_FACTOR
			/*.max_restransmission 			= */4,		//MAX_RETRANSMIT
	};

	/**
	 * Start arguments read
	 */

	Command_Line cmd(argc, argv);

	//Set noncofirmable
	if(cmd("N"))
	{
		mtype = CoAP::Message::type::nonconfirmable;
	}

	//Set token
	cmd("T", token);
	if(token) token_len = std::strlen(token);

	if(cmd("c", method_str))
	{
		if(std::strcmp(method_str, "get") == 0)
			mcode = CoAP::Message::code::get;
		else if(std::strcmp(method_str, "post") == 0)
			mcode = CoAP::Message::code::post;
		else if(std::strcmp(method_str, "put") == 0)
			mcode = CoAP::Message::code::put;
		else if(std::strcmp(method_str, "delete") == 0)
			mcode = CoAP::Message::code::cdelete;
		else if(std::strcmp(method_str, "fetch") == 0)
			mcode = CoAP::Message::code::fetch;
		else if(std::strcmp(method_str, "patch") == 0)
			mcode = CoAP::Message::code::patch;
		else if(std::strcmp(method_str, "ipatch") == 0)
			mcode = CoAP::Message::code::ipatch;
		else
			exit_error("Method not found");
	}

	//Set ack timeout
	if(cmd("a", ack_timeout_str))
	{
		char* tail = nullptr;
		tconfig.ack_timeout_seconds = std::strtod(ack_timeout_str, &tail);
		if(*tail != '\0' || tconfig.ack_timeout_seconds <= 0.0)
		{
			exit_error("Ack timeout must be a float value > 0");
		}
		std::printf("Ack timeout: %f\n", tconfig.ack_timeout_seconds);
	}

	//Set retransmit
	if(cmd("R", retransmit_str))
	{
		char* tail = nullptr;
		long retransmit = std::strtol(retransmit_str, &tail, 10);
		if(*tail != '\0' || retransmit < 0)
		{
			exit_error("Retransmit must be a a interger >= 0");
		}
		tconfig.max_restransmission = static_cast<unsigned>(retransmit);
		std::printf("Retransmit set: %u\n", tconfig.max_restransmission);
	}

	//Set ack random factor
	if(cmd("r", ack_factor_str))
	{
		char* tail = nullptr;
		tconfig.ack_random_factor = std::strtod(ack_factor_str, &tail);
		if(*tail != '\0' || tconfig.ack_random_factor < 1.0)
		{
			exit_error("Ack random factor must be a float value > 1.0");
		}
		std::printf("Ack factor: %f\n", tconfig.ack_random_factor);
	}

	if(cmd("h", host_str))
	{
		CoAP::Message::Option::create(host_op.value, CoAP::Message::Option::code::uri_host, host_str);
	}

	if(cmd("p", local_port_str))
	{
		char* tail = nullptr;
		long portv = std::strtol(local_port_str, &tail, 10);
		if((*tail != '\0' || portv < 0) ||
			(portv < 1024 || portv > 65535))
		{
			exit_error("Bind port must be a interger > 1024 and < 65535");
		}
		port = static_cast<uint16_t>(portv);
	}

	if(cmd("f", payload_str))
	{
		payload_len = std::strlen(payload_str);
	}

	//Uri
	uri_str = cmd[0];
	if(!uri_str)
	{
		exit_error("URI not set");
	}

	std::printf("Args: type: %s/token: %s\nack_timeout: %s/retransmit: %s/factor: %s\nURI: %s/uri-host: %s/bind port: %u\npayload: %s\n",
			mtype == CoAP::Message::type::confirmable ? "CON" : "NON",
			token ? token : "null", ack_timeout_str ? ack_timeout_str : "null",
			retransmit_str ? retransmit_str : "null", ack_factor_str ? ack_factor_str : "null",
			uri_str,
			host_op.value ? static_cast<const char*>(host_op.value.value) : "<no-host>",
			port,
			payload_str ? payload_str : "<none>");
	/**
	 * End arguments read
	 */

	/**
	 * 'uri<ip_type>' support both IPv4 and IPv6 address. It you only
	 * desire one kind, function 'decompose' is also defined to:
	 * * 'uri<in_addr>': only IPv4;
	 * * 'uri<in6_addr>': only IPv6;
	 *
	 * * 'struct uri' is defined at 'uri/types.hpp'.
	 *
	 * The template parameter of function decompose (true by default) is to accept
	 * reliable connections, as defined at RFC8323. If you don't want to accept
	 * this kind scheme, set it to false (COAP_TE_RELIABLE_CONNECTION macro must
	 * also be == 1, the default)
	 */
	CoAP::URI::uri<CoAP::URI::ip_type> uri;
	if(!CoAP::URI::decompose<true>(uri, uri_str))
	{
		exit_error("Invalid URI");
	}

	/**
	 * As function 'decompose', function 'print_uri_decomposed'
	 * is defined to 'uri<ip_type>', 'uri<in_addr>', and 'uri<in6_addr>'.
	 */
	CoAP::Debug::print_uri_decomposed(uri);

	/**
	 * Decomposing path and querying
	 */
	CoAP::Message::Option::List list;		//List to add all options uri_path and uri_query

	/**
	 * Why do we need a buffer? As we don't know how many paths/query
	 * options we are going to need, and also need to percent-decode all data,
	 * we need 'some place' to put this information.
	 *
	 * Other problem is that we are NOT using any kind of dynamic memory allocation.
	 * So all this info will be recorded at the buffer. The buffer size IS a limitation...
	 */
	std::uint8_t buffer[BUFFER_LEN];
	std::size_t buffer_len = BUFFER_LEN;

	/**
	 * This function decompose both uri_path and uri_query. It adds it to the
	 * option list
	 */
	if(!CoAP::URI::decompose_to_list(buffer, buffer_len, uri, list))
	{
		exit_error("Invalid URI");
	}

	if(host_op.value) list.add(host_op);

	printf("\nPrinting options...\n");
	CoAP::Debug::print_options(list.head());

	CoAP::init();

	CoAP::Port::POSIX::endpoint_ip ep;
	switch(uri.host.type)
	{
		using namespace CoAP::URI;
		case host_type::ipv4:
			ep.set(uri.host.host.ip4.s_addr, uri.port);
			break;
		case host_type::ipv6:
			ep.set(uri.host.host.ip6, uri.port);
			break;
		default:
			exit_error("Invalid host type");
			break;
	}

	switch(uri.uri_scheme)
	{
		using namespace CoAP::URI;
		case scheme::coap:
			run_udp(ep, port, tconfig, mtype, mcode, token, token_len, list, payload_str, payload_len);
			break;
		case scheme::coap_tcp:
			run_tcp(ep, port, mcode, token, token_len, list, payload_str, payload_len);
			break;
		case scheme::coaps:
		case scheme::coaps_tcp:
		case scheme::coap_ws:
		case scheme::coaps_ws:
			exit_error("Scheme not supported!");
			break;
		default:
			exit_error("Scheme not defined!");
			break;
	}

	return 0;
}
