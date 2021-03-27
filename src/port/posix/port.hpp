#ifndef COAP_TE_PORT_POSIX_HPP__
#define COAP_TE_PORT_POSIX_HPP__

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#include "windows.hpp"
#elif defined(__unix__)
#include "unix.hpp"
#else
#error "System not supported"
#endif

#include "endpoint_ipv4.hpp"
#include "udp_socket.hpp"

namespace CoAP{

using socket = Port::POSIX::udp<Port::POSIX::endpoint_ipv4, MSG_DONTWAIT>;

}//CoAP

#endif /* COAP_TE_PORT_POSIX_HPP__ */
