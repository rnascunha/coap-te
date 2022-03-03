#ifndef COAP_TE_INTERNAL_META_HPP__
#define COAP_TE_INTERNAL_META_HPP__

#include <cstdint>

namespace CoAP{

template<std::size_t Size, typename T = std::uint8_t>
struct buffer_type
{
	using type = T[Size];
};

template<>
struct buffer_type<0>
{
	using type = struct{};
};

template<std::size_t Size, typename T = std::uint8_t>
struct buffer_or_pointer_type
{
	using type = T[Size];
};

template<typename T>
struct buffer_or_pointer_type<0, T>
{
	using type = T*;
};

}//Coap

#endif /* COAP_TE_INTERNAL_META_HPP__ */
