#include <iostream>
#include <cstdint>
#include <ctime>

#include "../coap-te.hpp"

#define BUFFER_LEN		1000
#define USE_INTERNAL_BUFFER
#define USE_INTERNAL_MESSAGE_ID

using namespace CoAP::Message;

int main()
{
#ifndef USE_INTERNAL_BUFFER
	std::uint8_t buffer[BUFFER_LEN];
#endif /* USE_INTERNAL_BUFFER */
#ifndef USE_INTERNAL_MESSAGE_ID
	message_id mid(std::time(NULL));
#endif /* USE_INTERNAL_MESSAGE_ID */
	CoAP::Error ec;

	/**
	 * All object lifetime must be is of user responsability. Util "serialize" method
	 * been called, nothing is copied.
	 */
	std::uint8_t token[] = {0x03, 0x04, 0x05, 0x06, 0x07};

	content_format format{content_format::application_json};
	const char* path = "mypath";
	const char* path2 = "1234";

	const char* payload = "TESTE";

	option_node content_op{format};
	option_node path_op1{option_code::uri_path, path};
	option_node path_op2{option_code::uri_path, path2};

	std::cout << "Testing factory...\n";

#if defined(USE_INTERNAL_BUFFER) && defined(USE_INTERNAL_MESSAGE_ID)
	std::cout << "Internal buffer / Internal message id\n";
	Factory<BUFFER_LEN, message_id> fac(message_id(std::time(NULL)));
#elif defined(USE_INTERNAL_BUFFER) && !defined(USE_INTERNAL_MESSAGE_ID)
	std::cout << "Internal buffer / NO internal message id\n";
	Factory<BUFFER_LEN> fac;
#elif !defined(USE_INTERNAL_BUFFER) && defined(USE_INTERNAL_MESSAGE_ID)
	std::cout << "NO internal buffer / Internal message id\n";
	Factory<0, message_id> fac(message_id(std::time(NULL)));
#else
	std::cout << "NO internal buffer / NO internal message id\n";
	Factory<0> fac;
#endif
	std::cout << "Size factory: " << sizeof(fac) << "\n";

	fac.header(type::confirmable, code::get)
		.token(token, sizeof(token))
		.add_option(content_op)
		.add_option(path_op1)
		.add_option(path_op2)
		.payload(payload);

#if defined(USE_INTERNAL_BUFFER) && defined(USE_INTERNAL_MESSAGE_ID)
	std::size_t size = fac.serialize(ec);
#elif defined(USE_INTERNAL_BUFFER) && !defined(USE_INTERNAL_MESSAGE_ID)
	std::size_t size = fac.serialize(mid(), ec);
#elif !defined(USE_INTERNAL_BUFFER) && defined(USE_INTERNAL_MESSAGE_ID)
	std::size_t size = fac.serialize(buffer, BUFFER_LEN, ec);
#else
	std::size_t size = fac.serialize(buffer, BUFFER_LEN, mid(), ec);
#endif
	/**
	 * Checking serialize
	 */
	if(ec)
	{
		std::cerr << "ERROR! [" << ec.value() << "] " << ec.message() << "\n";
		return EXIT_FAILURE;
	}
	/**
	 * After serialize, you can use the factory again, but first you need to call reset
	 *
	 * fac.reset();
	 * .
	 * .
	 * .
	 */

	std::cout << "------------\n";

	/**
	 * Serialized success
	 */
	std::cout << "Serialize succeded! [" << size << "]...";
	std::cout << "\nPrinting message bytes...\n";
#ifndef USE_INTERNAL_BUFFER
	CoAP::Debug::print_byte_message(buffer, size);
#else /* USE_INTERNAL_BUFFER */
	CoAP::Debug::print_byte_message(fac.buffer(), size);
#endif

	std::cout << "-------------\nParsing message...\n";
	message msg;
#ifndef USE_INTERNAL_BUFFER
	auto size_parse = parse(msg, buffer, size, ec);
#else /* USE_INTERNAL_BUFFER */
	auto size_parse = parse(msg, fac.buffer(), size, ec);
#endif /* USE_INTERNAL_BUFFER */
	/**
	 * Checking parse
	 */
	if(ec)
	{
		std::cerr << "ERROR! [" << ec.value() << "] " << ec.message() << "\n";
		return EXIT_FAILURE;
	}
	/**
	 * Parsed succeded!
	 */
	std::cout << "Parsing succeded! [" << size_parse << "]...\n";
	std::cout << "Printing message...\n";
	CoAP::Debug::print_message(msg);

	return EXIT_SUCCESS;
}
