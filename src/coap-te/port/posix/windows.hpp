#ifndef COAP_TE_PORT_POSIX_WINDOWS_HPP__
#define COAP_TE_PORT_POSIX_WINDOWS_HPP__

#include <cstdint>

#define NOMINMAX
#include <winsock2.h>
#include <ws2tcpip.h>

#define MSG_DONTWAIT		0x40 /* Nonblocking IO.  */

#define SHUT_RD				SD_RECEIVE
#define SHUT_WR				SD_SEND
#define SHUT_RDWR			SD_BOTH

#if defined(_MSC_VER)
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#endif
#undef NOMINMAX

using sa_family_t = unsigned short int;
using in_addr_t = std::uint32_t;

#endif /* COAP_TE_PORT_POSIX_WINDOWS_HPP__ */
