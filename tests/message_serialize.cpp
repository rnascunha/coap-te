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
#include <list>

#include "coap-te/core/const_buffer.hpp"
#include "coap-te/core/mutable_buffer.hpp"
#include "coap-te/core/sorted_list.hpp"
#include "coap-te/core/sorted_no_alloc_list.hpp"
#include "coap-te/message/config.hpp"
#include "coap-te/message/code.hpp"
#include "coap-te/message/token.hpp"
#include "coap-te/message/serialize.hpp"
#include "coap-te/message/parse.hpp"
#include "coap-te/message/options/option.hpp"

namespace msg = coap_te::message;
using opt = msg::options::option;
// requests
using req = msg::message<msg::options::vector_options>;
using req_sort = msg::message<coap_te::core::sorted_list<opt>>;
using req_vector = msg::message<std::vector<opt>>;
using req_list = msg::message<std::list<opt>>;
using req_no_alloc_sort =
          msg::message<coap_te::core::sorted_no_alloc_list<opt>>;

template<typename Message,
         bool Parse = true>
void serialize_parse_header_success_impl(
            msg::type tp,
            msg::code co,
            msg::message_id mid,
            const coap_te::const_buffer& tk,
            std::size_t expected_size) noexcept {
  std::error_code ec;
  std::uint8_t data[msg::minimum_header_size + msg::max_token_size];    // NOLINT
  coap_te::mutable_buffer buf(data);
  auto size = msg::serialize_header(
                tp, co, mid, tk, buf, ec);
  EXPECT_FALSE(ec);
  EXPECT_EQ(size, expected_size);

  // Parsing
  if constexpr (Parse) {
    coap_te::const_buffer buf_p(data, size);
    std::error_code ecp;
    Message resp;
    auto size_p = msg::parse(buf_p, resp, ec);
    EXPECT_FALSE(ec);
    EXPECT_EQ(size, size_p);
    EXPECT_EQ(tp, resp.get_type());
    EXPECT_EQ(co, resp.get_code());
    EXPECT_EQ(mid, resp.mid());
    // the token size provided may be truncated
    EXPECT_EQ(expected_size - 4, resp.token().size());
    EXPECT_EQ(0, std::memcmp(resp.token().data(), tk.data(),
                             resp.token().size()));
  }
}

template<typename Arg, typename ...Args>
void serialize_parse_header_success(
            msg::type tp,
            msg::code co,
            msg::message_id mid,
            const coap_te::const_buffer& tk,
            std::size_t expected_size) noexcept {
  serialize_parse_header_success_impl<Arg>(tp, co, mid, tk, expected_size);
  if constexpr (sizeof...(Args) > 0)
    serialize_parse_header_success<Args...>(tp, co, mid, tk, expected_size);
}

TEST(CoAPMessage, SerializeParseHeaderSuccess) {
  {
    SCOPED_TRACE("Minimum header");
    serialize_parse_header_success<req,
                                   req_sort,
                                   req_vector,
                                   req_list>(
                    msg::type::confirmable,
                    msg::code::put,
                    0x1234U,                    // message_id
                    coap_te::const_buffer{},    // empty token
                    4);
    serialize_parse_header_success_impl<req_no_alloc_sort, false>(
                    msg::type::confirmable,
                    msg::code::put,
                    0x1234U,                    // message_id
                    coap_te::const_buffer{},    // empty token
                    4);
  }
  {
    SCOPED_TRACE("Header + token");
    serialize_parse_header_success<req,
                                   req_sort,
                                   req_vector,
                                   req_list>(
                    msg::type::confirmable,
                    msg::code::put,
                    0x1234U,                         // message_id
                    coap_te::const_buffer{"teste"},
                    4 + 5);
    serialize_parse_header_success_impl<req_no_alloc_sort, false>(
                    msg::type::confirmable,
                    msg::code::put,
                    0x1234U,                    // message_id
                    coap_te::const_buffer{"teste"},    // empty token
                    4 + 5);
  }
  {
    SCOPED_TRACE("Header + long token (truncaded)");
    serialize_parse_header_success<req,
                                   req_sort,
                                   req_vector,
                                   req_list>(
                    msg::type::confirmable,
                    msg::code::put,
                    0x1234U,                         // message_id
                    coap_te::const_buffer{"teste12345678"},
                    4 + 8);
    serialize_parse_header_success_impl<req_no_alloc_sort, false>(
                    msg::type::confirmable,
                    msg::code::put,
                    0x1234U,                          // message_id
                    coap_te::const_buffer{"teste12345678"},
                    4 + 8);
  }
}

TEST(CoAPMessage, SerializeHeaderFail) {
  {
    SCOPED_TRACE("Header = Buffer to small");
    std::error_code ec;
    std::uint8_t data[msg::minimum_header_size - 1];    // NOLINT
    coap_te::mutable_buffer buf(data);
    auto size = msg::serialize_header(
                  msg::type::nonconfirmable,
                  msg::code::post,
                  0x1234,
                  coap_te::const_buffer{},
                  buf,
                  ec);
    EXPECT_TRUE(ec);
    EXPECT_EQ(ec, std::errc::no_buffer_space);
    EXPECT_EQ(size, 0);
  }
  {
    SCOPED_TRACE("Header + token = Buffer to small");
    std::error_code ec;
    std::uint8_t data[msg::minimum_header_size + 3];    // NOLINT
    coap_te::mutable_buffer buf(data);
    auto size = msg::serialize_header(
                  msg::type::nonconfirmable,
                  msg::code::post,
                  0x1234,
                  coap_te::const_buffer{"teste"},
                  buf,
                  ec);
    EXPECT_TRUE(ec);
    EXPECT_EQ(ec, std::errc::no_buffer_space);
    EXPECT_EQ(size, 0);
  }
}

template<typename Message,
         typename Option,
         bool Parse = true>
void serialize_parse_success_impl(
            msg::type tp,
            msg::code co,
            msg::message_id mid,
            const coap_te::const_buffer& tk,
            std::initializer_list<Option> opt_list,
            const coap_te::const_buffer& payload,
            std::size_t expected_size) noexcept {
  std::error_code ec;
  std::uint8_t data[100];
  coap_te::mutable_buffer buf(data);
  Message req{tp, co, tk};
  for (auto op : opt_list)
    req.add_option(op);
  req.payload(payload);

  auto size = req.serialize(mid, buf, ec);
  EXPECT_FALSE(ec);
  EXPECT_EQ(size, expected_size);

  // Parsing
  if constexpr (Parse) {
    coap_te::const_buffer buf_p(data, size);
    std::error_code ecp;
    Message resp;
    auto size_p = msg::parse(buf_p, resp, ec);
    EXPECT_FALSE(ec);
    EXPECT_EQ(size, size_p);
    // Header
    EXPECT_EQ(tp, resp.get_type());
    EXPECT_EQ(co, resp.get_code());
    EXPECT_EQ(mid, resp.mid());
    EXPECT_EQ(msg::token_size(tk.size()), resp.token().size());
    EXPECT_EQ(0, std::memcmp(resp.token().data(), tk.data(),
                             resp.token().size()));
    // Option
    // Payload
    EXPECT_EQ(payload.size(), resp.payload().size());
    EXPECT_EQ(0, std::memcmp(resp.payload().data(), payload.data(),
                             resp.payload().size()));
  }
}

// template<typename Arg, typename ...Args>
// void serialize_parse_success(
//             msg::type tp,
//             msg::code co,
//             msg::message_id mid,
//             const coap_te::const_buffer& tk,
//             std::size_t expected_size) noexcept {
//   serialize_parse_success_impl<Arg>(tp, co, mid, tk, expected_size);
//   if constexpr (sizeof...(Args) > 0)
//     serialize_parse_success<Args...>(tp, co, mid, tk, expected_size);
// }

TEST(CoAPMessage, SerializeParseMessage) {
  {
    SCOPED_TRACE("Header + token + no option + no payload");
    serialize_parse_success_impl<req_sort, opt>(
                    msg::type::confirmable,
                    msg::code::put,
                    0x1234U,
                    coap_te::const_buffer{"teste"},         // token
                    std::initializer_list<opt>{},           // option list
                    coap_te::const_buffer{},                // payload
                    4 + 5);                                 // output
  }
  //   std::error_code ec;
  //   std::uint8_t data[msg::minimum_header_size + 5];    // NOLINT
  //   coap_te::mutable_buffer buf(data);

  //   auto size = msg::serialize(
  //                   msg::type::confirmable,
  //                   msg::code::put,
  //                   0x1234U,
  //                   coap_te::const_buffer{"teste"},
  //                   std::vector<msg::options::option>{},    // option list
  //                   coap_te::const_buffer{},                // payload
  //                   buf,                                    // output
  //                   ec);
  //   EXPECT_FALSE(ec);
  //   EXPECT_EQ(size, 4 + 5);
  // }
  // {
  //   SCOPED_TRACE("Header + token + no option + payload");

  //   std::error_code ec;
  //   std::uint8_t data[msg::minimum_header_size + 5 + 17];    // NOLINT
  //   coap_te::mutable_buffer buf(data);

  //   auto size = msg::serialize(
  //                   msg::type::confirmable,
  //                   msg::code::put,
  //                   0x1234U,
  //                   coap_te::const_buffer{"teste"},
  //                   std::vector<msg::options::option>{},      // option list
  //                   coap_te::const_buffer{"payload123456790"},  // payload
  //                   buf,                                      // output
  //                   ec);
  //   EXPECT_FALSE(ec);
  //   EXPECT_EQ(size, 4 + 5 + 17);
  // }
}
