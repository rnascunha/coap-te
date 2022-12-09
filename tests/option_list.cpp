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
#include <cstdint>

#include "coap-te/core/mutable_buffer.hpp"
#include "coap-te/message/options/config.hpp"
#include "coap-te/message/options/option.hpp"
// #include "coap-te/message/options/vector_options.hpp"

namespace opt = coap_te::message::options;

std::size_t calc_options_size(
              opt::number before,
              opt::number current,
              std::size_t size_input) {
  std::size_t size = 1 + size_input;
  std::size_t diff = static_cast<opt::number_type>(current) -
                     static_cast<opt::number_type>(before);
  for (std::size_t i : {diff, size_input}) {
    if (i >= 269)
      size += 2;
    else if (i >= 13)
      size += 1;
  }
  return size;
}

TEST(OptionList, List) {
  {
    auto op = opt::option::create<opt::check_all, false>
                            (opt::number::if_none_match);
    EXPECT_TRUE(op.is_valid());

    std::uint8_t data[100];
    coap_te::mutable_buffer buf(data);
    std::size_t size = 0;
    opt::number prev = opt::number::invalid;
    std::error_code ec;
    size += op.serialize<opt::check_none>(prev, buf, ec);
    EXPECT_FALSE(ec);
    EXPECT_EQ(op.option_number(), opt::number::if_none_match);
    // prev = op.option_number();
    EXPECT_EQ(size, calc_options_size(prev, opt::number::if_none_match, 0));

    // opt::vector_options vec(coap_te::const_buffer(data, size));
    // int i = 0;
    // for (auto it = vec.begin(); it.is_end(); ++it, ++i) {
    //   auto op = *it;
    //   EXPECT_EQ(op, list[i]);
    // }
    // for (auto it = vec.begin(); i < list.size(); ++it, ++i) {
    //   auto op = *it;
    //   EXPECT_EQ(op, list[i]);
    // }
  }
  {
    auto op = opt::option::create<opt::check_all, false>
                            (opt::number::uri_path, "123456");
    EXPECT_TRUE(op.is_valid());

    std::uint8_t data[100];
    coap_te::mutable_buffer buf(data);
    std::size_t size = 0;
    opt::number prev = opt::number::invalid;
    std::error_code ec;
    size += op.serialize<opt::check_none>(prev, buf, ec);
    EXPECT_FALSE(ec);
    EXPECT_EQ(op.option_number(), opt::number::uri_path);
    // prev = op.option_number();
    EXPECT_EQ(size, calc_options_size(prev, opt::number::uri_path, 6));
    // std::vector<opt::option> list;
    // list.push_back(opt::option::create(opt::number::if_none_match));
    // EXPECT_EQ(list.size(), 1);
    // EXPECT_TRUE(list[0].is_valid());

    // std::uint8_t data[100];
    // coap_te::mutable_buffer buf(data);
    // std::size_t size = 0;
    // opt::number prev = opt::number::invalid;
    // for (auto const& op : list) {
    //   std::error_code ec;
    //   size += op.serialize<opt::check_none>(prev, buf, ec);
    //   EXPECT_FALSE(ec);
    //   EXPECT_EQ(op.option_number(), opt::number::if_none_match);
    //   prev = op.option_number();
    // }
    // EXPECT_EQ(size, calc_options_size(prev, opt::number::if_none_match, 0));

    // opt::vector_options vec(coap_te::const_buffer(data, size));
    // int i = 0;
    // for (auto it = vec.begin(); it.is_end(); ++it, ++i) {
    //   auto op = *it;
    //   EXPECT_EQ(op, list[i]);
    // }
    // for (auto it = vec.begin(); i < list.size(); ++it, ++i) {
    //   auto op = *it;
    //   EXPECT_EQ(op, list[i]);
    // }
  }
}
