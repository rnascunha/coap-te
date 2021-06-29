#ifndef COAP_TE_RESOURCE_TYPES_HPP__
#define COAP_TE_RESOURCE_TYPES_HPP__

#include "../transmission/response.hpp"
#include "../message/types.hpp"
#include "../internal/tree.hpp"
#include <functional>

namespace CoAP{
namespace Resource{

template<typename Endpoint>
using callback = void(
		CoAP::Message::message const&,
		CoAP::Transmission::Response<Endpoint>&,
		void*) noexcept;

template<typename Endpoint>
using callback_functional = std::function<void(
		CoAP::Message::message const&,
		CoAP::Transmission::Response<Endpoint>&,
		void*)>;

template<typename Message,
		typename Response>
using callback_reliable = void(
		Message const&,
		Response&,
		void*) noexcept;

template<typename Message,
		typename Response>
using callback_reliable_function = std::function<void(
		Message const&,
		Response&,
		void*)>;

namespace Link_Format{

struct link_format{
	link_format() = default;
	
	const char*		link = nullptr;
	std::size_t		link_len = 0;
	const char*		description = nullptr;
	std::size_t		desc_len = 0;

	void reset() noexcept
	{
		link = nullptr;
		link_len = 0;
		description = nullptr;
		desc_len = 0;
	}
};

struct attribute{
	attribute() = default;
	
	const char* 	attr = nullptr;
	std::size_t		attr_len = 0;
	const char*		value = nullptr;
	std::size_t		value_len = 0;

	void reset() noexcept
	{
		attr = nullptr;
		attr_len = 0;
		value = nullptr;
		value_len = 0;
	}
};

struct link_value{	
	link_value() = default;

	const char*		value = nullptr;
	std::size_t		value_len = 0;
	const char*		next_value = nullptr;

	void reset() noexcept
	{
		value = nullptr;
		value_len = 0;
	}
};

}//Link_Format

}//Resource
}//CoAP

#endif /* COAP_TE_RESOURCE_TYPES_HPP__ */
