#ifndef COAP_TE_PORT_HPP__
#define COAP_TE_PORT_HPP__

#if COAP_TE_PORT_POSIX == 1
#include "posix/port.hpp"
#endif /* COAP_TE_PORT_POSIX == 1 */
#if COAP_TE_PORT_ESP_MESH == 1
#include "esp_mesh/port.hpp"
#endif /* COAP_TE_PORT_POSIX == 1 */
#if COAP_TE_PORT_ESP_MESH != 1 && COAP_TE_PORT_POSIX != 1
#error "System not defined"
#endif

namespace CoAP{

#ifdef COAP_TE_PORT_C_STANDARD
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
