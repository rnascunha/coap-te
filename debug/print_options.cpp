#include "print_options.hpp"
#include <iostream>

#include "output_string.hpp"
#include "helper.hpp"
#include "message/types.hpp"
#include "other/helper.hpp"

namespace CoAP{
namespace Debug{

static void print_payload(CoAP::Message::option const& op)
{
	if(op.code == CoAP::Message::option_code::content_format)
	{
		auto const* buffer_u8 = static_cast<std::uint8_t const*>(op.value);
		std::cout << static_cast<unsigned>(buffer_u8[0]) << " "
				<< content_format_string(static_cast<CoAP::Message::content_format>(buffer_u8[0]));
		return;
	}

	CoAP::Message::option_config const* config = CoAP::Message::get_option_config(op.code);
	switch(config->type)
	{
		using namespace CoAP::Message;
		case option_type::empty:
			std::cout << "<empty>";
			break;
		case option_type::string:
			print_string(op.value, op.length);
			break;
		case option_type::uint:
			unsigned value;
			CoAP::Helper::array_to_unsigned(static_cast<std::uint8_t const*>(op.value), op.length, value);
			std::cout << value;
			break;
		default:
			print_array(op.value, op.length);
			break;
	}
}

void print_option(CoAP::Message::option const& op, bool payload /* = true */)
{
	std::cout << static_cast<int>(op.code) << "|"
			<< CoAP::Debug::option_string(op.code) << "[" << op.length << "]: ";

	if(payload)
		print_payload(op);
}

void print_options(CoAP::Message::option const* op, std::size_t option_num)
{
	for(std::size_t i = 0; i < option_num; i++)
	{
		std::cout << i << ": ";
		print_option(op[i]);
		std::cout << "\n";
	}
}

void print_options(CoAP::Message::option_node const* list)
{
	int c = 0;
	for(CoAP::Message::option_node const* i = list; i != nullptr; i = i->next)
	{
		std::cout << c++ << ": ";
		print_option(i->value);
		std::cout << "\n";
	}
}

}//Debug
}//CoAP
