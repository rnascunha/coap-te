#include <cstdlib>
#include <cstdio>
#include <cstdint>

#include "error.hpp"
#include "port/port.hpp"

using namespace CoAP;

static void exit_error(Error& ec, const char* what = "")
{
	printf("ERROR! [%d] %s [%s]", ec.value(), ec.message(), what);
	exit(EXIT_FAILURE);
}

#define BUFFER_LEN		1000

#define SOCKET_SERVER

#ifdef SOCKET_SERVER
int main()
{
	Error ec;
	std::uint8_t buffer[BUFFER_LEN];

	endpoint ep{INADDR_ANY, 8080};

	connection conn{Port::Linux::socket{}};

	conn.native_handler().open(ec);
	if(ec) exit_error(ec, "open");

	conn.native_handler().bind(ep, ec);
	if(ec) exit_error(ec, "bind");

	char addr_str[20];
	std::printf("Listening: %s:%u\n", ep.address(addr_str), ep.port());
	while(true)
	{
		endpoint recv_addr;
		std::size_t size = conn.receive(buffer, BUFFER_LEN, recv_addr, ec);
		if(ec) if(ec) exit_error(ec, "read");
		buffer[size] = '\0';

		char addr_str[20];
		std::printf("Received %s:%u [%lu]: %s\n", recv_addr.address(addr_str), recv_addr.port(), size, buffer);
		std::printf("Echoing...\n");
		conn.send(buffer, size, recv_addr, ec);
		if(ec) if(ec) exit_error(ec, "write");
	}

	return EXIT_SUCCESS;
}
#else /* SOCKET_SERVER */

int main()
{
	Error ec;
	std::uint8_t buffer[BUFFER_LEN];

	const char* payload = "Teste";
	std::size_t payload_len = std::strlen(payload);

	std::memcpy(buffer, payload, payload_len + 1);

	endpoint to{"127.0.0.1", 8080, ec};
	connection conn{Port::Linux::socket{}};

	conn.native_handler().open(ec);
	if(ec) exit_error(ec, "open");

	conn.send(buffer, payload_len, to, ec);
	if(ec) exit_error(ec, "send");
	printf("Send succeced!\n");

	endpoint from;
	std::size_t size = conn.receive(buffer, BUFFER_LEN, from, ec);
	if(ec) exit_error(ec, "read");

	char addr_str[20];
	std::printf("Received %s:%u [%lu]: %s\n", from.address(addr_str), from.port(), size, buffer);

	return EXIT_SUCCESS;
}

#endif /* SOCKET_SERVER */

//#include <iostream>
//#include <cstdint>
//#include <cstring>
//#include <ctime>
//
//#include "coap-te.hpp"
//
//#include "internal/helper.hpp"
//#include "internal/ascii.hpp"
//
//#define BUFFER_LEN		1000
//
//using namespace CoAP::Message;
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
//	std::cout << "Testing URI...\n";
//	Factory<BUFFER_LEN, message_id> fac(message_id(std::time(NULL)));
//
//	fac.header(type::confirmable, code::get)
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
//		std::cerr << "ERROR! [" << ec.value() << "] " << ec.message() << "\n";
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
//	std::cout << "------------\n";
//
//	/**
//	 * Serialized success
//	 */
//	std::cout << "Serialize succeded! [" << size << "]...";
//	std::cout << "\n-----------\nPrinting message bytes...\n";
//	CoAP::Debug::print_byte_message(fac.buffer(), size);
//
//	std::cout << "-------------\nParsing message...\n";
//	message msg;
//	auto size_parse = parse(msg, fac.buffer(), size, ec);
//	/**
//	 * Checking parse
//	 */
//	if(ec)
//	{
//		std::cerr << "ERROR! [" << ec.value() << "] " << ec.message() << "\n";
//		return EXIT_FAILURE;
//	}
//	/**
//	 * Parsed succeded!
//	 */
//	std::cout << "Parsing succeded! [" << size_parse << "]...\n";
//	std::cout << "Printing message...\n";
//	CoAP::Debug::print_message(msg);
//
//	std::cout << "\n-----------\nComposing URI...\n";
//	char buffer_uri[500];
//	Option_Parser opt_parser(msg);
//	auto size_uri = CoAP::URI::compose(buffer_uri, 500, opt_parser, ec);
//	if(ec)
//	{
//		std::cerr << "ERROR! [" << ec.value() << "] " << ec.message() << "\n";
//		return EXIT_FAILURE;
//	}
//	std::printf("URI[%lu]: %.*s\n", size_uri, static_cast<int>(size_uri), buffer_uri);
//
//	return EXIT_SUCCESS;
//}
