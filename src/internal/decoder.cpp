#include "decoder.hpp"
#include "ascii.hpp"
#include <cstdlib>	//std::strtoul
#include <cstdio>

namespace CoAP{
namespace Helper{

bool percent_decode(char* buffer_out, std::size_t& buffer_out_len,
					const char* buffer_in, std::size_t buffer_in_len) noexcept
{
	std::size_t j = 0;
	for(std::size_t i = 0; i < buffer_in_len;)
	{
		if(j >= buffer_out_len)
			return false;

		if(buffer_in[i] == '%')
		{
			//Has '%'. Check if has 2 characters
			if((buffer_in_len - i) >= 2)
			{
				//Check if characters are hexadecimal
				if(is_hexa(buffer_in[i+1]) && is_hexa(buffer_in[i+2]))
				{
					//Characters are hexadecimal
					buffer_out[j] = (hexa_char_to_int(buffer_in[i+1]) << 4) |
									hexa_char_to_int(buffer_in[i+2]);
					i += 3; j++;
					continue;
				}
			}
		}
		buffer_out[j] = buffer_in[i];
		j++; i++;
	}
	buffer_out_len = j;
	return true;
}

}//Helper
}//CoAP

