#include "helper.hpp"
#include <cstring>
#include <cstdio>

namespace CoAP{
namespace Helper{

void make_short_unsigned(unsigned& value, unsigned& size) noexcept
{
	bool skip = false;
	size = 0;
	std::uint8_t d[sizeof(unsigned)];
	for(unsigned i = 0; i < sizeof(unsigned); i++)
	{
		int v = value >> ((sizeof(unsigned) - (i + 1)) * 8);
		std::uint8_t nv = static_cast<std::uint8_t>(v);
		if(!skip && nv == 0) continue;
		d[size++] = nv;
		skip = true;
	}
	if(size == 0)
	{
		size = 0;
		value = 0;
		return;
	}
	std::memcpy(reinterpret_cast<uint8_t*>(&value), d, size);
}

bool array_to_unsigned(std::uint8_t const* arr, std::size_t length, unsigned& value)
{
	value = 0;
	if(length > sizeof(unsigned)) return false;

	for(std::size_t i = 0; i < length; i++)
		value |= arr[i] << ((length - i - 1) * 8);

	return true;
}

void shift_left(std::uint8_t* buffer, std::size_t size, std::size_t shift) noexcept
{
	std::uint8_t *begin = buffer + shift;
	while(size--)
		*buffer++ = *begin++;
}

void shift_right(std::uint8_t* buffer, std::size_t size, std::size_t shift) noexcept
{
	std::uint8_t *end = buffer + size - 1,
				*n_end = buffer + size + shift - 1;
	while(size--)
		*n_end-- = *end--;
}

}//Helper
}//CoAP
