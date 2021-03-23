#ifndef COAP_TE_MESSAGE_PARSER_HPP__
#define COAP_TE_MESSAGE_PARSER_HPP__

#include "error.hpp"
#include "types.hpp"
#include "options.hpp"

namespace CoAP{
namespace Message{

unsigned parse(message& msg,
		std::uint8_t const* const buffer, std::size_t buffer_len,
		CoAP::Error&);

template<bool CheckOptions = true>
unsigned parse_option(Option::option& opt,
		std::uint8_t const* const buffer, std::size_t buffer_len,
		unsigned delta,
		CoAP::Error& ec);

bool query_by_key(message const& msg, const char* key, const void** value, unsigned& length) noexcept;

class Option_Parser
{
	public:
		Option_Parser(message const&);

		template<bool CheckOptions = false>
		Option::option const* next() noexcept;
		template<bool CheckOptions = true>
		Option::option const* next(CoAP::Error& ec) noexcept;

		void reset() noexcept;
		Option::option const* current() const noexcept;
		unsigned current_number() const noexcept;
		std::size_t total_number() const noexcept;
		unsigned offset() const noexcept;
	private:
		message const& msg_;

		Option::option opt_;

		unsigned current_opt_ = 0;
		unsigned delta_ = 0;
		unsigned offset_ = 0;
};

}//Message
}//CoAP

#include "impl/parser_impl.hpp"

#endif /* COAP_TE_MESSAGE_PARSER_HPP__ */
