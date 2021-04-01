#ifndef COAP_TE_MESSAGE_RELIABLE_FACTORY_HPP__
#define COAP_TE_MESSAGE_RELIABLE_FACTORY_HPP__

#include <cstdint>
#include "types.hpp"
#include "../types.hpp"
#include "../options/options.hpp"
#include "error.hpp"

namespace CoAP{
namespace Message{
namespace Reliable{

template<std::size_t BufferSize = 0,
		CoAP::Message::code Code = CoAP::Message::code::get>
class Factory{
	private:
		using empty = struct{};
		using buffer_type = typename std::conditional<BufferSize == 0, empty, std::uint8_t[BufferSize]>::type;
	public:
		using OptionCode = typename option_type<Code>::type;

		Factory();

		Factory& code(code) noexcept;

		Factory& token(void const* token, std::size_t token_len) noexcept;
		Factory& token(const char*) noexcept;

		Factory& add_option(CoAP::Message::Option::node_option<OptionCode>&) noexcept;

		Factory& payload(void const*, std::size_t) noexcept;
		Factory& payload(const char*) noexcept;

		Factory& reset() noexcept;

		CoAP::Message::code code() const noexcept;

		std::uint8_t const* buffer() const noexcept;

		/**
		 * To be used when internal message id is provided
		 */
		template<bool SetLength = true,
				bool SortOptions = true,
				bool CheckOpOrder = !SortOptions,
				bool CheckOpRepeat = true>
		std::size_t serialize(
				std::uint8_t* buffer, std::size_t buffer_len,
				CoAP::Error&) noexcept;

		/**
		 * To be used with internal buffer and internal message id;
		 */
		template<bool SetLength = true,
				bool SortOptions = true,
				bool CheckOpOrder = !SortOptions,
				bool CheckOpRepeat = true>
		std::size_t serialize(CoAP::Error&) noexcept;

	private:
		buffer_type			buffer_;

		CoAP::Message::code	code_ = Code;
		void const*			token_ = nullptr;
		std::size_t			token_len_ = 0;
		CoAP::Message::Option::List_Option<OptionCode>		opt_list_;
		void const*			payload_ = nullptr;
		std::size_t			payload_len_ = 0;
};

}//Reliable
}//Message
}//CoAP

#include "impl/factory_impl.hpp"

#endif /* COAP_TE_MESSAGE_RELIABLE_FACTORY_HPP__ */
