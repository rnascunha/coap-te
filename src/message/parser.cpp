#include "parser.hpp"

namespace CoAP{
namespace Message{

static unsigned parse_header(message& msg,
		std::uint8_t const* const buffer, std::size_t buffer_len,
		CoAP::Error& ec);
static unsigned parse_options(message& msg,
		std::uint8_t const* const buffer, std::size_t buffer_len,
		CoAP::Error& ec);
static unsigned parse_payload(message& msg,
		std::uint8_t const* const buffer, std::size_t buffer_len,
		CoAP::Error& ec);

unsigned parse(message& msg,
		std::uint8_t const* const buffer, std::size_t buffer_len,
		CoAP::Error& ec)
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

bool query_by_key(message const& msg, const char* key, const void** value, unsigned& length) noexcept
{
	Option_Parser parser(msg);
	Option::option const *opt;

	while((opt = parser.next()))
	{
		if(opt->ocode == Option::code::uri_query)
		{
			const char *nkey = key, *opt_value = static_cast<const char*>(opt->value);
			unsigned len = opt->length;
			int i = 0;
			while(len && nkey[i] && nkey[i] == opt_value[i])
			{
				i++; len--;
			}
			if(!nkey[i])
			{
				if(len > 0 && opt_value[i] == '=')
				{
					*value = (opt_value + i) + 1;
					length = len;
					return true;
				}
				if(len == 0)
				{
					length = 0;
					return true;
				}
			}
		}
	}
	return false;
}

/**
 *
 *
 *
 */

unsigned parse_header(message& msg,
		std::uint8_t const* const buffer, std::size_t buffer_len,
		CoAP::Error& ec)
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
	offset += msg.token_len;

	return offset;
}

static unsigned parse_options(message& msg,
		std::uint8_t const* const buffer, std::size_t buffer_len,
		CoAP::Error& ec)
{
	unsigned offset = 0, delta = 0;

	msg.option_init = buffer;
	while(buffer[offset] != payload_marker && (buffer_len - offset) != 0)
	{
		Option::option opt;
		offset += parse_option<true>(opt, buffer + offset, buffer_len - offset, delta, ec);
		if(ec)
			return offset;
		delta = static_cast<unsigned>(opt.ocode);
		msg.option_num++;
	}
	msg.options_len = offset;

	return offset;
}

static unsigned parse_payload(message& msg,
		std::uint8_t const* const buffer, std::size_t buffer_len,
		CoAP::Error& ec)
{
	unsigned offset = 0;
	if(buffer[0] != payload_marker)
	{
		ec = CoAP::errc::payload_no_marker;
		return offset;
	}

	offset += 1;
	msg.payload = &buffer[1];
	msg.payload_len = buffer_len - 1;
	offset += msg.payload_len;

	return offset;
}

Option_Parser::Option_Parser(message const& msg)
	: msg_(msg){}

void Option_Parser::reset() noexcept
{
	current_opt_ = 0;
	delta_ = 0;
	offset_ = 0;
}

Option::option const* Option_Parser::current() noexcept
{
	return &opt_;
}

unsigned Option_Parser::current_number() noexcept
{
	return current_opt_;
}

std::size_t Option_Parser::total_number() noexcept
{
	return msg_.option_num;
}

}//Message
}//CoAP
