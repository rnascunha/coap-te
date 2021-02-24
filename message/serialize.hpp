#ifndef COAP_TE_SERIALIZE_HPP__
#define COAP_TE_SERIALIZE_HPP__

#include <cstdint>

#include "other/error.hpp"
#include "types.hpp"
#include "codes.hpp"
#include "options.hpp"

namespace CoAP{
namespace Message{

template<bool SortOptions = true,
		bool CheckOpOrder = !SortOptions,
		bool CheckOpRepeat = true>
std::size_t serialize(std::uint8_t* buffer, std::size_t buffer_len,
		type mtype, code mcode, std::uint16_t message_id,
		void const* const token, std::size_t token_len,
		option* options, std::size_t option_num,
		void const* const payload, std::size_t payload_len,
		CoAP::Error& ec) noexcept;

template<bool SortOptions = true,
		bool CheckOpOrder = !SortOptions,
		bool CheckOpRepeat = true>
std::size_t serialize(std::uint8_t* buffer, std::size_t buffer_len,
		type mtype, code mcode, std::uint16_t message_id,
		void const* const token, std::size_t token_len,
		option_node*,
		void const* const payload, std::size_t payload_len,
		CoAP::Error& ec) noexcept;

template<bool SortOptions = true,
		bool CheckOpOrder = !SortOptions,
		bool CheckOpRepeat = true>
std::size_t serialize(std::uint8_t* buffer, std::size_t buffer_len,
		type mtype, code mcode, std::uint16_t message_id,
		void const* const token, std::size_t token_len,
		Option_List&,
		void const* const payload, std::size_t payload_len,
		CoAP::Error& ec) noexcept;

}//Message
}//CoAP

#include "impl/serialize_impl.hpp"

#endif /* COAP_TE_SERIALIZE_HPP__ */

