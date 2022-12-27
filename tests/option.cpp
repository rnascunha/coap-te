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
#include "coap-te/message/options/serialize.hpp"
#include "coap-te/message/options/option_func.hpp"

namespace opt = coap_te::message::options;

template<typename ...Args>
constexpr auto
create(opt::number op, Args&& ... args)  {
  return opt::create<opt::check_all, true>(
              op, std::forward<Args>(args)...);
}

TEST(CoAPMessage, OptionConstructor) {
  {
    SCOPED_TRACE("Option constructor tests");
    {
      opt::option invalid_op;
      EXPECT_EQ(invalid_op.data_size(), 0);
      EXPECT_FALSE(opt::is_valid(invalid_op));
    }
    {
      opt::option empty_op = opt::create(opt::number::if_none_match);
      EXPECT_EQ(empty_op.data_size(), 0);
      EXPECT_TRUE(opt::is_valid(empty_op));
    }
    {
      opt::option uint_op = opt::create(opt::number::uri_port, 10);
      EXPECT_EQ(uint_op.data_size(), 1);
      EXPECT_TRUE(opt::is_valid(uint_op));
      EXPECT_EQ(unsigned_option(uint_op), 10);
    }
    {
      opt::option string_op =
                    opt::create(opt::number::uri_host, "myhost");
      EXPECT_EQ(string_op.data_size(), 6);
      EXPECT_TRUE(opt::is_valid(string_op));
    }
    {
      unsigned char arr[] = {1, 2, 3, 4, 5};
      opt::option opaque_op =
                    opt::create(
                      opt::number::if_match,
                      coap_te::const_buffer(arr));
      EXPECT_EQ(opaque_op.data_size(), 5);
      EXPECT_TRUE(opt::is_valid(opaque_op));
    }
    {
      opt::option accept(opt::accept::text_plain);
      EXPECT_EQ(accept.data_size(), 0);
      EXPECT_TRUE(opt::is_valid(accept));
      EXPECT_EQ(unsigned_option(accept),
                static_cast<unsigned>(opt::content::text_plain));
    }
    {
      opt::option content(opt::content::octet_stream);
      EXPECT_EQ(content.data_size(), 1);
      EXPECT_TRUE(opt::is_valid(content));
      EXPECT_EQ(unsigned_option(content),
                static_cast<unsigned>(opt::content::octet_stream));
    }
  }
  {
    SCOPED_TRACE("Faling constructor - NOT throwing!");
    {
      // Empty option not empty
      opt::option empty_op = opt::create<
                                    opt::check_all,
                                    false>(opt::number::if_none_match, 1);
      EXPECT_EQ(empty_op.data_size(), 0);
      EXPECT_FALSE(opt::is_valid(empty_op));
    }
    {
      // Unsigned option empty
      opt::option uint_op = opt::create<
                                    opt::check_all,
                                    false>(opt::number::uri_port);
      EXPECT_EQ(uint_op.data_size(), 0);
      EXPECT_FALSE(opt::is_valid(uint_op));
    }
    {
      // String option opaque
      unsigned char arr[] = {1, 2, 3, 4, 5};
      opt::option string_op = opt::create<
                                    opt::check_all,
                                    false>(opt::number::uri_host,
                                     coap_te::const_buffer(arr));
      EXPECT_EQ(string_op.data_size(), 0);
      EXPECT_FALSE(opt::is_valid(string_op));
    }
    {
      // Opaque option string
      opt::option opaque_op = opt::create<
                                    opt::check_all,
                                    false>(opt::number::if_match, "myoption");
      EXPECT_EQ(opaque_op.data_size(), 0);
      EXPECT_FALSE(opt::is_valid(opaque_op));
    }
  }
#if COAP_TE_ENABLE_EXCEPTIONS == 1
  {
    SCOPED_TRACE("Faling constructor - throwing!");
    {
      // Empty option not empty
      EXPECT_THROW(::create(opt::number::if_none_match, 1), coap_te::exception);
    }
    {
      // Unsigned option empty
      EXPECT_THROW(::create(opt::number::uri_port), coap_te::exception);
    }
    {
      // String option opaque
      unsigned char arr[] = {1, 2, 3, 4, 5};
      EXPECT_THROW(::create(opt::number::uri_host, coap_te::const_buffer(arr)),
                   coap_te::exception);
    }
    {
      // Opaque option string
      EXPECT_THROW(::create(opt::number::if_match, "myoption"),
                   coap_te::exception);
    }
  }
#endif  // COAP_TE_ENABLE_EXCEPTIONS == 1
}

TEST(CoAPMessage, OptionViewConstructor) {
  {
    SCOPED_TRACE("Option View constructor tests");
    {
      opt::option_view invalid_op;
      EXPECT_EQ(invalid_op.data_size(), 0);
      EXPECT_FALSE(opt::is_valid(invalid_op));
    }
    {
      opt::option_view empty_op{opt::number::if_none_match};
      EXPECT_EQ(empty_op.data_size(), 0);
      EXPECT_TRUE(opt::is_valid(empty_op));
    }
    {
      unsigned port = 10;
      opt::option_view uint_op{opt::number::uri_port, port};
      EXPECT_EQ(uint_op.data_size(), 1);
      EXPECT_TRUE(opt::is_valid(uint_op));
      EXPECT_EQ(unsigned_option(uint_op), 10);
    }
    {
      unsigned port = 0;
      opt::option_view uint_op{opt::number::uri_port, port};
      EXPECT_EQ(uint_op.data_size(), 0);
      EXPECT_TRUE(opt::is_valid(uint_op));
    }
    {
      opt::option_view string_op{opt::number::uri_host, "myhost"};
      EXPECT_EQ(string_op.data_size(), 6);
      EXPECT_TRUE(opt::is_valid(string_op));
    }
    {
      unsigned char arr[] = {1, 2, 3, 4, 5};
      opt::option_view opaque_op{opt::number::if_match,
                                 coap_te::const_buffer(arr)};
      EXPECT_EQ(opaque_op.data_size(), 5);
      EXPECT_TRUE(opt::is_valid(opaque_op));
    }
    {
      auto acc = opt::accept::text_plain;
      opt::option_view accept(acc);
      EXPECT_EQ(accept.data_size(), 0);
      EXPECT_TRUE(opt::is_valid(accept));
      EXPECT_EQ(unsigned_option(accept),
                static_cast<unsigned>(opt::content::text_plain));
    }
    {
      auto con = opt::content::octet_stream;
      opt::option_view content(con);
      EXPECT_EQ(content.data_size(), 1);
      EXPECT_TRUE(opt::is_valid(content));
      EXPECT_EQ(unsigned_option(content),
                static_cast<unsigned>(opt::content::octet_stream));
    }
  }
//   {
//     SCOPED_TRACE("Faling constructor - NOT throwing!");
//     {
//       // Empty option not empty
//       opt::option_view empty_op = opt::create<
//                                     opt::check_all,
//                                     false>(opt::number::if_none_match, 1);
//       EXPECT_EQ(empty_op.data_size(), 0);
//       EXPECT_FALSE(empty_op.is_valid());
//     }
//     {
//       // Unsigned option empty
//       opt::option_view uint_op = opt::create<
//                                     opt::check_all,
//                                     false>(opt::number::uri_port);
//       EXPECT_EQ(uint_op.data_size(), 0);
//       EXPECT_FALSE(uint_op.is_valid());
//     }
//     {
//       // String option opaque
//       unsigned char arr[] = {1, 2, 3, 4, 5};
//       opt::option_view string_op = opt::create<
//                                     opt::check_all,
//                                     false>(opt::number::uri_host,
//                                      coap_te::const_buffer(arr));
//       EXPECT_EQ(string_op.data_size(), 0);
//       EXPECT_FALSE(string_op.is_valid());
//     }
//     {
//       // Opaque option string
//       opt::option_view opaque_op = opt::create<
//                                     opt::check_all,
//                                     false>(opt::number::if_match, "myoption");
//       EXPECT_EQ(opaque_op.data_size(), 0);
//       EXPECT_FALSE(opaque_op.is_valid());
//     }
//   }
// #if COAP_TE_ENABLE_EXCEPTIONS == 1
//   {
//     SCOPED_TRACE("Faling constructor - throwing!");
//     {
//       // Empty option not empty
//       EXPECT_THROW(::create(opt::number::if_none_match, 1), coap_te::exception);
//     }
//     {
//       // Unsigned option empty
//       EXPECT_THROW(::create(opt::number::uri_port), coap_te::exception);
//     }
//     {
//       // String option opaque
//       unsigned char arr[] = {1, 2, 3, 4, 5};
//       EXPECT_THROW(::create(opt::number::uri_host, coap_te::const_buffer(arr)),
//                    coap_te::exception);
//     }
//     {
//       // Opaque option string
//       EXPECT_THROW(::create(opt::number::if_match, "myoption"),
//                    coap_te::exception);
//     }
//   }
// #endif  // COAP_TE_ENABLE_EXCEPTIONS == 1
}

#if COAP_TE_ENABLE_DYNAMIC_ALLOC_SUPPORT == 1

TEST(CoAPMessage, OptionContainerConstructor) {
  {
    SCOPED_TRACE("Option View constructor tests");
    {
      opt::option_container invalid_op;
      EXPECT_EQ(invalid_op.data_size(), 0);
      EXPECT_FALSE(opt::is_valid(invalid_op));
    }
    {
      opt::option_container empty_op{opt::number::if_none_match};
      EXPECT_EQ(empty_op.data_size(), 0);
      EXPECT_TRUE(opt::is_valid(empty_op));
    }
    {
      opt::option_container uint_op{opt::number::uri_port, 10};
      EXPECT_EQ(uint_op.data_size(), 1);
      EXPECT_TRUE(opt::is_valid(uint_op));
      EXPECT_EQ(unsigned_option(uint_op), 10);
    }
    {
      opt::option_container uint_op{opt::number::uri_port, 0};
      EXPECT_EQ(uint_op.data_size(), 0);
      EXPECT_TRUE(opt::is_valid(uint_op));
    }
    {
      opt::option_container string_op{opt::number::uri_host, "myhost"};
      EXPECT_EQ(string_op.data_size(), 6);
      EXPECT_TRUE(opt::is_valid(string_op));
    }
    {
      unsigned char arr[] = {1, 2, 3, 4, 5};
      opt::option_container opaque_op{opt::number::if_match,
                                 coap_te::const_buffer(arr)};
      EXPECT_EQ(opaque_op.data_size(), 5);
      EXPECT_TRUE(opt::is_valid(opaque_op));
    }
    {
      opt::option_container accept(opt::accept::text_plain);
      EXPECT_EQ(accept.data_size(), 0);
      EXPECT_TRUE(opt::is_valid(accept));
      EXPECT_EQ(unsigned_option(accept),
                static_cast<unsigned>(opt::content::text_plain));
    }
    {
      opt::option_container content(opt::content::octet_stream);
      EXPECT_EQ(content.data_size(), 1);
      EXPECT_TRUE(opt::is_valid(content));
      EXPECT_EQ(unsigned_option(content),
                static_cast<unsigned>(opt::content::octet_stream));
    }
  }
//   {
//     SCOPED_TRACE("Faling constructor - NOT throwing!");
//     {
//       // Empty option not empty
//       opt::option_view empty_op = opt::create<
//                                     opt::check_all,
//                                     false>(opt::number::if_none_match, 1);
//       EXPECT_EQ(empty_op.data_size(), 0);
//       EXPECT_FALSE(empty_op.is_valid());
//     }
//     {
//       // Unsigned option empty
//       opt::option_view uint_op = opt::create<
//                                     opt::check_all,
//                                     false>(opt::number::uri_port);
//       EXPECT_EQ(uint_op.data_size(), 0);
//       EXPECT_FALSE(uint_op.is_valid());
//     }
//     {
//       // String option opaque
//       unsigned char arr[] = {1, 2, 3, 4, 5};
//       opt::option_view string_op = opt::create<
//                                     opt::check_all,
//                                     false>(opt::number::uri_host,
//                                      coap_te::const_buffer(arr));
//       EXPECT_EQ(string_op.data_size(), 0);
//       EXPECT_FALSE(string_op.is_valid());
//     }
//     {
//       // Opaque option string
//       opt::option_view opaque_op = opt::create<
//                                     opt::check_all,
//                                     false>(opt::number::if_match, "myoption");
//       EXPECT_EQ(opaque_op.data_size(), 0);
//       EXPECT_FALSE(opaque_op.is_valid());
//     }
//   }
// #if COAP_TE_ENABLE_EXCEPTIONS == 1
//   {
//     SCOPED_TRACE("Faling constructor - throwing!");
//     {
//       // Empty option not empty
//       EXPECT_THROW(::create(opt::number::if_none_match, 1), coap_te::exception);
//     }
//     {
//       // Unsigned option empty
//       EXPECT_THROW(::create(opt::number::uri_port), coap_te::exception);
//     }
//     {
//       // String option opaque
//       unsigned char arr[] = {1, 2, 3, 4, 5};
//       EXPECT_THROW(::create(opt::number::uri_host, coap_te::const_buffer(arr)),
//                    coap_te::exception);
//     }
//     {
//       // Opaque option string
//       EXPECT_THROW(::create(opt::number::if_match, "myoption"),
//                    coap_te::exception);
//     }
//   }
// #endif  // COAP_TE_ENABLE_EXCEPTIONS == 1
}
#endif  // COAP_TE_ENABLE_DYNAMIC_ALLOC_SUPPORT == 1

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

template<typename Option>
void test_serialize_parse_success(
                  opt::number before,
                  opt::number current,
                  const coap_te::const_buffer& buf_in) {
  std::uint8_t data[256];
  coap_te::mutable_buffer buf(data);
  coap_te::error_code ecs;

  Option ops(current, buf_in);
  auto size_s = opt::serialize<opt::check_none>(before, ops, buf, ecs);
  EXPECT_FALSE(ecs);
  EXPECT_EQ(size_s, calc_options_size(
                        static_cast<opt::number_type>(before),
                        static_cast<opt::number_type>(current),
                        buf_in.size()));

  coap_te::error_code ecp;
  Option opp;
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

template<typename Option>
void test_serialize_parse_success(
                  opt::number before,
                  opt::number current,
                  unsigned value) {
  auto [data, size] = coap_te::core::to_small_big_endian(value);
  test_serialize_parse_success<Option>(before,
                               current,
                               coap_te::const_buffer(&data, size));
}

template<typename Option>
void test_serialize_parse_success(
                  opt::number before,
                  opt::number current) {
  test_serialize_parse_success<Option>(before,
                               current,
                               coap_te::const_buffer{});
}

template<typename ...Args>
void test_serialize_parse_all(
  opt::number before,
  opt::number current,
  Args&& ...args) {
  {
    SCOPED_TRACE("Option");
    test_serialize_parse_success<opt::option>(
                    before, current, std::forward<Args>(args)...);
  }
  {
    SCOPED_TRACE("Option View");
    test_serialize_parse_success<opt::option_view>(
                    before, current, std::forward<Args>(args)...);
  }
#if COAP_TE_ENABLE_DYNAMIC_ALLOC_SUPPORT == 1
  {
    SCOPED_TRACE("Option Container");
    test_serialize_parse_success<opt::option_container>(
                    before, current, std::forward<Args>(args)...);
  }
#endif  // COAP_TE_ENABLE_DYNAMIC_ALLOC_SUPPORT == 1
}

TEST(CoAPMessage, OptionSerializeParse) {
  {
    SCOPED_TRACE("Serialize parse string uri-host");
    test_serialize_parse_all(opt::number::invalid,
                               opt::number::uri_host,
                               coap_te::const_buffer("192.168.0.1"));
  }
  {
    SCOPED_TRACE("Serialize parse big string uri-host");
    test_serialize_parse_all(opt::number::invalid,
                               opt::number::uri_host,
                               coap_te::const_buffer("192.168.111.111"));
  }
  {
    SCOPED_TRACE("Serialize parse number max-age");
    test_serialize_parse_all(opt::number::invalid,
                               opt::number::max_age,
                               60);
  }
  {
    SCOPED_TRACE("Serialize parse empty if_none_match");
    test_serialize_parse_all(opt::number::invalid,
                               opt::number::if_none_match);
  }
  {
    SCOPED_TRACE("Serialize parse big string uri-host");
    test_serialize_parse_all(opt::number::uri_path,
                               opt::number::uri_path,
                               coap_te::const_buffer(
                                "testedeumpathgrandemasmuito"
                                "grandemesmoqueatepergoacont"
                                "adequaograndeelaemasegrande"
                                "mesmoemnossoquegrandemeudeu"
                                "sgrandao"));
  }
}
