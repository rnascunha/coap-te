/**
 * Percent-encode exemple
 *
 * All encode is done locally
 */
#include <cstdio>
#include <cstdint>
#include <cstring>

#define USE_FUNCTION

#include "coap-te/internal/encoder.hpp"
#ifdef USE_FUNCTION
#include "coap-te/internal/ascii.hpp"
#endif

int main()
{

#ifndef USE_FUNCTION
	char buffer[1000] = "my/teste_now;tchau";
	char encoded_list[] = {'/','_', '\\', ';'};
	std::size_t s = sizeof(encoded_list) / sizeof(char);
#else
	char buffer[1000] = "AbCdEfGh";
#endif

	//std::cout << "Initial: " << buffer << ", size: " << std::strlen(buffer) <<"\n";
	std::printf("Initial: %s, size %zu\n", buffer, std::strlen(buffer));
#ifndef USE_FUNCTION
	/**
	 * This call will percent-encode any character present at the encoded_list
	 */
	int size = CoAP::Helper::percent_encode(buffer, std::strlen(buffer), 1000, encoded_list, s);
#else
	/**
	 * This call will percent-encode any character that satisfy the function
	 */
	int size = CoAP::Helper::percent_encode(buffer, std::strlen(buffer), 1000, CoAP::Helper::is_lower_alpha);
#endif /* USE_FUNCTION */
	if(size < 0)
	{
		// std::cout << "ERROR! " << size << "\n";
		std::printf("ERROR! %d\n", size);
		return EXIT_FAILURE;
	}

	std::printf("Encoded[%d]: %.*s\n", size, size, buffer);

	return EXIT_SUCCESS;
}
