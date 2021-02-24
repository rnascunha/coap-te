#include <iostream>
#include <cstdint>
#include <ctime>

#include "coap.te.hpp"

#define BUFFER_LEN		1000
#define USE_INTERNAL_BUFFER
#define USE_INTERNAL_MESSAGE_ID

using namespace CoAP::Message;

int main()
{
#ifndef USE_INTERNAL_BUFFER
	std::uint8_t buffer[BUFFER_LEN];
#endif /* USE_INTERNAL_BUFFER */
	CoAP::Error ec;

	std::uint8_t token[] = {0x03, 0x04, 0x05, 0x06, 0x07};
	message_id mid(time(NULL));

	unsigned content_format = static_cast<unsigned>(content_format::application_json);
	const char* path = "mypath";
	const char* path2 = "1234";
	const char* payload = "TESTE";

	std::cout << "Testing factory...\n";

#ifdef USE_INTERNAL_BUFFER
#	ifdef USE_INTERNAL_MESSAGE_ID
	std::cout << "Internal buffer / Internal message id\n";
	Factory<BUFFER_LEN, message_id> fac(mid);
#	else /* USE_INTERNAL_MESSAGE_ID */
	std::cout << "Internal buffer / NO internal message id\n";
	Factory<BUFFER_LEN> fac;
#	endif /* USE_INTERNAL_MESSAGE_ID */
#else /* USE_INTERNAL_BUFFER */
#	ifdef USE_INTERNAL_MESSAGE_ID
	std::cout << "NO internal buffer / Internal message id\n";
	Factory<0, message_id> fac(mid);
#	else /* USE_INTERNAL_MESSAGE_ID */
	std::cout << "NO internal buffer / NO internal message id\n";
	Factory<0> fac;
#	endif /* USE_INTERNAL_MESSAGE_ID */
#endif /* USE_INTERNAL_BUFFER */
	std::cout << "Size factory: " << sizeof(fac) << "\n";

	fac.header(type::confirmable, code::get);
	fac.token(token, sizeof(token));

	option_node first{option_code::content_format, content_format};
	option_node second{option_code::uri_path, path};
	option_node last{option_code::uri_path, path2};

	fac.add_option(first);
	fac.add_option(second);
	fac.add_option(last);

	fac.payload(payload);

#ifdef USE_INTERNAL_BUFFER
#	ifdef USE_INTERNAL_MESSAGE_ID
	std::size_t size = fac.serialize(ec);
#	else /* USE_INTERNAL_MESSAGE_ID */
	std::size_t size = fac.serialize(mid(), ec);
#	endif /* USE_INTERNAL_MESSAGE_ID */
#else /* USE_INTERNAL_BUFFER */
#	ifdef USE_INTERNAL_MESSAGE_ID
	std::size_t size = fac.serialize(buffer, BUFFER_LEN, ec);
#	else /* USE_INTERNAL_MESSAGE_ID */
	std::size_t size = fac.serialize(buffer, BUFFER_LEN, mid(), ec);
#	endif /* USE_INTERNAL_MESSAGE_ID */
#endif /* USE_INTERNAL_BUFFER */

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
	 * Checking serialize
	 */
	if(ec)
	{
		std::cerr << "ERROR! [" << ec.value() << "] " << ec.message() << "\n";
		return EXIT_FAILURE;
	}

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
