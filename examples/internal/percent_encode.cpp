#include <iostream>
#include <cstdint>
#include <cstring>

//#define OPT_2

#include "internal/encoder.hpp"
#ifdef OPT_2
#include "internal/ascii.hpp"
#endif

int main()
{

#ifndef OPT_2
	char buffer[1000] = "my/teste_now;tchau";
	char encoded_list[] = {'/','_', '\\', ';'};
	std::size_t s = sizeof(encoded_list) / sizeof(char);
#else
	char buffer[1000] = "AbCdEfGh";
#endif

	std::cout << "Initial: " << buffer << ", size: " << std::strlen(buffer) <<"\n";

#ifndef OPT_2
	int size = CoAP::Helper::percent_encode(buffer, std::strlen(buffer), 1000, encoded_list, s);
#else
	int size = CoAP::Helper::percent_encode(buffer, std::strlen(buffer), 1000, CoAP::Helper::is_lower_alpha);
#endif
	if(size < 0)
	{
		std::cout << "ERROR! " << size << "\n";
		return EXIT_FAILURE;
	}

	std::printf("Encoded[%d]: %.*s\n", size, size, buffer);

	return EXIT_SUCCESS;
}
