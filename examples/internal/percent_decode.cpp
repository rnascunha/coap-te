/**
 * Percent decode test
 */

#include <cstdio>
#include <cstring>
#include <cstdlib>

#include "internal/decoder.hpp"

#define BUFFER_LEN		512

int main(int argv, char** argc)
{
	if(argv < 2)
	{
		std::printf("ERROR! A text argument must be provided.\n");
		std::printf("How to use:\n\t%s <text> [<text>...]\n", argc[0]);
		return EXIT_FAILURE;
	}

	char buffer[BUFFER_LEN];
	std::size_t buffer_len = BUFFER_LEN;
	int i = 1;
	while(argv > 1)
	{
		if(CoAP::Helper::percent_decode(buffer, buffer_len, argc[i], std::strlen(argc[i])))
		{
			std::printf("Decoded [%lu]: %s -> %.*s\n", buffer_len, argc[i],
					static_cast<int>(buffer_len), buffer);
		}
		else
		{
			std::printf("Error decoding [%d]: %s\n", i, argc[i]);
		}
		i++;
		buffer_len = BUFFER_LEN;
		argv--;
	}

	return EXIT_SUCCESS;
}

