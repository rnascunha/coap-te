#ifndef COAP_TE_MESSAGE_SERIALIZE_HPP__
#define COAP_TE_MESSAGE_SERIALIZE_HPP__

#include <cstdint>

#include "error.hpp"
#include "types.hpp"
#include "codes.hpp"
#include "options.hpp"

namespace CoAP{
namespace Message{

std::size_t empty_message(std::uint8_t* buffer, std::size_t buffer_len,
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

