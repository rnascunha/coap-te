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

#include "coap-te/core/utility.hpp"
#include "coap-te/core/byte_order.hpp"
#include "coap-te/buffer/mutable_buffer.hpp"
#include "coap-te/message/options/definitions.hpp"
#include "coap-te/message/options/serialize.hpp"
#include "coap-te/message/options/parse.hpp"

namespace opt = coap_te::message::options;
namespace core = coap_te::core;

template<typename CheckOptions, typename Arg>
void test_options_definitions_fail(
        opt::number before,
        opt::number current,
        Arg         arg,
        coap_te::errc   expected_error) {
  coap_te::error_code ec;
  coap_te::mutable_buffer mbuf(nullptr, 0);

  auto size = opt::serialize<CheckOptions>(
                      core::to_underlying(before),
                      core::to_underlying(current),
                      arg,
                      mbuf,
                      ec);
  EXPECT_TRUE(ec);
  EXPECT_EQ(size, 0);
  EXPECT_EQ(ec, expected_error);
}

TEST(Options, CheckDefinitions) {
  {
    SCOPED_TRACE("Fail invalid option");
    test_options_definitions_fail<opt::check_all>(opt::number::invalid,
                                  opt::number::invalid,
                                  8u,
                                  coap_te::errc::invalid_option);
  }
  {
    SCOPED_TRACE("Fail wrong type");
    test_options_definitions_fail<opt::check_all>(opt::number::invalid,
                                  opt::number::if_match,
                                  1u,
                                  coap_te::errc::option_format);
  }
  {
    SCOPED_TRACE("Fail sequence options error");
    test_options_definitions_fail<opt::check_all>(opt::number::etag,
                                  opt::number::uri_host,
                                  coap_te::const_buffer(nullptr, 100),
                                  coap_te::errc::option_sequence);
  }
  {
    SCOPED_TRACE("Fail sequence options repeated invalid");
    test_options_definitions_fail<opt::check_all>(opt::number::uri_host,
                                  opt::number::uri_host,
                                  coap_te::const_buffer(nullptr, 100),
                                  coap_te::errc::option_sequence);
  }
  {
    SCOPED_TRACE("Fail options length bigger");
    test_options_definitions_fail<opt::check_all>(opt::number::invalid,
                                  opt::number::uri_host,
                                  coap_te::const_buffer(nullptr, 256),
                                  coap_te::errc::option_length);
  }
  {
    SCOPED_TRACE("Fail options length smaller");
    test_options_definitions_fail<opt::check_all>(opt::number::invalid,
                                  opt::number::etag,
                                  coap_te::const_buffer(nullptr, 0),
                                  coap_te::errc::option_length);
  }
}

TEST(Options, IgnoreCheckDefinitions) {
  /*
   * All this tests must ignore check definitions. It should still
   * fail because we are not providing any buffer. So the error will
   * be 'std::no_buffer_space'.
   */
  {
    SCOPED_TRACE("Ignore invalid option");
    test_options_definitions_fail<opt::check_type<false, false, false>>(
                                  opt::number::invalid,
                                  opt::number::invalid,
                                  8u,
                                  coap_te::errc::no_buffer_space);
  }
  {
    SCOPED_TRACE("Ignore wrong type");
    test_options_definitions_fail<opt::check_type<true, false, true>>(
                                  opt::number::invalid,
                                  opt::number::if_match,
                                  1u,
                                  coap_te::errc::no_buffer_space);
  }
  {
    SCOPED_TRACE("Ignore sequence options error");
    test_options_definitions_fail<opt::check_type<false, true, true>>(
                                  opt::number::etag,
                                  opt::number::uri_host,
                                  coap_te::const_buffer(nullptr, 100),
                                  coap_te::errc::no_buffer_space);
  }
  {
    SCOPED_TRACE("Ignore sequence options repeated invalid");
    test_options_definitions_fail<opt::check_type<false, true, true>>(
                                  opt::number::uri_host,
                                  opt::number::uri_host,
                                  coap_te::const_buffer(nullptr, 100),
                                  coap_te::errc::no_buffer_space);
  }
  {
    SCOPED_TRACE("Ignore options length bigger");
    test_options_definitions_fail<opt::check_type<true, true, false>>(
                                  opt::number::invalid,
                                  opt::number::uri_host,
                                  coap_te::const_buffer(nullptr, 256),
                                  coap_te::errc::no_buffer_space);
  }
  {
    SCOPED_TRACE("Ignore options length smaller");
    test_options_definitions_fail<opt::check_type<true, true, false>>(
                                  opt::number::invalid,
                                  opt::number::etag,
                                  coap_te::const_buffer(nullptr, 0),
                                  coap_te::errc::no_buffer_space);
  }
}

std::size_t calc_options_size(
              opt::number_type before,
              opt::number_type current,
              std::size_t size_input) {
  std::size_t size = 1 + size_input;
  std::size_t diff = current - before;
  for (std::size_t i : {diff, size_input}) {
    if (i >= 269)
      size += 2;
    else if (i >= 13)
      size += 1;
  }
  return size;
}

void test_serialize_parse_success(
                  opt::number before_n,
                  opt::number current_n,
                  const coap_te::const_buffer& buf_in) {
  std::uint8_t data[256];
  opt::number_type before = core::to_underlying(before_n);
  opt::number_type current = core::to_underlying(current_n);
  coap_te::error_code ecs;

  auto size_s = opt::serialize<opt::check_type<true, false, true>>(
                before,
                current,
                buf_in,
                coap_te::buffer(data),
                ecs);
  EXPECT_FALSE(ecs);
  EXPECT_EQ(size_s, calc_options_size(before, current, buf_in.size()));

  coap_te::const_buffer buf_out;
  coap_te::error_code ecp;
  opt::number_type current_parse = core::to_underlying(opt::number::invalid);
  coap_te::const_buffer input(data, size_s);
  auto size_p = opt::parse(before,
                          input,
                          current_parse,
                          buf_out,
                          ecp);

  EXPECT_FALSE(ecp);
  EXPECT_EQ(size_s, size_p);
  EXPECT_EQ(current, current_parse);
  EXPECT_EQ(buf_in.size(), buf_out.size());
  EXPECT_EQ(0, std::memcmp(buf_out.data(),
                           buf_in.data(), buf_out.size()));
}

void test_serialize_parse_success(
                  opt::number before_n,
                  opt::number current_n,
                  unsigned value) {
  auto [data, size] = core::to_small_big_endian(value);
  test_serialize_parse_success(before_n,
                               current_n,
                               coap_te::const_buffer(&data, size));
}

void test_serialize_parse_success(
                  opt::number before_n,
                  opt::number current_n) {
  test_serialize_parse_success(before_n,
                               current_n,
                               coap_te::const_buffer(nullptr, 0));
}

TEST(Options, SerializeParse) {
  {
    SCOPED_TRACE("Serialize parse string uri-host");
    test_serialize_parse_success(opt::number::invalid,
                               opt::number::uri_host,
                               coap_te::buffer("192.168.0.1"));
  }
  {
    SCOPED_TRACE("Serialize parse big string uri-host");
    test_serialize_parse_success(opt::number::invalid,
                               opt::number::uri_host,
                               coap_te::buffer("192.168.111.111"));
  }
  {
    SCOPED_TRACE("Serialize parse number max-age");
    test_serialize_parse_success(opt::number::invalid,
                               opt::number::max_age,
                               60);
  }
  {
    SCOPED_TRACE("Serialize parse empty if_none_match");
    test_serialize_parse_success(opt::number::invalid,
                               opt::number::if_none_match);
  }
  {
    SCOPED_TRACE("Serialize parse big string uri-host");
    test_serialize_parse_success(opt::number::uri_path,
                               opt::number::uri_path,
                               coap_te::buffer(
                                "testedeumpathgrandemasmuito"
                                "grandemesmoqueatepergoacont"
                                "adequaograndeelaemasegrande"
                                "mesmoemnossoquegrandemeudeu"
                                "sgrandao"));
  }
}
