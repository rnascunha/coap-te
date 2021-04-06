#ifndef COAP_TE_TRANSMISSION_RELIABLE_REQUEST_HPP__
#define COAP_TE_TRANSMISSION_RELIABLE_REQUEST_HPP__

#include "types.hpp"
#include "message/codes.hpp"
#include "message/reliable/factory.hpp"

namespace CoAP{
namespace Transmission{
namespace Reliable{

template<typename Callback_Functor,
		CoAP::Message::code Code = CoAP::Message::code::get>
class Request{
	public:
		/**
		 * This constructor is to be used by clients that
		 * already have socket defined
		 */
		Request() : socket_(0){}
		Request(int socket) : socket_(socket){}
		Request(separate_response const& data, CoAP::Message::code mcode)
		: socket_(data.socket)
		{
			fac_.code(mcode);
			fac_.token(data.token, data.token_len);
		}

		CoAP::Message::Reliable::Factory<0, Code>& factory(){ return fac_; }

		template<typename ...Args>
		Request& code(Args&& ...args) noexcept
		{
			fac_.code(std::forward<Args>(args)...);
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

		Request& socket(int socket) noexcept
		{
			socket_ = socket;
			return *this;
		}

		Request& callback(Callback_Functor cb_func, void* data = nullptr) noexcept
		{
			cb_ = cb_func;
			data_ = data;

			return *this;
		}

		int socket() noexcept
		{
			return socket_;
		}

		Callback_Functor callback() noexcept
		{
			return cb_;
		}

		void* data() noexcept
		{
			return data_;
		}

		template<bool SetLength,
				bool SortOptions,
				bool CheckOpOrder,
				bool CheckOpRepeat,
				typename ...Args>
		std::size_t serialize(Args&&... args) const noexcept
		{
			return fac_.template serialize<SetLength, SortOptions, CheckOpOrder, CheckOpRepeat>(std::forward<Args>(args)...);
		}

		void reset() noexcept
		{
			fac_.reset();
			cb_ = nullptr;
			data_ = nullptr;
		}

	private:
		CoAP::Message::Reliable::Factory<0, Code> fac_;
		int socket_;

		Callback_Functor cb_ = nullptr;
		void* data_ = nullptr;
};

}//CoAP
}//Transmission
}//Reliable

#endif /* COAP_TE_TRANSMISSION_RELIABLE_REQUEST_HPP__ */
