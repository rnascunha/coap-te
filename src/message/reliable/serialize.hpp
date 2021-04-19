#ifndef COAP_TE_MESSAGE_RELIABLE_SERIALIZE_HPP__
#define COAP_TE_MESSAGE_RELIABLE_SERIALIZE_HPP__

#include <cstdint>

#include "defines/defaults.hpp"

#include "error.hpp"
#include "../types.hpp"
#include "../codes.hpp"
#include "../options/options.hpp"

namespace CoAP{
namespace Message{
namespace Reliable{

#if COAP_TE_RELIABLE_CONNECTION == 1

unsigned make_header(std::uint8_t* buffer, std::size_t buffer_len,
		code mcode,
		void const* const token, std::size_t token_len,
		CoAP::Error& ec) noexcept;

std::size_t set_message_length(std::uint8_t* buffer,
		std::size_t buffer_len, std::size_t buffer_used,
		std::size_t size,
		CoAP::Error& ec) noexcept;

template<bool SetLength = true,
		typename OptionCode = Option::code,
		bool SortOptions = true,
		bool CheckOpOrder = !SortOptions,
		bool CheckOpRepeat = true>
std::size_t serialize(std::uint8_t* buffer, std::size_t buffer_len,
		code mcode,
		void const* const token, std::size_t token_len,
		CoAP::Message::Option::option_template<OptionCode>* options, std::size_t option_num,
		void const* const payload, std::size_t payload_len,
		CoAP::Error& ec) noexcept;

template<bool SetLength = true,
		typename OptionCode = Option::code,
		bool SortOptions = true,
		bool CheckOpOrder = !SortOptions,
		bool CheckOpRepeat = true>
std::size_t serialize(std::uint8_t* buffer, std::size_t buffer_len,
		code mcode,
		void const* const token, std::size_t token_len,
		CoAP::Message::Option::node_option<OptionCode>*,
		void const* const payload, std::size_t payload_len,
		CoAP::Error& ec) noexcept;

template<bool SetLength = true,
		typename OptionCode = Option::code,
		bool SortOptions = true,
		bool CheckOpOrder = !SortOptions,
		bool CheckOpRepeat = true>
std::size_t serialize(std::uint8_t* buffer, std::size_t buffer_len,
		code mcode,
		void const* const token, std::size_t token_len,
		CoAP::Message::Option::List_Option<OptionCode>&,
		void const* const payload, std::size_t payload_len,
		CoAP::Error& ec) noexcept;

#endif /* COAP_TE_RELIABLE_CONNECTION == 1 */

}//Reliable
}//Message
}//CoAP

#include "impl/serialize_impl.hpp"

#endif /* COAP_TE_MESSAGE_RELIABLE_SERIALIZE_HPP__ */

