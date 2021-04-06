#ifndef COAP_TE_MESSAGE_RELIABLE_FACTORY_IMPL_HPP__
#define COAP_TE_MESSAGE_RELIABLE_FACTORY_IMPL_HPP__

#include "../factory.hpp"
#include "../serialize.hpp"

namespace CoAP{
namespace Message{
namespace Reliable{

template<std::size_t BufferSize,
	CoAP::Message::code Code>
Factory<BufferSize, Code>::
Factory(){}

template<std::size_t BufferSize,
	CoAP::Message::code Code>
Factory<BufferSize, Code>&
Factory<BufferSize, Code>::
code(CoAP::Message::code _code) noexcept
{
	static_assert(std::is_same<OptionCode, CoAP::Message::Option::code>::value, "Must be 'Option::code' type");
	code_ = _code;

	return *this;
}

template<std::size_t BufferSize,
	CoAP::Message::code Code>
Factory<BufferSize, Code>&
Factory<BufferSize, Code>::
token(void const* token, std::size_t token_len) noexcept
{
	token_ = token;
	token_len_ =  token_len;

	return *this;
}

template<std::size_t BufferSize,
	CoAP::Message::code Code>
Factory<BufferSize, Code>&
Factory<BufferSize, Code>::
token(const char* token) noexcept
{
	token_ = token;
	token_len_ = std::strlen(token);

	return *this;
}

template<std::size_t BufferSize,
	CoAP::Message::code Code>
Factory<BufferSize, Code>&
Factory<BufferSize, Code>::
add_option(Option::node_option<OptionCode>& node) noexcept
{
	opt_list_.add(node);

	return *this;
}

template<std::size_t BufferSize,
	CoAP::Message::code Code>
Factory<BufferSize, Code>&
Factory<BufferSize, Code>::
payload(void const* pd, std::size_t payload_len) noexcept
{
	payload_ = pd;
	payload_len_ = payload_len;

	return *this;
}

template<std::size_t BufferSize,
	CoAP::Message::code Code>
Factory<BufferSize, Code>&
Factory<BufferSize, Code>::
payload(const char* pd) noexcept
{
	payload_ = pd;
	payload_len_ = std::strlen(pd);

	return *this;
}

template<std::size_t BufferSize,
	CoAP::Message::code Code>
Factory<BufferSize, Code>&
Factory<BufferSize, Code>::
reset() noexcept
{
	code_ = code::get;
	token_ = nullptr;
	token_len_ = 0;
	opt_list_.clear();
	payload_ = nullptr;
	payload_len_ = 0;

	return *this;
}

template<std::size_t BufferSize,
	CoAP::Message::code Code>
CoAP::Message::code
Factory<BufferSize, Code>::
code() const noexcept
{
	return code_;
}

template<std::size_t BufferSize,
	CoAP::Message::code Code>
std::uint8_t const*
Factory<BufferSize, Code>::
buffer() const noexcept
{
	return buffer_;
}

template<std::size_t BufferSize,
	CoAP::Message::code Code>
template<bool SetLength /* = true */,
		bool SortOptions /* = true */,
		bool CheckOpOrder /* = !SortOptions */,
		bool CheckOpRepeat /* = true */>
std::size_t
Factory<BufferSize, Code>::
serialize(
		std::uint8_t* buffer, std::size_t buffer_len,
		CoAP::Error& ec) const noexcept
{
	return CoAP::Message::Reliable::serialize<SetLength, OptionCode, SortOptions, CheckOpOrder, CheckOpRepeat>(
				buffer, buffer_len,
				code_,
				token_, token_len_,
				opt_list_.head(),
				payload_, payload_len_,
				ec);

}

template<std::size_t BufferSize,
	CoAP::Message::code Code>
template<bool SetLength /* = true */,
		bool SortOptions /* = true */,
		bool CheckOpOrder /* = !SortOptions */,
		bool CheckOpRepeat /* = true */>
std::size_t
Factory<BufferSize, Code>::
serialize(CoAP::Error& ec) noexcept
{
	return CoAP::Message::Reliable::serialize<SetLength, OptionCode, SortOptions, CheckOpOrder, CheckOpRepeat>(
				buffer_, BufferSize,
				code_,
				token_, token_len_,
				opt_list_.head(),
				payload_, payload_len_,
				ec);
}

}//Reliable
}//Message
}//CoAP

#endif /* COAP_TE_MESSAGE_RELIABLE_FACTORY_IMPL_HPP__ */
