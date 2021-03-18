#ifndef COAP_TE_PORT_HPP__
#define COAP_TE_PORT_HPP__

#include "linux/port.hpp"

namespace CoAP{

#ifdef COAP_TE_PORT_C_STANDARD

using time_t = std::time_t;

#endif /* COAP_TE_PORT_C_STANDART */

/**
 * \brief Return time in epoch format (seconds)
 */
time_t time() noexcept;

/**
 * \brief Random number generator
 */
unsigned random_generator() noexcept;

}//CoAP

#endif /* COAP_TE_PORT_HPP__ */
