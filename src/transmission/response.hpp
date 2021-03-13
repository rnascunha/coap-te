#ifndef COAP_TE_TRANSMISSION_RESPONSE_HPP__
#define COAP_TE_TRANSMISSION_RESPONSE_HPP__

#include <cstring>
#include "port/port.hpp"
#include "message/types.hpp"
#include "message/factory.hpp"
#include "message/serialize.hpp"

namespace CoAP{
namespace Transmission{

class Response{
	public:
		Response(CoAP::endpoint const& ep,
			CoAP::Message::type mtype,
			std::uint16_t mid,
			const void* token,
			std::size_t token_len,
			std::uint8_t* buffer = nullptr,
			std::size_t buffer_len = 0
			) : ep_(ep), mid_(mid), token_len_(token_len),
			buffer_(buffer), buffer_len_(buffer_len), buffer_used_(0)
		{
			std::memcpy(token_, token, token_len_);
			fac_.header(mtype == CoAP::Message::type::confirmable ?
								CoAP::Message::type::acknowledgement :
								CoAP::Message::type::nonconfirmable,
					CoAP::Message::code::content,
					token_,
					token_len_);
		}

		CoAP::Message::Factory<>& factory(){ return fac_; }

		Response& code(CoAP::Message::code rcode) noexcept
		{
			fac_.header(CoAP::Message::type::acknowledgement,
					rcode,
					token_,
					token_len_);
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

		CoAP::endpoint endpoint() noexcept
		{
			return ep_;
		}

		template<bool SortOptions = true,
				bool CheckOpOrder = !SortOptions,
				bool CheckOpRepeat = true>
		std::size_t serialize() noexcept
		{
			buffer_used_ = fac_.template serialize<SortOptions, CheckOpOrder, CheckOpRepeat>(
					buffer_, buffer_len_, mid_, ec_);
			return buffer_used_;
		}

		template<bool SortOptions = true,
						bool CheckOpOrder = !SortOptions,
						bool CheckOpRepeat = true>
		std::size_t serialize(std::uint8_t* buffer, std::size_t buffer_len) noexcept
		{
			buffer_ = buffer;
			buffer_len_ = buffer_len;
			return serialize<SortOptions, CheckOpOrder, CheckOpRepeat>();
		}

		std::size_t serialize_empty_message() noexcept
		{
			buffer_used_ = CoAP::Message::empty_message(buffer_, buffer_len_, mid_, ec_);
			return buffer_used_;
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
		CoAP::Message::Factory<> 	fac_;
		CoAP::endpoint 				ep_;
		std::uint16_t 				mid_;
		std::uint8_t				token_[8];
		std::size_t					token_len_;
		std::uint8_t*				buffer_;
		std::size_t					buffer_len_;
		std::size_t					buffer_used_;
		CoAP::Error					ec_;
};

}//Transmission
}//CoAP

#endif /* COAP_TE_TRANSMISSION_RESPONSE_HPP__ */
