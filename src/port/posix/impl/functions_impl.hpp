#ifndef COAP_TE_PORT_POSIX_SOCKET_FUNCTIONS_IMPL_HPP__
#define COAP_TE_PORT_POSIX_SOCKET_FUNCTIONS_IMPL_HPP__

#include "../functions.hpp"

#include "../port.hpp"

namespace CoAP{
namespace Port{
namespace POSIX{

template<typename Handler>
bool nonblock_socket(Handler socket)
{
#ifdef _WIN32
   unsigned long mode = 1;
   return (ioctlsocket(socket, FIONBIO, &mode) == 0) ? true : false;
#else
   int flags = ::fcntl(socket, F_GETFL, 0);
   if (flags == -1) return false;
   flags = flags | O_NONBLOCK;
   return (fcntl(socket, F_SETFL, flags) == 0) ? true : false;
#endif
}

}//POSIX
}//Port
}//CoAP

#endif /* COAP_TE_PORT_POSIX_SOCKET_FUNCTIONS_IMPL_HPP__ */
