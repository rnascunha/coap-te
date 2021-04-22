/**
 * This examples shows the use of UDP posix-like socket, used
 * at CoAP protocol
 *
 * We are going to implement a simple server that will wait request from clients
 * and echo the payload received back.
 *
 * This example is implemented using IPv4 and IPv6.
 *
 * \note Before running this example you must configure your envirioment variables
 * using 'idf.py menuconfig'. You must set your wifi network parameters, SSID and
 * password, and choose the endpoint type (IPv4 or IPv6) and the the server port
 * the device will bind. If you are using linux, you can connect to the device
 * using 'nc -u <ip_address> <port>'.
 */

#include <cstdlib>
#include <cstdio>
#include <cstdint>

#include "example_init.hpp"

#include "error.hpp"
#include "port/posix/udp_socket.hpp"

using namespace CoAP;

/**
 * Defining the endpoint
 */
#if CONFIG_ENDPOINT_TYPE == 1
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

#define CONN_PORT			CONFIG_SERVER_PORT

/**
 * Auxiliary call
 */
static void exit_error(Error& ec, const char* what = "")
{
	printf("ERROR! [%d] %s [%s]", ec.value(), ec.message(), what);
	while(true) vTaskDelay(portMAX_DELAY);
}

#define BUFFER_LEN		1000

/**
 * Defining the UDP socket.
 *
 * The fisrt argument is the endpoint (IPv4 or IPv6) that we are
 * going to open and connect.
 */
using udp_socket = Port::POSIX::udp<endpoint>;

extern "C" void app_main(void)
{
	std::printf("Echo UDP server init...\n");

	/**
	 * This is a very naive implementation of the WIFI / TCP/IP stack initializaition.
	 * It will BLOCK until connect, or get stuck if fail... Should not be used in
	 * production code
	 */
	wifi_stack_init();

	Error ec;
	std::uint8_t buffer[BUFFER_LEN];

	udp_socket::endpoint ep{BIND_ADDR, CONN_PORT};

	udp_socket conn;

	conn.open(ec);
	if(ec) exit_error(ec, "open");

	conn.bind(ep, ec);
	if(ec) exit_error(ec, "bind");

	char addr_str[46];
	std::printf("Listening: [%s]:%u\n", ep.address(addr_str), ep.port());
	while(true)
	{
		vTaskDelay(pdMS_TO_TICKS(50));

		udp_socket::endpoint recv_addr;
		std::size_t size = conn.receive(buffer, BUFFER_LEN, recv_addr, ec);
		if(ec) exit_error(ec, "read");
		if(size == 0) continue;

		buffer[size] = '\0';

		char addr_str2[46];
		std::printf("Received [%s]:%u [%zu]: %s\n", recv_addr.address(addr_str2),
													recv_addr.port(),
													size, buffer);
		std::printf("Echoing...\n");
		conn.send(buffer, size, recv_addr, ec);
		if(ec) exit_error(ec, "write");
	}

	while(true) vTaskDelay(portMAX_DELAY);
}

