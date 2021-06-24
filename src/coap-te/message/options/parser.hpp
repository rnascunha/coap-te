#ifndef COAP_TE_MESSAGE_OPTIONS_PARSER_HPP__
#define COAP_TE_MESSAGE_OPTIONS_PARSER_HPP__

#include "../types.hpp"
#include "../../error.hpp"
#include "template_class.hpp"

namespace CoAP{
namespace Message{
namespace Option{

template<typename OptionCode,
		bool CheckOptions = true>
unsigned parse(option_template<OptionCode>& opt,
		std::uint8_t const* const buffer, std::size_t buffer_len,
		unsigned delta,
		CoAP::Error& ec) noexcept;

template<typename OptionCode = Option::code>
class Parser
{
	public:
		template<typename Message>
		Parser(Message const&);

		template<bool CheckOptions = false>
		option_template<OptionCode> const* next() noexcept;
		template<bool CheckOptions = true>
		option_template<OptionCode> const* next(CoAP::Error& ec) noexcept;

		void reset() noexcept;
		option_template<OptionCode> const* current() const noexcept;
		unsigned current_number() const noexcept;
		std::size_t total_number() const noexcept;
		unsigned offset() const noexcept;
	private:
		std::size_t const		length_;
		std::uint8_t const*		init_;
		std::size_t const		total_num_;

		option_template<OptionCode> opt_;

		unsigned current_opt_ = 0;
		unsigned delta_ = 0;
		unsigned offset_ = 0;
};

template<typename OptionCode,
		typename Message>
bool get_option(Message const& msg,
		option_template<OptionCode>& opt,
		OptionCode ocode, unsigned count = 0) noexcept;

}//Option
}//Message
}//CoAP

#include "impl/parser_impl.hpp"

#endif /* COAP_TE_MESSAGE_OPTIONS_PARSER_HPP__ */
