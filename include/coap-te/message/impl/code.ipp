/**
 * @file code.ipp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-07
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef COAP_TE_MESSAGE_IMPL_CODE_IPP_
#define COAP_TE_MESSAGE_IMPL_CODE_IPP_

namespace coap_te {
namespace message {

[[nodiscard]] constexpr bool
is_empty(code c) noexcept {
  return c == code::empty;
}

[[nodiscard]] constexpr bool
is_request(code c) noexcept {
  return !is_empty(c) &&
         (static_cast<std::uint8_t>(c) >> 5) == cclass::request;
}

[[nodiscard]] constexpr bool
is_response(code c) noexcept {
  return !is_empty(c) &&
         !is_request(c);
}

[[nodiscard]] constexpr bool
is_success(code c) noexcept {
  return !is_empty(c) &&
         (static_cast<std::uint8_t>(c) >> 5) == cclass::success;
}

[[nodiscard]] constexpr bool
is_client_error(code c) noexcept {
  return !is_empty(c) &&
         (static_cast<std::uint8_t>(c) >> 5) == cclass::client_error;
}

[[nodiscard]] constexpr bool
is_server_error(code c) noexcept {
  return !is_empty(c) &&
         (static_cast<std::uint8_t>(c) >> 5) == cclass::server_error;
}

[[nodiscard]] constexpr bool
is_error(code c) noexcept {
  return is_server_error(c) ||
         is_client_error(c);
}

#if COAP_TE_RELIABLE_CONNECTION == 1
[[nodiscard]] constexpr bool
is_signaling(code c) noexcept {
  return !is_empty(c) &&
         (static_cast<std::uint8_t>(c) >> 5) == cclass::signaling;
}

#endif /* COAP_TE_RELIABLE_CONNECTION == 1 */

[[nodiscard]] constexpr bool
check_code(code c) noexcept {
  switch (c) {
    case code::empty:       // 0.00 Empty
    //  request
    case code::get:         // 0.01 GET
    case code::post:        // 0.02 POST
    case code::put:         // 0.03 PUT
    case code::cdelete:     // 0.04 DELETE
    #if COAP_TE_FETCH_PATCH == 1
    case code::fetch:       // 0.05 FETCH
    case code::patch:       // 0.06 PATCH
    case code::ipatch:      // 0.07 PATCH
    #endif /* COAP_TE_FETCH_PATCH == 1 */
    //  response
    //  success
    case code::success:     // 2.00 Success
    case code::created:     // 2.01 Created
    case code::deleted:     // 2.02 Deleted
    case code::valid:       // 2.03 Valid
    case code::changed:     // 2.04 Changed
    case code::content:     // 2.05 Content
#if COAP_TE_BLOCKWISE_TRANSFER == 1
    case code::ccontinue:   // 2.31 Continue
#endif /* COAP_TE_BLOCKWISE_TRANSFER == 1 */
    //  Client Error
    case code::bad_request:   // 4.00 Bad Request
    case code::unauthorized:  // 4.01 Unauthorized
    case code::bad_option:    // 4.02 Bad Option
    case code::forbidden:     // 4.03 Forbidden
    case code::not_found:     // 4.04 Not Found
    case code::method_not_allowed:   // 4.05 Method Not Allowed
    case code::not_accpetable:  // 4.06 Not Acceptable
#if COAP_TE_BLOCKWISE_TRANSFER == 1
    case code::request_entity_incomplete:  // 4.08 Request Entity Incomplete
#endif /* COAP_TE_BLOCKWISE_TRANSFER == 1 */
#if COAP_TE_FETCH_PATCH == 1
    case code::conflict:       // 4.09 Conflict
#endif /* COAP_TE_FETCH_PATCH == 1 */
    case code::precondition_failed:         // 4.12 Precondition Failed
    case code::request_entity_too_large:    // 4.13 Request Entity Too Large
    case code::unsupported_content_format:  // 4.15 Unsupported Content-Format
#if COAP_TE_FETCH_PATCH == 1
    case code::unprocessable_entity:  // 4.22 Unprocessable Entity
#endif /* COAP_TE_FETCH_PATCH == 1 */
    //  Server Error
    case code::internal_server_error:   // 5.00 Internal Server Error
    case code::not_implemented:         // 5.01 Not Implemented
    case code::bad_gateway:             // 5.02 Bad Gateway
    case code::service_unavaiable:      // 5.03 Service Unavailable
    case code::gateway_timeout:         // 5.04 Gateway Timeout
    case code::proxying_not_supported:  // 5.05 Proxying Not Supported
#if COAP_TE_RELIABLE_CONNECTION == 1
    case code::csm:               // 7.01 CSM
    case code::ping:              // 7.02 Ping
    case code::pong:              // 7.03 Pong
    case code::release:            // 7.04 Release
    case code::abort:              // 7.05 Abort
#endif /* COAP_TE_RELIABLE_CONNECTION == 1 */
      return true;
    default:
      break;
  }
  return false;
}

}  //  namespace message
}  //  namespace coap_te

#endif  //  COAP_TE_MESSAGE_IMPL_CODE_IPP_
