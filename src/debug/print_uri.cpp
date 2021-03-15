#include "print_uri.hpp"
#include <cstdio>

namespace CoAP{
namespace Debug{

template<typename Host,
		typename PrintHost>
static void print_uri(CoAP::URI::uri<Host> const &uri, PrintHost print_host) noexcept
{
	std::printf("%s://",
				uri.uri_scheme == CoAP::URI::scheme::coap ? "coap" : "coaps");
	print_host(uri.host);
	std::printf(":%u/%.*s?%.*s", uri.port,
			static_cast<int>(uri.path_len), uri.path,
			static_cast<int>(uri.query_len), uri.query);
}

template<typename Host,
		typename PrintHost>
static void print_uri_decomposed(CoAP::URI::uri<Host> const &uri, PrintHost print_host) noexcept
{
	std::printf("Scheme: %s\n",
				uri.uri_scheme == CoAP::URI::scheme::coap ? "coap" : "coaps");
	std::printf("Host: ");
	print_host(uri.host);
	std::printf("\n");
	std::printf("Port: %u\nPath: %.*s\nQuery: %.*s\n", uri.port,
			static_cast<int>(uri.path_len), uri.path,
			static_cast<int>(uri.query_len), uri.query);
}

static void print_ipv4(in_addr const& host) noexcept
{
	char print_buffer[20];
	std::printf("%s", inet_ntop(AF_INET, &host, print_buffer, sizeof(sockaddr_in)));
}

static void print_ipv6(in6_addr const& host) noexcept
{
	char print_buffer[40];
	std::printf("[%s]", inet_ntop(AF_INET6, &host, print_buffer, sizeof(sockaddr_in6)));
}

static void print_ip_v4_v6(CoAP::URI::ip_type const& host) noexcept
{
	if(host.type ==  CoAP::URI::host_type::ipv4)
		print_ipv4(host.host.ip4);
	else
		print_ipv6(host.host.ip6);
}

void print_uri(CoAP::URI::uri<in_addr> const& uri) noexcept
{
	print_uri(uri, print_ipv4);
}

void print_uri(CoAP::URI::uri<in6_addr> const& uri) noexcept
{
	print_uri(uri, print_ipv6);
}

void print_uri(CoAP::URI::uri<CoAP::URI::ip_type> const& uri) noexcept
{
	print_uri(uri, print_ip_v4_v6);
}

void print_uri_decomposed(CoAP::URI::uri<in_addr> const& uri) noexcept
{
	print_uri_decomposed(uri, print_ipv4);
}

void print_uri_decomposed(CoAP::URI::uri<in6_addr> const& uri) noexcept
{
	print_uri_decomposed(uri, print_ipv6);
}

void print_uri_decomposed(CoAP::URI::uri<CoAP::URI::ip_type> const& uri) noexcept
{
	print_uri_decomposed(uri, print_ip_v4_v6);
}

}//Debug
}//CoAP
