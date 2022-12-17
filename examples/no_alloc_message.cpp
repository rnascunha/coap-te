/**
 * @file no_alloc_message.cpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-15
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <cstdint>
#include <iostream>
#include <system_error>       // NOLINT

#include "coap-te.hpp"        // NOLINT
#include "coap-te-debug.hpp"  // NOLINT

int main() {
  using namespace coap_te;    // NOLINT

  /*
   * Making request
   */
  std::cout << "Creating a request message\n-----------------\n";

  no_alloc::request req{type::confirmable, code::get, const_buffer("token")};
  no_alloc::option accept_op{accept::text_plain};
  no_alloc::option content_op{content::json};
  no_alloc::option if_none_op{number::if_none_match};
  no_alloc::option path1_op{number::uri_path, "my"};
  no_alloc::option path2_op{number::uri_path, "path"};

  req.mid(0x1234)
    .add_option(accept_op)
    .add_option(content_op)
    .add_option(if_none_op)
    .add_option(path1_op)
    .add_option(path2_op)
    .payload(const_buffer{"my payload"});

  std::cout << "Request";
  debug::print_message<true>(req);
  std::cout << "\n-----------------\n";

  /**
   * Serializing request
   */
  std::cout << "Serializing... ";
  std::uint8_t data[100];
  mutable_buffer buf(data, 100);
  std::error_code ecs;
  auto size_s = req.serialize(0x1234, buf, ecs);
  if (ecs) {
    std::cerr << "Fail["
              << ecs.value() << "]: "
              << ecs.message() << '\n';
    return EXIT_FAILURE;
  }

  std::cout << "OK[" << size_s << "]\n";


  /**
   * Parsing serialized request (response)
   */
  std::cout << "-----------------\nParsing... ";
  const_buffer buf_p(data, size_s);
  std::error_code ecp;
  response resp;
  auto size_p = message::parse(buf_p, resp, ecp);
  if (ecp) {
    std::cerr << "Fail["
              << ecs.value() << "]: "
              << ecs.message() << '\n';
    return EXIT_FAILURE;
  }

  std::cout << "OK[" << size_p << "]\n-----------------\n";

  std::cout << "Response";
  debug::print_message<true>(resp);
  std::cout << '\n';

  return EXIT_SUCCESS;
}
