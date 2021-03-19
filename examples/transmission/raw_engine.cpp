/**
 * This is a example on how to define a CoAP::engine. The engine
 * is where all the CoAP-te features are aggregate. All the transaction
 * complexity, resources management and others are hidden from the user.
 *
 * Thinking that different environments have different needs, it was develop
 * a configurable engine. More about how to configure it to your needs below.
 */

#include "log.hpp"
#include "coap-te.hpp"
#include "coap-te-debug.hpp"

#define USE_TRANSACTION_LIST_VECTOR

#ifdef USE_TRANSACTION_LIST_VECTOR
#include "transmission/transaction_list_vector.hpp"
#endif /* USE_TRANSACTION_LIST_VECTOR */

using namespace CoAP;
using namespace CoAP::Log;
using namespace CoAP::Message;

#define COAP_PORT		5683
#define TRANSACT_NUM	4
#define BUFFER_LEN		512
#define HOST_ADDR		"127.0.0.1"

static constexpr module example_mod = {
		.name = "MAIN",
		.max_level = CoAP::Log::type::debug
};

static constexpr const CoAP::Transmission::configure tconfigure = {
	.ack_timeout_seconds 			= 2.0,	//ACK_TIMEOUT
	.ack_ramdom_factor 				= 1.5,	//ACK_RANDOM_FACTOR
	.max_restransmission 			= 4,	//MAX_RETRANSMIT
};

static bool response_flag = false;

void exit_error(CoAP::Error& ec, const char* what = nullptr)
{
	error(example_mod, ec, what);
	exit(EXIT_FAILURE);
}

using transaction_t = CoAP::Transmission::transaction<
		512,
		CoAP::Transmission::transaction_cb,
		CoAP::socket::endpoint>;

#ifdef USE_TRANSACTION_LIST_VECTOR
using transaction_list_t =
		CoAP::Transmission::transaction_list_vector<transaction_t>;
#else /* USE_TRANSACTION_LIST_VECTOR */
using transaction_list_t =
		CoAP::Transmission::transaction_list<
			transaction_t,
			TRANSACT_NUM>,
#endif /* USE_TRANSACTION_LIST_VECTOR */

using engine = CoAP::Transmission::engine<
		CoAP::Transmission::profile::client,
		CoAP::socket,
		CoAP::Message::message_id,
		transaction_list_t,
		void*,		//default callback disabled
		CoAP::Resource::callback<CoAP::socket::endpoint>
	>;

void request_cb(void const* trans, CoAP::Message::message const* response, void*)
{
	auto const* t = static_cast<engine::transaction_t const*>(trans);
	status(example_mod, "Status: %s", CoAP::Debug::transaction_status_string(t->status()));
	if(response)
	{
		status(example_mod, "Response received!");
		CoAP::Debug::print_message(*response);
	}
	else
	{
		status(example_mod, "Response NOT received");
	}
	response_flag = true;
}

int main()
{
	debug(example_mod, "Init engine code...");

	Error ec;

	/**
	 * Socket
	 */
	CoAP::socket socket;

	socket.open(ec);
	if(ec) exit_error(ec, "Error trying to open socket...");

	debug(example_mod, "Socket opened...");

	CoAP::socket::endpoint ep{HOST_ADDR, COAP_PORT, ec};
	if(ec) exit_error(ec);

	CoAP::socket conn;

	CoAP::Message::Option::node path_op1{CoAP::Message::Option::code::uri_path, ".well-known"};
	CoAP::Message::Option::node path_op2{CoAP::Message::Option::code::uri_path, "core"};

	status(example_mod, "Testing Engine...");

	engine::request request(ep);
	request.header(CoAP::Message::type::confirmable, CoAP::Message::code::get)
			.token("token")
			.add_option(path_op1)
			.add_option(path_op2)
			.callback(request_cb);

	engine coap_engine(std::move(socket),
			CoAP::Message::message_id(CoAP::time()),
			tconfigure);

	coap_engine.send(request, ec);
	if(ec) exit_error(ec, "send");

	while(!response_flag)
	{
		coap_engine(ec);
		if(ec) exit_error(ec, "run");
	}

	return EXIT_SUCCESS;
}
