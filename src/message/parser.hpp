#ifndef COAP_TE_MESSAGE_PARSER_HPP__
#define COAP_TE_MESSAGE_PARSER_HPP__

#include "error.hpp"
#include "types.hpp"
#include "options/options.hpp"

namespace CoAP{
namespace Message{

unsigned parse_header(message& msg,
		std::uint8_t const* const buffer, std::size_t buffer_len,
		CoAP::Error& ec) noexcept;

template<typename Message,
		typename OptionCode = CoAP::Message::Option::code,
		bool CheckOptions = true>
unsigned parse_options(Message& msg,
		std::uint8_t const* const buffer, std::size_t buffer_len,
		CoAP::Error& ec) noexcept;

template<typename Message>
unsigned parse_payload(Message& msg,
		std::uint8_t const* const buffer, std::size_t buffer_len,
		CoAP::Error& ec) noexcept;

unsigned parse(message& msg,
		std::uint8_t const* const buffer, std::size_t buffer_len,
		CoAP::Error&) noexcept;

template<typename Message>
bool query_by_key(Message const& msg,
		const char* key, const void** value,
		unsigned& length) noexcept;

}//Message
}//CoAP

#include "impl/parser_impl.hpp"

#endif /* COAP_TE_MESSAGE_PARSER_HPP__ */
