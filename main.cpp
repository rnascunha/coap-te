#include "message.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstdint>

#define BUFFER_LEN		1000

using namespace CoAP::Message;

#define TEST_LIST

void print_array(std::uint8_t const* array, std::size_t size)
{
	for(std::size_t i = 0; i < size; i++)
		printf("%02X ", array[i]);
}

int main()
{
	std::uint8_t buffer[BUFFER_LEN];
	CoAP::Error ec;

//	std::uint8_t token[] = {0x03, 0x04, 0x05, 0x06, 0x07};
	std::uint8_t content_format = static_cast<std::uint8_t>(content_format::application_json);
	const char* path = "mypath";
	const char* path2 = "1234";

#ifndef TEST_LIST
	option_t options[] = {
			{option_code::content_format, sizeof(std::uint8_t), &content_format},
			{option_code::uri_path, 6, path},
			{option_code::uri_path, 4, path2}
	};

	std::size_t size = serialize(buffer, BUFFER_LEN,
			type::confirmable, code::get, 0x0102,
//			token, sizeof(token),
			nullptr, 0,
			options, 3,
			"TESTE", 5,
//			nullptr, 0,
			ec);
#else
	Option_List list;

//	option_list first = {
//		.value{option_code::content_format, sizeof(std::uint8_t), &content_format},
//		.next = nullptr
//	};
//	option_list second{
//		.value{option_code::uri_path, 6, path},
//		.next = nullptr
//	};
//	option_list last = {
//		.value{option_code::uri_path, 4, path2},
//		.next = nullptr
//	};
	option_node first{option_code::content_format, static_cast<unsigned>(sizeof(std::uint8_t)), reinterpret_cast<const void*>(&content_format)};
	option_node second{option_code::uri_path, 6, path};
	option_node last{option_code::uri_path, 4, path2};

	list.add(first);
	list.add(second);
	list.add(last);

	std::size_t size = serialize(buffer, BUFFER_LEN,
			type::confirmable, code::get, 0x0102,
//			token, sizeof(token),
			nullptr, 0,
			list,
			"TESTE", 5,
//			nullptr, 0,
			ec);
#endif /* TEST_LIST */
	if(ec)
	{
		std::cerr << "ERROR! [" << ec.value() << "] " << ec.message() << "\n";
		return EXIT_FAILURE;
	}

	std::cout << "Success! [" << size << "]: ";
	print_array(buffer, size);
	std::cout << "\n";

	return EXIT_SUCCESS;
}
