/**
 * @file message.cpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-15
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <cstdint>

#include "coap-te.hpp"
#include "coap-te-debug.hpp"

int main() {
  namespace msg = coap_te::message;
  namespace opt = msg::options;
  using msg_s = msg::message<
                  coap_te::const_buffer,
                  coap_te::core::sorted_list<msg::options::option>>;
  using msg_p = msg::message<>;

  static constexpr const char* token = "token";

  msg_s m1;
  m1.header(msg::type::confirmable, msg::code::get)
    .token(coap_te::const_buffer(token))
    .mid(0x1234)
    .add_option(opt::option(opt::accept::text_plain))
    .add_option(opt::option(opt::content::json))
    .add_option(opt::option::create(opt::number::if_none_match))
    .add_option(opt::option::create(opt::number::uri_path, "my"))
    .add_option(opt::option::create(opt::number::uri_path, "path"))
    .payload(coap_te::const_buffer("my payload"));

  coap_te::debug::print_message<msg_s, true>(std::cout, m1);
  std::cout << '\n';

  std::cout << "Serializing...\n";
  std::uint8_t data[100];
  coap_te::mutable_buffer buf(data, 100);
  std::error_code ecs;
  auto size_s = m1.serialize(0x1234, buf, ecs);
  if (ecs) {
    std::cerr << "Error serializing! ["
              << ecs.value() << "]: "
              << ecs.message() << '\n';
    return EXIT_FAILURE;
  }

  std::cout << "Serialized! [" << size_s << "]\n";

  std::cout << "Parsing...\n";
  coap_te::const_buffer buf_p(data, size_s);
  std::error_code ecp;
  msg_p mout;
  auto size_p = coap_te::message::parse(buf_p, mout, ecp);
  if (ecp) {
    std::cerr << "Error parsing! ["
              << ecs.value() << "]: "
              << ecs.message() << '\n';
    return EXIT_FAILURE;
  }

  std::cout << "Parsed! [" << size_p << "]\n";
  coap_te::debug::print_message<msg_p, true>(std::cout, mout);
  std::cout << '\n';

  return EXIT_SUCCESS;
}
