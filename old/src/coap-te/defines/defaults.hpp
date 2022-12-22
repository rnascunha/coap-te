#ifndef COAT_TE_DEFINES_DEFAULTS_HPP__
#define COAT_TE_DEFINES_DEFAULTS_HPP__

#include <cstdint>

/**
 * RFC7959 - Block-Wise Transfers in the Constrained Application
 * 				Protocol (CoAP)
 * https://tools.ietf.org/html/rfc7959
 */
#ifndef COAP_TE_MESSAGE_OPTION_BLOCKWISE_TRANSFER
#define COAP_TE_MESSAGE_OPTION_BLOCKWISE_TRANSFER	1
#endif /* COAP_TE_MESSAGE_OPTION_BLOCKWISE_TRANSFER */

/**
 * RFC6690 - Constrained RESTful Environments (CoRE) Link Format
 * https://tools.ietf.org/html/rfc6690
 */
#ifndef COAP_TE_LINK_FORMAT
#define COAP_TE_LINK_FORMAT	1
#endif /* COAP_TE_LINK_FORMAT */

/**
 * RFC8283 - CoAP (Constrained Application Protocol) over TCP, TLS, and WebSockets
 * https://tools.ietf.org/html/rfc8323
 */
#ifndef COAP_TE_ENABLE_STREAM_CONNECTION
#define COAP_TE_ENABLE_STREAM_CONNECTION 1
#endif /* COAP_TE_ENABLE_STREAM_CONNECTION */

/**
 * RFC7641 - Observing Resources in the Constrained Application Protocol (CoAP)
 * https://tools.ietf.org/html/rfc7641
 */
#ifndef COAP_TE_OBSERVABLE_RESOURCE
#define COAP_TE_OBSERVABLE_RESOURCE 1
#endif /* COAP_TE_OBSERVABLE_RESOURCE */

/**
 * RFC8132 - PATCH and FETCH Methods for the Constrained Application Protocol (CoAP)
 * https://tools.ietf.org/html/rfc8132
 */
#ifndef COAP_TE_ENABLE_FETCH_PATCH_VERBS
#define COAP_TE_ENABLE_FETCH_PATCH_VERBS	1
#endif /* COAP_TE_ENABLE_FETCH_PATCH_VERBS */

/**
 * RFC7967 - Constrained Application Protocol (CoAP) Option for No Server Response
 * https://tools.ietf.org/html/rfc7967
 */
#ifndef COAP_TE_OPTION_NO_RESPONSE
#define COAP_TE_OPTION_NO_RESPONSE	1
#endif /* COAP_TE_OPTION_NO_RESPONSE */

/**
 * RFC8768 - Constrained Application Protocol (CoAP) Hop-Limit Option
 * https://tools.ietf.org/html/rfc8768
 */
#ifndef COAP_TE_OPTION_HOP_LIMIT
#define COAP_TE_OPTION_HOP_LIMIT 1
#endif /* COAP_TE_OPTION_HOP_LIMIT */

namespace CoAP{

static constexpr std::uint16_t default_port = 5683;
static constexpr std::uint16_t default_security_port = 5684;
#if COAP_TE_ENABLE_STREAM_CONNECTION == 1
static constexpr std::uint16_t default_ws_port = 80;
static constexpr std::uint16_t default_wss_port = 433;
#endif /* COAP_TE_ENABLE_STREAM_CONNECTION */

using disable = void*;

}

#endif /* COAT_TE_DEFINES_DEFAULTS_HPP__ */
