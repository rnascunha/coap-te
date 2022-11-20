#ifndef COAP_TE_PORT_POSIX_UNIX_HPP__
#define COAP_TE_PORT_POSIX_UNIX_HPP__

#include <netinet/in.h>
#include <arpa/inet.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <fcntl.h>
#include <unistd.h>
#include <sys/epoll.h>

#endif /* COAP_TE_PORT_POSIX_UNIX_HPP__ */
