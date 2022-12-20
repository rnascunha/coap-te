/**
 * @file message.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-15
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef COAP_TE_DEBUG_MESSAGE_MESSAGE_HPP_
#define COAP_TE_DEBUG_MESSAGE_MESSAGE_HPP_

#include <iostream>
#include <iomanip>

#include "coap-te/message/message.hpp"
#include "coap-te/message/traits.hpp"
#include "coap-te/debug/to_string.hpp"
#include "coap-te/debug/message/to_string.hpp"
#include "coap-te/debug/message/options/option.hpp"

namespace coap_te {
namespace debug {

template<bool PayloadAsString = false,
         typename Message>
void print_message(const Message& message,
                   std::ostream& os = std::cout) noexcept {
  static_assert(coap_te::message::is_message_v<Message>,
                "Must be message type");
  // header
  os << "[" << message.size() << "]"
     << to_string(message.get_type()) << "|"
     << to_string(message.get_code()) << "["
     << std::setw(4) << std::setfill('0') << std::hex
     << message.mid() << std::dec << "]/t["
     << message.token().size() << "]:"
     << to_hex(message.token(), "-") << "\n";

  // options
  os << "op[" << message.count_options() << "]\n";
  for (auto op : message) {
    os << "\t" << op << "\n";
  }

  // payload
  os << "P[" << message.payload().size() << "]:";
  if constexpr(PayloadAsString)
    os << to_string(message.payload());
  else
    os << to_hex(message.payload());
}

}  // namespace debug
}  // namespace coap_te

#endif  // COAP_TE_DEBUG_MESSAGE_MESSAGE_HPP_
