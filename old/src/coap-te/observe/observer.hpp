#ifndef COAP_TE_OBSERVE_OBSERVER_HPP__
#define COAP_TE_OBSERVE_OBSERVER_HPP__

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <type_traits>

#include "types.hpp"
#include "../message/types.hpp"
#include "../message/options/options.hpp"
#include "../message/options/parser.hpp"
#include "../message/options/functions2.hpp"

namespace CoAP{
namespace Observe{

template<typename Endpoint,
		bool SetOrderParameters = true>
class observe{
	using empty = struct{};
	public:
		using endpoint_t = Endpoint;
		using order_type =
				typename std::conditional<SetOrderParameters, order, empty>::type;

		template<typename Message>
		void set(endpoint_t const& ep, Message const& msg) noexcept
		{
			ep_ = ep;
			token_len_ = msg.token_len;
			std::memcpy(token_, msg.token, token_len_);

			using namespace CoAP::Message;

			used_ = true;
		}

		Endpoint const& endpoint() const noexcept{ return ep_; }
		std::uint8_t const* token() const noexcept{ return token_; }
		std::size_t token_len() const noexcept{ return token_len_; }

		template<typename Message>
		bool check(endpoint_t const& ep, Message const& msg) const noexcept
		{
			return used_ && ep == ep_ && *this == msg;
		}

		void clear() noexcept
		{
			token_len_ = 0;
			used_ = false;
		}

		bool is_used() const noexcept{ return used_; }

		template<typename Message>
		bool operator==(Message const& msg) const noexcept
		{
			return token_len_ == msg.token_len
					&& std::memcmp(token_, msg.token, token_len_) == 0;
		}

		order_type& fresh_data() noexcept
		{
			static_assert(SetOrderParameters, "Order parameter not enabled");
			return order_;
		}

	private:
		Endpoint		ep_;
		std::uint8_t	token_[8];
		std::size_t		token_len_ = 0;

		bool used_ = false;

		order_type		order_;
};

}//CoAP
}//Observe

#endif /* COAP_TE_OBSERVE_OBSERVER_HPP__ */
