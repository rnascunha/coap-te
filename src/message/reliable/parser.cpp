#include "parser.hpp"
#include "../parser.hpp"
#include "internal/helper.hpp"

namespace CoAP{
namespace Message{
namespace Reliable{

unsigned parse_header(message& msg,
		std::uint8_t const* const buffer, std::size_t buffer_len,
		CoAP::Error& ec) noexcept
{
	unsigned offset = 0, shift = 0;
	extend_length length;
	if(buffer_len < 1)
	{
		ec = CoAP::errc::message_too_small;
		return offset;
	}

	length = static_cast<extend_length>(buffer[0] >> 4);
	if(length == extend_length::one_byte)
		shift = 1;
	else if(length == extend_length::two_bytes)
		shift = 2;
	else if(length == extend_length::three_bytes)
		shift = 3;
	else
		msg.len = static_cast<unsigned>(length);

	if(shift)
	{
		CoAP::Helper::array_to_unsigned(&buffer[1], shift, msg.len);
		msg.len += length == extend_length::one_byte ? 13 : (length == extend_length::two_bytes ? 269 : 65805);
		offset += shift;
	}

	msg.token_len = buffer[0] & 0x0F;
	if(msg.token_len > 8)
	{
		ec = CoAP::errc::invalid_token_length;
		return offset;
	}
	offset += 1;
	if(buffer_len < (offset + msg.token_len))
	{
		ec = CoAP::errc::message_too_small;
		return offset;
	}

	msg.mcode = static_cast<code>(*(buffer + offset));
	if(!check_code(msg.mcode))
	{
		ec = CoAP::errc::code_invalid;
		return offset;
	}

	offset += 1;
	if(msg.mcode == code::empty && buffer_len != 4)
	{
		ec = CoAP::errc::empty_format_error;
		return offset;
	}

	if(msg.token_len)
		msg.token = &buffer[offset];
	offset += static_cast<unsigned>(msg.token_len);

	return offset;
}

static unsigned parse_option_choose(message& msg,
		std::uint8_t const* const buffer, std::size_t buffer_len,
		CoAP::Error& ec) noexcept
{
	if(CoAP::Message::is_signaling(msg.mcode))
	{
		switch(msg.mcode)
		{
			case code::csm:
				return CoAP::Message::parse_options<Reliable::message, Option::csm>(msg, buffer, buffer_len, ec);
			case code::ping:
			case code::pong:
				return CoAP::Message::parse_options<Reliable::message, Option::ping_pong>(msg, buffer, buffer_len, ec);
			case code::release:
				return CoAP::Message::parse_options<Reliable::message, Option::release>(msg, buffer, buffer_len, ec);
			case code::abort:
				return CoAP::Message::parse_options<Reliable::message, Option::abort>(msg, buffer, buffer_len, ec);
			default:
				break;
		}
		ec = CoAP::errc::code_invalid;
		return 0;
	}

	return CoAP::Message::parse_options<Reliable::message, Option::code>(msg, buffer, buffer_len, ec);
}

unsigned parse(message& msg,
		std::uint8_t const* const buffer, std::size_t buffer_len,
		CoAP::Error& ec) noexcept
{
	unsigned offset = parse_header(msg, buffer, buffer_len, ec);
	if(ec) return offset;

	/**
	 * If it's websocket, msg.len must be zero, and buffer_len must be the size of
	 * a complete packet. If is TCP, msg.len must be the size of the packet.
	 * Here we a trying to figure out this...
	 */
	std::size_t length = msg.len == 0 ? buffer_len : msg.len + offset;
	if(!length) return offset;

	offset += parse_option_choose(msg, buffer + offset, length - offset, ec);
	if(ec || (length - offset) == 0)
		return offset;

	offset += CoAP::Message::parse_payload(msg, buffer + offset, length - offset, ec);

	return offset;
}

}//Reliable
}//Message
}//CoAP
