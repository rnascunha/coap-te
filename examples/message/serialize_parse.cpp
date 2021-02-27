#include <iostream>
#include <cstdint>
#include <ctime>

#include "../coap-te.hpp"

#define BUFFER_LEN		1000

using namespace CoAP::Message;

#define TEST_FACTORY
#define TEST_LIST
#define SHOW_ITERATE_OPTION

int main()
{
	std::uint8_t buffer[BUFFER_LEN];
	CoAP::Error ec;

	std::uint8_t token[] = {0x03, 0x04, 0x05, 0x06, 0x07};
	message_id mid(time(NULL));

	unsigned content_format = static_cast<unsigned>(content_format::application_json);
	const char* path = "mypath";
	const char* path2 = "1234";
	const char* payload = "TESTE";

#if !defined(TEST_LIST) && !defined(TEST_FACTORY)
	std::cout << "Testing options array...\n";

	option options[] = {
			{option_code::content_format, content_format},
			{option_code::uri_path, path},
			{option_code::uri_path, path2}
	};

	std::size_t size = serialize(buffer, BUFFER_LEN,
			type::confirmable, code::get, mid(),
			token, sizeof(token),
			options, sizeof(options) / sizeof(option),
			payload, std::strlen(payload),
			ec);
#elif defined(TEST_FACTORY)
	std::cout << "Testing factory...\n";

	Factory<> fac;

	fac.header(type::confirmable, code::get);
	fac.token(token, sizeof(token));

	option_node first{option_code::content_format, content_format};
	option_node second{option_code::uri_path, path};
	option_node last{option_code::uri_path, path2};

	fac.add_option(first);
	fac.add_option(second);
	fac.add_option(last);

	fac.payload(payload);

	std::size_t size = fac.serialize(buffer, BUFFER_LEN, mid(), ec);

	/**
	 * After serialize, you can use the factory again, but first you need to call reset
	 *
	 * fac.reset();
	 * .
	 * .
	 * .
	 */
#else
	std::cout << "Testing options list...\n";

	Option_List list;

	option_node first{option_code::content_format, content_format};
	option_node second{option_code::uri_path, path};
	option_node last{option_code::uri_path, path2};

	list.add(first);
	list.add(second);
	list.add(last);

	std::size_t size = serialize(buffer, BUFFER_LEN,
			type::confirmable, code::get, mid(),
			token, sizeof(token),
			list,
			payload, std::strlen(payload),
			ec);
#endif /* TEST_LIST */
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
	CoAP::Debug::print_byte_message(buffer, size);

	std::cout << "-------------\nParsing message...\n";
	message msg;
	auto size_parse = parse(msg, buffer, size, ec);
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

#ifdef SHOW_ITERATE_OPTION
	/**
	 * Showing how to iterate through options
	 */
	std::cout << "\n--------------\nIterate options...\n\n";
	Option_Parser op(msg);
	option const* opt;
	while((opt = op.next()))
	{
		CoAP::Debug::print_option(*opt);
		std::cout << "\n";
	}
#endif /* SHOW_ITERATE_OPTION */

	return EXIT_SUCCESS;
}
