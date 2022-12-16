/**
 * @file option.cpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-06
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "coap-te/core/byte_order.hpp"
#include "coap-te/message/options/definitions.hpp"
#include "coap-te/message/options/option.hpp"
#include "coap-te/message/options/parse.hpp"

namespace opt = coap_te::message::options;

template<typename ...Args>
constexpr auto
create(opt::number op, Args&& ... args)  {
  return opt::option::create<opt::check_all, true>(
              op, std::forward<Args>(args)...);
}

TEST(CoAPMessage, OptionConstructor) {
  // Testing constructors
  {
    opt::option invalid_op;
    EXPECT_EQ(invalid_op.data_size(), 0);
    EXPECT_FALSE(invalid_op.is_valid());
  }
  {
    opt::option empty_op = opt::option::create(opt::number::if_none_match);
    EXPECT_EQ(empty_op.data_size(), 0);
    EXPECT_TRUE(empty_op.is_valid());
  }
  {
    opt::option uint_op = opt::option::create(opt::number::uri_port, 10);
    EXPECT_EQ(uint_op.data_size(), 1);
    EXPECT_TRUE(uint_op.is_valid());
  }
  {
    opt::option string_op =
                  opt::option::create(opt::number::uri_host, "myhost");
    EXPECT_EQ(string_op.data_size(), 6);
    EXPECT_TRUE(string_op.is_valid());
  }
  {
    unsigned char arr[] = {1, 2, 3, 4, 5};
    opt::option opaque_op =
                  opt::option::create(
                    opt::number::if_match,
                    coap_te::const_buffer(arr));
    EXPECT_EQ(opaque_op.data_size(), 5);
    EXPECT_TRUE(opaque_op.is_valid());
  }
  // Faling constructor - NOT throwing!
  {
    {
      // Empty option not empty
      opt::option empty_op = opt::option::create<
                                    opt::check_all,
                                    false>(opt::number::if_none_match, 1);
      EXPECT_EQ(empty_op.data_size(), 0);
      EXPECT_FALSE(empty_op.is_valid());
    }
    {
      // Unsigned option empty
      opt::option uint_op = opt::option::create<
                                    opt::check_all,
                                    false>(opt::number::uri_port);
      EXPECT_EQ(uint_op.data_size(), 0);
      EXPECT_FALSE(uint_op.is_valid());
    }
    {
      // String option opaque
      unsigned char arr[] = {1, 2, 3, 4, 5};
      opt::option string_op = opt::option::create<
                                    opt::check_all,
                                    false>(opt::number::uri_host,
                                     coap_te::const_buffer(arr));
      EXPECT_EQ(string_op.data_size(), 0);
      EXPECT_FALSE(string_op.is_valid());
    }
    {
      // Opaque option string
      opt::option opaque_op = opt::option::create<
                                    opt::check_all,
                                    false>(opt::number::if_match, "myoption");
      EXPECT_EQ(opaque_op.data_size(), 0);
      EXPECT_FALSE(opaque_op.is_valid());
    }
  }
  // Faling constructor - throwing!
  {
    {
      // Empty option not empty
      EXPECT_THROW(create(opt::number::if_none_match, 1), std::system_error);
    }
    {
      // Unsigned option empty
      EXPECT_THROW(create(opt::number::uri_port), std::system_error);
    }
    {
      // String option opaque
      unsigned char arr[] = {1, 2, 3, 4, 5};
      EXPECT_THROW(create(opt::number::uri_host, coap_te::const_buffer(arr)),
                   std::system_error);
    }
    {
      // Opaque option string
      EXPECT_THROW(create(opt::number::if_match, "myoption"),
                   std::system_error);
    }
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
                  opt::number before,
                  opt::number current,
                  const coap_te::const_buffer& buf_in) {
  std::uint8_t data[256];
  coap_te::mutable_buffer buf(data);
  std::error_code ecs;

  opt::option ops = opt::option::create
                  <opt::check_type<true, false, true>>
                  (current, buf_in);
  auto size_s = ops.serialize<opt::check_none>(before, buf, ecs);
  EXPECT_FALSE(ecs);
  EXPECT_EQ(size_s, calc_options_size(
                        static_cast<opt::number_type>(before),
                        static_cast<opt::number_type>(current),
                        buf_in.size()));

  std::error_code ecp;
  opt::option opp;
  coap_te::const_buffer input(data, size_s);
  auto size_p = opt::parse(static_cast<opt::number_type>(before),
                          input,
                          opp,
                          ecp);

  EXPECT_FALSE(ecp);
  EXPECT_EQ(size_s, size_p);
  EXPECT_EQ(current, opp.option_number());
  EXPECT_EQ(buf_in.size(), opp.data_size());
  EXPECT_EQ(0, std::memcmp(opp.data(),
                           buf_in.data(), opp.data_size()));
}

void test_serialize_parse_success(
                  opt::number before,
                  opt::number current,
                  unsigned value) {
  auto [data, size] = coap_te::core::to_small_big_endian(value);
  test_serialize_parse_success(before,
                               current,
                               coap_te::const_buffer(&data, size));
}

void test_serialize_parse_success(
                  opt::number before,
                  opt::number current) {
  test_serialize_parse_success(before,
                               current,
                               coap_te::const_buffer{});
}

TEST(CoAPMessage, OptionSerializeParse) {
  {
    SCOPED_TRACE("Serialize parse string uri-host");
    test_serialize_parse_success(opt::number::invalid,
                               opt::number::uri_host,
                               coap_te::const_buffer("192.168.0.1"));
  }
  {
    SCOPED_TRACE("Serialize parse big string uri-host");
    test_serialize_parse_success(opt::number::invalid,
                               opt::number::uri_host,
                               coap_te::const_buffer("192.168.111.111"));
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
                               coap_te::const_buffer(
                                "testedeumpathgrandemasmuito"
                                "grandemesmoqueatepergoacont"
                                "adequaograndeelaemasegrande"
                                "mesmoemnossoquegrandemeudeu"
                                "sgrandao"));
  }
}
