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
#include "coap-te/debug/message/options/to_string.hpp"

#include "coap-te/message/options/config.hpp"
#include "coap-te/message/options/option.hpp"
#include "coap-te/message/options/option_func.hpp"
#include "coap-te/message/options/traits.hpp"

namespace coap_te {
namespace debug {

template<typename Option>
void
print_option_data(std::ostream& os,
                  const Option& op,
                  coap_te::message::options::format f) noexcept {
  namespace opt = coap_te::message::options;
  static_assert(opt::is_option_v<Option>,
                "Must be option type");

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
      auto u = unsigned_option(op);
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

template<typename Option>
void
print_option(std::ostream& os,
             const Option& op) noexcept {
  namespace opt = coap_te::message::options;
  static_assert(opt::is_option_v<Option>,
                "Must be option type");

  auto& def = opt::get_definition(op.option_number());
  os << static_cast<opt::number_type>(op.option_number()) << '|'
     << def.name << '['
     << op.data_size() << "]:";
  print_option_data(os, op, def.type);
}

}  // namespace debug
}  // namespace coap_te

template<typename Option,
         typename =
          std::enable_if_t<
            coap_te::message::options::is_option_v<Option>>>
std::ostream&
operator<<(std::ostream& os,
           const Option& op) noexcept {
  coap_te::debug::print_option(os, op);
  return os;
}

#endif  // COAP_TE_DEBUG_MESSAGE_OPTIONS_OPTION_HPP_
