#ifndef COAP_TE_MESSAGE_PARSER_IMPL_HPP__
#define COAP_TE_MESSAGE_PARSER_IMPL_HPP__

#include "../parser.hpp"

namespace CoAP{
namespace Message{

template<bool CheckOptions /* = true */>
unsigned parse_option(Option::option& opt,
		std::uint8_t const* const buffer, std::size_t buffer_len,
		unsigned delta,
		CoAP::Error& ec)
{
	unsigned offset = 0, code, length;

	code = buffer[0] >> 4;
	length = buffer[0] & 0x0F;
	offset += 1;

	switch(static_cast<Option::delta_special>(code))
	{
		case Option::delta_special::one_byte_extend:
			if constexpr(CheckOptions)
			{
				if((buffer_len - offset) < 1)
				{
					ec = CoAP::errc::message_too_small;
					return offset;
				}
			}
			opt.code_ = static_cast<Option::code>(delta + buffer[1] + code);
			offset += 1;
			break;
		case Option::delta_special::two_byte_extend:
			if constexpr(CheckOptions)
			{
				if((buffer_len - offset) < 2)
				{
					ec = CoAP::errc::message_too_small;
					return offset;
				}
			}
			opt.code_ = static_cast<Option::code>(delta + (buffer[1] << 8 | buffer[2]) + code);
			offset += 2;
			break;
		case Option::delta_special::error:
			ec = CoAP::errc::option_parse_error;
			return offset;
		default:
			opt.code_ = static_cast<Option::code>(delta + code);
			break;
	}

	Option::config const* config = Option::get_config(opt.code_);
	if constexpr(CheckOptions)
	{
		if(!config)
		{
			ec = CoAP::errc::option_invalid;
			return offset;
		}
	}

	switch(static_cast<Option::length_special>(length))
	{
		case Option::length_special::one_byte_extend:
			if constexpr(CheckOptions)
			{
				if((buffer_len - offset) < 1)
				{
					ec = CoAP::errc::message_too_small;
					return offset;
				}
			}
			opt.length_ = buffer[offset + 1];
			offset += 1;
			break;
		case Option::length_special::two_byte_extend:
			if constexpr(CheckOptions)
			{
				if((buffer_len - offset) < 2)
				{
					ec = CoAP::errc::message_too_small;
					return offset;
				}
			}
			opt.length_ = buffer[offset + 1] << 8 | buffer[offset + 2];
			offset += 2;
			break;
		case Option::length_special::error:
			ec = CoAP::errc::option_parse_error;
			return offset;
		default:
			opt.length_ = length;
			break;
	}
	if(opt.length_)
	{
		if constexpr(CheckOptions)
		{
			if(config->otype == Option::type::empty)
			{
				ec = CoAP::errc::option_parse_error;
				return offset;
			}
			if((buffer_len - offset) < opt.length_)
			{
				ec = CoAP::errc::message_too_small;
				return offset;
			}
		}
		opt.value_ = &buffer[offset];
		offset += opt.length_;
	}
	return offset;
}

template<bool CheckOptions /* = false */>
Option::option const*
Option_Parser::next() noexcept
{
	CoAP::Error ec;
	if((msg_.options_len - offset_) != 0)
	{
		offset_ += parse_option<CheckOptions>(opt_, msg_.option_init + offset_, msg_.options_len - offset_, delta_, ec);
		if(ec)
		{
			reset();
			return nullptr;
		}
		delta_ = static_cast<unsigned>(opt_.code_);
		current_opt_++;
	} else return nullptr;

	return &opt_;
}

template<bool CheckOptions /* = true */>
Option::option const* Option_Parser::next(CoAP::Error& ec) noexcept
{
	if((msg_.options_len - offset_) != 0)
	{
		offset_ += parse_option<CheckOptions>(opt_, msg_.option_init + offset_, msg_.options_len - offset_, delta_, ec);
		if(ec)
			return nullptr;

		delta_ = static_cast<unsigned>(opt_.code_);
		current_opt_++;
	} else return nullptr;

	return &opt_;
}

}//Message
}//CoAP

#endif /* COAP_TE_MESSAGE_PARSER_IMPL_HPP__ */

