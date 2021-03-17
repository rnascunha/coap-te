#ifndef COAP_TE_MESSAGE_SERIALIZE_HPP__
#define COAP_TE_MESSAGE_SERIALIZE_HPP__

#include <cstdint>

#include "error.hpp"
#include "types.hpp"
#include "codes.hpp"
#include "options.hpp"

namespace CoAP{
namespace Message{

unsigned make_header(std::uint8_t* buffer, std::size_t buffer_len,
		type mtype, code mcode, std::uint16_t message_id,
		void const* const token, std::size_t token_len,
		CoAP::Error& ec) noexcept;

template<bool SortOptions = true,
		bool CheckOpOrder = !SortOptions,
		bool CheckOpRepeat = true>
unsigned make_options(std::uint8_t* buffer, std::size_t buffer_len,
		Option::option* options, std::size_t option_num,
		CoAP::Error& ec) noexcept;

template<bool SortOptions = true,
		bool CheckOpOrder = !SortOptions,
		bool CheckOpRepeat = true>
unsigned make_options(std::uint8_t* buffer,
		std::size_t buffer_len,
		Option::node* list,
		CoAP::Error& ec) noexcept;

template<bool CheckOpOrder = true,
		bool CheckOpRepeat = true>
unsigned make_option(std::uint8_t* buffer, std::size_t buffer_len,
		Option::option const& option, unsigned& delta,
		Option::code& last_option,
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

}//Message
}//CoAP

#include "impl/serialize_impl.hpp"

#endif /* COAP_TE_MESSAGE_SERIALIZE_HPP__ */

