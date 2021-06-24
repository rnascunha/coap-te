#ifndef COAP_TE_MESSAGE_RELIABLE_SERIALIZE_IMPL_HPP__
#define COAP_TE_MESSAGE_RELIABLE_SERIALIZE_IMPL_HPP__

#include <cstdint>

#include "../../../defines/defaults.hpp"

#include "../serialize.hpp"
#include "../../serialize.hpp"

namespace CoAP{
namespace Message{
namespace Reliable{

#if COAP_TE_RELIABLE_CONNECTION == 1

template<bool SetLength /* = true */,
		typename OptionCode /* = Option::code */,
		bool SortOptions /* = true */,
		bool CheckOpOrder /* = !SortOptions */,
		bool CheckOpRepeat /* = true */>
std::size_t serialize(std::uint8_t* buffer, std::size_t buffer_len,
		code mcode,
		void const* const token, std::size_t token_len,
		CoAP::Message::Option::option_template<OptionCode>* options, std::size_t option_num,
		void const* const payload, std::size_t payload_len,
		CoAP::Error& ec) noexcept
{
	std::size_t offset = make_header(
					buffer, buffer_len,
					mcode,
					token, token_len, ec);
	if(ec) return offset;

	offset += make_options<OptionCode, SortOptions, CheckOpOrder, CheckOpRepeat>
			(buffer + offset, buffer_len - offset, options, option_num, ec);
	if(ec) return offset;

	offset += make_payload(buffer + offset, buffer_len - offset, payload, payload_len, ec);
	if(ec) return offset;

	return SetLength ? set_message_length(buffer, buffer_len, offset, offset - 2 - token_len, ec) : offset;
}

template<bool SetLength /* = true */,
		typename OptionCode /* = Option::code */,
		bool SortOptions /* = true */,
		bool CheckOpOrder /* = !SortOptions */,
		bool CheckOpRepeat /* = true */>
std::size_t serialize(std::uint8_t* buffer, std::size_t buffer_len,
		code mcode,
		void const* const token, std::size_t token_len,
		CoAP::Message::Option::node_option<OptionCode>* options,
		void const* const payload, std::size_t payload_len,
		CoAP::Error& ec) noexcept
{
	std::size_t offset = make_header(
							buffer, buffer_len,
							mcode,
							token, token_len, ec);
	if(ec) return offset;

	offset += make_options<OptionCode, SortOptions, CheckOpOrder, CheckOpRepeat>
				(buffer + offset, buffer_len - offset, options, ec);
	if(ec)return offset;

	offset += make_payload(buffer + offset, buffer_len - offset, payload, payload_len, ec);
	if(ec) return offset;

	return SetLength ? set_message_length(buffer, buffer_len, offset, offset - 2 - token_len, ec) : offset;
}

template<bool SetLength /* = true */,
		typename OptionCode /* = Option::code */,
		bool SortOptions /* = true */,
		bool CheckOpOrder /* = !SortOptions */,
		bool CheckOpRepeat /* = true */>
std::size_t serialize(std::uint8_t* buffer, std::size_t buffer_len,
		code mcode,
		void const* const token, std::size_t token_len,
		CoAP::Message::Option::List_Option<OptionCode>& options,
		void const* const payload, std::size_t payload_len,
		CoAP::Error& ec) noexcept
{
	std::size_t offset = make_header(
								buffer, buffer_len,
								mcode,
								token, token_len, ec);
	if(ec) return offset;

	offset += make_options<OptionCode, SortOptions, CheckOpOrder, CheckOpRepeat>
				(buffer + offset, buffer_len - offset, options.head(), ec);
	if(ec) return offset;

	offset += make_payload(buffer + offset, buffer_len - offset, payload, payload_len, ec);
	if(ec) return offset;

	return SetLength ? set_message_length(buffer, buffer_len, offset, offset - 2 - token_len, ec) : offset;
}

#endif /* COAP_TE_RELIABLE_CONNECTION == 1 */

}//Reliable
}//Message
}//CoAP

#endif /* COAP_TE_MESSAGE_RELIABLE_SERIALIZE_IMPL_HPP__ */

