#ifndef COAP_TE_MESSAGE_SERIALIZE_IMPL_HPP__
#define COAP_TE_MESSAGE_SERIALIZE_IMPL_HPP__

#include <cstring>

#include "internal/helper.hpp"

#include "../options/functions2.hpp"
#include "../options/parser.hpp"
#include "../serialize.hpp"
#include "../parser.hpp"

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

	offset += make_options<Option::code, SortOptions, CheckOpOrder, CheckOpRepeat>
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

	offset += make_options<Option::code, SortOptions, CheckOpOrder, CheckOpRepeat>
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

	offset += make_options<Option::code, SortOptions, CheckOpOrder, CheckOpRepeat>
				(buffer + offset, buffer_len - offset,
				options.head(), ec);
	if(ec)return offset;

	offset += make_payload(buffer + offset, buffer_len - offset, payload, payload_len, ec);
	//No need to check ec return value because alredy end of function;

	return offset;
}

template<typename OptionCode /* = Option::code */,
		bool SortOptions /* = true */,
		bool CheckOpOrder /* = !SortOptions */,
		bool CheckOpRepeat /* = true */>
unsigned make_options(std::uint8_t* buffer, std::size_t buffer_len,
		Option::option_template<OptionCode>* options, std::size_t option_num,
		CoAP::Error& ec) noexcept
{
	if constexpr(SortOptions)
		Option::sort(options, option_num);

	unsigned delta = 0, offset = 0;
	OptionCode last_option = Option::invalid<OptionCode>();
	for(std::size_t i = 0; i < option_num; i++)
		offset += make_option<OptionCode, CheckOpOrder, CheckOpRepeat>(buffer + offset, buffer_len - offset,
				options[i], delta, last_option, ec);

	return offset;
}

template<typename OptionCode /* = Option::code */,
		bool SortOptions /* = true */,
		bool CheckOpOrder /* = !SortOptions */,
		bool CheckOpRepeat /* = true */>
unsigned make_options(std::uint8_t* buffer,
		std::size_t buffer_len,
		Option::node_option<OptionCode>* list,
		CoAP::Error& ec) noexcept
{
	if constexpr(SortOptions)
		Option::sort<OptionCode>(list);

	unsigned delta = 0, offset = 0;
	OptionCode last_option = Option::invalid<OptionCode>();
	for(Option::node_option<OptionCode>* i = list; i != nullptr; i = i->next)
		offset += make_option<OptionCode, CheckOpOrder, CheckOpRepeat>(buffer + offset, buffer_len - offset,
				i->value, delta, last_option, ec);

	return offset;
}

template<typename OptionCode /* = Option::code */,
		bool CheckOpOrder /* = true */,
		bool CheckOpRepeat /* = true */>
unsigned make_option(std::uint8_t* buffer, std::size_t buffer_len,
							Option::option_template<OptionCode> const& option, unsigned& delta,
							OptionCode& last_option,
							CoAP::Error& ec) noexcept
{
	unsigned offset = 0;
	Option::config<OptionCode> const * const oconfig = Option::get_config(option.ocode);
	if(oconfig == nullptr)
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
		if(!oconfig->repeatable && last_option == option.ocode)
		{
			ec = CoAP::errc::option_repeated;
			return offset;
		}
	}
	last_option = option.ocode;

	unsigned option_size = 1;
	unsigned offset_delta = static_cast<unsigned>(option.ocode) - delta, delta_opt = 0;
	std::uint16_t delta_ext = 0;
	delta += offset_delta;
	if(offset_delta > 12)
	{
		if(offset_delta <= 269)
		{
			delta_opt = static_cast<unsigned>(Option::delta_special::one_byte_extend);
			delta_ext = static_cast<uint16_t>(offset_delta - 13);
			option_size += 1;
		}
		else
		{
			delta_opt = static_cast<unsigned>(Option::delta_special::two_byte_extend);
			delta_ext = static_cast<std::uint16_t>(offset_delta - 269);
			option_size += 2;
		}
	}
	else
		delta_opt = offset_delta;

	unsigned length_opt = 0;
	std::uint16_t length_ext = 0;
	if(option.length > 12)
	{
		if(option.length < 269)
		{
			length_opt = static_cast<unsigned>(Option::length_special::one_byte_extend);
			length_ext = static_cast<uint16_t>(option.length - 13);
			option_size += 1;
		}
		else
		{
			length_opt = static_cast<unsigned>(Option::length_special::two_byte_extend);
			length_ext = static_cast<uint16_t>(option.length - 269);
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
	byte[0] = static_cast<uint8_t>((delta_opt << 4) | (length_opt));
	unsigned next_byte = 1;
	switch(delta_opt)
	{
		case 13:
			byte[next_byte] = static_cast<std::uint8_t>(delta_ext);
			next_byte = 2;
			break;
		case 14:
			CoAP::Helper::interger_to_big_endian_array(&byte[next_byte], delta_ext);
//			CoAP::Helper::interger_to_big_endian_array(&byte[2], delta_ext);
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

template<bool CheckRepeat /* = true */>
bool Serialize::add_option(Option::option&& op, CoAP::Error& ec) noexcept
{
	if constexpr(CheckRepeat)
	{
		Option::config<Option::code> const * const oconfig = Option::get_config(op.ocode);
		if(!oconfig->repeatable)
		{
			Option::Parser<Option::code> parser(msg_);
			Option::option const* opt;
			while((opt = parser.next()))
			{
				if(opt->ocode == op.ocode)
				{
					ec = CoAP::errc::option_repeated;
					return false;
				}
				if(opt->ocode > op.ocode) break;
			}
		}
	}

	Option::Parser<Option::code> parser(msg_);
	Option::option const *next;
	Option::option current;

	unsigned offset = 0;
	while((next = parser.next()))
	{
		if(next->ocode > op.ocode)
			break;
		current = *next;
		offset = parser.offset();
	}

	std::size_t opt_size = Option::serialized_size(op, current);
	if((msg_.size() + opt_size) > size_)
	{
		ec = CoAP::errc::insufficient_buffer;
		return false;
	}

	std::uint8_t* n_buf = buffer_ + 4 + msg_.token_len + offset;
	CoAP::Helper::shift_right(n_buf,
							msg_.size() - msg_.token_len - offset - 4,
							opt_size);

//	CoAP::Error ec;
	unsigned delta = current ? static_cast<unsigned>(current.ocode) : 0;
	Option::code c = current.ocode;
	make_option<Option::code, false, false>(n_buf, opt_size,
								op, delta,
								c, ec);
	if(ec) return false;

	msg_.option_num++;
	msg_.options_len += opt_size;
	msg_.option_init = buffer_ + 4 + msg_.token_len;

	msg_.payload = static_cast<std::uint8_t const*>(msg_.payload) + opt_size;

	/**
	 * Changing the next option
	 */
	if(next)
	{
		unsigned n_delta = current ? static_cast<unsigned>(current.ocode) : 0;

		n_buf += opt_size;
		//CoAP::Error ec;
		Option::option opt;
		std::size_t off = Option::parse<Option::code>(opt, n_buf, msg_.options_len - offset - opt_size, n_delta, ec);
		if(ec) return false;

		n_delta = static_cast<unsigned>(op.ocode);
		Option::code co = op.ocode;
		unsigned after = make_option<Option::code, false, false>(n_buf, off,
										opt, delta,
										co, ec);
		if(ec) return false;

		std::size_t diff = off - after;
		if(diff)
		{
			CoAP::Helper::shift_left(n_buf + after, msg_.size() - offset - after, diff);
			msg_.options_len -= diff;
			msg_.payload = static_cast<std::uint8_t const*>(msg_.payload) - diff;
		}
	}

	return true;
}

template<bool CheckRepeat /* = true */>
bool Serialize::add_option(Option::option&& op) noexcept
{
	CoAP::Error ec;
	return add_option<CheckRepeat>(std::forward<Option::option>(op), ec);
}

}//Message
}//CoAP

#endif /* COAP_TE_MESSAGE_SERIALIZE_IMPL_HPP__ */
