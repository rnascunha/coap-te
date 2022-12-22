/**
 * @file code.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-07
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef COAP_TE_MESSAGE_CODE_HPP_
#define COAP_TE_MESSAGE_CODE_HPP_

namespace coap_te {
namespace message {

// https://tools.ietf.org/html/rfc7252#section-5.9
enum cclass : std::uint8_t {
  request       = 0,
  // Response
  success       = 2,
  client_error  = 4,
  server_error  = 5,
#if COAP_TE_ENABLE_STREAM_CONNECTION == 1
  signaling      = 7
#endif /* COAP_TE_ENABLE_STREAM_CONNECTION == 1 */
};

constexpr std::uint8_t
mcode(cclass code_class, uint8_t detail) noexcept {
  return code_class << 5 | detail;
}

enum class code : std::uint8_t {
  empty             = 0,                                // 0.00 Empty
  // request - https://tools.ietf.org/html/rfc7252#section-12.1.1
  get         = mcode(cclass::request, 1),  // 0.01 GET
  post        = mcode(cclass::request, 2),  // 0.02 POST
  put         = mcode(cclass::request, 3),  // 0.03 PUT
  cdelete     = mcode(cclass::request, 4),  // 0.04 DELETE
  #if COAP_TE_ENABLE_FETCH_PATCH_VERBS == 1
  // https://tools.ietf.org/html/rfc8132#section-6
  fetch       = mcode(cclass::request, 5),  // 0.05 FETCH
  patch       = mcode(cclass::request, 6),  // 0.06 PATCH
  ipatch      = mcode(cclass::request, 7),  // 0.07 PATCH
  #endif /* COAP_TE_ENABLE_FETCH_PATCH_VERBS == 1 */
  // response - https://tools.ietf.org/html/rfc7252#section-12.1.2
  // success
  success     = mcode(cclass::success, 0),  // 2.00 Success
  created     = mcode(cclass::success, 1),  // 2.01 Created
  deleted     = mcode(cclass::success, 2),  // 2.02 Deleted
  valid       = mcode(cclass::success, 3),  // 2.03 Valid
  changed     = mcode(cclass::success, 4),  // 2.04 Changed
  content     = mcode(cclass::success, 5),  // 2.05 Content
  #if COAP_TE_MESSAGE_OPTION_BLOCKWISE_TRANSFER == 1
  ccontinue   = mcode(cclass::success, 31),  // 2.31 Continue
  #endif /* COAP_TE_MESSAGE_OPTION_BLOCKWISE_TRANSFER == 1 */
  // Client Error
  bad_request = mcode(cclass::client_error, 0),  // 4.00 Bad Request
  unauthorized = mcode(cclass::client_error, 1),  // 4.01 Unauthorized
  bad_option  = mcode(cclass::client_error, 2),  // 4.02 Bad Option
  forbidden   = mcode(cclass::client_error, 3),  // 4.03 Forbidden
  not_found   = mcode(cclass::client_error, 4),  // 4.04 Not Found
  method_not_allowed
              = mcode(cclass::client_error, 5),  // 4.05 Method Not Allowed
  not_accpetable
              = mcode(cclass::client_error, 6),  // 4.06 Not Acceptable
  #if COAP_TE_MESSAGE_OPTION_BLOCKWISE_TRANSFER == 1
  // 4.08 Request Entity Incomplete
  request_entity_incomplete
              = mcode(cclass::client_error, 8),
  #endif /* COAP_TE_MESSAGE_OPTION_BLOCKWISE_TRANSFER == 1 */
  #if COAP_TE_ENABLE_FETCH_PATCH_VERBS == 1
  // https://tools.ietf.org/html/rfc8132#section-6
  conflict    = mcode(cclass::client_error, 9),  // 4.09 Conflict
  #endif /* COAP_TE_ENABLE_FETCH_PATCH_VERBS == 1 */
  precondition_failed
              = mcode(cclass::client_error, 12),  // 4.12 Precondition Failed
  // 4.13 Request Entity Too Large
  request_entity_too_large
              = mcode(cclass::client_error, 13),
  // 4.15 Unsupported Content-Format
  unsupported_content_format
              = mcode(cclass::client_error, 15),
  #if COAP_TE_ENABLE_FETCH_PATCH_VERBS == 1
  // https://tools.ietf.org/html/rfc8132#section-6
  unprocessable_entity
              = mcode(cclass::client_error, 22),  //  4.22 Unprocessable Entity
  #endif /* COAP_TE_ENABLE_FETCH_PATCH_VERBS == 1 */
  // Server Error
  internal_server_error
              = mcode(cclass::server_error, 0),  // 5.00 Internal Server Error
  not_implemented
              = mcode(cclass::server_error, 1),  // 5.01 Not Implemented
  bad_gateway = mcode(cclass::server_error, 2),  // 5.02 Bad Gateway
  service_unavaiable
              = mcode(cclass::server_error, 3),  // 5.03 Service Unavailable
  gateway_timeout
              = mcode(cclass::server_error, 4),  // 5.04 Gateway Timeout
  proxying_not_supported
              = mcode(cclass::server_error, 5),  // 5.05 Proxying Not Supported
  #if COAP_TE_OPTION_HOP_LIMIT == 1
  hop_limit_reached
              = mcode(cclass::server_error, 8),  // 5.08 Hop Limit Reached
  #endif /* COAP_TE_OPTION_HOP_LIMIT == 1 */
  #if COAP_TE_ENABLE_STREAM_CONNECTION == 1
  csm         = mcode(cclass::signaling, 1),  // 7.01 CSM
  ping        = mcode(cclass::signaling, 2),  // 7.02 Ping
  pong        = mcode(cclass::signaling, 3),  // 7.03 Pong
  release     = mcode(cclass::signaling, 4),  // 7.04 Release
  abort       = mcode(cclass::signaling, 5),  // 7.05 Abort
#endif /* COAP_TE_ENABLE_STREAM_CONNECTION == 1 */
};

[[nodiscard]] constexpr bool
is_empty(code) noexcept;
[[nodiscard]] constexpr bool
is_request(code) noexcept;
[[nodiscard]] constexpr bool
is_response(code) noexcept;
[[nodiscard]] constexpr bool
is_success(code) noexcept;
[[nodiscard]] constexpr bool
is_client_error(code) noexcept;
[[nodiscard]] constexpr bool
is_server_error(code) noexcept;
[[nodiscard]] constexpr bool
is_error(code) noexcept;
#if COAP_TE_ENABLE_STREAM_CONNECTION == 1
[[nodiscard]] constexpr bool
is_signaling(code) noexcept;
#endif /* COAP_TE_ENABLE_STREAM_CONNECTION == 1 */

[[nodiscard]] constexpr bool
check_code(code) noexcept;

}   // namespace message
}   // namespace coap_te

#include "impl/code.ipp"

#endif  // COAP_TE_MESSAGE_CODE_HPP_
