/**
 * @file debug.cpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-02-09
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "emscripten/bind.h"

#include "coap-te/message/config.hpp"
#include "coap-te/message/code.hpp"
#include "coap-te/message/options/config.hpp"

#include "coap-te/debug/message/to_string.hpp"
#include "coap-te/debug/message/options/to_string.hpp"

namespace coap_te {
namespace embind {

[[nodiscard]] std::string
to_string_type(coap_te::message::type t) noexcept {
  return std::string(coap_te::debug::to_string(t));
}

[[nodiscard]] std::string
to_string_code(coap_te::message::code c) {
  return std::string(coap_te::debug::to_string(c));
}

[[nodiscard]] std::string
to_string_code_value(coap_te::message::code c) {
  return std::string(coap_te::debug::to_string_code(c));
}

[[nodiscard]] std::string
to_string_op_format(coap_te::message::options::format f) noexcept {
  return std::string(coap_te::debug::to_string(f));
}

[[nodiscard]] std::string
to_string_content(coap_te::message::options::content c) noexcept {
  return std::string(coap_te::debug::to_string(c));
}

}  // namespace embind
}  // namespace coap_te

EMSCRIPTEN_BINDINGS(coap_te_debug) {
  namespace em = emscripten;
  namespace co = coap_te::message;
  namespace cod = coap_te::debug;
  namespace coe = coap_te::embind;

  em::function("to_string_type", &coe::to_string_type);
  em::function("to_string_code", &coe::to_string_code);
  em::function("to_string_code_value", &coe::to_string_code_value);
  em::function("to_string_of_format", &coe::to_string_op_format);
}
