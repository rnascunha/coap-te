#include <ctime>

#include "log.hpp"
#include "transmission/engine.hpp"
#include "port/linux/port.hpp"
#include "port/port.hpp"
#include "message/message_id.hpp"

#include <chrono>
#include <thread>

using namespace CoAP;
using namespace CoAP::Log;
using namespace CoAP::Transmission;

#define COAP_PORT		5683
#define COAP_SLEEP_MS	100

static constexpr module main_mod = {
		.name = "MAIN",
		.max_level = CoAP::Log::type::debug
};

static constexpr const configure tconfigure = {
	.ack_timeout_seconds 			= 2,	//ACK_TIMEOUT
	.ack_ramdom_factor 				= 1.5,	//ACK_RANDOM_FACTOR
	.max_restransmission 			= 4,	//MAX_RETRANSMIT
	.max_interaction 				= 1,	//NSTART
	.default_leisure_seconds 		= 5,	//DEFAULT_LEISURE
	.probing_rate_byte_per_seconds 	= 1,	//PROBING_RATE
	//Implementation
	.max_packet_size = 1024
};

int main()
{
	debug(main_mod, "Init engine code...");
	char print_buffer[20];

	Error ec;
	CoAP::socket socket;

	socket.open(ec);
	if(ec)
	{
		error(main_mod, "Error trying to open socket...");
		exit(EXIT_FAILURE);
	}
	debug(main_mod, "Socket opened...");

	endpoint ep{INADDR_ANY, COAP_PORT};
	socket.bind(ep, ec);
	if(ec)
	{
		error(main_mod, "Error trying to bind socket");
		exit(EXIT_FAILURE);
	}
	debug(main_mod, "Socket binded to %s:%u", ep.host(print_buffer), ep.port());

	debug(main_mod, "Starting CoAP engine...");
	engine<connection, tconfigure, Message::message_id>
		coap_run(
			connection{std::move(socket)},
			Message::message_id{static_cast<unsigned>(CoAP::time())});

	while(true)
	{
		if(!coap_run(ec))
		{
			error(main_mod, ec, "reading socket");
			exit(EXIT_FAILURE);
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(COAP_SLEEP_MS));
	}
}

//#include <cstdint>
//#include <cstring>
//#include <ctime>
//
//#include "coap-te.hpp"
//
//#include "log.hpp"
//
//#define BUFFER_LEN		1000
//
//using namespace CoAP::Message;
//using namespace CoAP::Log;
//
//static constexpr module main_mod = {
//		.name = "MAIN",
//		.max_level = CoAP::Log::type::status
//};
//
//int main()
//{
//	CoAP::Error ec;
//
//	/**
//	 * All object lifetime if of user responsability. Util "serialize" method
//	 * been called, nothing is copied.
//	 */
//	std::uint8_t token[] = {0x03, 0x04, 0x05, 0x06, 0x07};
//
//	content_format format{content_format::application_json};
//	unsigned port = 123;
//
//	const char* payload = "TESTE";
//
//	Option::node content_op{format};
//	Option::node host_op{Option::code::uri_host, "My_HoSt"};
//	Option::node port_op{Option::code::uri_port, port};
//	Option::node query_op1{Option::code::uri_query, "myque?ry=1"};
//	Option::node query_op2{Option::code::uri_query, "myque ry2=2"};
//	Option::node query_op3{Option::code::uri_query, "myqu&ery3=3"};
//	Option::node path_op1{Option::code::uri_path, "myp ath"};
//	Option::node path_op2{Option::code::uri_path, "12?34"};
//
//	status("Testing URI...");
//	Factory<BUFFER_LEN, message_id> fac(message_id(std::time(NULL)));
//
//	fac.header(CoAP::Message::type::confirmable, code::get)
//		.token(token, sizeof(token))
//		.add_option(content_op)
//		.add_option(path_op1)
//		.add_option(query_op2)
//		.add_option(port_op)
//		.add_option(host_op)
//		.add_option(path_op2)
//		.add_option(query_op1)
//		.add_option(query_op3)
//		.payload(payload);
//
//	std::size_t size = fac.serialize(ec);
//	/**
//	 * Checking serialize
//	 */
//	if(ec)
//	{
//		error(main_mod, "ERROR! [%d] %s", ec.value(), ec.message());
//		return EXIT_FAILURE;
//	}
//	/**
//	 * After serialize, you can use the factory again, but first you need to call reset
//	 *
//	 * fac.reset();
//	 * .
//	 * .
//	 * .
//	 */
//
//	status("------------\n");
//
//	/**
//	 * Serialized success
//	 */
//	status(main_mod, "Serialize succeded! [%lu]...", size);
//	status(main_mod, "Printing message bytes...");
//	status("-----------");
//	CoAP::Debug::print_byte_message(fac.buffer(), size);
//
//	status("-------------");
//	status(main_mod, "Parsing message...");
//	message msg;
//	auto size_parse = parse(msg, fac.buffer(), size, ec);
//	/**
//	 * Checking parse
//	 */
//	if(ec)
//	{
//		error(main_mod, "ERROR! [%d] %s", ec.value(), ec.message());
//		return EXIT_FAILURE;
//	}
//	/**
//	 * Parsed succeded!
//	 */
//	status(main_mod, "Parsing succeded! [%lu]...\n", size_parse);
//	status(main_mod, "Printing message...");
//	CoAP::Debug::print_message(msg);
//
//	status("\n-----------");
//	status(main_mod, "Composing URI...\n");
//	char buffer_uri[500];
//	Option_Parser opt_parser(msg);
//	auto size_uri = CoAP::URI::compose(buffer_uri, 500, opt_parser, ec);
//	if(ec)
//	{
//		error(main_mod, "ERROR! [%d] %s", ec.value(), ec.message());
//		return EXIT_FAILURE;
//	}
//	status(main_mod, "URI[%lu]: %.*s\n", size_uri, static_cast<int>(size_uri), buffer_uri);
//
//	return EXIT_SUCCESS;
//}
