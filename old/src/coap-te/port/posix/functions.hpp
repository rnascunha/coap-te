#ifndef COAP_TE_PORT_POSIX_SOCKET_FUNCTIONS_HPP__
#define COAP_TE_PORT_POSIX_SOCKET_FUNCTIONS_HPP__

namespace CoAP{
namespace Port{
namespace POSIX{

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
bool init() noexcept;
#else /* defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) */
inline bool init() noexcept{ return true; }
#endif /* defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) */

template<typename Handler>
bool nonblock_socket(Handler socket);

}//POSIX
}//Port
}//CoAP

#include "impl/functions_impl.hpp"

#endif /* COAP_TE_PORT_POSIX_SOCKET_FUNCTIONS_HPP__ */
