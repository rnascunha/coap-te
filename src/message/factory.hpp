#ifndef COAP_TE_MESSAGE_FACTORY_HPP__
#define COAP_TE_MESSAGE_FACTORY_HPP__

#include <cstdint>
#include "types.hpp"
#include "options.hpp"
#include "error.hpp"

namespace CoAP{
namespace Message{

template<std::size_t BufferSize = 0, typename MessageID = void*>
class Factory{
	private:
		using empty = struct{};
		using buffer_type = typename std::conditional<BufferSize == 0, empty, std::uint8_t>::type;
		using message_id_type = typename std::conditional<std::is_same<MessageID, void*>::value, empty, MessageID>::type;
	public:
		Factory();
		Factory(message_id_type&&);

		Factory& header(type, code, void const* const token = nullptr, std::size_t token_len = 0) noexcept;

		Factory& token(void const* token, std::size_t token_len) noexcept;
		Factory& token(const char*) noexcept;

		Factory& add_option(Option::node&) noexcept;

		Factory& payload(void const*, std::size_t) noexcept;
		Factory& payload(const char*) noexcept;

		Factory& reset() noexcept;

		CoAP::Message::type type() const noexcept;
		CoAP::Message::code code() const noexcept;

		template<bool SortOptions = true,
				bool CheckOpOrder = !SortOptions,
				bool CheckOpRepeat = true>
		std::size_t serialize(
				std::uint8_t* buffer, std::size_t buffer_len,
				std::uint16_t mid, CoAP::Error&) const noexcept;

		/**
		 * To be used with internal buffer;
		 */
		buffer_type const* buffer() const noexcept;
		template<bool SortOptions = true,
						bool CheckOpOrder = !SortOptions,
						bool CheckOpRepeat = true>
		std::size_t serialize(std::uint16_t mid, CoAP::Error&) noexcept;

		/**
		 * To be used when internal message id is provided
		 */
		template<bool SortOptions = true,
						bool CheckOpOrder = !SortOptions,
						bool CheckOpRepeat = true>
		std::size_t serialize(
				std::uint8_t* buffer, std::size_t buffer_len,
				CoAP::Error&) noexcept;

		/**
		 * To be used with internal buffer and internal message id;
		 */
		template<bool SortOptions = true,
						bool CheckOpOrder = !SortOptions,
						bool CheckOpRepeat = true>
		std::size_t serialize(CoAP::Error&) noexcept;

	private:
		buffer_type		buffer_[BufferSize];
		message_id_type	mid_;

		CoAP::Message::type	type_ = type::confirmable;
		CoAP::Message::code	code_ = code::get;
		void const*			token_ = nullptr;
		std::size_t			token_len_ = 0;
		Option::List		opt_list_;
		void const*			payload_ = nullptr;
		std::size_t			payload_len_ = 0;
};

}//Message
}//CoAP

#include "impl/factory_impl.hpp"

#endif /* COAP_TE_MESSAGE_FACTORY_HPP__ */
