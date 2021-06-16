#ifndef COAP_TE_MESSAGE_SERIALIZE_HPP__
#define COAP_TE_MESSAGE_SERIALIZE_HPP__

#include <cstdint>

#include "defines/defaults.hpp"

#include "error.hpp"
#include "types.hpp"
#include "codes.hpp"
#include "options/options.hpp"

namespace CoAP{
namespace Message{

unsigned make_header(std::uint8_t* buffer, std::size_t buffer_len,
		type mtype, code mcode, std::uint16_t message_id,
		void const* const token, std::size_t token_len,
		CoAP::Error& ec) noexcept;

template<typename OptionCode = Option::code,
		bool SortOptions = true,
		bool CheckOpOrder = !SortOptions,
		bool CheckOpRepeat = true>
unsigned make_options(std::uint8_t* buffer, std::size_t buffer_len,
		Option::option_template<OptionCode>* options, std::size_t option_num,
		CoAP::Error& ec) noexcept;

template<typename OptionCode = Option::code,
		bool SortOptions = true,
		bool CheckOpOrder = !SortOptions,
		bool CheckOpRepeat = true>
unsigned make_options(std::uint8_t* buffer,
		std::size_t buffer_len,
		Option::node_option<OptionCode>* list,
		CoAP::Error& ec) noexcept;

template<typename OptionCode = Option::code,
		bool CheckOpOrder = true,
		bool CheckOpRepeat = true>
unsigned make_option(std::uint8_t* buffer, std::size_t buffer_len,
		Option::option_template<OptionCode> const& option, unsigned& delta,
		OptionCode& last_option,
		CoAP::Error& ec) noexcept;

unsigned make_payload(uint8_t* buffer, std::size_t buffer_len,
		void const* const payload, std::size_t payload_len,
		CoAP::Error& ec) noexcept;

std::size_t empty_message(type mtype, std::uint8_t* buffer, std::size_t buffer_len,
		std::uint16_t mid, CoAP::Error&) noexcept;

template<bool SortOptions = true,
		bool CheckOpOrder = !SortOptions,
		bool CheckOpRepeat = true>
std::size_t serialize(std::uint8_t* buffer, std::size_t buffer_len,
		type mtype, code mcode, std::uint16_t message_id,
		void const* const token, std::size_t token_len,
		Option::option* options, std::size_t option_num,
		void const* const payload, std::size_t payload_len,
		CoAP::Error& ec) noexcept;

template<bool SortOptions = true,
		bool CheckOpOrder = !SortOptions,
		bool CheckOpRepeat = true>
std::size_t serialize(std::uint8_t* buffer, std::size_t buffer_len,
		type mtype, code mcode, std::uint16_t message_id,
		void const* const token, std::size_t token_len,
		Option::node*,
		void const* const payload, std::size_t payload_len,
		CoAP::Error& ec) noexcept;

template<bool SortOptions = true,
		bool CheckOpOrder = !SortOptions,
		bool CheckOpRepeat = true>
std::size_t serialize(std::uint8_t* buffer, std::size_t buffer_len,
		type mtype, code mcode, std::uint16_t message_id,
		void const* const token, std::size_t token_len,
		Option::List&,
		void const* const payload, std::size_t payload_len,
		CoAP::Error& ec) noexcept;

class Serialize{
	public:
		Serialize(std::uint8_t* buffer, std::size_t buffer_size);
		Serialize(std::uint8_t* buffer, std::size_t buffer_size, message&);

		std::uint8_t const* buffer() const noexcept;
		std::size_t	size() const noexcept;
		std::size_t used() const noexcept;

		message const& get_message() const noexcept;

		bool header(type, code, std::uint16_t message_id) noexcept;

		bool token(const char*) noexcept;
		bool token(const void*, std::size_t) noexcept;

		template<bool CheckRepeat = true>
		bool add_option(Option::option&&) noexcept;
		template<bool CheckRepeat = true>
		bool add_option(Option::option&&, CoAP::Error&) noexcept;

		bool payload(const char*) noexcept;
		bool payload(const void*, std::size_t) noexcept;

		bool remove_token() noexcept;
		bool remove_option(Option::code) noexcept;
		bool remove_option(Option::code, CoAP::Error&) noexcept;
		bool remove_payload() noexcept;
	private:
		std::uint8_t* 		buffer_;
		std::size_t const 	size_;

		message 			msg_;
};

}//Message
}//CoAP

#include "impl/serialize_impl.hpp"

#endif /* COAP_TE_MESSAGE_SERIALIZE_HPP__ */

