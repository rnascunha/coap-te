#include <ctime>

#include "log.hpp"
#include "transmission/types.hpp"
#include "transmission/transaction.hpp"
#include "port/linux/port.hpp"
#include "port/port.hpp"
#include "message/message_id.hpp"
#include "coap-te.hpp"

using namespace CoAP;
using namespace CoAP::Log;
using namespace CoAP::Message;
using namespace CoAP::Transmission;

#define COAP_PORT		5683
#define COAP_SLEEP_MS	100
#define BUFFER_LEN		1024
#define HOST_ADDR		"127.0.0.1"

#define USE_INTERNAL_BUFFER

static constexpr module main_mod = {
		.name = "MAIN",
		.max_level = CoAP::Log::type::debug
};

static constexpr const configure tconfigure = {
	.ack_timeout_seconds 			= 2.0,	//ACK_TIMEOUT
	.ack_ramdom_factor 				= 1.5,	//ACK_RANDOM_FACTOR
	.max_restransmission 			= 4,	//MAX_RETRANSMIT
	.max_interaction 				= 1,	//NSTART
	.default_leisure_seconds 		= 5,	//DEFAULT_LEISURE
	.probing_rate_byte_per_seconds 	= 1,	//PROBING_RATE
};

static bool response_flag = false;

void exit_error(CoAP::Error& ec, const char* what = nullptr)
{
	error(main_mod, ec, what);
	exit(EXIT_FAILURE);
}

void cb(void const* trans, CoAP::Message::message const* r, void*)
{
#ifndef USE_INTERNAL_BUFFER
	transaction<> const* t = static_cast<transaction<> const*>(trans);
#else
	transaction<BUFFER_LEN> const* t = static_cast<transaction<BUFFER_LEN> const*>(trans);
#endif
	status(main_mod, "Status: %s", CoAP::Debug::transaction_status_string(t->status()));
	if(r)
	{
		status(main_mod, "Response received!");
		CoAP::Debug::print_message(*r);
	}
	else
	{
		status(main_mod, "Response not received");
	}
	response_flag = true;
}

int main()
{
	debug(main_mod, "Init transaction code...");

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

	/**
	 * Creating object
	 * All object lifetime if of user responsability. Until "serialize" method
	 * been called, nothing is copied.
	 */
//	Option::node path_op{Option::code::uri_path, "time"};
	Option::node path_op1{Option::code::uri_path, ".well-known"};
	Option::node path_op2{Option::code::uri_path, "core"};

	status(main_mod, "Testing URI...");
	Factory<0, message_id> fac(message_id(CoAP::time()));

	fac.header(CoAP::Message::type::confirmable, code::get)
		.add_option(path_op1)
		.add_option(path_op2);

#ifndef USE_INTERNAL_BUFFER
	debug(main_mod, "Using transaction external buffer...");
	std::uint8_t buffer_send[BUFFER_LEN];
	transaction<> tr;

	std::size_t size = fac.serialize(buffer_send, BUFFER_LEN, ec);
	if(ec) exit_error(ec, "serialize");

	debug(main_mod, "Message to send");
	CoAP::Debug::print_byte_message(buffer_send, size);

	debug(main_mod, "Sending data... [%d]", size);
	conn.send(buffer_send, size, ep, ec);
	if(ec) exit_error(ec, "send");

	tr.init(tconfigure, ep, buffer_send, size, cb, nullptr, ec);
	if(ec) exit_error(ec, "init transaction");
#else
	debug(main_mod, "Using transaction internal buffer...");
	transaction<BUFFER_LEN> tr;
	tr.serialize(fac, ec);
	if(ec) exit_error(ec, "serialize");

	debug(main_mod, "Message to send [%d]",tr.buffer_used());
	CoAP::Debug::print_byte_message(tr.buffer(), tr.buffer_used());

	debug(main_mod, "Sending data... [%d]", tr.buffer_used());
	conn.send(tr.buffer(), tr.buffer_used(), ep, ec);
	if(ec) exit_error(ec, "send");

	tr.init(tconfigure, ep, cb, nullptr, ec);
	if(ec) exit_error(ec, "init transaction");
#endif

	/**
	 * Receiving
	 */
	std::uint8_t buffer_recv[BUFFER_LEN];
	char print_buffer[20];

	endpoint ep_recv;
	CoAP::Message::message response;
	std::size_t size_recv = 0;
	while(!response_flag)
	{
		size_recv = conn.receive(buffer_recv, BUFFER_LEN, ep_recv, ec);
		if(ec) exit_error(ec, "read");

		if(size_recv)
		{
			status(main_mod, "From: %s:%u", ep_recv.host(print_buffer), ep_recv.port());
			CoAP::Message::parse(response, buffer_recv, size_recv, ec);
			if(ec) exit_error(ec, "parse");

			if(tr.check_response(response))
				status(main_mod, "Response received correctly");
		}
		else if(tr.check(tconfigure))
		{
			status(main_mod, "Message timeout");
#ifndef USE_INTERNAL_BUFFER
			conn.send(buffer_send, size, ep, ec);
			if(ec) exit_error(ec, "retransmit");
#else
			conn.send(tr.buffer(), tr.buffer_used(), ep, ec);
			if(ec) exit_error(ec, "retransmit");
			tr.retransmit();
#endif
		}
	}
	return EXIT_SUCCESS;
}
