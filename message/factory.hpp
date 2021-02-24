#ifndef COAP_TE_MESSAGE_FACTORY_HPP__
#define COAP_TE_MESSAGE_FACTORY_HPP__

#include <cstdint>
#include "types.hpp"
#include "options.hpp"
#include "other/error.hpp"

namespace CoAP{
namespace Message{

template<std::size_t BufferSize = 0>
class Factory{
	private:
		using empty = struct{};
		using buffer_type = typename std::conditional<BufferSize == 0, empty, std::uint8_t>::type;
	public:
		Factory();

		void header(type, code, void const* const token = nullptr, std::size_t token_len = 0) noexcept;

		void token(void const* token, std::size_t token_len) noexcept;
		void token(const char*) noexcept;

		void add_option(option_node&) noexcept;

		void payload(void const*, std::size_t) noexcept;
		void payload(const char*) noexcept;

		void reset() noexcept;

		template<bool SortOptions = true,
				bool CheckOpOrder = !SortOptions,
				bool CheckOpRepeat = true>
		std::size_t serialize(std::uint8_t* buffer,
				std::size_t buffer_len,
				std::uint16_t mid, CoAP::Error&) const noexcept;

		/**
		 * To be used with internal buffer;
		 */
		buffer_type const* buffer() const noexcept;
		template<bool SortOptions = true,
						bool CheckOpOrder = !SortOptions,
						bool CheckOpRepeat = true>
		std::size_t serialize(std::uint16_t mid, CoAP::Error&) noexcept;
	private:
		buffer_type		buffer_[BufferSize];

		type			type_ = type::confirmable;
		code			code_ = code::get;
		void const*		token_ = nullptr;
		std::size_t		token_len_ = 0;
		Option_List		opt_list_;
		void const*		payload_ = nullptr;
		std::size_t		payload_len_ = 0;
};

}//Message
}//CoAP

#include "impl/factory_impl.hpp"

#endif /* COAP_TE_MESSAGE_FACTORY_HPP__ */
