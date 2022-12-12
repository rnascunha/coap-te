/**
 * @file option_list.cpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-08
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <gtest/gtest.h>

#include <vector>
#include <set>
#include <cstdint>

#include "coap-te/core/mutable_buffer.hpp"
#include "coap-te/core/sorted_no_alloc_list.hpp"
#include "coap-te/message/options/config.hpp"
#include "coap-te/message/options/option.hpp"
#include "coap-te/message/options/checks.hpp"
#include "coap-te/message/options/vector_options.hpp"

namespace opt = coap_te::message::options;

template<bool Sort = true, typename Container>
void test_option_list_success(Container& list) {    // NOLINT
  if constexpr (Sort) {
    std::stable_sort(list.begin(), list.end());
  }

  std::uint8_t data[100];
  coap_te::mutable_buffer buf(data);
  std::error_code ec;
  std::size_t size = opt::serialize<opt::check_none>(list, buf, ec);
  EXPECT_FALSE(ec);

  opt::vector_options list_s(coap_te::const_buffer(data, size));
  auto i = list.begin();
  for (auto it = list_s.begin(); it.is_end(); ++i) {
    auto op = *it;
    EXPECT_EQ(op, *i);
    EXPECT_EQ(op.option_number(), i->option_number());
    EXPECT_EQ(op.size(), i->size());
    EXPECT_EQ(0, std::memcmp(op.data(), i->data(), op.size()));
    ++i;
  }
}

TEST(OptionList, List) {
  {
    std::vector<opt::option> list{
      opt::option::create<opt::check_all, false>
                            (opt::number::if_none_match),
      opt::option::create<opt::check_all, false>
                            (opt::number::uri_host, "192.169.9.2"),
      opt::option::create<opt::check_all, false>
                            (opt::number::uri_port, 5683)};
    test_option_list_success(list);
  }
  {
    std::vector<opt::option> list{
      opt::option::create<opt::check_all, false>
                            (opt::number::if_none_match),
      opt::option::create<opt::check_all, false>
                            (opt::number::uri_host, "192.169.9.2"),
      opt::option::create<opt::check_all, false>
                            (opt::number::uri_port, 5683),
      opt::option::create<opt::check_all, false>
                            (opt::number::uri_path, "my"),
      opt::option::create<opt::check_all, false>
                            (opt::number::uri_path, "path"),
      opt::option::create<opt::check_all, false>
                            (opt::number::uri_path, "resource")};
    test_option_list_success(list);
  }
  {
    std::vector<opt::option> list{
      opt::option::create<opt::check_all, false>
                            (opt::number::uri_host, "192.169.9.2"),
      opt::option::create<opt::check_all, false>
                            (opt::number::if_none_match),
      opt::option::create<opt::check_all, false>
                            (opt::number::uri_path, "path"),
      opt::option::create<opt::check_all, false>
                            (opt::number::uri_path, "my"),
      opt::option::create<opt::check_all, false>
                            (opt::number::uri_path, "resource"),
      opt::option::create<opt::check_all, false>
                            (opt::number::uri_port, 5683)};
    test_option_list_success(list);
  }

  // multiset
  {
    std::multiset<opt::option> list{
      opt::option::create<opt::check_all, false>
                            (opt::number::if_none_match),
      opt::option::create<opt::check_all, false>
                            (opt::number::uri_host, "192.169.9.2"),
      opt::option::create<opt::check_all, false>
                            (opt::number::uri_port, 5683)};
    test_option_list_success<false>(list);
  }
  {
    std::multiset<opt::option> list{
      opt::option::create<opt::check_all, false>
                            (opt::number::if_none_match),
      opt::option::create<opt::check_all, false>
                            (opt::number::uri_host, "192.169.9.2"),
      opt::option::create<opt::check_all, false>
                            (opt::number::uri_port, 5683),
      opt::option::create<opt::check_all, false>
                            (opt::number::uri_path, "my"),
      opt::option::create<opt::check_all, false>
                            (opt::number::uri_path, "path"),
      opt::option::create<opt::check_all, false>
                            (opt::number::uri_path, "resource")};
    test_option_list_success<false>(list);
  }
  {
    std::multiset<opt::option> list{
      opt::option::create<opt::check_all, false>
                            (opt::number::uri_host, "192.169.9.2"),
      opt::option::create<opt::check_all, false>
                            (opt::number::if_none_match),
      opt::option::create<opt::check_all, false>
                            (opt::number::uri_path, "path"),
      opt::option::create<opt::check_all, false>
                            (opt::number::uri_path, "my"),
      opt::option::create<opt::check_all, false>
                            (opt::number::uri_path, "resource"),
      opt::option::create<opt::check_all, false>
                            (opt::number::uri_port, 5683)};
    test_option_list_success<false>(list);
  }

  using list = coap_te::core::sorted_no_alloc_list<opt::option>;
  {
    std::vector<list::node> storage_list{
      opt::option::create<opt::check_all, false>
                            (opt::number::if_none_match),
      opt::option::create<opt::check_all, false>
                            (opt::number::uri_host, "192.169.9.2"),
      opt::option::create<opt::check_all, false>
                            (opt::number::uri_port, 5683)};
    list op_list(storage_list.begin(), storage_list.end());
    test_option_list_success<false>(op_list);
  }
  {
    std::vector<list::node> storage_list{
      opt::option::create<opt::check_all, false>
                            (opt::number::if_none_match),
      opt::option::create<opt::check_all, false>
                            (opt::number::uri_host, "192.169.9.2"),
      opt::option::create<opt::check_all, false>
                            (opt::number::uri_port, 5683),
      opt::option::create<opt::check_all, false>
                            (opt::number::uri_path, "my"),
      opt::option::create<opt::check_all, false>
                            (opt::number::uri_path, "path"),
      opt::option::create<opt::check_all, false>
                            (opt::number::uri_path, "resource")};
    list op_list(storage_list.begin(), storage_list.end());
    test_option_list_success<false>(op_list);
  }
  {
    std::vector<list::node> storage_list{
      opt::option::create<opt::check_all, false>
                            (opt::number::uri_host, "192.169.9.2"),
      opt::option::create<opt::check_all, false>
                            (opt::number::if_none_match),
      opt::option::create<opt::check_all, false>
                            (opt::number::uri_path, "path"),
      opt::option::create<opt::check_all, false>
                            (opt::number::uri_path, "my"),
      opt::option::create<opt::check_all, false>
                            (opt::number::uri_path, "resource"),
      opt::option::create<opt::check_all, false>
                            (opt::number::uri_port, 5683)};

    list op_list(storage_list.begin(), storage_list.end());
    test_option_list_success<false>(op_list);
  }
}
