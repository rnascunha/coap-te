#ifndef COAP_TE_PORT_POSIX_HPP__
#define COAP_TE_PORT_POSIX_HPP__

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#include "windows.hpp"
#elif defined(__unix__)
#include "unix.hpp"
#elif COAP_TE_ESP_IDF_PLATAFORM == 1
#include "esp_idf.hpp"
#else
#error "System not supported"
#endif

#include "endpoint_ipv4.hpp"
#include "endpoint_ipv6.hpp"
#include "udp_socket.hpp"
#include "tcp_client.hpp"
#include "tcp_server.hpp"

#endif /* COAP_TE_PORT_POSIX_HPP__ */
