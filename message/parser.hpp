#ifndef COAP_TE_PARSER_HPP__
#define COAP_TE_PARSER_HPP__

#include "other/error.hpp"
#include "types.hpp"
#include "options.hpp"

namespace CoAP{
namespace Message{

unsigned parse(message& msg,
		std::uint8_t const* const buffer, std::size_t buffer_len,
		CoAP::Error&);

template<bool CheckOptions = true>
unsigned parse_option(option& opt,
		std::uint8_t const* const buffer, std::size_t buffer_len,
		unsigned delta,
		CoAP::Error& ec);

class Option_Parser
{
	public:
		Option_Parser(message const&);

		template<bool CheckOptions = false>
		option const* next() noexcept;
		template<bool CheckOptions = true>
		option const* next(CoAP::Error& ec) noexcept;

		void reset() noexcept;
		option const* current() noexcept;
		unsigned current_number() noexcept;
		std::size_t total_number() noexcept;
	private:
		message const& msg_;

		option opt_;

		unsigned current_opt_ = 0;
		unsigned delta_ = 0;
		unsigned offset_ = 0;
};

}//Message
}//CoAP

#include "impl/parser_impl.hpp"

#endif /* COAP_TE_PARSER_HPP__ */
