#include "helper.hpp"
#include <cstdio>

#include <cstdint>

namespace CoAP{
namespace Debug{

int print_array(const void* const buffer, std::size_t len, unsigned separate /* = 2 */)
{
	auto const* buffer_u8 = static_cast<std::uint8_t const*>(buffer);
	int size = 0;
	for(std::size_t i = 0; i < len; i++)
	{
		if(i != 0 && separate != 0 && i % separate == 0) size += std::printf(" ");
		size += std::printf("%02X", buffer_u8[i]);
	}

	return size;
}

void print_string(const void* const buffer, std::size_t len)
{
	auto const* char_u8 = static_cast<char const*>(buffer);
	for(std::size_t i = 0; i < len; i++)
		std::printf("%c", char_u8[i]);
}

}//Debug
}//CoAP
