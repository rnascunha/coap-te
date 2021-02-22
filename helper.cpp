#include "helper.hpp"
#include <cstring>

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
		size = 1;
		value = 0;
		return;
	}
	std::memcpy(reinterpret_cast<uint8_t*>(&value), d, size);
}

}//Helper
}//CoAP
