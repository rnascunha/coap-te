/**
 * @file to_string.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-15
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef COAP_TE_DEBUG_MESSAGE_TO_STRING_HPP_
#define COAP_TE_DEBUG_MESSAGE_TO_STRING_HPP_

#include <string_view>

#include "coap-te/message/config.hpp"
#include "coap-te/message/code.hpp"

namespace coap_te {
namespace debug {

[[nodiscard]] constexpr std::string_view
to_string(coap_te::message::type tp) noexcept {
  switch (tp) {
    using type = coap_te::message::type;
    case type::confirmable:
      return "confirmable";
    case type::nonconfirmable:
      return "nonconfirmable";
    case type::ack:
      return "ack";
    case type::reset:
      return "reset";
    default:
      break;
  }
  return "unknow";
}

[[nodiscard]] constexpr std::string_view
to_string(coap_te::message::code co) noexcept {
  switch (co) {
    using code = coap_te::message::code;
    case code::empty:            // 0.00 Empty
      return "empty";
    //  request
    case code::get:              // 0.01 GET
      return "get";
    case code::post:            // 0.02 POST
      return "post";
    case code::put:              // 0.03 PUT
      return "put";
    case code::cdelete:          // 0.04 DELETE
      return "delete";
    #if  COAP_TE_FETCH_PATCH  == 1
    case code::fetch:            // 0.05 FETCH
      return "fetch";
    case code::patch:            // 0.06 PATCH
      return "patch";
    case code::ipatch:          // 0.07 PATCH
      return "ipatch";
    #endif /* COAP_TE_FETCH_PATCH == 1 */
    //  response
    //  success
    case code::success:          // 2.00 Success
      return "success";
    case code::created:          // 2.01 Created
      return "created";
    case code::deleted:          // 2.02 Deleted
      return "deleted";
    case code::valid:            // 2.03 Valid
      return "valid";
    case code::changed:          // 2.04 Changed
      return "changed";
    case code::content:          // 2.05 Content
      return "content";
#if  COAP_TE_BLOCKWISE_TRANSFER == 1
    case code::ccontinue:        // 2.31 Continue
      return "continue";
#endif /* COAP_TE_BLOCKWISE_TRANSFER == 1 */
    //  Client Error
    case code::bad_request:      // 4.00 Bad Request
      return "bad request";
    case code::unauthorized:    // 4.01 Unauthorized
      return "unauthorized";
    case code::bad_option:      // 4.02 Bad Option
      return "bad option";
    case code::forbidden:        // 4.03 Forbidden
      return "forbidden";
    case code::not_found:        // 4.04 Not Found
      return "not found";
    case code::method_not_allowed:      // 4.05 Method Not Allowed
      return "method not allowed";
    case code::not_accpetable:  // 4.06 Not Acceptable
      return "not accpetable";
#if  COAP_TE_BLOCKWISE_TRANSFER == 1
    case code::request_entity_incomplete:  // 4.08 Request Entity Incomplete
      return "request entity incomplete";
#endif /* COAP_TE_BLOCKWISE_TRANSFER == 1 */
#if  COAP_TE_FETCH_PATCH  == 1
    case code::conflict:                  // 4.09 Conflict
      return "conflict";
#endif /* COAP_TE_FETCH_PATCH == 1 */
    case code::precondition_failed:        // 4.12 Precondition Failed
      return "precondition failed";
    case code::request_entity_too_large:  // 4.13 Request Entity Too Large
      return "request entity too large";
    case code::unsupported_content_format:  // 4.15 Unsupported Content-Format
      return "unsupported content format";
#if  COAP_TE_FETCH_PATCH  == 1
    case code::unprocessable_entity:    // 4.22 Unprocessable Entity
      return "unprocessable entity";
#endif /* COAP_TE_FETCH_PATCH == 1 */
    //  Server Error
    case code::internal_server_error:    // 5.00 Internal Server Error
      return "internal server error";
    case code::not_implemented:          // 5.01 Not Implemented
      return "not implemented";
    case code::bad_gateway:              // 5.02 Bad Gateway
      return "bad gateway";
    case code::service_unavaiable:      // 5.03 Service Unavailable
      return "service unavaiable";
    case code::gateway_timeout:          // 5.04 Gateway Timeout
      return "gateway timeout";
    case code::proxying_not_supported:  // 5.05 Proxying Not Supported
      return "proxying not supported";
#if COAP_TE_RELIABLE_CONNECTION == 1
    case code::csm:                      // 7.01 CSM
      return "csm";
    case code::ping:                    // 7.02 Ping
      return "ping";
    case code::pong:                    // 7.03 Pong
      return "pong";
    case code::release:                  // 7.04 Release
      return "release";
    case code::abort:                    // 7.05 Abort
      return "abort";
#endif /* COAP_TE_RELIABLE_CONNECTION == 1 */
    default:
      break;
  }
  return "unknow";
}

[[nodiscard]] constexpr std::string_view
to_string_code(coap_te::message::code co) noexcept {
  switch (co) {
    using code = coap_te::message::code;
    case code::empty:            // 0.00 Empty
      return "0.00";
    //  request
    case code::get:              // 0.01 GET
      return "0.01";
    case code::post:            // 0.02 POST
      return "0.02";
    case code::put:              // 0.03 PUT
      return "0.03";
    case code::cdelete:          // 0.04 DELETE
      return "0.04";
    #if  COAP_TE_FETCH_PATCH  == 1
    case code::fetch:            // 0.05 FETCH
      return "0.05";
    case code::patch:            // 0.06 PATCH
      return "0.06";
    case code::ipatch:          // 0.07 PATCH
      return "0.07";
    #endif /* COAP_TE_FETCH_PATCH == 1 */
    //  response
    //  success
    case code::success:          // 2.00 Success
      return "2.00";
    case code::created:          // 2.01 Created
      return "2.01";
    case code::deleted:          // 2.02 Deleted
      return "2.02";
    case code::valid:            // 2.03 Valid
      return "2.03";
    case code::changed:          // 2.04 Changed
      return "2.04";
    case code::content:          // 2.05 Content
      return "2.05";
#if  COAP_TE_BLOCKWISE_TRANSFER == 1
    case code::ccontinue:        // 2.31 Continue
      return "2.31";
#endif /* COAP_TE_BLOCKWISE_TRANSFER == 1 */
    //  Client Error
    case code::bad_request:      // 4.00 Bad Request
      return "4.00";
    case code::unauthorized:    // 4.01 Unauthorized
      return "4.01";
    case code::bad_option:      // 4.02 Bad Option
      return "4.02";
    case code::forbidden:        // 4.03 Forbidden
      return "4.03";
    case code::not_found:        // 4.04 Not Found
      return "4.04";
    case code::method_not_allowed:      // 4.05 Method Not Allowed
      return "4.05";
    case code::not_accpetable:  // 4.06 Not Acceptable
      return "4.06";
#if  COAP_TE_BLOCKWISE_TRANSFER == 1
    case code::request_entity_incomplete:  // 4.08 Request Entity Incomplete
      return "4.08";
#endif /* COAP_TE_BLOCKWISE_TRANSFER == 1 */
#if  COAP_TE_FETCH_PATCH  == 1
    case code::conflict:                  // 4.09 Conflict
      return "4.09";
#endif /* COAP_TE_FETCH_PATCH == 1 */
    case code::precondition_failed:        // 4.12 Precondition Failed
      return "4.12";
    case code::request_entity_too_large:  // 4.13 Request Entity Too Large
      return "4.13";
    case code::unsupported_content_format:  // 4.15 Unsupported Content-Format
      return "4.15";
#if  COAP_TE_FETCH_PATCH  == 1
    case code::unprocessable_entity:    // 4.22 Unprocessable Entity
      return "4.22";
#endif /* COAP_TE_FETCH_PATCH == 1 */
    //  Server Error
    case code::internal_server_error:    // 5.00 Internal Server Error
      return "5.00";
    case code::not_implemented:          // 5.01 Not Implemented
      return "5.01";
    case code::bad_gateway:              // 5.02 Bad Gateway
      return "5.02";
    case code::service_unavaiable:      // 5.03 Service Unavailable
      return "5.03";
    case code::gateway_timeout:          // 5.04 Gateway Timeout
      return "5.04";
    case code::proxying_not_supported:  // 5.05 Proxying Not Supported
      return "5.05";
#if COAP_TE_RELIABLE_CONNECTION == 1
    case code::csm:                      // 7.01 CSM
      return "7.01";
    case code::ping:                    // 7.02 Ping
      return "7.02";
    case code::pong:                    // 7.03 Pong
      return "7.03";
    case code::release:                  // 7.04 Release
      return "7.04";
    case code::abort:                    // 7.05 Abort
      return "7.05";
#endif /* COAP_TE_RELIABLE_CONNECTION == 1 */
    default:
      break;
  }
  return "unknow";
}

}  // namespace debug
}  // namespace coap_te

#endif  // COAP_TE_DEBUG_MESSAGE_TO_STRING_HPP_
