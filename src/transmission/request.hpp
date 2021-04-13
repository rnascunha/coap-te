#ifndef COAP_TE_TRANSMISSION_REQUEST_HPP__
#define COAP_TE_TRANSMISSION_REQUEST_HPP__

#include "defines/defaults.hpp"
#include "types.hpp"
#include "port/port.hpp"
#include "message/factory.hpp"

#if COAP_TE_OBSERVABLE_RESOURCE == 1
#include "observe/observer.hpp"
#endif /* COAP_TE_OBSERVABLE_RESOURCE == 1 */

namespace CoAP{
namespace Transmission{

template<typename Endpoint,
		typename Callback_Functor>
class Request{
	public:
		Request(Endpoint const& ep) : ep_(ep){}
		Request(separate_response<Endpoint>const& data, CoAP::Message::code mcode)
		: ep_(data.ep)
		{
			fac_.header(data.type, mcode, data.token, data.token_len);
		}

		Request(separate_response<Endpoint>const& data, CoAP::Message::type mtype, CoAP::Message::code mcode)
		: ep_(data.ep)
		{
			fac_.header(mtype, mcode, data.token, data.token_len);
		}

#if COAP_TE_OBSERVABLE_RESOURCE == 1
		Request(CoAP::Observe::observe<Endpoint> const& obs,
				CoAP::Message::type mtype,
				CoAP::Message::code mcode = CoAP::Message::code::content)
		: ep_(obs.endpoint())
		{
			fac_.header(mtype,
					mcode,
					obs.token(), obs.token_len());
		}

		Request(CoAP::Observe::observe<Endpoint> const& obs,
				CoAP::Message::code mcode = CoAP::Message::code::content)
		: ep_(obs.endpoint())
		{
			fac_.header(CoAP::Message::type::nonconfirmable,
					mcode,
					obs.token(), obs.token_len());
		}
#endif /* COAP_TE_OBSERVABLE_RESOURCE == 1 */

		CoAP::Message::Factory<>& factory(){ return fac_; }

		template<typename ...Args>
		Request& header(Args&& ...args) noexcept
		{
			fac_.header(std::forward<Args>(args)...);
			return *this;
		}

		template<typename ...Args>
		Request& token(Args&& ...args) noexcept
		{
			fac_.token(std::forward<Args>(args)...);
			return *this;
		}

		template<typename ...Args>
		Request& add_option(Args&& ...args) noexcept
		{
			fac_.add_option(std::forward<Args>(args)...);
			return *this;
		}

		template<typename ...Args>
		Request& payload(Args&& ...args) noexcept
		{
			fac_.payload(std::forward<Args>(args)...);
			return *this;
		}

		Request& endpoint(Endpoint const& ep) noexcept
		{
			ep_ = ep;
			return *this;
		}

		Request& callback(Callback_Functor cb_func, void* data = nullptr) noexcept
		{
			cb_ = cb_func;
			data_ = data;

			return *this;
		}

		Endpoint& endpoint() noexcept
		{
			return ep_;
		}

		Callback_Functor callback() noexcept
		{
			return cb_;
		}

		void* data() noexcept
		{
			return data_;
		}

		template<bool SortOptions,
						bool CheckOpOrder,
						bool CheckOpRepeat,
						typename ...Args>
		std::size_t serialize(Args&&... args) const noexcept
		{
			return fac_.template serialize<SortOptions, CheckOpOrder, CheckOpRepeat>(std::forward<Args>(args)...);
		}

		void reset() noexcept
		{
			fac_.reset();
			cb_ = nullptr;
			data_ = nullptr;
		}

	private:
		CoAP::Message::Factory<> fac_;
		Endpoint ep_;

		Callback_Functor cb_ = nullptr;
		void* data_ = nullptr;
};

}//Transmission
}//CoAP

#endif /* COAP_TE_TRANSMISSION_REQUEST_HPP__ */
