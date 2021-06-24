/**
 * This examples shows the use of TCP client posix-like socket, used
 * at CoAP protocol
 *
 * We are going to implement a simple client request and wait for a answer.
 *
 * This example is implemented using IPv4 and IPv6.
 *
 * \note Before running this example you must configure your envirioment variables
 * using 'idf.py menuconfig'. You must set your wifi network parameters, SSID and
 * password, and choose the endpoint type (IPv4 or IPv6) and the address of the server
 * that the device will connect (IP and port). If you are using linux, you can open
 * a listening socket using 'nc -l <port>'.
 */

#include <cstdlib>
#include <cstdio>
#include <cstdint>

#include "example_init.hpp" //helper initialization functions

#include "coap-te/error.hpp"
#include "coap-te/port/posix/tcp_client.hpp"

using namespace CoAP;

/**
 * Defining the endpoint
 */
#if CONFIG_ENDPOINT_TYPE == 1
/**
 * IPv6 definitions
 */
#include "coap-te/port/posix/endpoint_ipv6.hpp"
using endpoint = Port::POSIX::endpoint_ipv6;
#else
/**
 * IPv4 definitions
 */
#include "coap-te/port/posix/endpoint_ipv4.hpp"
using endpoint = Port::POSIX::endpoint_ipv4;
#endif /* USE_IPV6 */

#define LOCALHOST_ADDR		CONFIG_SERVER_IP
#define CONN_PORT			CONFIG_SERVER_PORT

/**
 * Auxiliary call
 */
static void exit_error(Error& ec, const char* what = "")
{
	printf("ERROR! [%d] %s [%s]\n", ec.value(), ec.message(), what);
	while(true) vTaskDelay(portMAX_DELAY);
}

#define BUFFER_LEN		1000

/**
 * Defining the TCP socket.
 *
 * The template argument is the endpoint (IPv4 or IPv6) that we are
 * going to open and connect.
 */
using tcp_client = Port::POSIX::tcp_client<endpoint>;

extern "C" void app_main(void)
{
	printf("TCP Client example init...\n");

	/**
	 * This is a very naive implementation of the WIFI / TCP/IP stack initializaition.
	 * It will BLOCK until connect, or get stuck if fail... Should not be used in
	 * production code
	 */
	wifi_stack_init();

	Error ec;

	/**
	 * Endpoint of the host to connect
	 */
	tcp_client::endpoint to{LOCALHOST_ADDR, CONN_PORT, ec};
	if(ec)
		exit_error(ec, "Error parsing address\n");

	/**
	 * TCP client instance
	 */
	tcp_client conn;

	/**
	 * Connecting to host
	 */
	char buffer_addr[46];
	printf("Connection to %s\n", to.address(buffer_addr));

	conn.open(to, ec);
	if(ec) exit_error(ec, "open");

	/**
	 * Sending a payload
	 */
	conn.send("teste", std::strlen("teste"), ec);
	if(ec) exit_error(ec, "send");
	printf("Send succeced!\n");

	/**
	 * Receiving buffer
	 */
	std::uint8_t buffer[BUFFER_LEN];

	/**
	 * Loop to wait for a answer
	 */
	while(true)
	{
		/**
		 * Waiting for a response
		 */
		std::size_t size = conn.receive(buffer, BUFFER_LEN, ec);
		if(ec) exit_error(ec, "read");
		if(size == 0)
		{
			vTaskDelay(pdMS_TO_TICKS(50));
			continue;
		}

		/**
		 * Printing response received
		 */
		std::printf("Received[%zu]: %.*s\n", size, static_cast<int>(size), buffer);

		break;
	}
	conn.close();

	std::printf("Success!\n");
	/*
	 * Hang here
	 */
	while(true) vTaskDelay(portMAX_DELAY);
}
