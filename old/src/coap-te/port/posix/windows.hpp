#ifndef COAP_TE_PORT_POSIX_WINDOWS_HPP__
#define COAP_TE_PORT_POSIX_WINDOWS_HPP__

#include <cstdint>

#ifndef COAP_TE_USE_SELECT
#define COAP_TE_USE_SELECT 1
#endif /* COAP_TE_USE_SELECT */

#ifndef NOMINMAX
#define COAP_TE_DEFINED_TO_UNDEF    1
#define NOMINMAX
#endif /* NOMINMAX */

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

#ifdef COAP_TE_DEFINED_TO_UNDEF
#undef NOMINMAX
#undef COAP_TE_DEFINED_TO_UNDEF
#endif /* COAP_TE_DEFINED_TO_UNDEF */

using sa_family_t = unsigned short int;
using in_addr_t = std::uint32_t;

#endif /* COAP_TE_PORT_POSIX_WINDOWS_HPP__ */
