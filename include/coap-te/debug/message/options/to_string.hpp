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
#ifndef COAP_TE_DEBUG_MESSAGE_OPTIONS_TO_STRING_HPP_
#define COAP_TE_DEBUG_MESSAGE_OPTIONS_TO_STRING_HPP_

#include <string_view>

#include "coap-te/message/options/config.hpp"
#include "coap-te/message/options/definitions.hpp"

namespace coap_te {
namespace debug {

[[nodiscard]] constexpr std::string_view
to_string(coap_te::message::options::format t) noexcept {
  switch (t) {
    using format = coap_te::message::options::format;
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
to_string(const coap_te::message::options::definition& def) noexcept {
  return def.name;
}

[[nodiscard]] constexpr std::string_view
to_string(coap_te::message::options::number op) noexcept {
  return to_string(get_definition(op));
}

[[nodiscard]] constexpr std::string_view
to_string(coap_te::message::options::content format) noexcept {
  switch (format) {
    using ftype =
      coap_te::message::options::content;
    case ftype::text_plain:    //  = 0,    // text/plain;charset=utf-8
      return "text/plain";
    case ftype::link_format:   //  = 40,   // application/link-format
      return "link-format";
    case ftype::xml:           //  = 41,   // application/xml
      return "xml";
    case ftype::octet_stream:  //  = 42,   // application/octet-stream
      return "octet-stream";
    case ftype::exi:           //  = 47,   // application/exi
      return "exi";
    case ftype::json:          //  = 50,   // application/json
      return "json";
#if COAP_TE_ENABLE_FETCH_PATCH_VERBS == 1
    case ftype::json_patch_json:  // = 51,   // application/json-patch+json
      return "json-patch+json";
    case ftype::merge_patch_json:  // = 52   // application/merge-patch+json
      return "merge-patch+json";
#endif /* COAP_TE_ENABLE_FETCH_PATCH_VERBS == 1 */
    default:
      break;
  }
  return "unknown";
}

}  // namespace debug
}  // namespace coap_te

#endif  // COAP_TE_DEBUG_MESSAGE_OPTIONS_TO_STRING_HPP_
