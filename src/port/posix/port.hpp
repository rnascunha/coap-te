#ifndef COAP_TE_PORT_POSIX_HPP__
#define COAP_TE_PORT_POSIX_HPP__

#include "endpoint_ipv4.hpp"
#include "udp_socket.hpp"
#include <ctime>
#include <cstdlib>

namespace CoAP{

using socket = Port::POSIX::udp<Port::POSIX::endpoint_ipv4, MSG_DONTWAIT, 0>;

}//CoAP

#endif /* COAP_TE_PORT_POSIX_HPP__ */
