/**
 * This examples shows the use of TCP server posix-like socket, used
 * at CoAP protocol
 *
 * We are going to implement a simple server that will wait request from clients
 * and echo the payload received back.
 *
 * This example is implemented using IPv4 and IPv6.
 *
 * \note After running this example, run tcp_client to make the requests
 */

#include <cstdlib>
#include <cstdio>
#include <cstdint>

#include "error.hpp"
#include "port/posix/tcp_server.hpp"

/**
 * Using IPv6. Commenting the following line to use IPv4
 */
#define USE_IPV6

using namespace CoAP;

/**
 * Defining the endpoint
 */
#ifdef USE_IPV6
/**
 * IPv6 definitions
 */
#include "port/posix/endpoint_ipv6.hpp"
using endpoint = Port::POSIX::endpoint_ipv6;
#define BIND_ADDR		IN6ADDR_ANY_INIT
#else
/**
 * IPv4 definitions
 */
#include "port/posix/endpoint_ipv4.hpp"
using endpoint = Port::POSIX::endpoint_ipv4;
#define BIND_ADDR		INADDR_ANY
#endif /* USE_IPV6 */

/**
 * Auxiliary call
 */
static void exit_error(Error& ec, const char* what = "")
{
	printf("ERROR! [%d] %s [%s]\n", ec.value(), ec.message(), what);
	exit(EXIT_FAILURE);
}

#define BUFFER_LEN		1000

/**
 * Defining the UDP socket.
 *
 * The template argument is the endpoint (IPv4 or IPv6) that we are
 * going to open and connect.
 */
using tcp_server = Port::POSIX::tcp_server<endpoint>;

/**
 * The server works with callback function when accpet a new socket
 * (someone connects), receive data or close a connection
 */

/**
 * Open connection callback
 */
void open_cb(int socket) noexcept
{
	tcp_server::endpoint ep;
	if(!ep.copy_peer_address(socket))
		return;
	char buf[46];
	printf("Openned socket [%s]:%u\n", ep.address(buf), ep.port());
}

/**
 * Close connection callback
 */
void close_cb(int socket) noexcept
{
	tcp_server::endpoint ep;
	if(!ep.copy_peer_address(socket))
	{
		printf("Closed socket\n");
		return;
	}
	char buf[46];
	printf("Closed socket [%s]:%u\n", ep.address(buf), ep.port());
}

/**
 * Receving data callback
 */
void read_cb(int socket, void* buffer, std::size_t buffer_len) noexcept
{
	tcp_server::endpoint ep;
	if(!ep.copy_peer_address(socket))
		return;

	char buf[46];
	printf(">[%s]:%u[%zu]: %.*s\n",
			ep.address(buf), ep.port(),
			buffer_len,
			static_cast<int>(buffer_len),
			static_cast<const char*>(buffer));

	/**
	 * Echoing data received back
	 */
	::send(socket, buffer, buffer_len, MSG_DONTWAIT);
}

int main()
{
	Error ec;

	/**
	 * TCP server instance
	 */
	tcp_server conn;

	/**
	 * Endpoint to bind
	 */
	tcp_server::endpoint ep{BIND_ADDR, 8080};
	/**
	 * Open socket and binding enpoint
	 */
	conn.open(ep, ec);
	if(ec) exit_error(ec, "open");

	char addr_str[46];
	std::printf("Listening: [%s]:%u\n", ep.address(addr_str), ep.port());

	/**
	 * Receiving buffer
	 */
	std::uint8_t buffer[BUFFER_LEN];

	/**
	 * Working loop
	 *
	 * The run function template parameter are:
	 * * read callback
	 * * block time in miliseconds: 0 (no block), -1 (indefenily)
	 * * open connection callback
	 * * close connection callback
	 * * max event permited (ommited, defaulted to 32)
	 */
	while(conn.run<read_cb, 0 , open_cb, close_cb>(buffer, BUFFER_LEN, ec))
	{
		/**
		 * Your code
		 */
	}

	if(ec) exit_error(ec, "run");
	return EXIT_SUCCESS;
}

