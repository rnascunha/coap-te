/**
 * @file message_serialize.cpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-07
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <gtest/gtest.h>

#include <cstdint>
#include <system_error>     // NOLINT
#include <vector>

#include "coap-te/core/const_buffer.hpp"
#include "coap-te/core/mutable_buffer.hpp"
#include "coap-te/message/config.hpp"
#include "coap-te/message/code.hpp"
#include "coap-te/message/serialize.hpp"
#include "coap-te/message/options/option.hpp"

namespace msg = coap_te::message;

TEST(Message, SerializeHeader) {
  {
    SCOPED_TRACE("Minimum header");

    std::error_code ec;
    std::uint8_t data[msg::minimum_header_size];    // NOLINT
    coap_te::mutable_buffer buf(data);

    auto size = msg::serialize_header(
                    msg::type::confirmable,
                    msg::code::put,
                    0x1234U,                    // message_id
                    coap_te::const_buffer{},    // empty token
                    buf,
                    ec);
    EXPECT_FALSE(ec);
    EXPECT_EQ(size, 4);
  }
  {
    SCOPED_TRACE("Header + token");

    std::error_code ec;
    std::uint8_t data[msg::minimum_header_size + 5];    // NOLINT
    coap_te::mutable_buffer buf(data);

    auto size = msg::serialize_header(
                    msg::type::confirmable,
                    msg::code::put,
                    0x1234U,
                    coap_te::const_buffer{"teste"},
                    buf,
                    ec);
    EXPECT_FALSE(ec);
    EXPECT_EQ(size, 4 + 5);
  }
  {
    SCOPED_TRACE("Header + long token (truncaded)");

    std::error_code ec;
    std::uint8_t data[msg::minimum_header_size +
                      msg::max_token_size];
    coap_te::mutable_buffer buf(data);

    auto size = msg::serialize_header(
                    msg::type::confirmable,
                    msg::code::put,
                    0x1234U,
                    coap_te::const_buffer{"teste12345678"},
                    buf,
                    ec);
    EXPECT_FALSE(ec);
    EXPECT_EQ(size, 4 + 8);
  }
  {
    SCOPED_TRACE("Error: buffer too small");

    std::error_code ec;
    std::uint8_t data[5];
    coap_te::mutable_buffer buf(data);

    auto size = msg::serialize_header(
                    msg::type::confirmable,
                    msg::code::put,
                    0x1234U,
                    coap_te::const_buffer{"teste12345678"},
                    buf,
                    ec);
    EXPECT_TRUE(ec);
    EXPECT_EQ(ec, std::errc::no_buffer_space);
    EXPECT_EQ(size, 0);
  }
  {
    SCOPED_TRACE("Error: throw exception");

    std::uint8_t data[5];
    coap_te::mutable_buffer buf(data);

    EXPECT_THROW(msg::serialize_header(
                    msg::type::confirmable,
                    msg::code::put,
                    0x1234U,
                    coap_te::const_buffer{"teste12345678"},
                    buf), std::system_error);
  }
}

TEST(Message, Serialize) {
  {
    SCOPED_TRACE("Header + token + no option + no payload");

    std::error_code ec;
    std::uint8_t data[msg::minimum_header_size + 5];    // NOLINT
    coap_te::mutable_buffer buf(data);

    auto size = msg::serialize(
                    msg::type::confirmable,
                    msg::code::put,
                    0x1234U,
                    coap_te::const_buffer{"teste"},
                    std::vector<msg::options::option>{},    // option list
                    coap_te::const_buffer{},                // payload
                    buf,                                    // output
                    ec);
    EXPECT_FALSE(ec);
    EXPECT_EQ(size, 4 + 5);
  }
  {
    SCOPED_TRACE("Header + token + no option + payload");

    std::error_code ec;
    std::uint8_t data[msg::minimum_header_size + 5 + 17];    // NOLINT
    coap_te::mutable_buffer buf(data);

    auto size = msg::serialize(
                    msg::type::confirmable,
                    msg::code::put,
                    0x1234U,
                    coap_te::const_buffer{"teste"},
                    std::vector<msg::options::option>{},      // option list
                    coap_te::const_buffer{"payload123456790"},  // payload
                    buf,                                      // output
                    ec);
    EXPECT_FALSE(ec);
    EXPECT_EQ(size, 4 + 5 + 17);
  }
}
