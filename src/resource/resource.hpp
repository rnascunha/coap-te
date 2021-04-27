#ifndef COAP_TE_RESOURCE_HPP__
#define COAP_TE_RESOURCE_HPP__

#include "defines/defaults.hpp"
#include <cstring>
#include "message/codes.hpp"
#include "internal/list.hpp"

namespace CoAP{
namespace Resource{

template<typename Callback_Functor,
		bool HasDescription = false,
		bool UseExtraMethods = false>
class resource
{
	using empty = struct{};
	public:
		static constexpr const bool has_description = HasDescription;
		using description_type = typename std::conditional<HasDescription, const char*, empty>::type;
		using callback_t = Callback_Functor;

		using callback_ex_t = typename std::conditional<UseExtraMethods, callback_t, empty>::type;
		using callback_ex_ptr_t = typename std::conditional<UseExtraMethods, callback_t*, empty>::type;

		resource(const char* path,
				callback_t get = nullptr, callback_t post = nullptr,
				callback_t put = nullptr, callback_t del = nullptr)
		: path_(path), get_(get), post_(post), put_(put), del_(del)
		{
			if constexpr(has_description)
				desc_ = nullptr;
		}

		resource(const char* path, const char* description [[maybe_unused]],
						callback_t get = nullptr, callback_t post = nullptr,
						callback_t put = nullptr, callback_t del = nullptr)
				: path_(path), get_(get), post_(post), put_(put), del_(del)
		{
			if constexpr(has_description)
				desc_ = description;
		}

		const char* path() const noexcept{ return path_; }
		const char* description() const noexcept
		{
			if constexpr(has_description)
				return desc_;
			else return nullptr;
		}

		void description(const char* desc [[maybe_unused]]) noexcept
		{
			if constexpr(has_description)
				desc_ = desc;
		}

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

#if COAP_TE_FETCH_PATCH == 1
		resource& fetch(callback_t cb = nullptr) noexcept
		{
			static_assert(UseExtraMethods, "Disabled extra methods");
			fetch_ = cb;
			return *this;
		}

		resource& patch(callback_t cb = nullptr) noexcept
		{
			static_assert(UseExtraMethods, "Disabled extra methods");
			patch_ = cb;
			return *this;
		}

		resource& ipatch(callback_t cb = nullptr) noexcept
		{
			static_assert(UseExtraMethods, "Disabled extra methods");
			ipatch_ = cb;
			return *this;
		}
#endif /* COAP_TE_FETCH_PATCH == 1 */
		bool has_callback() const noexcept
		{
			if constexpr(!UseExtraMethods)
				return get_ != nullptr || post_ != nullptr || del_ != nullptr || put_ != nullptr;
#if COAP_TE_FETCH_PATCH == 1
			else
				return get_ != nullptr || post_ != nullptr || del_ != nullptr || put_ != nullptr
						|| fetch_ != nullptr || patch_ != nullptr || ipatch_ != nullptr;
#endif /* COAP_TE_FETCH_PATCH == 1 */
		}

		template<typename Message,
				typename Response>
		bool call(CoAP::Message::code code,
			Message const& request,
			Response& response,
			void* engine) const noexcept
		{
			switch(code)
			{
				using namespace CoAP::Message;
				case code::get:
					return get_ ? get_(request, response, engine), true : false;
					break;
				case code::post:
					return post_ ? post_(request, response, engine), true : false;
					break;
				case code::put:
					return put_ ? put_(request, response, engine), true : false;
					break;
				case code::cdelete:
					return del_ ? del_(request, response, engine), true : false;
					break;
				default:
#if COAP_TE_FETCH_PATCH == 1
					if constexpr(UseExtraMethods)
					{
						switch(code)
						{
							case code::fetch:
								return fetch_ ? fetch_(request, response, engine), true : false;
							case code::patch:
								return patch_ ? patch_(request, response, engine), true : false;
							case code::ipatch:
								return ipatch_ ? ipatch_(request, response, engine), true : false;
							default:
								break;
						}
					}
#endif /* COAP_TE_FETCH_PATCH == 1 */
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
		const char*			path_;

		callback_t*			get_;
		callback_t*			post_;
		callback_t*			put_;
		callback_t*			del_;

#if COAP_TE_FETCH_PATCH == 1
		callback_ex_ptr_t	fetch_;
		callback_ex_ptr_t	patch_;
		callback_ex_ptr_t	ipatch_;
#endif /* COAP_TE_FETCH_PATCH == 1 */

		description_type	desc_;
};

}//Resource
}//CoAP



#endif /* COAP_TE_RESOURCE_HPP__ */
