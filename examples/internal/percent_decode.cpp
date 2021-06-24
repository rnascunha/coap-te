/**
 * Percent decode test
 */

#include <cstdio>
#include <cstring>
#include <cstdlib>

#include "coap-te/internal/decoder.hpp"

//#define USE_SAME_BUFFER

#define BUFFER_LEN		512

int main(int argv, char** argc)
{
	if(argv < 2)
	{
		std::printf("ERROR! A text argument must be provided.\n");
		std::printf("How to use:\n\t%s <text> [<text>...]\n", argc[0]);
		return EXIT_FAILURE;
	}
#ifdef USE_SAME_BUFFER
	int i = 1;
	std::size_t size;
	while(argv > 1)
	{
		if((size = CoAP::Helper::percent_decode(argc[i])))
		{
			std::printf("Decoded [%zu]: %s\n", size, argc[i]);
		}
		else
		{
			std::printf("Error decoding [%d]: %s\n", i, argc[i]);
		}
		i++;
		argv--;
	}
#else
	char buffer[BUFFER_LEN];
	std::size_t size;
	int i = 1;
	while(argv > 1)
	{
		if((size = CoAP::Helper::percent_decode(buffer, 512, argc[i], std::strlen(argc[i]))))
		{
			std::printf("Decoded [%zu]: %s -> %.*s\n", size, argc[i],
					static_cast<int>(size), buffer);
		}
		else
		{
			std::printf("Error decoding [%d]: %s\n", i, argc[i]);
		}
		i++;
		argv--;
	}
#endif /* USE_SAME_BUFFER */

	return EXIT_SUCCESS;
}

