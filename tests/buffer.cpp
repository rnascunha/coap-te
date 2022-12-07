/**
 * @file buffer.cpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-11-23
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <vector>
#include <array>
#include <string>
#include <string_view>

#include "coap-te/core/mutable_buffer.hpp"
#include "coap-te/core/const_buffer.hpp"

void test_const_buffer_12345(const coap_te::const_buffer& buf) noexcept {
  EXPECT_EQ(buf.size(), 5);
  EXPECT_THAT(
    static_cast<const char*>(buf.data()), ::testing::StartsWith("12345"));
  {
    // Operator[] test
    char c = '1';
    for (auto i = 0u; i < buf.size(); ++i) {
      EXPECT_EQ(buf[i], c + i);
    }
  }
  {
    // Range for test
    unsigned char c = '1';
    for (auto v : buf) {
      EXPECT_EQ(v, c);
      ++c;
    }
  }
  {
    // Iterator deference and post increment test
    unsigned char c = '1';
    for (auto it = buf.begin(); it != buf.end();) {
      EXPECT_EQ(*it++, c);
      ++c;
    }
  }
  {
    // Iterator deference and pre increment test
    unsigned char c = '2';
    auto end = buf.end();
    end--;
    for (auto it = buf.begin(); it != end;) {
      EXPECT_EQ(*(++it), c);
      ++c;
    }
  }
}

TEST(CoreBuffer, ConstBufferChar) {
  {
    // const char* constructor
    coap_te::const_buffer buf("12345");
    test_const_buffer_12345(buf);
  }
  {
    char data[] = {'1', '2', '3', '4', '5'};
    coap_te::const_buffer buf(data, 5);
    test_const_buffer_12345(buf);
  }
  {
    std::array<char, 5> data{'1', '2', '3', '4', '5'};
    coap_te::const_buffer buf(data);
    test_const_buffer_12345(buf);
  }
  {
    std::vector<char> data{'1', '2', '3', '4', '5'};
    coap_te::const_buffer buf(data);
    test_const_buffer_12345(buf);
  }
  {
    std::string data{"12345"};
    coap_te::const_buffer buf(data);
    test_const_buffer_12345(buf);
  }
  {
    std::string_view data{"12345"};
    coap_te::const_buffer buf(data);
    test_const_buffer_12345(buf);
  }
}

void test_const_buffer_int_12345(const coap_te::const_buffer& buf) {
  EXPECT_EQ(buf.size(), 5 * sizeof(int));
  {
    int size = static_cast<int>(buf.size() / sizeof(int));
    int v = 1;
    auto it = buf.begin();
    for (int i = 0; i < size; ++i) {
      auto vv = *reinterpret_cast<const int*>(&(*it));
      EXPECT_EQ(vv, v);
      ++v;
      it += sizeof(int);
    }
  }
  {
    int size = static_cast<int>(buf.size() / sizeof(int));
    int v = 1;
    auto it = buf.begin();
    for (int i = 0; i < size; ++i) {
      auto vv = *reinterpret_cast<const int*>(&(*it));
      EXPECT_EQ(vv, v);
      ++v;
      it = it + sizeof(int);
    }
  }
}

TEST(CoreBuffer, ConstBufferInt) {
  {
    int data[]{1, 2, 3, 4, 5};
    coap_te::const_buffer buf(data);
    test_const_buffer_int_12345(buf);
  }
  {
    std::array<int, 5> data{1, 2, 3, 4, 5};
    coap_te::const_buffer buf(data);
    test_const_buffer_int_12345(buf);
  }
  {
    std::vector<int> data{1, 2, 3, 4, 5};
    coap_te::const_buffer buf(data);
    test_const_buffer_int_12345(buf);
  }
}

TEST(CoreBuffer, Const2MutableBufferChar) {
  {
    char data[] = {'1', '2', '3', '4', '5'};
    coap_te::mutable_buffer buf(data, 5);
    test_const_buffer_12345(buf);
  }
  {
    std::array<char, 5> data{'1', '2', '3', '4', '5'};
    coap_te::mutable_buffer buf(data);
    test_const_buffer_12345(buf);
  }
  {
    std::vector<char> data{'1', '2', '3', '4', '5'};
    coap_te::mutable_buffer buf(data);
    test_const_buffer_12345(buf);
  }
  {
    std::string data{"12345"};
    coap_te::mutable_buffer buf(data);
    test_const_buffer_12345(buf);
  }
}

TEST(CoreBuffer, Const2MutableBufferInt) {
  {
    int data[]{1, 2, 3, 4, 5};
    coap_te::mutable_buffer buf(data);
    test_const_buffer_int_12345(buf);
  }
  {
    std::array<int, 5> data{1, 2, 3, 4, 5};
    coap_te::mutable_buffer buf(data);
    test_const_buffer_int_12345(buf);
  }
  {
    std::vector<int> data{1, 2, 3, 4, 5};
    coap_te::mutable_buffer buf(data);
    test_const_buffer_int_12345(buf);
  }
}

void test_change_mutable_buffer(coap_te::mutable_buffer& buf) noexcept {  // NOLINT
  for (std::uint8_t i = 0; i < 5; ++i)
      buf[i] = i + 1 + '0';
  test_const_buffer_12345(buf);
}

TEST(CoreBuffer, ChangingMutableBufferChar) {
  {
    char data[5];
    coap_te::mutable_buffer buf(data, 5);
    test_change_mutable_buffer(buf);
  }
  {
    std::array<char, 5> data;
    coap_te::mutable_buffer buf(data);
    test_change_mutable_buffer(buf);
  }
  {
    std::vector<char> data(5);
    coap_te::mutable_buffer buf(data);
    test_change_mutable_buffer(buf);
  }
  {
    std::string data{"12345"};
    coap_te::mutable_buffer buf(data);
    test_change_mutable_buffer(buf);
  }
}
