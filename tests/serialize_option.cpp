/**
 * @file option.cpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-11-29
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <cstdint>
#include <cstring>
#include <system_error>   // NOLINT
#include <vector>

#include "coap-te/core/mutable_buffer.hpp"
#include "coap-te/core/utility.hpp"
#include "coap-te/message/options/definitions.hpp"
#include "coap-te/message/options/serialize.hpp"
#include "coap-te/message/options/parse.hpp"

namespace opt = coap_te::message::options;
namespace core = coap_te::core;

void check_option_header_success(
                        opt::number_type before,
                        opt::number_type current,
                        std::size_t buffer_size,
                        std::size_t output_size) {
  std::vector<std::uint8_t> data(buffer_size, 0);
  coap_te::mutable_buffer buf(data);
  std::error_code ec;

  auto size = opt::serialize(
                      before,
                      current,
                      buf,
                      ec);
  EXPECT_FALSE(ec);       // No error
  EXPECT_EQ(size, output_size);
  EXPECT_EQ(buf.size(), buffer_size - size);

  auto diff = current - before;
  opt::number_type header = diff;
  EXPECT_THAT(coap_te::const_buffer(data.data(), size),
              ::testing::ElementsAre(
                header << 4 | 0));

  coap_te::const_buffer input(data.data(), size);
  coap_te::const_buffer output{};
  opt::number_type op_num{};
  auto size_parse = opt::parse(before, input, op_num, output, ec);
  EXPECT_FALSE(ec);
  EXPECT_EQ(size_parse, size);
  EXPECT_EQ(op_num, current);
  EXPECT_EQ(output.size(), 0);
}

void check_option_header_fail(
                        opt::number_type before,
                        opt::number_type current,
                        std::size_t buffer_size) {
  std::vector<std::uint8_t> data(buffer_size, 0);
  coap_te::mutable_buffer buf(data);
  std::error_code ec;
  auto size = opt::serialize(
                    before,
                    current,
                    buf,
                    ec);
  EXPECT_TRUE(ec);
  EXPECT_EQ(ec, std::errc::no_buffer_space);
  EXPECT_EQ(size, 0);
}

TEST(Options, Serialize) {
  check_option_header_success(
    0, core::to_underlying(opt::number::if_none_match), 10, 1);
  check_option_header_fail(0, 5, 0);
}
