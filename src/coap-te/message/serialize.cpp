#include "serialize.hpp"
#include "types.hpp"
#include "../internal/helper.hpp"

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
	byte[0] = static_cast<uint8_t>((version << 6) | (static_cast<std::uint8_t>(mtype) << 4) | token_len);
	byte[1] = static_cast<std::uint8_t>(mcode);
	CoAP::Helper::interger_to_big_endian_array(&byte[2], message_id);
	std::memcpy(buffer, byte, 4);
	offset += 4;

	//Token
	std::memcpy(buffer + offset, token, token_len);
	offset += static_cast<unsigned>(token_len);

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

	return static_cast<unsigned>(payload_len + 1);
}

Serialize::Serialize(std::uint8_t* buffer, std::size_t buffer_size)
	: buffer_(buffer), size_(buffer_size){}

Serialize::Serialize(std::uint8_t* buffer, std::size_t buffer_size, message& msg)
	: buffer_(buffer), size_(buffer_size), msg_(msg)
{}

std::uint8_t const* Serialize::buffer() const noexcept
{
	return buffer_;
}

std::size_t	Serialize::size() const noexcept
{
	return size_;
}

std::size_t Serialize::used() const noexcept
{
	return msg_.size();
}

message const& Serialize::get_message() const noexcept
{
	return msg_;
}

bool Serialize::header(type mtype, code mcode, std::uint16_t message_id) noexcept
{
	CoAP::Error ec;

	make_header(buffer_, size_,
				mtype, mcode, message_id,
				msg_.token, msg_.token_len,
				ec);
	if(ec) return false;

	msg_.mtype = mtype;
	msg_.mcode = mcode;
	msg_.mid = message_id;

	return true;
}

bool Serialize::token(const char* tok) noexcept
{
	return token(tok, std::strlen(tok));
}

bool Serialize::token(const void* tok, std::size_t token_len) noexcept
{
	if(token_len > msg_.token_len)
	{
		std::size_t diff = token_len - msg_.token_len;
		if((msg_.size() + diff) > size_) return false;

		CoAP::Helper::shift_right(buffer_ + msg_.token_len, msg_.size() - msg_.token_len, diff);

		msg_.option_init = static_cast<const std::uint8_t*>(msg_.option_init) + diff;
		msg_.payload = static_cast<const std::uint8_t*>(msg_.payload) + diff;
	}
	else if(token_len < msg_.token_len)
	{
		std::size_t diff = msg_.token_len - token_len;

		CoAP::Helper::shift_left(buffer_ + token_len + 1, msg_.size() - 1, diff);

		msg_.option_init = static_cast<const std::uint8_t*>(msg_.option_init) - diff;
		msg_.payload = static_cast<const std::uint8_t*>(msg_.payload) - diff;
	}

	msg_.token = buffer_ + 4;
	msg_.token_len = token_len;

	CoAP::Error ec;
	make_header(buffer_, size_, msg_.mtype, msg_.mcode, msg_.mid, tok, token_len, ec);
	if(ec) return false;

	return true;
}

bool Serialize::payload(const char* load) noexcept
{
	return payload(load, std::strlen(load));
}

bool Serialize::payload(const void* load, std::size_t size) noexcept
{
	if(!msg_.payload_len)
	{
		if((msg_.size() + 1 + size) > size_) return false;
		buffer_[msg_.size()] = payload_marker;
		msg_.payload = buffer_ + msg_.size() + 1;
	}
	else if(size > msg_.payload_len &&
			(msg_.size() + size - msg_.payload_len) > size_)
		return false;

	msg_.payload_len = size;
	/**
	 * not using msg_.payload because is const void*
	 */
	std::memcpy(buffer_ + msg_.size() - msg_.payload_len, load, size);


	return true;
}

bool Serialize::remove_token() noexcept
{
	return token(nullptr, 0);
}

bool Serialize::remove_option(Option::code op) noexcept
{
	CoAP::Error ec;
	return remove_option(op, ec);
}

bool Serialize::remove_option(Option::code op, CoAP::Error& ec) noexcept
{
	Option::Parser<Option::code> parser(msg_);
	Option::option const* next;
	Option::option current, before;
	unsigned offset = 0;

	while((next = parser.next()))
	{
		before = current;
		current = *next;
		if(current.ocode == op)
			break;

		offset = parser.offset();
	}

	if(!next)
	{
		ec = CoAP::errc::option_not_found;
		return false;
	}

	next = parser.next();

	std::size_t opt_size = Option::serialized_size(current, before);

	std::uint8_t* n_buf = buffer_ + 4 + msg_.token_len + offset;
	CoAP::Helper::shift_left(n_buf,
							msg_.size() - 4 - msg_.token_len - offset - opt_size,
							opt_size);

	msg_.option_num--;
	msg_.options_len -= opt_size;
	msg_.payload = static_cast<std::uint8_t const*>(msg_.payload) - opt_size;

	/**
	 * Checking if need to recalculate next option
	 */
	if(next)
	{
		std::size_t s_after = Option::serialized_size(*next, before),
					s_bef = Option::serialized_size(*next, current);

		unsigned diff = static_cast<unsigned>(s_after - s_bef);
		if(diff)
		{
			CoAP::Helper::shift_right(n_buf + s_bef - next->length,
					msg_.size() - 4 - msg_.token_len - offset - s_bef + next->length,
					diff);

			msg_.options_len += diff;
			msg_.payload = static_cast<std::uint8_t const*>(msg_.payload) + diff;
		}

		unsigned n_delta = current ? static_cast<unsigned>(current.ocode) : 0;

		Option::option opt;
//		CoAP::Error ec;
		Option::parse(opt, n_buf, s_bef, n_delta, ec);
		if(ec) return false;

		/**
		 *
		 */
		if(diff)
			opt.value = static_cast<std::uint8_t const*>(opt.value) + diff;

		n_delta = before ? static_cast<unsigned>(before.ocode) : 0;
		Option::code c = before ? before.ocode : static_cast<Option::code>(0);
		make_option<Option::code, false, false>(n_buf, s_after,
										opt, n_delta,
										c, ec);
		if(ec) return false;
	}

	return true;
}

bool Serialize::remove_payload() noexcept
{
	msg_.payload = nullptr;
	msg_.payload_len = 0;

	return true;
}

}//Message
}//CoAP

