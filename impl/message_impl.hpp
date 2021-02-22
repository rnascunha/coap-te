#ifndef COAP_TE_MESSAGE_IMPL_HPP__
#define COAP_TE_MESSAGE_IMPL_HPP__

#include "../message.hpp"
#include <cstring>
#include "../helper.hpp"

#include <iostream>

#include "../debug/output_string.hpp"
#include "../debug/print_options.hpp"

namespace CoAP{
namespace Message{

static unsigned make_header(std::uint8_t* buffer, std::size_t buffer_len,
		type mtype, code mcode, std::uint16_t message_id,
		std::uint8_t const* token, std::size_t token_len,
		CoAP::Error& ec) noexcept;

template<bool SortOptions = true,
		bool CheckOpOrder = !SortOptions,
		bool CheckOpRepeat = true>
static unsigned make_options(std::uint8_t* buffer, std::size_t buffer_len,
		option const* options, std::size_t option_num,
		CoAP::Error& ec) noexcept;

template<bool SortOptions = true,
		bool CheckOpOrder = !SortOptions,
		bool CheckOpRepeat = true>
static unsigned make_options(std::uint8_t* buffer,
		std::size_t buffer_len,
		option_node* list,
		CoAP::Error& ec) noexcept;

template<bool CheckOpOrder = true,
		bool CheckOpRepeat = true>
static unsigned make_option(std::uint8_t* buffer, std::size_t buffer_len,
		option const& option, unsigned& delta,
		option_code& last_option,
		CoAP::Error& ec) noexcept;

static unsigned make_payload(uint8_t* buffer, std::size_t buffer_len,
		void const* const payload, std::size_t payload_len,
		CoAP::Error& ec) noexcept;

template<bool SortOptions /* = true */,
		bool CheckOpOrder /* = !SortOptions */,
		bool CheckOpRepeat /* = true */>
std::size_t serialize(std::uint8_t* buffer, std::size_t buffer_len,
		type mtype, code mcode, std::uint16_t message_id,
		std::uint8_t const* const token, std::size_t token_len,
		option* options, std::size_t option_num,
		void const * const payload, std::size_t payload_len,
		CoAP::Error& ec) noexcept
{
	std::size_t offset = make_header(
					buffer, buffer_len,
					mtype, mcode, message_id,
					token, token_len, ec);
	if(ec) return offset;

	offset += make_options<SortOptions, CheckOpOrder, CheckOpRepeat>
			(buffer + offset, buffer_len - offset,
			options, option_num, ec);
	if(ec)return offset;

	offset += make_payload(buffer + offset, buffer_len - offset, payload, payload_len, ec);
	//No need to check ec return value because alredy end of function;

	return offset;
}

template<bool SortOptions /* = true */,
		bool CheckOpOrder /* = !SortOptions */,
		bool CheckOpRepeat /* = true */>
std::size_t serialize(std::uint8_t* buffer, std::size_t buffer_len,
		type mtype, code mcode, std::uint16_t message_id,
		std::uint8_t const* const token, std::size_t token_len,
		option_node* options,
		void const* const payload, std::size_t payload_len,
		CoAP::Error& ec) noexcept
{
	std::size_t offset = make_header(
						buffer, buffer_len,
						mtype, mcode, message_id,
						token, token_len, ec);
	if(ec) return offset;

	offset += make_options<SortOptions, CheckOpOrder, CheckOpRepeat>
				(buffer + offset, buffer_len - offset,
				options, ec);
	if(ec)return offset;

	offset += make_payload(buffer + offset, buffer_len - offset, payload, payload_len, ec);
	//No need to check ec return value because alredy end of function;

	return offset;
}

template<bool SortOptions /* = true */,
		bool CheckOpOrder /* = !SortOptions */,
		bool CheckOpRepeat /* = true */>
std::size_t serialize(std::uint8_t* buffer, std::size_t buffer_len,
		type mtype, code mcode, std::uint16_t message_id,
		std::uint8_t const* const token, std::size_t token_len,
		Option_List& options,
		void const* const payload, std::size_t payload_len,
		CoAP::Error& ec) noexcept
{
	std::size_t offset = make_header(
						buffer, buffer_len,
						mtype, mcode, message_id,
						token, token_len, ec);
	if(ec) return offset;

	offset += make_options<SortOptions, CheckOpOrder, CheckOpRepeat>
				(buffer + offset, buffer_len - offset,
				options.head(), ec);
	if(ec)return offset;

	offset += make_payload(buffer + offset, buffer_len - offset, payload, payload_len, ec);
	//No need to check ec return value because alredy end of function;

	return offset;
}

/**
 *
 */
static unsigned make_header(std::uint8_t* buffer, std::size_t buffer_len,
		type mtype, code mcode, std::uint16_t message_id,
		std::uint8_t const* token, std::size_t token_len,
		CoAP::Error& ec) noexcept
{
	unsigned offset = 0;
	if(token_len > 8)
	{
		ec = CoAP::errc::invalid_token_length;
		return offset;
	}

	if(buffer_len < (4 + token_len))
	{
		ec = CoAP::errc::insufficient_buffer;
		return offset;
	}
	std::uint8_t byte[4];

	//first byte;
	byte[0] = (version << 6) | (static_cast<std::uint8_t>(mtype) << 4) | token_len;
	byte[1] = static_cast<std::uint8_t>(mcode);
	CoAP::Helper::interger_to_big_endian_array(&byte[2], message_id);
	std::memcpy(buffer, byte, 4);
	offset += 4;

	//Token
	std::memcpy(buffer + offset, token, token_len);
	offset += token_len;

	return offset;
}

template<bool SortOptions /* = true */,
		bool CheckOpOrder /* = !SortOptions */,
		bool CheckOpRepeat /* = true */>
static unsigned make_options(std::uint8_t* buffer, std::size_t buffer_len,
		option* options, std::size_t option_num,
		CoAP::Error& ec) noexcept
{
	CoAP::Debug::print_options(options, option_num);
	if constexpr(SortOptions)
		sort_options(options, option_num);
	std::cout << "Sorted\n";
	CoAP::Debug::print_options(options, option_num);

	unsigned delta = 0, offset = 0;
	option_code last_option = option_code::invalid;
	for(std::size_t i = 0; i < option_num; i++)
		offset += make_option<CheckOpOrder, CheckOpRepeat>(buffer + offset, buffer_len - offset,
				options[i], delta, last_option, ec);

	return offset;
}

template<bool SortOptions = true,
		bool CheckOpOrder = !SortOptions,
		bool CheckOpRepeat = true>
static unsigned make_options(std::uint8_t* buffer,
		std::size_t buffer_len,
		option_node* list,
		CoAP::Error& ec) noexcept
{
	CoAP::Debug::print_options(list);
	if constexpr(SortOptions)
		sort_options(list);
	std::cout << "Sorted\n";
	CoAP::Debug::print_options(list);

	unsigned delta = 0, offset = 0;
	option_code last_option = option_code::invalid;
	for(option_node* i = list; i != nullptr; i = i->next)
		offset += make_option<CheckOpOrder, CheckOpRepeat>(buffer + offset, buffer_len - offset,
				i->value, delta, last_option, ec);

	return offset;
}

template<bool CheckOpOrder = true,
		bool CheckOpRepeat = true>
static unsigned make_option(std::uint8_t* buffer, std::size_t buffer_len,
							option const& option, unsigned& delta,
							option_code& last_option,
							CoAP::Error& ec) noexcept
{
	unsigned offset = 0;
	option_config const * const config = get_option_config(option.code);
	if(config == nullptr)
	{
		ec = CoAP::errc::option_invalid;
		return offset;
	}

	if constexpr(CheckOpOrder)
	{
		if(option.code < last_option)
		{
			ec = CoAP::errc::option_out_of_order;
			return offset;
		}
	}

	if constexpr(CheckOpRepeat)
	{
		if(!config->repeatable && last_option == option.code)
		{
			ec = CoAP::errc::option_repeated;
			return offset;
		}
	}
	last_option = option.code;

	unsigned option_size = 1;
	unsigned offset_delta = static_cast<std::uint8_t>(option.code) - delta, delta_opt = 0;
	std::uint16_t delta_ext = 0;
	delta += offset_delta;
	if(offset_delta > 12)
	{
		if(offset_delta <= 255)
		{
			delta_opt = static_cast<unsigned>(option_delta_special::one_byte_extend);
			delta_ext = offset_delta - 13;
			option_size += 1;
		}
		else
		{
			delta_opt = static_cast<unsigned>(option_delta_special::two_byte_extend);
			delta_ext = offset_delta - 269;
			option_size += 2;
		}
	}
	else
		delta_opt = offset_delta;

	unsigned length_opt = 0;
	std::uint16_t length_ext = 0;
	if(option.length > 12)
	{
		if(option.length <= 255)
		{
			length_opt = static_cast<unsigned>(option_length_special::one_byte_extend);
			length_ext = option.length - 13;
			option_size += 1;
		}
		else
		{
			length_opt = static_cast<unsigned>(option_length_special::two_byte_extend);
			length_ext = option.length - 269;
			option_size += 2;
		}
	}
	else
		length_opt = option.length;

	if((option_size + option.length + offset) > buffer_len)
	{
		ec = CoAP::errc::insufficient_buffer;
		return offset;
	}

	std::uint8_t byte[5];
	byte[0] = (delta_opt << 4) | (length_opt);
	unsigned next_byte = 1;
	switch(delta_opt)
	{
		case 13:
			byte[1] = static_cast<std::uint8_t>(delta_ext);
			next_byte = 2;
			break;
		case 14:
			CoAP::Helper::interger_to_big_endian_array(&byte[2], delta_ext);
			next_byte = 3;
			break;
		default:
			break;
	}

	switch(length_opt)
	{
		case 13:
			byte[next_byte] = static_cast<std::uint8_t>(length_ext);
			break;
		case 14:
			CoAP::Helper::interger_to_big_endian_array(&byte[next_byte], length_ext);
			break;
		default:
			break;
	}
	std::memcpy(buffer + offset, &byte, option_size);
	offset += option_size;
	std::memcpy(buffer + offset, option.value, option.length);
	offset += option.length;

	return offset;
}

static unsigned make_payload(uint8_t* buffer, std::size_t buffer_len,
								void const* const payload, std::size_t payload_len,
								CoAP::Error& ec) noexcept
{
	if(payload_len)
	{
		if((payload_len + 1) > buffer_len)
		{
			ec = CoAP::errc::insufficient_buffer;
			return 0;
		}
	} else
		return 0;

	std::memcpy(buffer, &payload_marker, 1);
	std::memcpy(buffer + 1, payload, payload_len);

	return payload_len + 1;
}

}//Message
}//CoAP

#endif /* COAP_TE_MESSAGE_IMPL_HPP__ */
