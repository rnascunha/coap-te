#ifndef COAP_TE_TRANSMISSION_REQUEST_HPP__
#define COAP_TE_TRANSMISSION_REQUEST_HPP__

#include "port/port.hpp"
#include "message/factory.hpp"

namespace CoAP{
namespace Transmission{

template<typename Callback_Functor>
class Request2{
	public:
		Request2();

		CoAP::Message::Factory<>& factory(){ return fac_; }

		void endpoint(CoAP::endpoint const& ep) noexcept
		{
			ep_ = ep;
		}

		CoAP::endpoint endpoint(CoAP::endpoint const& ep) const noexcept
		{
			return ep_;
		}

		void callback(Callback_Functor cb_func, void* data) noexcept
		{
			cb_ = cb_func;
			data_ = data;
		}

		Callback_Functor callback() noexcept
		{
			return cb_;
		}

		void* data() noexcept
		{
			return data_;
		}

		template<bool SortOptions2,
				bool CheckOpOrder2,
				bool CheckOpRepeat2>//,
//				typename ...Args>
		std::size_t serialize2(std::uint8_t* buffer,
				std::size_t buffer_len,
				std::uint16_t mid,
				CoAP::Error& ec) const noexcept
		{
			return fac_.serialize<SortOptions2, CheckOpOrder2, CheckOpRepeat2>(buffer, buffer_len, mid, ec);
//			return fac_.serialize<SortOptions, CheckOpOrder, CheckOpRepeat>(std::forward<Args>(args)...);
		}

		void reset() noexcept
		{
			fac_.reset();
			cb_ = nullptr;
			data_ = nullptr;
		}

	private:
		CoAP::Message::Factory<> fac_;
		CoAP::endpoint ep_;

		Callback_Functor cb_ = nullptr;
		void* data_ = nullptr;
};

}//Transmission
}//CoAP

#endif /* COAP_TE_TRANSMISSION_REQUEST_HPP__ */
