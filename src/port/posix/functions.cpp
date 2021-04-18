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

bool nonblock_socket(int socket)
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
