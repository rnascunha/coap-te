/**
 * This example will decompose a URI passed by command line
 * to a internal structure.
 *
 * At first it will separate the macro format:
 * <scheme>://<host>:<port>/<path>?<query>
 * (the presence of a fragment '#' will generate a error)
 * Where host can be ipv4 or ipv6 address.
 *
 * 'struct uri' is defined at 'uri/types.hpp'.
 *
 * At a second part, it breaks the path and query to a
 * option list.
 */

#include <cstdio>
#include <cstdlib>

#include "uri/types.hpp"
#include "uri/decompose.hpp"

#include "debug/print_options.hpp"
#include "debug/print_uri.hpp"

#include "message/options.hpp"

#define BUFFER_LEN		512

/**
 * Use of decompose_to_list function (breaks path and query together)
 */
#define USE_DECOMPOSE_TO_LIST

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

	/**
	 * Decomposing path and querying
	 */
	CoAP::Message::Option::List list;
	std::uint8_t buffer[BUFFER_LEN];
	std::size_t buffer_len = BUFFER_LEN;
#ifdef USE_DECOMPOSE_TO_LIST
	printf("\nDecomposing PATH/QUERY\n");
	if(!decompose_to_list(buffer, buffer_len, uri, list))
	{
		printf("Error decomposing PATH/QUERY\n");
		return EXIT_FAILURE;
	}
#else
	printf("\nDecomposing PATH\n");

	if(!CoAP::URI::path_to_list(buffer, buffer_len, uri.path, uri.path_len, list))
	{
		printf("Error decomposing PATH\n");
		return EXIT_FAILURE;
	}

	printf("Decomposing QUERY\n");
	std::uint8_t* n_buffer = buffer + buffer_len;
	buffer_len = BUFFER_LEN - buffer_len;
	if(!CoAP::URI::query_to_list(n_buffer, buffer_len, uri.query, uri.query_len, list))
	{
		printf("Error decomposing QUERY\n");
		return EXIT_FAILURE;
	}
#endif
	printf("\nPrinting options...\n");
	CoAP::Debug::print_options(list.head());

	return EXIT_SUCCESS;
}

