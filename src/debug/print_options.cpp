#include "defines/defaults.hpp"
#include "print_options.hpp"
#include <iostream>

#include "output_string.hpp"
#include "helper.hpp"
#include "message/types.hpp"
#include "internal/helper.hpp"

namespace CoAP{
namespace Debug{

static void print_payload(CoAP::Message::Option::option const& op)
{
	if(op.ocode == CoAP::Message::Option::code::content_format)
	{
		if(op.length == 0)
		{
			std::uint8_t b = 0;
			std::cout << 0 << " " << content_format_string(static_cast<CoAP::Message::content_format>(b));
			return;
		}
		auto const* buffer_u8 = static_cast<std::uint8_t const*>(op.value);
		std::cout << static_cast<unsigned>(buffer_u8[0]) << " "
				<< content_format_string(static_cast<CoAP::Message::content_format>(buffer_u8[0]));
		return;
	}

#if	COAP_TE_BLOCKWISE_TRANSFER == 1
	if(op.ocode == CoAP::Message::Option::code::block1 ||
		op.ocode == CoAP::Message::Option::code::block2)
	{
		unsigned value;
		CoAP::Helper::array_to_unsigned(static_cast<std::uint8_t const*>(op.value), op.length, value);
		std::printf("%u %u(%u)/%c/%u",
				value,
				CoAP::Message::Option::block_number(value),
				CoAP::Message::Option::byte_offset(value),
				CoAP::Message::Option::more(value) ? 'M' : '_',
				CoAP::Message::Option::block_size(value));

		return;
	}
#endif /* COAP_TE_BLOCKWISE_TRANSFER == 1 */

	CoAP::Message::Option::config<CoAP::Message::Option::code> const* config = CoAP::Message::Option::get_config(op.ocode);
	switch(config->otype)
	{
		using namespace CoAP::Message;
		case Option::type::empty:
			std::cout << "<empty>";
			break;
		case Option::type::string:
			print_string(op.value, op.length);
			break;
		case Option::type::uint:
			unsigned value;
			CoAP::Helper::array_to_unsigned(static_cast<std::uint8_t const*>(op.value), op.length, value);
			std::cout << value;
			break;
		default:
			print_array(op.value, op.length);
			break;
	}
}

void print_option(CoAP::Message::Option::option const& op, bool payload /* = true */)
{
	std::cout << static_cast<int>(op.ocode) << "|"
			<< CoAP::Debug::option_string(op.ocode) << "[" << op.length << "]: ";

	if(payload)
		print_payload(op);
}

void print_options(CoAP::Message::Option::option const* op, std::size_t option_num)
{
	for(std::size_t i = 0; i < option_num; i++)
	{
		std::cout << i << ": ";
		print_option(op[i]);
		std::cout << "\n";
	}
}

void print_options(CoAP::Message::Option::node const* list)
{
	int c = 0;
	for(CoAP::Message::Option::node const* i = list; i != nullptr; i = i->next)
	{
		std::cout << c++ << ": ";
		print_option(i->value);
		std::cout << "\n";
	}
}

}//Debug
}//CoAP
