#ifndef COAP_TE_PORT_LINUX_HPP__
#define COAP_TE_PORT_LINUX_HPP__

#include "error.hpp"
#include "endpoint.hpp"
#include "socket.hpp"
#include "defines/connection.hpp"
#include <ctime>
#include <cstdlib>

namespace CoAP{

using time_t = std::time_t;
auto time = []{ return std::time(NULL); };

auto random_generator = [](){ return std::rand(); };


using endpoint = Port::Linux::endpoint;
using connection = connection_base<Port::Linux::socket, endpoint>;

}//CoAP

#endif /* COAP_TE_PORT_LINUX_HPP__ */
