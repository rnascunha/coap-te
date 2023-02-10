/**
 * @file definitions.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief Definitions of options values
 * @version 0.1
 * @date 2022-11-21
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef COAP_TE_MESSAGE_OPTIONS_DEFINITIONS_HPP_
#define COAP_TE_MESSAGE_OPTIONS_DEFINITIONS_HPP_

#include "coap-te/message/options/config.hpp"

namespace coap_te {
namespace message {
namespace options {

// https://tools.ietf.org/html/rfc7252#section-5.10
enum class number : number_type {
  invalid       = invalid,  // Invalid option
  if_match      = 1,        // If-Match
  uri_host      = 3,        // Uri-Host
  etag          = 4,        // ETag
  if_none_match = 5,        // If-None-Match
#if COAP_TE_MESSAGE_OPTION_OBSERVABLE_RESOURCE == 1
  observe       = 6,        // Observe
#endif /* COAP_TE_MESSAGE_OPTION_OBSERVABLE_RESOURCE == 1 */
  uri_port      = 7,        // Uri-Port
  location_path = 8,        // Location-Path
  uri_path      = 11,       // Uri-Path
  content_format  = 12,     // Content-Format
  max_age       = 14,       // Max-Age
  uri_query     = 15,       // Uri-Query
#if COAP_TE_MESSAGE_OPTION_HOP_LIMIT == 1
  hop_limit     = 16,       // Hop-Limit
#endif /* COAP_TE_MESSAGE_OPTION_HOP_LIMIT == 1 */
  accept        = 17,       // Accept
  location_query  = 20,     // Location-Query
#if COAP_TE_MESSAGE_OPTION_BLOCKWISE_TRANSFER == 1
  block2        = 23,       // Block2
  block1        = 27,       // Block1
  size2         = 28,       // Size2
#endif /* COAP_TE_MESSAGE_OPTION_BLOCKWISE_TRANSFER == 1 */
  proxy_uri     = 35,       // Proxy-Uri
  proxy_scheme  = 39,       // Proxy-Scheme
  size1         = 60,       // Size1
#if COAP_TE_MESSAGE_OPTION_NO_RESPONSE == 1
  no_response   = 258       // No response
#endif /* COAP_TE_MESSAGE_OPTION_NO_RESPONSE == 1 */
};

static constexpr const
definition definitions[] = {
  // number               name            repeat  format        min   max
  {number::invalid,       "Invalid",      false,  format::empty,  0,  0},
  {number::if_match,      "If-Match",     true,   format::opaque, 0,  8},
  {number::uri_host,      "Uri-Host",     false,  format::string, 1,  255},
  {number::etag,          "ETag",         true,   format::opaque, 1,  8},
  {number::if_none_match, "If-None-Match",  false,  format::empty,  0,  0},
#if COAP_TE_MESSAGE_OPTION_OBSERVABLE_RESOURCE == 1
  {number::observe,       "Observe",      false,  format::uint,   0,  3},
#endif /* COAP_TE_MESSAGE_OPTIONS_OBSERVABLE_RESOURCE == 1 */
  {number::uri_port,      "Uri-Port",     false,  format::uint,   0,  2},
  {number::location_path, "Location-Path",  true, format::string, 0,  255},
  {number::uri_path,      "Uri-Path",     true,   format::string, 0,  255},
  {number::content_format,  "Content-Format", false,  format::uint, 0,  2},
  {number::max_age,       "Max-Age",      false,  format::uint,   0,  4},
  {number::uri_query,     "Uri-Query",    true,   format::string, 0,  255},
#if COAP_TE_MESSAGE_OPTION_HOP_LIMIT == 1
  {number::hop_limit,     "Hop-Limit",    false,  format::uint,   0,  1},
#endif /* COAP_TE_MESSAGE_OPTION_HOP_LIMIT == 1 */
  {number::accept,        "Accept",       false,   format::uint,  0,  2},
  {number::location_query,  "Location-Query", true, format::string, 0,  255},
#if COAP_TE_MESSAGE_OPTION_BLOCKWISE_TRANSFER == 1
  {number::block2,        "Block2",       false,  format::uint,   0,  3},
  {number::block1,        "Block1",       false,  format::uint,   0,  3},
  {number::size2,         "Size2",        false,  format::uint,   0,  4},
#endif /* COAP_TE_MESSAGE_OPTION_BLOCKWISE_TRANSFER == 1 */
  {number::proxy_uri,     "Proxy-Uri",    false,  format::string, 1,  1034},
  {number::proxy_scheme,  "Proxy-Scheme", false,  format::string, 1,  255},
  {number::size1,         "Size1",        false,  format::uint,   0,  4},
#if COAP_TE_MESSAGE_OPTION_NO_RESPONSE == 1
  {number::no_response,   "No response",  false,  format::uint,   0,  1}
#endif /* COAP_TE_MESSAGE_OPTION_NO_RESPONSE == 1 */
};

[[nodiscard]] constexpr const definition&
get_definition(number n) noexcept;

}  // namespace options
}  // namespace message
}  // namespace coap_te

#include "impl/definitions.ipp"

#endif  // COAP_TE_MESSAGE_OPTIONS_DEFINITIONS_HPP_
