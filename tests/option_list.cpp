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
#include "coap-te/core/sorted_list.hpp"
#include "coap-te/message/options/config.hpp"
#include "coap-te/message/options/option.hpp"
#include "coap-te/message/options/checks.hpp"
#include "coap-te/message/options/vector_options.hpp"
#include "coap-te/message/options/serialize.hpp"

namespace opt = coap_te::message::options;

template<bool Sort = true, typename Container>
void test_option_list_success(Container& list) {    // NOLINT
  if constexpr (Sort) {
    std::stable_sort(list.begin(), list.end());
  }

  std::uint8_t data[100];
  coap_te::mutable_buffer buf(data);
  coap_te::error_code ec;
  std::size_t size = opt::serialize<opt::check_none>(list, buf, ec);
  EXPECT_FALSE(ec);

  opt::vector_options list_s(data, size);
  {
    auto i = list.begin();
    for (auto it = list_s.begin(); !it.is_end(); ++i, ++it) {
      auto op = *it;
      EXPECT_EQ(op, *i);
      EXPECT_EQ(op.option_number(), i->option_number());
      EXPECT_EQ(op.data_size(), i->data_size());
      EXPECT_EQ(0, std::memcmp(op.data(), i->data(), op.data_size()));
    }
  }
  {
    // same checks as above, but using range for interface
    auto i = list.begin();
    for (auto op : list_s) {
      EXPECT_EQ(op, *i);
      EXPECT_EQ(op.option_number(), i->option_number());
      EXPECT_EQ(op.data_size(), i->data_size());
      EXPECT_EQ(0, std::memcmp(op.data(), i->data(), op.data_size()));
      ++i;
    }
  }
}

template<bool Sort,
         typename Container>
void test_option_list_success_lists(
      const std::initializer_list<std::initializer_list<opt::option>>& lists) {
  for (auto const& list : lists) {
    Container l(list.begin(), list.end());
    test_option_list_success<Sort>(l);
  }
}

TEST(OptionList, List) {
  std::initializer_list<std::initializer_list<opt::option>>
  lists{
    {
      opt::create<opt::check_all, false>
                            (opt::number::if_none_match),
      opt::create<opt::check_all, false>
                            (opt::number::uri_host, "192.169.9.2"),
      opt::create<opt::check_all, false>
                            (opt::number::uri_port, 5683)
    },
    {
      opt::create<opt::check_all, false>
                          (opt::number::if_none_match),
      opt::create<opt::check_all, false>
                            (opt::number::uri_host, "192.169.9.2"),
      opt::create<opt::check_all, false>
                            (opt::number::uri_port, 5683),
      opt::create<opt::check_all, false>
                            (opt::number::uri_path, "my"),
      opt::create<opt::check_all, false>
                            (opt::number::uri_path, "path"),
      opt::create<opt::check_all, false>
                          (opt::number::uri_path, "resource")
    },
    {
      opt::create<opt::check_all, false>
                          (opt::number::uri_host, "192.169.9.2"),
      opt::create<opt::check_all, false>
                            (opt::number::if_none_match),
      opt::create<opt::check_all, false>
                            (opt::number::uri_path, "path"),
      opt::create<opt::check_all, false>
                            (opt::number::uri_path, "my"),
      opt::create<opt::check_all, false>
                            (opt::number::uri_path, "resource"),
      opt::create<opt::check_all, false>
                          (opt::number::uri_port, 5683)
    }
  };
  {
    test_option_list_success_lists<true, std::vector<opt::option>>(lists);
  }
  {
    std::vector<opt::option> list{
      opt::create<opt::check_all, false>
                            (opt::number::if_none_match),
      opt::create<opt::check_all, false>
                            (opt::number::uri_host, "192.169.9.2"),
      opt::create<opt::check_all, false>
                            (opt::number::uri_port, 5683)};
    test_option_list_success(list);
  }
  {
    std::vector<opt::option> list{
      opt::create<opt::check_all, false>
                            (opt::number::if_none_match),
      opt::create<opt::check_all, false>
                            (opt::number::uri_host, "192.169.9.2"),
      opt::create<opt::check_all, false>
                            (opt::number::uri_port, 5683),
      opt::create<opt::check_all, false>
                            (opt::number::uri_path, "my"),
      opt::create<opt::check_all, false>
                            (opt::number::uri_path, "path"),
      opt::create<opt::check_all, false>
                            (opt::number::uri_path, "resource")};
    test_option_list_success(list);
  }
  {
    std::vector<opt::option> list{
      opt::create<opt::check_all, false>
                            (opt::number::uri_host, "192.169.9.2"),
      opt::create<opt::check_all, false>
                            (opt::number::if_none_match),
      opt::create<opt::check_all, false>
                            (opt::number::uri_path, "path"),
      opt::create<opt::check_all, false>
                            (opt::number::uri_path, "my"),
      opt::create<opt::check_all, false>
                            (opt::number::uri_path, "resource"),
      opt::create<opt::check_all, false>
                            (opt::number::uri_port, 5683)};
    test_option_list_success(list);
  }

  // multiset
  {
    std::multiset<opt::option> list{
      opt::create<opt::check_all, false>
                            (opt::number::if_none_match),
      opt::create<opt::check_all, false>
                            (opt::number::uri_host, "192.169.9.2"),
      opt::create<opt::check_all, false>
                            (opt::number::uri_port, 5683)};
    test_option_list_success<false>(list);
  }
  {
    std::multiset<opt::option> list{
      opt::create<opt::check_all, false>
                            (opt::number::if_none_match),
      opt::create<opt::check_all, false>
                            (opt::number::uri_host, "192.169.9.2"),
      opt::create<opt::check_all, false>
                            (opt::number::uri_port, 5683),
      opt::create<opt::check_all, false>
                            (opt::number::uri_path, "my"),
      opt::create<opt::check_all, false>
                            (opt::number::uri_path, "path"),
      opt::create<opt::check_all, false>
                            (opt::number::uri_path, "resource")};
    test_option_list_success<false>(list);
  }
  {
    std::multiset<opt::option> list{
      opt::create<opt::check_all, false>
                            (opt::number::uri_host, "192.169.9.2"),
      opt::create<opt::check_all, false>
                            (opt::number::if_none_match),
      opt::create<opt::check_all, false>
                            (opt::number::uri_path, "path"),
      opt::create<opt::check_all, false>
                            (opt::number::uri_path, "my"),
      opt::create<opt::check_all, false>
                            (opt::number::uri_path, "resource"),
      opt::create<opt::check_all, false>
                            (opt::number::uri_port, 5683)};
    test_option_list_success<false>(list);
  }
  {
    coap_te::core::sorted_list<opt::option> list{
      opt::create<opt::check_all, false>
                            (opt::number::uri_host, "192.169.9.2"),
      opt::create<opt::check_all, false>
                            (opt::number::if_none_match),
      opt::create<opt::check_all, false>
                            (opt::number::uri_path, "path"),
      opt::create<opt::check_all, false>
                            (opt::number::uri_path, "my"),
      opt::create<opt::check_all, false>
                            (opt::number::uri_path, "resource"),
      opt::create<opt::check_all, false>
                            (opt::number::uri_port, 5683)};
    test_option_list_success<false>(list);
  }

  using list = coap_te::core::sorted_no_alloc_list<opt::option>;
  {
    std::vector<list::node> storage_list{
      opt::create<opt::check_all, false>
                            (opt::number::if_none_match),
      opt::create<opt::check_all, false>
                            (opt::number::uri_host, "192.169.9.2"),
      opt::create<opt::check_all, false>
                            (opt::number::uri_port, 5683)};
    list op_list(storage_list.begin(), storage_list.end());
    test_option_list_success<false>(op_list);
  }
  {
    std::vector<list::node> storage_list{
      opt::create<opt::check_all, false>
                            (opt::number::if_none_match),
      opt::create<opt::check_all, false>
                            (opt::number::uri_host, "192.169.9.2"),
      opt::create<opt::check_all, false>
                            (opt::number::uri_port, 5683),
      opt::create<opt::check_all, false>
                            (opt::number::uri_path, "my"),
      opt::create<opt::check_all, false>
                            (opt::number::uri_path, "path"),
      opt::create<opt::check_all, false>
                            (opt::number::uri_path, "resource")};
    list op_list(storage_list.begin(), storage_list.end());
    test_option_list_success<false>(op_list);
  }
  {
    std::vector<list::node> storage_list{
      opt::create<opt::check_all, false>
                            (opt::number::uri_host, "192.169.9.2"),
      opt::create<opt::check_all, false>
                            (opt::number::if_none_match),
      opt::create<opt::check_all, false>
                            (opt::number::uri_path, "path"),
      opt::create<opt::check_all, false>
                            (opt::number::uri_path, "my"),
      opt::create<opt::check_all, false>
                            (opt::number::uri_path, "resource"),
      opt::create<opt::check_all, false>
                            (opt::number::uri_port, 5683)};

    list op_list(storage_list.begin(), storage_list.end());
    test_option_list_success<false>(op_list);
  }
}
