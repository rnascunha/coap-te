/**
 * @file options.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-13
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef COAP_TE_DEBUG_MESSAGE_OPTIONS_OPTION_HPP_
#define COAP_TE_DEBUG_MESSAGE_OPTIONS_OPTION_HPP_

#include <iostream>
#include <variant>

#include "coap-te/core/utility.hpp"
#include "coap-te/core/const_buffer.hpp"
#include "coap-te/debug/to_string.hpp"

#include "coap-te/message/options/config.hpp"
#include "coap-te/message/options/option.hpp"
#include "coap-te/debug/message/options/to_string.hpp"

namespace coap_te {
namespace debug {

void
print_option_data(std::ostream& os,
                  const coap_te::message::options::option& op,
                  coap_te::message::options::format f) noexcept {
  namespace opt = coap_te::message::options;

  switch (f) {
    case opt::format::empty:
      os << "<empty>";
      break;
    case opt::format::opaque:
      os << to_hex(
              coap_te::const_buffer{op.data(), op.data_size()});
      break;
    case opt::format::uint:
    {
      auto u = op.get_unsigned();
      os << u;
      if (op.option_number() == opt::number::content_format ||
          op.option_number() == opt::number::accept) {
            os << ' '
               << to_string(
                    static_cast<opt::content>(u));
          }
    }
      break;
    case opt::format::string:
      os << to_string(
                coap_te::const_buffer{op.data(), op.data_size()});
      break;
    default:
      break;
  }
}

void
print_option_data(std::ostream& os,
  const coap_te::message::options::option::value_type& var) noexcept {
  namespace opt = coap_te::message::options;

  std::visit(coap_te::core::overloaded{
    [&](std::monostate){ os << "<no value>"; },
    [&](opt::empty_format){ os << "<empty>"; },
    [&](opt::option::unsigned_type data) { os << data; },
    [&](const coap_te::const_buffer& data) {
      os << to_hex(data);
    }
  }, var);
}

void
print_option(std::ostream& os,
             const coap_te::message::options::option& op) noexcept {
  namespace opt = coap_te::message::options;

  auto& def = opt::get_definition(op.option_number());
  os << static_cast<opt::number_type>(op.option_number()) << '|'
     << def.name << '['
     << op.data_size() << "]:";
  print_option_data(os, op, def.type);
}

}  // namespace debug
}  // namespace coap_te

std::ostream&
operator<<(std::ostream& os,
          const coap_te::message::options::option::value_type& var) noexcept {
  coap_te::debug::print_option_data(os, var);
  return os;
}

std::ostream&
operator<<(std::ostream& os,
           const coap_te::message::options::option& op) noexcept {
  coap_te::debug::print_option(os, op);
  return os;
}

#endif  // COAP_TE_DEBUG_MESSAGE_OPTIONS_OPTION_HPP_
