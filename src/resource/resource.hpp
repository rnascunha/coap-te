#ifndef COAP_TE_RESOURCE_HPP__
#define COAP_TE_RESOURCE_HPP__

#include <cstring>
#include "message/codes.hpp"
#include "internal/list.hpp"

namespace CoAP{
namespace Resource{

template<typename Callback_Functor,
		bool HasDescription = false>
class resource
{
	using empty = struct{};
	public:
		static constexpr const bool has_description = HasDescription;
		using description_type = typename std::conditional<HasDescription, const char*, empty>::type;
		using callback_t = Callback_Functor;
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

		bool has_callback() const noexcept
		{
			return get_ != nullptr || post_ != nullptr || del_ != nullptr || put_ != nullptr;
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

		description_type	desc_;
};

}//Resource
}//CoAP



#endif /* COAP_TE_RESOURCE_HPP__ */
