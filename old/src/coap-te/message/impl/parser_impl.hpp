#ifndef COAP_TE_MESSAGE_PARSER_IMPL_HPP__
#define COAP_TE_MESSAGE_PARSER_IMPL_HPP__

#include "../parser.hpp"
#include "../options/functions.hpp"
#include "../options/parser.hpp"

namespace CoAP{
namespace Message{

template<typename Message,
		typename OptionCode,
		bool CheckOptions>
unsigned parse_options(Message& msg,
		std::uint8_t const* const buffer, std::size_t buffer_len,
		CoAP::Error& ec) noexcept
{
	unsigned offset = 0, delta = 0;

	msg.option_init = buffer;
	while(buffer[offset] != payload_marker && (buffer_len - offset) != 0)
	{
		Option::option_template<OptionCode> opt;
		offset += Option::parse<OptionCode, CheckOptions>(opt, buffer + offset, buffer_len - offset, delta, ec);
		if(ec)
			return offset;
		delta = static_cast<unsigned>(opt.ocode);
		msg.option_num++;
	}
	msg.options_len = offset;

	return offset;
}

template<typename Message>
unsigned parse_payload(Message& msg,
		std::uint8_t const* const buffer, std::size_t buffer_len,
		CoAP::Error& ec) noexcept
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
	offset += static_cast<unsigned>(msg.payload_len);

	return offset;
}

template<typename Message>
bool query_by_key(Message const& msg,
		const char* key, const void** value,
		unsigned& length) noexcept
{
	Option::Parser<Option::code> parser(msg);
	Option::option const *opt;

	while((opt = parser.next()) != nullptr)
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

}//Message
}//CoAP

#endif /* COAP_TE_MESSAGE_PARSER_IMPL_HPP__ */
