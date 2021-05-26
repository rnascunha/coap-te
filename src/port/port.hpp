#ifndef COAP_TE_PORT_HPP__
#define COAP_TE_PORT_HPP__

#if COAP_TE_PORT_POSIX == 1
#include "posix/port.hpp"
#elif COAP_TE_PORT_ESP_MESH == 1
#include "esp_mesh/port.hpp"
#else
#error "System not defined"
#endif

#ifdef COAP_TE_PORT_C_STANDARD
//#include <ctime>
#endif /* COAP_TE_PORT_C_STANDART */

namespace CoAP{

#ifdef COAP_TE_PORT_C_STANDARD
//using time_t = std::time_t;
using time_t = unsigned long;
#endif /* COAP_TE_PORT_C_STANDART */

/**
 * \brief Return time in epoch format (milliseconds)
 */
time_t time() noexcept;

/**
 * \brief Random number generator
 */
unsigned random_generator() noexcept;

/**
 * \brief Initalize system requiriments
 *
 * Window/Linux: Initialize random number generator
 * Windows: initialize winsock library
 */
void init() noexcept;

}//CoAP

#endif /* COAP_TE_PORT_HPP__ */
