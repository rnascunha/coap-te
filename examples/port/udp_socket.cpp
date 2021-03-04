#include <cstdlib>
#include <cstdio>
#include <cstdint>

#include "error.hpp"
#include "port/port.hpp"

using namespace CoAP;

static void exit_error(Error& ec, const char* what = "")
{
	printf("ERROR! [%d] %s [%s]", ec.value(), ec.message(), what);
	exit(EXIT_FAILURE);
}

#define BUFFER_LEN		1000

//#define SOCKET_SERVER

#ifdef SOCKET_SERVER
int main()
{
	Error ec;
	std::uint8_t buffer[BUFFER_LEN];

	endpoint ep{INADDR_ANY, 8080};

	connection conn{CoAP::socket{}};

	conn.native_handler().open(ec);
	if(ec) exit_error(ec, "open");

	conn.native_handler().bind(ep, ec);
	if(ec) exit_error(ec, "bind");

	char addr_str[20];
	std::printf("Listening: %s:%u\n", ep.address(addr_str), ep.port());
	while(true)
	{
		endpoint recv_addr;
		std::size_t size = conn.receive(buffer, BUFFER_LEN, recv_addr, ec);
		if(ec) exit_error(ec, "read");
		if(size == 0) continue;
		buffer[size] = '\0';

		char addr_str[20];
		std::printf("Received %s:%u [%lu]: %s\n", recv_addr.address(addr_str), recv_addr.port(), size, buffer);
		std::printf("Echoing...\n");
		conn.send(buffer, size, recv_addr, ec);
		if(ec) exit_error(ec, "write");
	}

	return EXIT_SUCCESS;
}
#else /* SOCKET_SERVER */

int main()
{
	Error ec;
	std::uint8_t buffer[BUFFER_LEN];

	const char* payload = "Teste";
	std::size_t payload_len = std::strlen(payload);

	std::memcpy(buffer, payload, payload_len + 1);

	endpoint to{"127.0.0.1", 8080, ec};
	connection conn{CoAP::socket{}};

	conn.native_handler().open(ec);
	if(ec) exit_error(ec, "open");

	conn.send(buffer, payload_len, to, ec);
	if(ec) exit_error(ec, "send");
	printf("Send succeced!\n");

	endpoint from;
	while(true)
	{
		std::size_t size = conn.receive(buffer, BUFFER_LEN, from, ec);
		if(ec) exit_error(ec, "read");
		if(size == 0)
		{
			std::printf(".");
			continue;
		}

		char addr_str[20];
		std::printf("Received %s:%u [%lu]: %s\n", from.address(addr_str), from.port(), size, buffer);
		break;
	}
	return EXIT_SUCCESS;
}

#endif /* SOCKET_SERVER */
