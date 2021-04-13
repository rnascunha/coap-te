#ifndef COAP_TE_OBSERVE_OBSERVER_HPP__
#define COAP_TE_OBSERVE_OBSERVER_HPP__

#include <cstdint>
#include <cstdlib>
#include <cstring>

#include "types.hpp"
#include "message/types.hpp"
#include "message/options/options.hpp"
#include "message/options/parser.hpp"
#include "message/options/functions2.hpp"

namespace CoAP{
namespace Observe{

template<typename Endpoint>
class observe{
	public:
		using endpoint_t = Endpoint;

		template<typename Message>
		void set(endpoint_t const& ep, Message const& msg) noexcept
		{
			ep_ = ep;
			token_len_ = msg.token_len;
			std::memcpy(token_, msg.token, token_len_);

			using namespace CoAP::Message;
			Option::option opt;
			if(Option::get_option(msg, opt, Option::code::accept))
			{
				format_ = static_cast<content_format>(Option::parse_unsigned(opt));
			}

			used_ = true;
		}

		Endpoint const& endpoint() const noexcept{ return ep_; }
		std::uint8_t const* token() const noexcept{ return token_; }
		std::size_t token_len() const noexcept{ return token_len_; }
		CoAP::Message::content_format format() const noexcept { return format_; }

		template<typename Message>
		bool check(endpoint_t const& ep, Message const& msg) const noexcept
		{
			return used_ && ep == ep_ && *this == msg;
		}

		void clear() noexcept
		{
			token_len_ = 0;
			format_ = invalid_format;
			used_ = false;
		}

		bool is_used() const noexcept{ return used_; }

		template<typename Message>
		bool operator==(Message const& msg) const noexcept
		{
			return token_len_ == msg.token_len
					&& std::memcmp(token_, msg.token, token_len_) == 0;
		}

	private:
		Endpoint		ep_;
		std::uint8_t	token_[8];
		std::size_t		token_len_ = 0;
		CoAP::Message::content_format format_ = invalid_format;

		bool used_ = false;
};

}//CoAP
}//Observe

#endif /* COAP_TE_OBSERVE_OBSERVER_HPP__ */
