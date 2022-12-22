/**
 * @file error_code.cpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-21
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <gtest/gtest.h>

#include "coap-te/core/error.hpp"
#include "coap-te/debug/error.hpp"

TEST(System, ErrorCode) {
  {
    SCOPED_TRACE("coap_te::error_code default construct");
    coap_te::error_code ec;
    EXPECT_FALSE(ec);
    ec = coap_te::errc::invalid_option;
    EXPECT_TRUE(ec);
    EXPECT_EQ(ec, coap_te::errc::invalid_option);
    EXPECT_NE(ec, coap_te::errc::option_length);
  }
  {
    SCOPED_TRACE("coap_te::error_code errc construct");
    coap_te::error_code ec = coap_te::no_buffer_space;
    EXPECT_TRUE(ec);
    EXPECT_EQ(ec, coap_te::errc::no_buffer_space);
    EXPECT_NE(ec, coap_te::errc::option_format);
  }

#if COAP_TE_ENABLE_STD_ERROR_CODE == 1
  {
    SCOPED_TRACE("std::error_code default construct");
    std::error_code ec;
    EXPECT_FALSE(ec);
    ec = coap_te::errc::invalid_option;
    EXPECT_TRUE(ec);
    EXPECT_EQ(ec, coap_te::errc::invalid_option);
    EXPECT_NE(ec, coap_te::errc::option_length);
  }
  {
    SCOPED_TRACE("std::error_code errc construct");
    std::error_code ec = coap_te::no_buffer_space;
    EXPECT_TRUE(ec);
    EXPECT_EQ(ec, coap_te::errc::no_buffer_space);
    EXPECT_NE(ec, coap_te::errc::option_format);
  }
  {
    SCOPED_TRACE("convert coap_te::error_code to std::error_code");
    coap_te::error_code ec = coap_te::no_buffer_space;
    std::error_code stdec = ec.error();
    EXPECT_TRUE(stdec);
    EXPECT_EQ(stdec, coap_te::errc::no_buffer_space);
    EXPECT_NE(stdec, coap_te::errc::option_format);
  }
#endif  // COAP_TE_ENABLE_STD_ERROR_CODE == 1
#if COAP_TE_ENABLE_EXCEPTIONS == 1
  {
    EXPECT_THROW({
      throw coap_te::exception(
          coap_te::error_code{coap_te::errc::no_buffer_space});
    }, coap_te::exception);
  }
#endif  // COAP_TE_ENABLE_EXCEPTIONS == 1
}
