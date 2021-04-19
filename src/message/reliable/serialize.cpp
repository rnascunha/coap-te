#include "serialize.hpp"

#include "types.hpp"

namespace CoAP{
namespace Message{
namespace Reliable{

#if COAP_TE_RELIABLE_CONNECTION == 1

unsigned make_header(std::uint8_t* buffer, std::size_t buffer_len,
		code mcode,
		void const* const token, std::size_t token_len,
		CoAP::Error& ec) noexcept
{
	unsigned offset = 0;
	if(token_len > 8)
	{
		ec = CoAP::errc::invalid_token_length;
		return offset;
	}

	if(buffer_len < (1 + token_len))
	{
		ec = CoAP::errc::insufficient_buffer;
		return offset;
	}

	//first byte;
	buffer[0] = static_cast<uint8_t>(token_len);
	offset += 1;
	buffer[1] = static_cast<uint8_t>(mcode);
	offset += 1;

	//Token
	if(token_len)
	{
		std::memcpy(buffer + offset, token, token_len);
		offset += static_cast<unsigned>(token_len);
	}
	return offset;
}

std::size_t set_message_length(std::uint8_t* buffer,
							std::size_t buffer_len, std::size_t buffer_used,
							std::size_t size,
							CoAP::Error& ec) noexcept
{
	unsigned shift = 0;
	extend_length opt_len;

	if(size < 13)
	{
		opt_len = extend_length::normal;
	}
	else if(size < 269)
	{
		opt_len = extend_length::one_byte;
		shift = 1;
		size -= 13;
	}
	else if(size < 65805)
	{
		opt_len = extend_length::two_bytes;
		shift = 2;
		size -= 269;
	}
	else
	{
		opt_len = extend_length::three_bytes;
		shift = 3;
		size -= 65805;
	}

	if((buffer_len - buffer_used) < shift)
	{
		ec = CoAP::errc::insufficient_buffer;
		return buffer_used;
	}

	buffer[0] |= (opt_len != extend_length::normal ? (static_cast<std::uint8_t>(opt_len) << 4) : (size << 4));
	if(shift)
	{
		CoAP::Helper::shift_right(buffer + shift, buffer_used - 1, shift);
		CoAP::Helper::interger_to_big_endian_array(&buffer[1], size, shift);
	}

	return buffer_used + shift;
}

}//Reliable
}//Message
}//CoAP


#endif /* COAP_TE_RELIABLE_CONNECTION == 1 */
