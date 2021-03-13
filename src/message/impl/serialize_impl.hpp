#ifndef COAP_TE_MESSAGE_SERIALIZE_IMPL_HPP__
#define COAP_TE_MESSAGE_SERIALIZE_IMPL_HPP__

#include "../serialize.hpp"
#include <cstring>
#include "internal/helper.hpp"

namespace CoAP{
namespace Message{

template<bool SortOptions /* = true */,
		bool CheckOpOrder /* = !SortOptions */,
		bool CheckOpRepeat /* = true */>
std::size_t serialize(std::uint8_t* buffer, std::size_t buffer_len,
		type mtype, code mcode, std::uint16_t message_id,
		void const* const token, std::size_t token_len,
		Option::option* options, std::size_t option_num,
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
		void const* const token, std::size_t token_len,
		Option::node* options,
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
		void const* const token, std::size_t token_len,
		Option::List& options,
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

template<bool SortOptions /* = true */,
		bool CheckOpOrder /* = !SortOptions */,
		bool CheckOpRepeat /* = true */>
static unsigned make_options(std::uint8_t* buffer, std::size_t buffer_len,
		Option::option* options, std::size_t option_num,
		CoAP::Error& ec) noexcept
{
	if constexpr(SortOptions)
		Option::sort(options, option_num);

	unsigned delta = 0, offset = 0;
	Option::code last_option = Option::code::invalid;
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
		Option::node* list,
		CoAP::Error& ec) noexcept
{
	if constexpr(SortOptions)
		Option::sort(list);

	unsigned delta = 0, offset = 0;
	Option::code last_option = Option::code::invalid;
	for(Option::node* i = list; i != nullptr; i = i->next)
		offset += make_option<CheckOpOrder, CheckOpRepeat>(buffer + offset, buffer_len - offset,
				i->value, delta, last_option, ec);

	return offset;
}

template<bool CheckOpOrder = true,
		bool CheckOpRepeat = true>
static unsigned make_option(std::uint8_t* buffer, std::size_t buffer_len,
							Option::option const& option, unsigned& delta,
							Option::code& last_option,
							CoAP::Error& ec) noexcept
{
	unsigned offset = 0;
	Option::config const * const config = Option::get_config(option.ocode);
	if(config == nullptr)
	{
		ec = CoAP::errc::option_invalid;
		return offset;
	}

	if constexpr(CheckOpOrder)
	{
		if(option.ocode < last_option)
		{
			ec = CoAP::errc::option_out_of_order;
			return offset;
		}
	}

	if constexpr(CheckOpRepeat)
	{
		if(!config->repeatable && last_option == option.ocode)
		{
			ec = CoAP::errc::option_repeated;
			return offset;
		}
	}
	last_option = option.ocode;

	unsigned option_size = 1;
	unsigned offset_delta = static_cast<std::uint8_t>(option.ocode) - delta, delta_opt = 0;
	std::uint16_t delta_ext = 0;
	delta += offset_delta;
	if(offset_delta > 12)
	{
		if(offset_delta <= 255)
		{
			delta_opt = static_cast<unsigned>(Option::delta_special::one_byte_extend);
			delta_ext = offset_delta - 13;
			option_size += 1;
		}
		else
		{
			delta_opt = static_cast<unsigned>(Option::delta_special::two_byte_extend);
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
			length_opt = static_cast<unsigned>(Option::length_special::one_byte_extend);
			length_ext = option.length - 13;
			option_size += 1;
		}
		else
		{
			length_opt = static_cast<unsigned>(Option::length_special::two_byte_extend);
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

}//Message
}//CoAP

#endif /* COAP_TE_MESSAGE_SERIALIZE_IMPL_HPP__ */
