#include <cstdio>
#include <cstdlib>

#include "uri/types.hpp"
#include "uri/decompose.hpp"

#include "debug/print_uri.hpp"

int main(int argv, char** argc)
{
	if(argv != 2)
	{
		std::printf("ERROR! Must provide a URL.\n");
		std::printf("How to use:\n\t%s '<coap|coaps>://<host>:<port>/<path>?query'\n", argc[0]);
		std::printf("Examples:\n");
		std::printf("\t%s coap://[::1]:5683/path/to/resource?query=key\n", argc[0]);
		std::printf("\t%s 'coaps://127.0.0.1:5683?value1=key&value2'\n", argc[0]);
		return EXIT_FAILURE;
	}

	CoAP::URI::uri<CoAP::URI::ip_type> uri;
	if(!CoAP::URI::decompose(uri, argc[1]))
	{
		printf("Error decomposing...\n");
		return EXIT_FAILURE;
	}

	CoAP::Debug::print_uri_decomposed(uri);

	return EXIT_SUCCESS;
}

