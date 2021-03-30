#ifndef COAT_TE_DEFINES_DEFAULTS_HPP__
#define COAT_TE_DEFINES_DEFAULTS_HPP__

#include <cstdint>

/**
 * RFC7959 - Block-Wise Transfers in the Constrained Application
 * 				Protocol (CoAP)
 * https://tools.ietf.org/html/rfc7959
 */
#ifndef COAP_TE_BLOCKWISE_TRANSFER
#define COAP_TE_BLOCKWISE_TRANSFER	1
#endif /* COAP_TE_BLOCKWISE_TRANSFER */

/**
 * RFC6690 - Constrained RESTful Environments (CoRE) Link Format
 * https://tools.ietf.org/html/rfc6690
 */
#ifndef COAP_TE_LINK_FORMAT
#define COAP_TE_LINK_FORMAT	1
#endif /* COAP_TE_LINK_FORMAT */

namespace CoAP{

static constexpr std::uint16_t default_port = 5683;
static constexpr std::uint16_t default_security_port = 5684;

using disable = void*;

}

#endif /* COAT_TE_DEFINES_DEFAULTS_HPP__ */
