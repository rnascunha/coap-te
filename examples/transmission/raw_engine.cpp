#include <ctime>

#include "log.hpp"
#include "transmission/types.hpp"
#include "transmission/transaction.hpp"
#include "transmission/request.hpp"
#include "transmission/engine.hpp"
#include "port/linux/port.hpp"
#include "port/port.hpp"
#include "message/message_id.hpp"
#include "coap-te.hpp"

using namespace CoAP;
using namespace CoAP::Log;
using namespace CoAP::Message;

#define COAP_PORT		5683
#define TRANSACT_NUM	4
#define BUFFER_LEN		512
#define HOST_ADDR		"127.0.0.1"

static constexpr module main_mod = {
		.name = "MAIN",
		.max_level = CoAP::Log::type::debug
};

static constexpr const CoAP::Transmission::configure tconfigure = {
	.ack_timeout_seconds 			= 2.0,	//ACK_TIMEOUT
	.ack_ramdom_factor 				= 1.5,	//ACK_RANDOM_FACTOR
	.max_restransmission 			= 4,	//MAX_RETRANSMIT
//	.max_interaction 				= 1,	//NSTART
//	.default_leisure_seconds 		= 5,	//DEFAULT_LEISURE
//	.probing_rate_byte_per_seconds 	= 1,	//PROBING_RATE
};

static bool response_flag = false;

void exit_error(CoAP::Error& ec, const char* what = nullptr)
{
	error(main_mod, ec, what);
	exit(EXIT_FAILURE);
}

using engine = CoAP::Transmission::engine<CoAP::connection, TRANSACT_NUM, BUFFER_LEN>;

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
	response_flag = true;
}

int main()
{
	debug(main_mod, "Init engine code...");

	Error ec;

	/**
	 * Socket
	 */
	CoAP::socket socket;

	socket.open(ec);
	if(ec) exit_error(ec, "Error trying to open socket...");

	debug(main_mod, "Socket opened...");

	endpoint ep{HOST_ADDR, COAP_PORT, ec};
	if(ec) exit_error(ec);

	CoAP::connection conn{std::move(socket)};

	CoAP::Message::Option::node path_op1{CoAP::Message::Option::code::uri_path, ".well-known"};
	CoAP::Message::Option::node path_op2{CoAP::Message::Option::code::uri_path, "core"};

	CoAP::Message::message_id mid(CoAP::time());

	status(main_mod, "Testing Engine...");

	engine::request request(ep);
	request.header(CoAP::Message::type::confirmable, CoAP::Message::code::get)
			.token("tok")
			.add_option(path_op1)
			.add_option(path_op2)
			.callback(cb);

	engine coap_engine(std::move(conn));

	coap_engine.send<true>(request, mid(), ec);
	if(ec) exit_error(ec, "send");

	while(!response_flag)
	{
		coap_engine(ec);
		if(ec) exit_error(ec, "run");
	}

	return EXIT_SUCCESS;
}
