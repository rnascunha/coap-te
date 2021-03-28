#ifndef COAP_TE_PORT_WINDOWS_HPP__
#define COAP_TE_PORT_WINDOWS_HPP__

#include <cstdint>

#include <winsock2.h>
#include <ws2tcpip.h>
#include <in6addr.h>
#include <ws2def.h>
#include <ws2ipdef.h>

#define MSG_DONTWAIT		0x40 /* Nonblocking IO.  */

using sa_family_t = unsigned short int;
using in_addr_t = std::uint32_t;

#endif /* COAP_TE_PORT_WINDOWS_HPP__ */
