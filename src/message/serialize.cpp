#include "serialize.hpp"
#include "types.hpp"

namespace CoAP{
namespace Message{

/**
 * https://tools.ietf.org/html/rfc7252#section-4.1
 *
 * An Empty message has the Code field set to 0.00.  The Token Length
 * field MUST be set to 0 and bytes of data MUST NOT be present after
 * the Message ID field.  If there are any bytes, they MUST be processed
 * as a message format error.
 */
std::size_t empty_message(type mtype, std::uint8_t* buffer, std::size_t buffer_len,
		std::uint16_t mid, CoAP::Error& ec) noexcept
{
	return make_header(buffer, buffer_len, mtype, code::empty, mid, nullptr, 0, ec);
}


/**
 *
 */
unsigned make_header(std::uint8_t* buffer, std::size_t buffer_len,
		type mtype, code mcode, std::uint16_t message_id,
		void const* const token, std::size_t token_len,
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

unsigned make_payload(uint8_t* buffer, std::size_t buffer_len,
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

