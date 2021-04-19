#ifndef COAP_PORT_POSIX_FUNCTIONS_HPP__
#define COAP_PORT_POSIX_FUNCTIONS_HPP__

#include "functions.hpp"

#include "port.hpp"

namespace CoAP{
namespace Port{
namespace POSIX{

bool init() noexcept
{
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
	WSADATA wsa;

	//Initialise winsock
	if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
	{
		return false;
	}
	return true;
#else /* defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) */
	return true;
#endif /* defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) */
}

}//POSIX
}//Port
}//CoAP

#endif /* COAP_PORT_POSIX_FUNCTIONS_HPP__ */