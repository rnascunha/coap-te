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
#include <vector>
#include <list>
#include <set>
#include <map>

#include "coap-te/core/error.hpp"
#include "coap-te/core/utility.hpp"
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
#include "coap-te/message/options/option_func.hpp"

namespace msg = coap_te::message;
// options
using opt = msg::options::option;
using opt_node = coap_te::core::sorted_no_alloc_list<opt>::node;
// requests
using resp = msg::message<msg::options::vector_options>;
using msg_sort = msg::message<coap_te::core::sorted_list<opt>>;
using msg_vector = msg::message<std::vector<opt>>;
using msg_list = msg::message<std::list<opt>>;
using msg_no_alloc_sort =
          msg::message<coap_te::core::sorted_no_alloc_list<opt>>;
using msg_multiset = msg::message<std::multiset<opt>>;
using msg_multimap = msg::message<std::multimap<msg::options::number, opt>>;

#define PARSEABLE_TYPES   resp,                       \
                          msg_sort,                   \
                          msg_vector,                 \
                          msg_list,                   \
                          msg_multiset,               \
                          msg_multimap

#define TYPES_COMBINE    msg_sort, resp,              \
                         msg_sort, msg_sort,          \
                         msg_sort, msg_vector,        \
                         msg_sort, msg_list,          \
                         msg_sort, msg_multiset,      \
                         msg_sort, msg_multimap,      \
                         msg_multiset, resp,          \
                         msg_multiset, msg_sort,      \
                         msg_multiset, msg_vector,    \
                         msg_multiset, msg_list,      \
                         msg_multiset, msg_multiset,  \
                         msg_multiset, msg_multimap,  \
                         msg_vector, resp,            \
                         msg_vector, msg_sort,        \
                         msg_vector, msg_vector,      \
                         msg_vector, msg_list,        \
                         msg_vector, msg_multiset,    \
                         msg_vector, msg_multimap,    \
                         msg_list, resp,              \
                         msg_list, msg_sort,          \
                         msg_list, msg_vector,        \
                         msg_list, msg_list,          \
                         msg_list, msg_multiset,      \
                         msg_list, msg_multimap,      \
                         msg_multimap, resp,          \
                         msg_multimap, msg_sort,      \
                         msg_multimap, msg_vector,    \
                         msg_multimap, msg_list,      \
                         msg_multimap, msg_multiset,  \
                         msg_multimap, msg_multimap

#define TYPES_COMBINE_NO_ALLOC    msg_no_alloc_sort, resp,              \
                                  msg_no_alloc_sort, msg_sort,          \
                                  msg_no_alloc_sort, msg_vector,        \
                                  msg_no_alloc_sort, msg_list,          \
                                  msg_no_alloc_sort, msg_multiset,      \
                                  msg_no_alloc_sort, msg_multimap


template<typename Message,
         bool Parse = true>
void serialize_parse_header_success_impl(
            msg::type tp,
            msg::code co,
            msg::message_id mid,
            const coap_te::const_buffer& tk,
            std::size_t expected_size) noexcept {
  coap_te::error_code ec;
  std::uint8_t data[msg::minimum_header_size + msg::max_token_size];    // NOLINT
  coap_te::mutable_buffer buf(data);
  auto size = msg::serialize_header(
                tp, co, mid, tk, buf, ec);
  EXPECT_FALSE(ec);
  EXPECT_EQ(size, expected_size);

  // Parsing
  if constexpr (Parse) {
    coap_te::const_buffer buf_p(data, size);
    coap_te::error_code ecp;
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
    serialize_parse_header_success<PARSEABLE_TYPES>(
                    msg::type::confirmable,
                    msg::code::put,
                    0x1234U,                    // message_id
                    coap_te::const_buffer{},    // empty token
                    4);
  }
  {
    SCOPED_TRACE("Header + token");
    serialize_parse_header_success<PARSEABLE_TYPES>(
                    msg::type::confirmable,
                    msg::code::put,
                    0x1234U,                         // message_id
                    coap_te::const_buffer{"teste"},
                    4 + 5);
  }
  {
    SCOPED_TRACE("Header + long token (truncaded)");
    serialize_parse_header_success<PARSEABLE_TYPES>(
                    msg::type::confirmable,
                    msg::code::put,
                    0x1234U,                         // message_id
                    coap_te::const_buffer{"teste12345678"},
                    4 + 8);
  }
}

TEST(CoAPMessage, SerializeHeaderFail) {
  {
    SCOPED_TRACE("Header = Buffer to small");
    coap_te::error_code ec;
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
    EXPECT_EQ(ec, coap_te::errc::no_buffer_space);
    EXPECT_EQ(size, 0);
  }
  {
    SCOPED_TRACE("Header + token = Buffer to small");
    coap_te::error_code ec;
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
    EXPECT_EQ(ec, coap_te::errc::no_buffer_space);
    EXPECT_EQ(size, 0);
  }
}

template<typename MessageSerialize,
         typename MessageParse,
         typename Option>
void serialize_parse_success_impl(
            msg::type tp,
            msg::code co,
            msg::message_id mid,
            const coap_te::const_buffer& tk,
            std::initializer_list<Option> opt_list,
            const coap_te::const_buffer& payload,
            std::size_t expected_size) noexcept {
  std::uint8_t data[110];
  std::size_t size_s = 0;
  // The storage is need for the no alloc sorted list
  std::vector<Option> storage(opt_list);

  // Serialzing
  coap_te::error_code ecs;
  coap_te::mutable_buffer buf(data);

  MessageSerialize req{tp, co, tk};
  for (auto& op : storage)
    req.add_option(op);
  EXPECT_EQ(opt_list.size(), req.count_options());

  req.payload(payload);

  size_s = serialize(req, mid, buf, ecs);
  EXPECT_FALSE(ecs);
  if (expected_size > 0) {
    EXPECT_EQ(size_s, expected_size);
  }

  // Parsing
  coap_te::const_buffer buf_p(data, size_s);
  coap_te::error_code ecp;
  MessageParse resp;
  auto size_p = msg::parse(buf_p, resp, ecp);
  EXPECT_FALSE(ecp);
  EXPECT_EQ(size_s, size_p);
  // Header
  EXPECT_EQ(tp, resp.get_type());
  EXPECT_EQ(co, resp.get_code());
  EXPECT_EQ(mid, resp.mid());
  EXPECT_EQ(msg::clamp_token_size(tk.size()), resp.token().size());
  EXPECT_EQ(0, std::memcmp(resp.token().data(), tk.data(),
                            resp.token().size()));
  // Option
  EXPECT_EQ(opt_list.size(), resp.count_options());
  auto it = resp.begin();
  msg::options::number prev = msg::options::number::invalid;
  for (auto& op0 : req) {
    auto op = coap_te::core::forward_second_if_pair(op0);
    auto op2 = coap_te::core::forward_second_if_pair(*it++);
    EXPECT_EQ(op, op2);
    EXPECT_EQ(op.option_number(), op2.option_number());
    EXPECT_EQ(op.data_size(), op2.data_size());
    EXPECT_EQ(msg::options::header_size(op, prev), msg::options::header_size(op2, prev));
    EXPECT_EQ(msg::options::size(op, prev), msg::options::size(op2, prev));
    EXPECT_EQ(0, std::memcmp(op.data(), op2.data(), op.data_size()));
    prev = op.option_number();
  }
  // Payload
  EXPECT_EQ(payload.size(), resp.payload().size());
  EXPECT_EQ(0, std::memcmp(resp.payload().data(), payload.data(),
                            resp.payload().size()));
}

template<typename MessageSerialize,
         typename MessageParse,
         typename ...Args,
         typename Option>
void serialize_parse_success(
            msg::type tp,
            msg::code co,
            msg::message_id mid,
            const coap_te::const_buffer& tk,
            std::initializer_list<Option> opt_list,
            const coap_te::const_buffer& payload,
            std::size_t expected_size = 0) noexcept {
  serialize_parse_success_impl<MessageSerialize, MessageParse, Option>(
              tp, co, mid, tk,
              opt_list, payload, expected_size);
  if constexpr (sizeof...(Args) >= 2)
    serialize_parse_success<Args...>(tp, co, mid, tk,
              opt_list, payload, expected_size);
}

TEST(CoAPMessage, SerializeParseMessage) {
  {
    SCOPED_TRACE("Header + token + no option + no payload 2");
    serialize_parse_success<TYPES_COMBINE>(
                    msg::type::confirmable,
                    msg::code::put,
                    0x1234U,
                    coap_te::const_buffer{"teste"},         // token
                    std::initializer_list<opt>{},           // option list
                    coap_te::const_buffer{},                // payload
                    4 + 5);                                 // output
  }
  {
    SCOPED_TRACE("Header + token + no option + no payload NO ALLOC");
    serialize_parse_success<TYPES_COMBINE_NO_ALLOC>(
                    msg::type::confirmable,
                    msg::code::put,
                    0x1234U,
                    coap_te::const_buffer{"teste"},         // token
                    std::initializer_list<opt_node>{},      // option list
                    coap_te::const_buffer{});               // payload
  }
  {
    SCOPED_TRACE("Header + token + options + no payload");
    serialize_parse_success<TYPES_COMBINE>(
                    msg::type::confirmable,
                    msg::code::put,
                    0x1234U,
                    coap_te::const_buffer{"teste"},         // token
                    std::initializer_list<opt>{
                      opt{msg::options::accept::text_plain},
                      opt{msg::options::content::json},
                      opt{msg::options::number::uri_port, 5683},
                      opt{msg::options::number::uri_path, "my"},
                      opt{msg::options::number::uri_path, "path"}
                    },           // option list
                    coap_te::const_buffer{});
  }
  {
    SCOPED_TRACE("Header + token + options + no payload NO ALLOC");
    serialize_parse_success<TYPES_COMBINE_NO_ALLOC>(
                    msg::type::confirmable,
                    msg::code::put,
                    0x1234U,
                    coap_te::const_buffer{"teste"},         // token
                    std::initializer_list<opt_node>{
                      opt_node{msg::options::accept::text_plain},
                      opt_node{msg::options::content::json},
                      opt_node{msg::options::number::uri_port, 5683},
                      opt_node{msg::options::number::uri_path, "my"},
                      opt_node{msg::options::number::uri_path, "path"}
                    },           // option list
                    coap_te::const_buffer{});
  }
  {
    SCOPED_TRACE("Header + token + options + payload");
    serialize_parse_success<TYPES_COMBINE>(
                    msg::type::confirmable,
                    msg::code::put,
                    0x1234U,
                    coap_te::const_buffer{"teste"},         // token
                    std::initializer_list<opt>{
                      opt{msg::options::accept::text_plain},
                      opt{msg::options::content::json},
                      opt{msg::options::number::uri_port, 5683},
                      opt{msg::options::number::uri_path, "my"},
                      opt{msg::options::number::uri_path, "path"},
                      opt{msg::options::number::if_none_match}
                    },           // option list
                    coap_te::const_buffer{"My very long long "
                                          "payload that I want"
                                          " to test but don't know"
                                          "what to right here"});
  }
  {
    SCOPED_TRACE("Header + token + options + payload NO ALLOC");
    serialize_parse_success<TYPES_COMBINE_NO_ALLOC>(
                    msg::type::confirmable,
                    msg::code::put,
                    0x1234U,
                    coap_te::const_buffer{"teste"},         // token
                    std::initializer_list<opt_node>{
                      opt_node{msg::options::accept::text_plain},
                      opt_node{msg::options::content::json},
                      opt_node{msg::options::number::uri_port, 5683},
                      opt_node{msg::options::number::uri_path, "my"},
                      opt_node{msg::options::number::uri_path, "path"},
                      opt_node{msg::options::number::if_none_match}
                    },           // option list
                    coap_te::const_buffer{"My very long long "
                                          "payload that I want"
                                          " to test but don't know"
                                          "what to right here"});
  }
}
