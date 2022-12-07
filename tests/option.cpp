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

#include "coap-te/message/options/definitions.hpp"
#include "coap-te/message/options/option.hpp"

namespace opt = coap_te::message::options;

template<typename ...Args>
constexpr auto
create(opt::number op, Args&& ... args)  {
  return opt::option::create<opt::check_all, true>(op, std::forward<Args>(args)...);
}

TEST(CoAPMessage, OptionConstructor) {
  // Testing constructors
  {
    opt::option invalid_op;
    EXPECT_EQ(invalid_op.size(), 0);
    EXPECT_FALSE(invalid_op.is_valid());
  }
  {
    opt::option empty_op = opt::option::create(opt::number::if_none_match);
    EXPECT_EQ(empty_op.size(), 0);
    EXPECT_TRUE(empty_op.is_valid());
  }
  {
    opt::option uint_op = opt::option::create(opt::number::uri_port, 10);
    EXPECT_EQ(uint_op.size(), 1);
    EXPECT_TRUE(uint_op.is_valid());
  }
  {
    opt::option string_op =
                  opt::option::create(opt::number::uri_host, "myhost");
    EXPECT_EQ(string_op.size(), 6);
    EXPECT_TRUE(string_op.is_valid());
  }
  {
    unsigned char arr[] = {1, 2, 3, 4, 5};
    opt::option opaque_op =
                  opt::option::create(
                    opt::number::if_match,
                    coap_te::const_buffer(arr));
    EXPECT_EQ(opaque_op.size(), 5);
    EXPECT_TRUE(opaque_op.is_valid());
  }
  // Faling constructor - NOT throwing!
  {
    {
      // Empty option not empty
      opt::option empty_op = opt::option::create<
                                    opt::check_all,
                                    false>(opt::number::if_none_match, 1);
      EXPECT_EQ(empty_op.size(), 0);
      EXPECT_FALSE(empty_op.is_valid());
    }
    {
      // Unsigned option empty
      opt::option uint_op = opt::option::create<
                                    opt::check_all,
                                    false>(opt::number::uri_port);
      EXPECT_EQ(uint_op.size(), 0);
      EXPECT_FALSE(uint_op.is_valid());
    }
    {
      // String option opaque
      unsigned char arr[] = {1, 2, 3, 4, 5};
      opt::option string_op = opt::option::create<
                                    opt::check_all,
                                    false>(opt::number::uri_host,
                                     coap_te::const_buffer(arr));
      EXPECT_EQ(string_op.size(), 0);
      EXPECT_FALSE(string_op.is_valid());
    }
    {
      // Opaque option string
      opt::option opaque_op = opt::option::create<
                                    opt::check_all,
                                    false>(opt::number::if_match, "myoption");
      EXPECT_EQ(opaque_op.size(), 0);
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
