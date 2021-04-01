#include "parser.hpp"
#include "options/functions.hpp"
#include "options/parser.hpp"

namespace CoAP{
namespace Message{

unsigned parse(message& msg,
		std::uint8_t const* const buffer, std::size_t buffer_len,
		CoAP::Error& ec) noexcept
{
	unsigned offset = parse_header(msg, buffer, buffer_len, ec);
	if(ec || (buffer_len - offset) == 0)
		return offset;

	offset += parse_options(msg, buffer + offset, buffer_len - offset, ec);
	if(ec || (buffer_len - offset) == 0)
		return offset;

	offset += parse_payload(msg, buffer + offset, buffer_len - offset, ec);

	return offset;
}

unsigned parse_header(message& msg,
		std::uint8_t const* const buffer, std::size_t buffer_len,
		CoAP::Error& ec) noexcept
{
	unsigned offset = 0;
	if(buffer_len < 4)
	{
		ec = CoAP::errc::message_too_small;
		return offset;
	}

	unsigned ver = buffer[0] >> 6;
	if(ver != version)
	{
		ec = CoAP::errc::version_invalid;
		return offset;
	}

	msg.mtype = static_cast<type>((buffer[0] >> 4) & 0x03);
	if(!check_type(msg.mtype))
	{
		ec = CoAP::errc::type_invalid;
		return offset;
	}

	msg.token_len = buffer[0] & 0x0F;
	if(msg.token_len > 8)
	{
		ec = CoAP::errc::invalid_token_length;
		return offset;
	}
	offset += 1;
	if(buffer_len < (4 + msg.token_len))
	{
		ec = CoAP::errc::message_too_small;
		return offset;
	}

	msg.mcode = static_cast<code>(buffer[1]);
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

	msg.mid = buffer[2] << 8 | buffer[3];
	offset += 2;

	if(msg.token_len)
		msg.token = &buffer[4];
	offset += static_cast<unsigned>(msg.token_len);

	return offset;
}

}//Message
}//CoAP
