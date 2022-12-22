#ifndef COAP_TE_DEBUG_PRINT_OPTIONS_IMPL_HPP__
#define COAP_TE_DEBUG_PRINT_OPTIONS_IMPL_HPP__

#include <cstdio>
#include <type_traits>

#include "../../defines/defaults.hpp"
#include "../print_options.hpp"

#include "../output_string.hpp"
#include "../helper.hpp"
#include "../../message/types.hpp"
#include "../../internal/helper.hpp"
#include "../../message/options/no_response.hpp"

namespace CoAP{
namespace Debug{

template<typename OptionCode>
static void print_payload(CoAP::Message::Option::option_template<OptionCode> const& op)
{
	if constexpr(std::is_same<OptionCode, CoAP::Message::Option::code>::value)
	{
		if(op.ocode == CoAP::Message::Option::code::content_format
			|| op.ocode == CoAP::Message::Option::code::accept)
		{
			if(op.length == 0)
			{
				std::uint8_t b = 0;
				std::printf("0 %s", content_format_string(static_cast<CoAP::Message::content_format>(b)));
				return;
			}
			auto const* buffer_u8 = static_cast<std::uint8_t const*>(op.value);
			std::printf("%u %s",
					buffer_u8[0],
					content_format_string(static_cast<CoAP::Message::content_format>(buffer_u8[0])));
			return;
		}
	}

#if COAP_TE_OPTION_NO_RESPONSE == 1
	if constexpr(std::is_same<OptionCode, CoAP::Message::Option::code>::value)
	{
		using namespace CoAP::Message::Option;
		if(op.ocode == code::no_response)
		{
			unsigned value = parse_unsigned(op);
			suppress s = static_cast<suppress>(value);

			std::printf("%u", value);
			if(s & suppress::success)
				std::printf("/success");
			if(s & suppress::client_error)
				std::printf("/client error");
			if(s & suppress::server_error)
				std::printf("/server error");
			return;
		}
	}
#endif /* COAP_TE_OPTION_NO_RESPONSE == 1 */

#if	COAP_TE_MESSAGE_OPTION_BLOCKWISE_TRANSFER == 1
	if constexpr(std::is_same<OptionCode, CoAP::Message::Option::code>::value)
	{
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
	}
#endif /* COAP_TE_MESSAGE_OPTION_BLOCKWISE_TRANSFER == 1 */

	CoAP::Message::Option::config<OptionCode> const* oconfig = CoAP::Message::Option::get_config(op.ocode);
	switch(oconfig->otype)
	{
		using namespace CoAP::Message;
		case Option::type::empty:
			std::printf("<empty>");
			break;
		case Option::type::string:
			print_string(op.value, op.length);
			break;
		case Option::type::uint:
			unsigned value;
			CoAP::Helper::array_to_unsigned(static_cast<std::uint8_t const*>(op.value), op.length, value);
			std::printf("%u", value);
			break;
		default:
			print_array(op.value, op.length);
			break;
	}
}

template<typename OptionCode>
void print_option(CoAP::Message::Option::option_template<OptionCode> const& op, bool payload /* = true */)
{
	std::printf("%d|%s[%u]: ",
			static_cast<int>(op.ocode),
			CoAP::Debug::option_string(op.ocode),
			op.length);

	if(payload)
		print_payload(op);
}

template<typename OptionCode>
void print_options(CoAP::Message::Option::option_template<OptionCode> const* op, std::size_t option_num)
{
	for(std::size_t i = 0; i < option_num; i++)
	{
		std::printf("%zu:", i);
		print_option(op[i]);
		std::printf("\n");
	}
}

template<typename OptionCode>
void print_options(CoAP::Message::Option::node_option<OptionCode> const* list,
				const char* prefix)
{
	int c = 0;
	for(CoAP::Message::Option::node const* i = list; i != nullptr; i = i->next)
	{
		std::printf("%s%d: ", prefix, c++);
		print_option(i->value);
		std::printf("\n");
	}
}

}//Debug
}//CoAP

#endif /* COAP_TE_DEBUG_PRINT_OPTIONS_IMPL_HPP__ */
