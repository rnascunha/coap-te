#ifndef COAP_TE_PORT_LINUX_HPP__
#define COAP_TE_PORT_LINUX_HPP__

#include "error.hpp"
#include "endpoint.hpp"
#include "socket.hpp"
#include <ctime>
#include <cstdlib>

namespace CoAP{

using socket = Port::Linux::socket<MSG_DONTWAIT, 0>;

}//CoAP

#endif /* COAP_TE_PORT_LINUX_HPP__ */
