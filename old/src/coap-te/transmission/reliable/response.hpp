#ifndef COAP_TE_TRANSMISSION_RELIABLE_RESPONSE_HPP__
#define COAP_TE_TRANSMISSION_RELIABLE_RESPONSE_HPP__

#include "../../message/reliable/factory.hpp"

namespace CoAP{
namespace Transmission{
namespace Reliable{

template<typename Handler>
class Response{
	public:
		Response(Handler socket,
			const void* token,
			std::size_t token_len,
			std::uint8_t* buffer = nullptr,
			std::size_t buffer_len = 0
			) : socket_(socket), token_len_(token_len),
			buffer_(buffer), buffer_len_(buffer_len), buffer_used_(0)
		{
			std::memcpy(token_, token, token_len_);
			fac_.code(CoAP::Message::code::content);
			fac_.token(token_, token_len_);
		}

		CoAP::Message::Reliable::Factory<>& factory(){ return fac_; }

		Response& code(CoAP::Message::code rcode) noexcept
		{
			fac_.code(rcode);
			return *this;
		}

		template<typename ...Args>
		Response& add_option(Args&& ...args) noexcept
		{
			fac_.add_option(std::forward<Args>(args)...);
			return *this;
		}

		template<typename ...Args>
		Response& payload(Args&& ...args) noexcept
		{
			fac_.payload(std::forward<Args>(args)...);
			return *this;
		}

		Handler const& socket() const noexcept
		{
			return socket_;
		}

		template<bool SetLength = true,
				bool SortOptions = true,
				bool CheckOpOrder = !SortOptions,
				bool CheckOpRepeat = true>
		std::size_t serialize() noexcept
		{
			buffer_used_ = fac_.template serialize<SetLength,
													SortOptions, CheckOpOrder, CheckOpRepeat>(
					buffer_, buffer_len_, ec_);
			return buffer_used_;
		}

		template<bool SetLength = true,
				bool SortOptions = true,
				bool CheckOpOrder = !SortOptions,
				bool CheckOpRepeat = true>
		std::size_t serialize(std::uint8_t* buffer, std::size_t buffer_len) noexcept
		{
			buffer_ = buffer;
			buffer_len_ = buffer_len;
			return serialize<SetLength,
					SortOptions, CheckOpOrder, CheckOpRepeat>();
		}

		std::uint8_t* buffer() noexcept{ return buffer_; }
		std::size_t buffer_used() const noexcept{ return buffer_used_; }
		CoAP::Error error() const noexcept{ return ec_; }

		void reset() noexcept
		{
			fac_.reset();
			buffer_used_ = 0;
			buffer_len_ = 0;
			buffer_ = nullptr;
		}

	private:
		CoAP::Message::Reliable::Factory<>
									fac_;
		Handler	 					socket_;
		std::uint8_t				token_[8];
		std::size_t					token_len_;
		std::uint8_t*				buffer_;
		std::size_t					buffer_len_;
		std::size_t					buffer_used_;
		CoAP::Error					ec_;
};

}//CoAP
}//Transmission
}//Reliable

#endif /* COAP_TE_TRANSMISSION_RELIABLE_RESPONSE_HPP__ */
