/**
 * This example will decompose a URI passed by command line
 * to a internal structure, dealing any percent encoded
 * characters.
 *
 * At first it will separate the macro format:
 * <scheme>://<host>:<port>/<path>?<query>
 * (the presence of a fragment '#' will generate a error)
 * Where host can be ipv4 or ipv6 address.
 *
 * At a second part, it breaks the path and query to a
 * option list.
 */

#include <cstdio>
#include <cstdlib>

#include "coap-te/defines/defaults.hpp"

#include "coap-te/uri/types.hpp"
#include "coap-te/uri/decompose.hpp"

#include "coap-te/debug/print_options.hpp"
#include "coap-te/debug/print_uri.hpp"

#include "coap-te/message/options/options.hpp"

#define BUFFER_LEN		512

/**
 * Use of decompose_to_list function (breaks path and query together)
 */
#define USE_DECOMPOSE_TO_LIST

int main(int argc, char** argv)
{
	if(argc != 2)
	{
		std::printf("ERROR! Must provide a URL.\n");
		std::printf("How to use:\n\t%s '<coap|coaps>://<host>:<port>/<path>?query'\n", argv[0]);
		std::printf("Examples:\n");
		std::printf("\t%s coap://[::1]:5683/path/to/resource?query=key\n", argv[0]);
		std::printf("\t%s 'coaps://127.0.0.1:5683?value1=key&value2'\n", argv[0]);
#if COAP_TE_RELIABLE_CONNECTION == 1
		std::printf("\t%s 'coap+tcp://127.0.0.1:5683?value1=key&value2'\n", argv[0]);
		std::printf("\t%s 'coaps+tcp://127.0.0.1:5683?value1=key&value2'\n", argv[0]);
		std::printf("\t%s 'coap+ws://127.0.0.1:5683?value1=key&value2'\n", argv[0]);
		std::printf("\t%s 'coaps+ws://127.0.0.1:5683?value1=key&value2'\n", argv[0]);
#endif /* COAP_TE_RELIABLE_CONNECTION == 1 */
		return EXIT_FAILURE;
	}

	/**
	 * 'uri<ip_type>' support both IPv4 and IPv6 address. It you only
	 * desire one kind, function 'decompose' is also defined to:
	 * * 'uri<in_addr>': only IPv4;
	 * * 'uri<in6_addr>': only IPv6;
	 *
	 * * 'struct uri' is defined at 'uri/types.hpp'.
	 *
	 * The template parameter of function decompose (true by default) is to accept
	 * reliable connections, as defined at RFC8323. If you don't want to accept
	 * this kind scheme, set it to false (COAP_TE_RELIABLE_CONNECTION macro must
	 * also be == 1, the default)
	 */
	CoAP::URI::uri<CoAP::URI::ip_type> uri;
	if(!CoAP::URI::decompose<true>(uri, argv[1]))
	{
		printf("Error decomposing...\n");
		return EXIT_FAILURE;
	}

	/**
	 * As function 'decompose', function 'print_uri_decomposed'
	 * is defined to 'uri<ip_type>', 'uri<in_addr>', and 'uri<in6_addr>'.
	 */
	CoAP::Debug::print_uri_decomposed(uri);

	/**
	 * Decomposing path and querying
	 */
	CoAP::Message::Option::List list;		//List to add all options uri_path and uri_query

	/**
	 * Why do we need a buffer? As we don't know how many paths/query
	 * options we are going to need, and also need to percent-decode all data,
	 * we need 'some place' to put this information.
	 *
	 * Other problem is that we are NOT using any kind of dynamic memory allocation.
	 * So all this info will be recorded at the buffer. The buffer size IS a limitation...
	 */
	std::uint8_t buffer[BUFFER_LEN];
	std::size_t buffer_len = BUFFER_LEN;
#ifdef USE_DECOMPOSE_TO_LIST
	/**
	 * This function decompose both uri_path and uri_query. It adds it to the
	 * option list
	 */
	printf("\nDecomposing PATH/QUERY\n");
	if(!decompose_to_list(buffer, buffer_len, uri, list))
	{
		printf("Error decomposing PATH/QUERY\n");
		return EXIT_FAILURE;
	}
#else
	/**
	 * This option decompose first uri_path and then uri_query. It adds all adds to the
	 * option list
	 */
	printf("\nDecomposing PATH\n");
	if(!CoAP::URI::path_to_list(buffer, buffer_len, static_cast<char const*>(uri.path), uri.path_len, list))
	{
		printf("Error decomposing PATH\n");
		return EXIT_FAILURE;
	}
	/**
	 * We need to update the buffer pointer values, and set
	 * the remaining buffer size.
	 */
	std::uint8_t* n_buffer = buffer + buffer_len;
	buffer_len = BUFFER_LEN - buffer_len;

	printf("Decomposing QUERY\n");
	if(!CoAP::URI::query_to_list(n_buffer, buffer_len, static_cast<char const*>(uri.query), uri.query_len, list))
	{
		printf("Error decomposing QUERY\n");
		return EXIT_FAILURE;
	}
#endif
	printf("\nPrinting options...\n");
	CoAP::Debug::print_options(list.head());

	return EXIT_SUCCESS;
}

