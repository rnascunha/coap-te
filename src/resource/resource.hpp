#ifndef COAP_TE_RESOURCE_HPP__
#define COAP_TE_RESOURCE_HPP__

#include <cstring>
#include "message/codes.hpp"
#include "internal/list.hpp"

namespace CoAP{
namespace Resource{

template<typename Callback_Functor>
class resource
{
	public:
		using callback_t = Callback_Functor;
		resource(const char* path,
				callback_t get = nullptr, callback_t post = nullptr,
				callback_t put = nullptr, callback_t del = nullptr)
		: path_(path), get_(get), post_(post), put_(put), del_(del){}

		const char* path() const noexcept{ return path_; }
		resource& get(callback_t cb = nullptr) noexcept
		{
			get_ = cb;
			return *this;
		}

		resource& post(callback_t cb = nullptr) noexcept
		{
			post_ = cb;
			return *this;
		}

		resource& put(callback_t cb = nullptr) noexcept
		{
			put_ = cb;
			return *this;
		}

		resource& del(callback_t cb = nullptr) noexcept
		{
			del_ = cb;
			return *this;
		}

		template<typename Endpoint>
		bool call(CoAP::Message::code code,
			CoAP::Message::message const& request,
			CoAP::Transmission::Response<Endpoint>& response) const noexcept
		{
			switch(code)
			{
				using namespace CoAP::Message;
				case code::get:
					return get_ ? get_(request, response), true : false;
					break;
				case code::post:
					return post_ ? post_(request, response), true : false;
					break;
				case code::put:
					return put_ ? put_(request, response), true : false;
					break;
				case code::cdelete:
					return del_ ? del_(request, response), true : false;
					break;
				default:
					break;
			}
			return false;
		}

		bool operator==(const char* path) const noexcept
		{
			return std::strcmp(path, path_) == 0;
		}

		bool operator==(resource const& rhs) const noexcept
		{
			return *this == rhs.path();
		}

		bool operator==(CoAP::Message::Option::option const& op) const noexcept
		{
			unsigned len = op.length;
			const char* p = path_;
			const std::uint8_t* v = reinterpret_cast<std::uint8_t const*>(op.value);
			while(len || *p != '\0')
			{
				if(*v++ != *p++) return false;
				len--;
			}
			if(*p == '\0' && len == 0) return true;
			return false;
		}


	private:
		const char*	path_;

		callback_t*	get_;
		callback_t*	post_;
		callback_t*	put_;
		callback_t*	del_;
};

}//Resource
}//CoAP



#endif /* COAP_TE_RESOURCE_HPP__ */
