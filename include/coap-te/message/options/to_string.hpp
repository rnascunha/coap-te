/**
 * @file to_string.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief Return string of option values
 * @version 0.1
 * @date 2022-11-21
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef COAP_TE_MESSAGE_OPTIONS_TO_STRING_HPP_
#define COAP_TE_MESSAGE_OPTIONS_TO_STRING_HPP_

#include <string_view>

#include "coap-te/message/options/config.hpp"
#include "coap-te/message/options/definitions.hpp"

namespace coap_te {
namespace message {
namespace options {

[[nodiscard]] constexpr std::string_view
to_string(format t) noexcept {
  switch (t) {
    case format::empty:
      return "empty";
    case format::opaque:
      return "opaque";
    case format::uint:
      return "uint";
    case format::string:
      return "string";
    default:
      break;
  }
  return "unknown";
}

[[nodiscard]] constexpr std::string_view
to_string(const definition& def) noexcept {
  return def.name;
}

[[nodiscard]] constexpr std::string_view
to_string(number op) noexcept {
  return to_string(get_definition(op));
}

[[nodiscard]] constexpr std::string_view
to_string(content format) noexcept {
  switch (format) {
    using type = content;  // typename content_format<IsContentFormat>::type;
    case type::text_plain:    //  = 0,    // text/plain;charset=utf-8
      return "text/plain";
    case type::link_format:   //  = 40,   // application/link-format
      return "link-format";
    case type::xml:           //  = 41,   // application/xml
      return "xml";
    case type::octet_stream:  //  = 42,   // application/octet-stream
      return "octet-stream";
    case type::exi:           //  = 47,   // application/exi
      return "exi";
    case type::json:          //  = 50,   // application/json
      return "json";
#if COAP_TE_FETCH_PATCH == 1
    case type::json_patch_json:  // = 51,   // application/json-patch+json
      return "json-patch+json"
    case type::merge_patch_json:  // = 52   // application/merge-patch+json
      return "merge-patch+json"
#endif /* COAP_TE_FETCH_PATCH == 1 */
    default:
      break;
  }
  return "unknown";
}

}  // namespace options
}  // namespace message
}  // namespace coap_te

#endif  // COAP_TE_MESSAGE_OPTIONS_TO_STRING_HPP_
