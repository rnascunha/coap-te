/**
 * @file option_list.cpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */
// #include <vector>
#include <algorithm>
#include <iostream>
#include <cstdint>
#include <system_error>       // NOLINT

#include "coap-te.hpp"        // NOLINT
#include "coap-te-debug.hpp"  // NOLINT

int main() {
  namespace opt = coap_te::message::options;

  std::uint8_t data[] = {1, 7, 243, 23};
  using container = coap_te::core::sorted_list<opt::option>;
  // using container = vector<opt::option>;

  container list{
      opt::option::create<opt::check_all, false>
                            (opt::number::if_none_match),
      opt::option::create<opt::check_all, false>
                            (opt::number::uri_host, "192.169.9.2"),
      opt::option::create<opt::check_all, false>
                            (opt::number::uri_port, 5683),
      opt::option::create<opt::check_all, false>
                            (opt::number::if_match,
                            coap_te::const_buffer(data))};

  // std::stable_sort(list.begin(), list.end());

  std::cout << "#|name[size]:data\n";
  std::cout << "-----------------\n";
  for (const auto& op : list) {
    std::cout << op << '\n';
  }

  std::cout << "\n";

  std::uint8_t data_s[100];
  coap_te::mutable_buffer buf_s(data_s);
  std::error_code ec;
  auto size_s = opt::serialize(list.begin(), list.end(), buf_s, ec);
  if (ec) {
    std::cerr << "Error! [" << ec.value() << "] " << ec.message() << '\n';
    return EXIT_FAILURE;
  }

  std::cout << "Serialized! [" << size_s << "]\n";

  opt::vector_options opt_vec(coap_te::const_buffer{data_s, size_s});

  // for (auto it = opt_vec.begin(); !it.is_end(); ++it) {
  //   std::cout << *it << "\n";
  // }

  for (auto op : opt_vec) {
    std::cout << op << "\n";
  }

  return EXIT_SUCCESS;
}
