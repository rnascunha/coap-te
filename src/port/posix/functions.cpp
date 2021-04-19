#ifndef COAP_PORT_POSIX_FUNCTIONS_HPP__
#define COAP_PORT_POSIX_FUNCTIONS_HPP__

#include "functions.hpp"

#include "port.hpp"

namespace CoAP{
namespace Port{
namespace POSIX{

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
bool init() noexcept
{

	WSADATA wsa;

	//Initialise winsock
	if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
	{
		return false;
	}
	return true;
}
#endif /* defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) */

}//POSIX
}//Port
}//CoAP

#endif /* COAP_PORT_POSIX_FUNCTIONS_HPP__ */
