#ifndef COAP_TE_PORT_POSIX_SOCKET_FUNCTIONS_HPP__
#define COAP_TE_PORT_POSIX_SOCKET_FUNCTIONS_HPP__

namespace CoAP{
namespace Port{
namespace POSIX{

bool init() noexcept;

template<typename Handler>
bool nonblock_socket(Handler socket);

}//POSIX
}//Port
}//CoAP

#include "impl/functions_impl.hpp"

#endif /* COAP_TE_PORT_POSIX_SOCKET_FUNCTIONS_HPP__ */
