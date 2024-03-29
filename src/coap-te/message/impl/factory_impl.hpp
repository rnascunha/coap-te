#ifndef COAP_TE_MESSAGE_FACTORY_IMPL_HPP__
#define COAP_TE_MESSAGE_FACTORY_IMPL_HPP__

#include "../factory.hpp"
#include "../serialize.hpp"

namespace CoAP{
namespace Message{

template<std::size_t BufferSize, typename MessageID>
Factory<BufferSize, MessageID>::
Factory()
{
	static_assert(std::is_same<MessageID, void*>::value);
}

template<std::size_t BufferSize, typename MessageID>
Factory<BufferSize, MessageID>::
Factory(message_id_type&& mid) : mid_(std::move(mid))
{
	static_assert(!std::is_same<MessageID, void*>::value);
}

template<std::size_t BufferSize, typename MessageID>
Factory<BufferSize, MessageID>&
Factory<BufferSize, MessageID>::
header(CoAP::Message::type _type, CoAP::Message::code _code,
		void const* const token /* = nullptr */, std::size_t token_len /* = 0 */) noexcept
{
	type_ = _type;
	code_ = _code;
	token_ = token;
	token_len_ =  token_len;

	return *this;
}

template<std::size_t BufferSize, typename MessageID>
Factory<BufferSize, MessageID>&
Factory<BufferSize, MessageID>::
token(void const* token, std::size_t token_len) noexcept
{
	token_ = token;
	token_len_ =  token_len;

	return *this;
}

template<std::size_t BufferSize, typename MessageID>
Factory<BufferSize, MessageID>&
Factory<BufferSize, MessageID>::
token(const char* token) noexcept
{
	token_ = token;
	token_len_ = std::strlen(token);

	return *this;
}

template<std::size_t BufferSize, typename MessageID>
Factory<BufferSize, MessageID>&
Factory<BufferSize, MessageID>::
add_option(Option::node& node) noexcept
{
	opt_list_.add(node);

	return *this;
}

template<std::size_t BufferSize, typename MessageID>
Factory<BufferSize, MessageID>&
Factory<BufferSize, MessageID>::
payload(void const* pd, std::size_t payload_len) noexcept
{
	payload_ = pd;
	payload_len_ = payload_len;

	return *this;
}

template<std::size_t BufferSize, typename MessageID>
Factory<BufferSize, MessageID>&
Factory<BufferSize, MessageID>::
payload(const char* pd) noexcept
{
	payload_ = pd;
	payload_len_ = std::strlen(pd);

	return *this;
}

template<std::size_t BufferSize, typename MessageID>
Factory<BufferSize, MessageID>&
Factory<BufferSize, MessageID>::
reset() noexcept
{
	type_ = type::confirmable;
	code_ = code::get;
	token_ = nullptr;
	token_len_ = 0;
	opt_list_.clear();
	payload_ = nullptr;
	payload_len_ = 0;

	return *this;
}

template<std::size_t BufferSize, typename MessageID>
template<bool SortOptions /* = true */,
				bool CheckOpOrder /* = !SortOptions */,
				bool CheckOpRepeat /* = true */>
std::size_t
Factory<BufferSize, MessageID>::
serialize(
		std::uint8_t* buffer, std::size_t buffer_len,
		std::uint16_t mid, CoAP::Error& ec) const noexcept
{
	return CoAP::Message::serialize<SortOptions, CheckOpOrder, CheckOpRepeat>(
			buffer, buffer_len,
			type_, code_, mid,
			token_, token_len_,
			opt_list_.head(),
			payload_, payload_len_,
			ec);
}

template<std::size_t BufferSize, typename MessageID>
std::uint8_t const*
Factory<BufferSize, MessageID>::
buffer() const noexcept
{
	static_assert(BufferSize != 0, "Can't be used with internal buffer disabled [BufferSize == 0]");
	return buffer_;
}

template<std::size_t BufferSize, typename MessageID>
template<bool SortOptions /* = true */,
		bool CheckOpOrder /* = !SortOptions */,
		bool CheckOpRepeat /* = true */>
std::size_t
Factory<BufferSize, MessageID>::
serialize(
		std::uint16_t mid, CoAP::Error& ec) noexcept
{
	static_assert(BufferSize != 0, "Can't be used with internal buffer disabled [BufferSize == 0]");
	return CoAP::Message::serialize<SortOptions, CheckOpOrder, CheckOpRepeat>(
			buffer_, BufferSize,
			type_, code_, mid,
			token_, token_len_,
			opt_list_.head(),
			payload_, payload_len_,
			ec);
}

template<std::size_t BufferSize, typename MessageID>
template<bool SortOptions /* = true */,
		bool CheckOpOrder /* = !SortOptions */,
		bool CheckOpRepeat /* = true */>
std::size_t
Factory<BufferSize, MessageID>::
serialize(
		std::uint8_t* buffer, std::size_t buffer_len,
		CoAP::Error& ec) noexcept
{
	static_assert(!std::is_same<MessageID, void*>::value);
	return CoAP::Message::serialize<SortOptions, CheckOpOrder, CheckOpRepeat>(
				buffer, buffer_len,
				type_, code_, mid_(),
				token_, token_len_,
				opt_list_.head(),
				payload_, payload_len_,
				ec);

}

template<std::size_t BufferSize, typename MessageID>
template<bool SortOptions /* = true */,
		bool CheckOpOrder /* = !SortOptions */,
		bool CheckOpRepeat /* = true */>
std::size_t
Factory<BufferSize, MessageID>::
serialize(CoAP::Error& ec) noexcept
{
	static_assert(!std::is_same<MessageID, void*>::value && BufferSize != 0);
	return CoAP::Message::serialize<SortOptions, CheckOpOrder, CheckOpRepeat>(
				buffer_, BufferSize,
				type_, code_, mid_(),
				token_, token_len_,
				opt_list_.head(),
				payload_, payload_len_,
				ec);
}

template<std::size_t BufferSize, typename MessageID>
CoAP::Message::type
Factory<BufferSize, MessageID>::
type() const noexcept
{
	return type_;
}

template<std::size_t BufferSize, typename MessageID>
CoAP::Message::code
Factory<BufferSize, MessageID>::
code() const noexcept
{
	return code_;
}

}//Message
}//Factory

#endif /* COAP_TE_MESSAGE_FACTORY_IMPL_HPP__ */
