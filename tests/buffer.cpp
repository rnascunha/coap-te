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

#include <iostream>

#include "coap-te/buffer/buffer.hpp"
#include "coap-te/buffer/buffer_compare.hpp"
#include "coap-te/buffer/buffers_iterator.hpp"
#include "coap-te/buffer/iterator_container.hpp"

TEST(BufferTraits, IsConstBufferTrait) {
  // Check if class can be used as buffer type
  {
    auto vv = coap_te::is_const_buffer_v<std::array<int, 4>>;
    EXPECT_TRUE(vv);
  }
  EXPECT_TRUE(coap_te::is_const_buffer_v<std::vector<int>>);
  EXPECT_TRUE(coap_te::is_const_buffer_v<std::string>);
  EXPECT_TRUE(coap_te::is_const_buffer_v<std::string_view>);

  struct A{};
  struct B {
    const void* data() { return nullptr; }
  };
  struct C {
    std::size_t size() { return 0; }
  };
  struct D {
    const void* data() { return nullptr; }
    std::size_t size() { return 0; }
  };
  struct E {
    int* data() { return nullptr; }
    std::size_t size() { return 0; }
  };
  struct F {
    char data() { return '0'; }
    std::size_t size() { return 0; }
  };
  struct G {
    const char* data() { return nullptr; }
    int size() { return 0; }
  };

  EXPECT_FALSE(coap_te::is_const_buffer_v<A>);
  EXPECT_FALSE(coap_te::is_const_buffer_v<B>);
  EXPECT_FALSE(coap_te::is_const_buffer_v<C>);
  EXPECT_TRUE(coap_te::is_const_buffer_v<D>);
  EXPECT_TRUE(coap_te::is_const_buffer_v<E>);
  EXPECT_FALSE(coap_te::is_const_buffer_v<F>);
  EXPECT_FALSE(coap_te::is_const_buffer_v<G>);
}

TEST(BufferTraits, IsMutableBufferTrait) {
  // Check if class can be used as buffer type
  {
    auto vv = coap_te::is_mutable_buffer_v<std::array<int, 4>>;
    EXPECT_TRUE(vv);
  }
  EXPECT_TRUE(coap_te::is_mutable_buffer_v<std::vector<int>>);
  EXPECT_TRUE(coap_te::is_mutable_buffer_v<std::string>);
  EXPECT_FALSE(coap_te::is_mutable_buffer_v<std::string_view>);

  struct A{};
  struct B {
    const void* data() { return nullptr; }
  };
  struct C {
    std::size_t size() { return 0; }
  };
  struct D {
    const void* data() { return nullptr; }
    std::size_t size() { return 0; }
  };
  struct E {
    int* data() { return nullptr; }
    std::size_t size() { return 0; }
  };
  struct F {
    char data() { return '0'; }
    std::size_t size() { return 0; }
  };
  struct G {
    const char* data() { return nullptr; }
    int size() { return 0; }
  };

  EXPECT_FALSE(coap_te::is_mutable_buffer_v<A>);
  EXPECT_FALSE(coap_te::is_mutable_buffer_v<B>);
  EXPECT_FALSE(coap_te::is_mutable_buffer_v<C>);
  EXPECT_FALSE(coap_te::is_mutable_buffer_v<D>);
  EXPECT_TRUE(coap_te::is_mutable_buffer_v<E>);
  EXPECT_FALSE(coap_te::is_mutable_buffer_v<F>);
  EXPECT_FALSE(coap_te::is_mutable_buffer_v<G>);

  // // Must fail because const
  {
    auto vv = coap_te::is_mutable_buffer_v<const std::array<int, 4>>;
    EXPECT_FALSE(vv);
  }
  EXPECT_FALSE(coap_te::is_mutable_buffer_v<const std::vector<int>>);
  EXPECT_FALSE(coap_te::is_mutable_buffer_v<const std::string>);
  EXPECT_FALSE(coap_te::is_mutable_buffer_v<const E>);
}

TEST(Buffer, BufferSequenceTrait) {
  {
    char d[5];
    auto buf = coap_te::buffer(d);
    EXPECT_TRUE(coap_te::is_mutable_buffer_sequence_v<decltype(buf)>);
    EXPECT_TRUE(coap_te::is_const_buffer_sequence_v<decltype(buf)>);
  }
  {
    const char d[5]{};
    auto buf = coap_te::buffer(d);
    EXPECT_FALSE(coap_te::is_mutable_buffer_sequence_v<decltype(buf)>);
    EXPECT_TRUE(coap_te::is_const_buffer_sequence_v<decltype(buf)>);
  }
  {
    char d1[5]{}, d2[5]{};
    std::vector<coap_te::mutable_buffer> v{coap_te::buffer(d1),
                                         coap_te::buffer(d2)};
    EXPECT_TRUE(coap_te::is_mutable_buffer_sequence_v<decltype(v)>);
    EXPECT_TRUE(coap_te::is_const_buffer_sequence_v<decltype(v)>);
  }
  {
    const char d1[5]{}, d2[5]{};
    std::vector<coap_te::const_buffer> v{coap_te::buffer(d1),
                                         coap_te::buffer(d2)};
    EXPECT_FALSE(coap_te::is_mutable_buffer_sequence_v<decltype(v)>);
    EXPECT_TRUE(coap_te::is_const_buffer_sequence_v<decltype(v)>);
  }
  {
    std::vector<char> v{};
    EXPECT_FALSE(coap_te::is_mutable_buffer_sequence_v<decltype(v)>);
    EXPECT_FALSE(coap_te::is_const_buffer_sequence_v<decltype(v)>);
  }
}

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

TEST(Buffer, ConstBufferChar) {
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

TEST(Buffer, ConstBufferInt) {
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

TEST(Buffer, Const2MutableBufferChar) {
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

TEST(Buffer, Const2MutableBufferInt) {
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

TEST(Buffer, ChangingMutableBufferChar) {
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

TEST(Buffer, BufferFunctions) {
  {
    SCOPED_TRACE("Mutable buffer constructor");
    {
      SCOPED_TRACE("Buffer std::vector overload");
      std::vector<char> v{1, 2, 3, 4, 5};
      auto buf = coap_te::buffer(v);
      EXPECT_EQ(buf.size(), v.size());
      EXPECT_TRUE(coap_te::is_mutable_buffer_v<decltype(buf)>);
    }
    {
      SCOPED_TRACE("Buffer std::array overload");
      std::array<char, 5> v{1, 2, 3, 4, 5};
      auto buf = coap_te::buffer(v);
      EXPECT_EQ(buf.size(), v.size());
      EXPECT_TRUE(coap_te::is_mutable_buffer_v<decltype(buf)>);
    }
    {
      SCOPED_TRACE("Buffer array overload");
      char v[]{1, 2, 3, 4, 5};
      auto buf = coap_te::buffer(v);
      EXPECT_EQ(buf.size(), sizeof(v) / sizeof(v[0]));
      EXPECT_TRUE(coap_te::is_mutable_buffer_v<decltype(buf)>);
    }
    {
      SCOPED_TRACE("Buffer std::string overload");
      std::string v = "12345";
      auto buf = coap_te::buffer(v);
      EXPECT_EQ(buf.size(), v.size());
      EXPECT_TRUE(coap_te::is_mutable_buffer_v<decltype(buf)>);
    }
  }
  {
    SCOPED_TRACE("Const buffer constructor");
    {
      SCOPED_TRACE("Buffer std::vector overload");
      const std::vector<char> v{1, 2, 3, 4, 5};
      auto buf = coap_te::buffer(v);
      EXPECT_EQ(buf.size(), v.size());
      EXPECT_TRUE(coap_te::is_const_buffer_v<decltype(buf)>);
    }
    {
      SCOPED_TRACE("Buffer std::array overload");
      const std::array<char, 5> v{1, 2, 3, 4, 5};
      auto buf = coap_te::buffer(v);
      EXPECT_EQ(buf.size(), v.size());
      EXPECT_TRUE(coap_te::is_const_buffer_v<decltype(buf)>);
    }
    {
      SCOPED_TRACE("Buffer array overload");
      const char v[]{1, 2, 3, 4, 5};
      auto buf = coap_te::buffer(v);
      EXPECT_EQ(buf.size(), sizeof(v) / sizeof(v[0]));
      EXPECT_TRUE(coap_te::is_const_buffer_v<decltype(buf)>);
    }
    {
      SCOPED_TRACE("Buffer std::string overload");
      const std::string v = "12345";
      auto buf = coap_te::buffer(v);
      EXPECT_EQ(buf.size(), v.size());
      EXPECT_TRUE(coap_te::is_const_buffer_v<decltype(buf)>);
    }
    {
      const std::string_view v = "12345";
      auto buf = coap_te::buffer(v);
      EXPECT_EQ(buf.size(), v.size());
      EXPECT_TRUE(coap_te::is_const_buffer_v<decltype(buf)>);
    }
  }
}

TEST(Buffer, BufferSequenece) {
  {
    std::vector<coap_te::const_buffer> v;
    char d1[5], d2[6], d3[7];
    v.push_back(coap_te::buffer(d1));
    v.push_back(coap_te::buffer(d2));
    v.push_back(coap_te::buffer(d3));
    EXPECT_EQ(coap_te::buffer_size(v), 18);
  }
  {
    char a[] = "12345", b[] = "67890";
    char c[12];
    std::vector<coap_te::const_buffer> v;
    v.push_back(coap_te::buffer(a));
    v.push_back(coap_te::buffer(b));
    auto m = coap_te::buffer(c);
    EXPECT_EQ(coap_te::buffer_size(v), 12);
    EXPECT_EQ(coap_te::buffer_size(m), 12);
    EXPECT_EQ(coap_te::buffer_copy(m, v), 12);
    EXPECT_EQ(0, std::memcmp(c, "12345\0" "67890\0", 12));
  }
  {
    char a[] = "12345", b[] = "67890";
    char c[12];
    coap_te::const_buffer v[] = {coap_te::buffer(a),
                                 coap_te::buffer(b)};
    auto m = coap_te::buffer(c);
    EXPECT_EQ(coap_te::buffer_size(v), 12);
    EXPECT_EQ(coap_te::buffer_size(m), 12);
    EXPECT_EQ(coap_te::buffer_copy(m, v), 12);
    EXPECT_EQ(0, std::memcmp(c, "12345\0" "67890\0", 12));
  }
}

TEST(Buffer, BufferCompare) {
  {
    SCOPED_TRACE("Compare both single buffers");
    {
      SCOPED_TRACE("Equal buffers same size");
      std::uint8_t data1[]{1, 2, 3, 4, 5};
      std::uint8_t data2[]{1, 2, 3, 4, 5};
      EXPECT_EQ(0, coap_te::buffer_compare(coap_te::buffer(data1),
                                          coap_te::buffer(data2)));
    }
    {
      SCOPED_TRACE("Equal buffers max size");
      std::uint8_t data1[]{1, 2, 3, 4, 6};
      std::uint8_t data2[]{1, 2, 3, 4, 5};
      EXPECT_EQ(0, coap_te::buffer_compare(coap_te::buffer(data1),
                                          coap_te::buffer(data2),
                                          sizeof(data1) - 1));
    }
    {
      SCOPED_TRACE("Equal buffers different size 1");
      std::uint8_t data1[]{1, 2, 3, 4};
      std::uint8_t data2[]{1, 2, 3, 4, 5};
      EXPECT_EQ(0, coap_te::buffer_compare(coap_te::buffer(data1),
                                          coap_te::buffer(data2)));
    }
    {
      SCOPED_TRACE("Equal buffers different size 2");
      std::uint8_t data1[]{1, 2, 3, 4, 5};
      std::uint8_t data2[]{1, 2, 3, 4};
      EXPECT_EQ(0, coap_te::buffer_compare(coap_te::buffer(data1),
                                          coap_te::buffer(data2)));
    }
    {
      SCOPED_TRACE("Different buffers same sizes (1 > 2)");
      std::uint8_t data1[]{1, 2, 4, 4, 5};
      std::uint8_t data2[]{1, 2, 3, 4, 5};
      EXPECT_EQ(1, coap_te::buffer_compare(coap_te::buffer(data1),
                                          coap_te::buffer(data2)));
    }
    {
      SCOPED_TRACE("Different buffers same sizes (1 < 2)");
      std::uint8_t data1[]{1, 2, 2, 4, 5};
      std::uint8_t data2[]{1, 2, 3, 4, 5};
      EXPECT_EQ(-1, coap_te::buffer_compare(coap_te::buffer(data1),
                                          coap_te::buffer(data2)));
    }
  }
  {
    SCOPED_TRACE("Compare single buffers + multiple buffer");
    {
      SCOPED_TRACE("Equal buffers same size");
      std::uint8_t data1[]{1, 2, 3, 4, 5};
      std::uint8_t data2[]{1, 2, 3};
      std::uint8_t data3[]{4, 5};
      coap_te::const_buffer buf[] = {coap_te::buffer(data2),
                                     coap_te::buffer(data3)};
      EXPECT_EQ(0, coap_te::buffer_compare(coap_te::buffer(data1),
                                           buf));
    }
    {
      SCOPED_TRACE("Equal buffers max size");
      std::uint8_t data1[]{1, 2, 3, 4, 6};
      std::uint8_t data2[]{1, 2, 3};
      std::uint8_t data3[]{4, 5};
      coap_te::const_buffer buf[] = {coap_te::buffer(data2),
                                     coap_te::buffer(data3)};
      EXPECT_EQ(0, coap_te::buffer_compare(coap_te::buffer(data1),
                                           buf,
                                           sizeof(data1) - 1));
    }
    {
      SCOPED_TRACE("Equal buffers different size 1");
      std::uint8_t data1[]{1, 2, 3, 4};
      std::uint8_t data2[]{1, 2, 3};
      std::uint8_t data3[]{4, 5};
      coap_te::const_buffer buf[] = {coap_te::buffer(data2),
                                     coap_te::buffer(data3)};
      EXPECT_EQ(0, coap_te::buffer_compare(coap_te::buffer(data1),
                                           buf));
    }
    {
      SCOPED_TRACE("Equal buffers different size 2");
      std::uint8_t data1[]{1, 2, 3, 4, 5};
      std::uint8_t data2[]{1, 2, 3};
      std::uint8_t data3[]{4};
      coap_te::const_buffer buf[] = {coap_te::buffer(data2),
                                     coap_te::buffer(data3)};
      EXPECT_EQ(0, coap_te::buffer_compare(coap_te::buffer(data1),
                                           buf));
    }
    {
      SCOPED_TRACE("Different buffers same sizes (1 > 2)");
      std::uint8_t data1[]{1, 2, 4, 4, 5};
      std::uint8_t data2[]{1, 2, 3};
      std::uint8_t data3[]{4};
      coap_te::const_buffer buf[] = {coap_te::buffer(data2),
                                     coap_te::buffer(data3)};
      EXPECT_EQ(1, coap_te::buffer_compare(coap_te::buffer(data1),
                                           buf));
    }
    {
      SCOPED_TRACE("Different buffers same sizes (1 < 2)");
      std::uint8_t data1[]{1, 2, 2, 4, 5};
      std::uint8_t data2[]{1, 2, 3};
      std::uint8_t data3[]{4};
      coap_te::const_buffer buf[] = {coap_te::buffer(data2),
                                     coap_te::buffer(data3)};
      EXPECT_EQ(-1, coap_te::buffer_compare(coap_te::buffer(data1),
                                           buf));
    }
    {
      SCOPED_TRACE("Different buffers same sizes (1 > 2) 2");
      std::uint8_t data1[]{1, 2, 3, 4, 5};
      std::uint8_t data2[]{1, 2, 2};
      std::uint8_t data3[]{4};
      coap_te::const_buffer buf[] = {coap_te::buffer(data2),
                                     coap_te::buffer(data3)};
      EXPECT_EQ(1, coap_te::buffer_compare(coap_te::buffer(data1),
                                           buf));
    }
    {
      SCOPED_TRACE("Different buffers same sizes (1 < 2) 2");
      std::uint8_t data1[]{1, 2, 3, 4, 5};
      std::uint8_t data2[]{1, 2, 4};
      std::uint8_t data3[]{4};
      coap_te::const_buffer buf[] = {coap_te::buffer(data2),
                                     coap_te::buffer(data3)};
      EXPECT_EQ(-1, coap_te::buffer_compare(coap_te::buffer(data1),
                                           buf));
    }
  }
  {
    SCOPED_TRACE("Compare multiple buffers + single buffer");
    {
      SCOPED_TRACE("Equal buffers same size");
      std::uint8_t data1[]{1, 2, 3, 4, 5};
      std::uint8_t data2[]{1, 2, 3};
      std::uint8_t data3[]{4, 5};
      coap_te::const_buffer buf[] = {coap_te::buffer(data2),
                                     coap_te::buffer(data3)};
      EXPECT_EQ(0, coap_te::buffer_compare(buf,
                                           coap_te::buffer(data1)));
    }
    {
      SCOPED_TRACE("Equal buffers same size");
      std::uint8_t data1[]{1, 2, 3, 4, 6};
      std::uint8_t data2[]{1, 2, 3};
      std::uint8_t data3[]{4, 5};
      coap_te::const_buffer buf[] = {coap_te::buffer(data2),
                                     coap_te::buffer(data3)};
      EXPECT_EQ(0, coap_te::buffer_compare(buf,
                                           coap_te::buffer(data1),
                                           sizeof(data1) - 1));
    }
    {
      SCOPED_TRACE("Equal buffers different size 1");
      std::uint8_t data1[]{1, 2, 3, 4};
      std::uint8_t data2[]{1, 2, 3};
      std::uint8_t data3[]{4, 5};
      coap_te::const_buffer buf[] = {coap_te::buffer(data2),
                                     coap_te::buffer(data3)};
      EXPECT_EQ(0, coap_te::buffer_compare(buf,
                                           coap_te::buffer(data1)));
    }
    {
      SCOPED_TRACE("Equal buffers different size 2");
      std::uint8_t data1[]{1, 2, 3, 4, 5};
      std::uint8_t data2[]{1, 2, 3};
      std::uint8_t data3[]{4};
      coap_te::const_buffer buf[] = {coap_te::buffer(data2),
                                     coap_te::buffer(data3)};
      EXPECT_EQ(0, coap_te::buffer_compare(buf,
                                           coap_te::buffer(data1)));
    }
    {
      SCOPED_TRACE("Different buffers same sizes (1 < 2)");
      std::uint8_t data1[]{1, 2, 4, 4, 5};
      std::uint8_t data2[]{1, 2, 3};
      std::uint8_t data3[]{4};
      coap_te::const_buffer buf[] = {coap_te::buffer(data2),
                                     coap_te::buffer(data3)};
      EXPECT_EQ(-1, coap_te::buffer_compare(buf,
                                           coap_te::buffer(data1)));
    }
    {
      SCOPED_TRACE("Different buffers same sizes (1 > 2)");
      std::uint8_t data1[]{1, 2, 2, 4, 5};
      std::uint8_t data2[]{1, 2, 3};
      std::uint8_t data3[]{4};
      coap_te::const_buffer buf[] = {coap_te::buffer(data2),
                                     coap_te::buffer(data3)};
      EXPECT_EQ(1, coap_te::buffer_compare(buf,
                                           coap_te::buffer(data1)));
    }
    {
      SCOPED_TRACE("Different buffers same sizes (1 < 2) 2");
      std::uint8_t data1[]{1, 2, 3, 4, 5};
      std::uint8_t data2[]{1, 2, 2};
      std::uint8_t data3[]{4};
      coap_te::const_buffer buf[] = {coap_te::buffer(data2),
                                     coap_te::buffer(data3)};
      EXPECT_EQ(-1, coap_te::buffer_compare(buf,
                                            coap_te::buffer(data1)));
    }
    {
      SCOPED_TRACE("Different buffers same sizes (1 > 2) 2");
      std::uint8_t data1[]{1, 2, 3, 4, 5};
      std::uint8_t data2[]{1, 2, 4};
      std::uint8_t data3[]{4};
      coap_te::const_buffer buf[] = {coap_te::buffer(data2),
                                     coap_te::buffer(data3)};
      EXPECT_EQ(1, coap_te::buffer_compare(buf,
                                            coap_te::buffer(data1)));
    }
  }
  {
    SCOPED_TRACE("Compare multiple buffers + multiple buffer");
    {
      SCOPED_TRACE("Equal buffers same size");
      std::uint8_t data1[]{1};
      std::uint8_t data2[]{2, 3};
      std::uint8_t data3[]{4, 5};
      std::uint8_t data4[]{1, 2, 3};
      std::uint8_t data5[]{4, 5};
      coap_te::const_buffer buf1[] = {coap_te::buffer(data1),
                                      coap_te::buffer(data2),
                                      coap_te::buffer(data3)};
      coap_te::const_buffer buf2[] = {coap_te::buffer(data4),
                                      coap_te::buffer(data5)};
      EXPECT_EQ(0, coap_te::buffer_compare(buf1, buf2));
    }
    {
      SCOPED_TRACE("Equal buffers max size");
      std::uint8_t data1[]{1};
      std::uint8_t data2[]{2, 3};
      std::uint8_t data3[]{4, 6};
      std::uint8_t data4[]{1, 2, 3};
      std::uint8_t data5[]{4, 5};
      coap_te::const_buffer buf1[] = {coap_te::buffer(data1),
                                      coap_te::buffer(data2),
                                      coap_te::buffer(data3)};
      coap_te::const_buffer buf2[] = {coap_te::buffer(data4),
                                      coap_te::buffer(data5)};
      EXPECT_EQ(0, coap_te::buffer_compare(buf1, buf2,
                                           coap_te::buffer_size(buf1) - 1));
    }
    {
      SCOPED_TRACE("Equal buffers different size 1");
      std::uint8_t data1[]{1};
      std::uint8_t data2[]{2, 3};
      std::uint8_t data3[]{4};
      std::uint8_t data4[]{1, 2, 3};
      std::uint8_t data5[]{4, 5};
      coap_te::const_buffer buf1[] = {coap_te::buffer(data1),
                                      coap_te::buffer(data2),
                                      coap_te::buffer(data3)};
      coap_te::const_buffer buf2[] = {coap_te::buffer(data4),
                                      coap_te::buffer(data5)};
      EXPECT_EQ(0, coap_te::buffer_compare(buf1, buf2));
    }
    {
      SCOPED_TRACE("Equal buffers different size 2");
      std::uint8_t data1[]{1};
      std::uint8_t data2[]{2, 3};
      std::uint8_t data3[]{4, 5};
      std::uint8_t data4[]{1, 2, 3};
      std::uint8_t data5[]{4};
      coap_te::const_buffer buf1[] = {coap_te::buffer(data1),
                                      coap_te::buffer(data2),
                                      coap_te::buffer(data3)};
      coap_te::const_buffer buf2[] = {coap_te::buffer(data4),
                                      coap_te::buffer(data5)};
      EXPECT_EQ(0, coap_te::buffer_compare(buf1, buf2));
    }
    {
      SCOPED_TRACE("Different buffers same sizes (1 < 2)");
      std::uint8_t data1[]{1};
      std::uint8_t data2[]{2, 2};
      std::uint8_t data3[]{4, 5};
      std::uint8_t data4[]{1, 2, 3};
      std::uint8_t data5[]{4, 5};
      coap_te::const_buffer buf1[] = {coap_te::buffer(data1),
                                      coap_te::buffer(data2),
                                      coap_te::buffer(data3)};
      coap_te::const_buffer buf2[] = {coap_te::buffer(data4),
                                      coap_te::buffer(data5)};
      EXPECT_GT(0, coap_te::buffer_compare(buf1, buf2));
    }
    {
      SCOPED_TRACE("Different buffers same sizes (1 > 2)");
      std::uint8_t data1[]{1};
      std::uint8_t data2[]{2, 4};
      std::uint8_t data3[]{4, 5};
      std::uint8_t data4[]{1, 2, 3};
      std::uint8_t data5[]{4, 5};
      coap_te::const_buffer buf1[] = {coap_te::buffer(data1),
                                      coap_te::buffer(data2),
                                      coap_te::buffer(data3)};
      coap_te::const_buffer buf2[] = {coap_te::buffer(data4),
                                      coap_te::buffer(data5)};
      EXPECT_LT(0, coap_te::buffer_compare(buf1, buf2));
    }
    {
      SCOPED_TRACE("Different buffers same sizes (1 < 2) 2");
      std::uint8_t data1[]{1};
      std::uint8_t data2[]{2, 3};
      std::uint8_t data3[]{4, 5};
      std::uint8_t data4[]{1, 2, 4};
      std::uint8_t data5[]{4, 5};
      coap_te::const_buffer buf1[] = {coap_te::buffer(data1),
                                      coap_te::buffer(data2),
                                      coap_te::buffer(data3)};
      coap_te::const_buffer buf2[] = {coap_te::buffer(data4),
                                      coap_te::buffer(data5)};
      EXPECT_GT(0, coap_te::buffer_compare(buf1, buf2));
    }
    {
      SCOPED_TRACE("Different buffers same sizes (1 > 2) 2");
      std::uint8_t data1[]{1};
      std::uint8_t data2[]{2, 3};
      std::uint8_t data3[]{4, 5};
      std::uint8_t data4[]{1, 2, 2};
      std::uint8_t data5[]{4, 5};
      coap_te::const_buffer buf1[] = {coap_te::buffer(data1),
                                      coap_te::buffer(data2),
                                      coap_te::buffer(data3)};
      coap_te::const_buffer buf2[] = {coap_te::buffer(data4),
                                      coap_te::buffer(data5)};
      EXPECT_LT(0, coap_te::buffer_compare(buf1, buf2));
    }
  }
}

TEST(Buffer, BufferIterator) {
  {
    SCOPED_TRACE("Buffer iterator increment and deference");
    unsigned char d1[] = {0, 1, 2, 3, 4, 5},
                  d2[] = {6, 7, 8, 9, 10};
    std::vector<coap_te::const_buffer> v{coap_te::buffer(d1),
                                         coap_te::buffer(d2)};

    auto b = coap_te::buffers_begin(v);
    auto e = coap_te::buffers_end(v);
    for (char i = 0; b != e; ++i, ++b) {
      EXPECT_EQ(*b, i);
    }
  }
  {
    SCOPED_TRACE("Buffer iterator decrement and deference");
    unsigned char d1[] = {0, 1, 2, 3, 4, 5},
                  d2[] = {6, 7, 8, 9, 10};
    std::vector<coap_te::const_buffer> v;
    v.push_back(coap_te::buffer(d1));
    v.push_back(coap_te::buffer(d2));

    auto b = coap_te::buffers_begin(v);
    auto e = coap_te::buffers_end(v) - 1;
    for (char i = 10; e >= b; --i, --e) {
      EXPECT_EQ(*e, i);
    }
  }
  {
    SCOPED_TRACE("Testing iterator data updates");
    unsigned char d1[5],
                  d2[5] = {5, 6, 7, 8, 9};
    std::vector<coap_te::mutable_buffer> v;
    v.push_back(coap_te::buffer(d1));
    v.push_back(coap_te::buffer(d2));

    auto b = coap_te::buffers_begin(v);
    *b = 0;
    b[1] = 1;
    *(b + 2) = 2;
    {
      auto b2 = b + 3;
      *b2 = 3;
      b2 += 1;
      b2[0] = 4;
    }
    auto e = coap_te::buffers_end(v);

    for (char i = 0; b != e; ++i, ++b) {
      EXPECT_EQ(*b, i);
    }
  }
}

TEST(Buffer, SingleBufferIterator) {
  {
    SCOPED_TRACE("Single buffer");
    unsigned char d1[] = {0, 1, 2, 3, 4, 5};

    auto b = coap_te::buffers_begin(coap_te::buffer(d1));
    auto e = coap_te::buffers_end(coap_te::buffer(d1));
    for (char i = 0; b != e; ++i, ++b) {
      EXPECT_EQ(*b, i);
    }
  }
  {
    SCOPED_TRACE("Constant single buffer");
    const unsigned char d1[] = {0, 1, 2, 3, 4, 5};

    auto b = coap_te::buffers_begin(coap_te::buffer(d1));
    auto e = coap_te::buffers_end(coap_te::buffer(d1));
    for (char i = 0; b != e; ++i, ++b) {
      EXPECT_EQ(*b, i);
    }
  }
  {
    SCOPED_TRACE("Testing iterator data updates");
    unsigned char d[5];
    auto b = coap_te::buffers_begin(coap_te::buffer(d));
    *b = 0;
    b[1] = 1;
    *(b + 2) = 2;
    {
      auto b2 = b + 3;
      *b2 = 3;
      b2 += 1;
      b2[0] = 4;
    }
    auto e = coap_te::buffers_end(coap_te::buffer(d));

    for (char i = 0; b != e; ++i, ++b) {
      EXPECT_EQ(*b, i);
    }
  }
}

TEST(Buffer, IteratorContainer) {
  std::uint8_t data1[]{0, 1, 2, 3, 4};
  std::uint8_t data2[]{5, 6, 7, 8, 9};
  {
    auto buf = coap_te::buffer(data1);
    EXPECT_TRUE(coap_te::is_mutable_buffer_sequence_v<decltype(buf)>);
    coap_te::iterator_container ic(coap_te::buffers_begin(buf),
                                   coap_te::buffers_end(buf));
    EXPECT_EQ(sizeof(data1), ic.size());
    EXPECT_EQ(sizeof(data1), coap_te::buffer_size(ic));
    EXPECT_FALSE(decltype(ic)::is_multiple);
  }
  {
    std::vector<coap_te::const_buffer> v{coap_te::buffer(data1),
                                         coap_te::buffer(data2)};
    EXPECT_FALSE(coap_te::is_mutable_buffer_sequence_v<decltype(v)>);
    EXPECT_TRUE(coap_te::is_const_buffer_sequence_v<decltype(v)>);
    coap_te::iterator_container ic(coap_te::buffers_begin(v),
                                 coap_te::buffers_end(v));
    EXPECT_EQ(sizeof(data1) + sizeof(data2), ic.size());
    EXPECT_EQ(sizeof(data1) + sizeof(data2), coap_te::buffer_size(ic));
    EXPECT_TRUE(decltype(ic)::is_multiple);
  }
  {
    auto buf = coap_te::buffer(data1);
    coap_te::iterator_container ic(coap_te::buffers_begin(buf),
                                  coap_te::buffers_end(buf));
    EXPECT_EQ(sizeof(data1), ic.size());
    EXPECT_EQ(coap_te::buffer_size(ic), ic.size());
    EXPECT_FALSE(decltype(ic)::is_multiple);
  }
  {
    coap_te::const_buffer buf[] = {coap_te::buffer(data1),
                                   coap_te::buffer(data2)};
    EXPECT_TRUE(coap_te::is_const_buffer_sequence_v<decltype(buf)>);
    coap_te::iterator_container ic(coap_te::buffers_begin(buf),
                                  coap_te::buffers_end(buf));
    EXPECT_EQ(sizeof(data1) + sizeof(data2), ic.size());
    EXPECT_EQ(sizeof(data1) + sizeof(data2), coap_te::buffer_size(ic));
    {
      int c = 0;
      for (auto i = ic.begin(); i != ic.end(); ++i) {
        EXPECT_EQ(*i, c++);
      }
    }
    {
      for (std::size_t c = 0; c < ic.size(); ++c) {
        EXPECT_EQ(c, ic[c]);
      }
    }
  }
  {
    std::vector<coap_te::const_buffer> buf{coap_te::buffer(data1),
                                            coap_te::buffer(data2)};
    EXPECT_TRUE(coap_te::is_const_buffer_sequence_v<decltype(buf)>);
    coap_te::iterator_container ic(buf);
    EXPECT_EQ(sizeof(data1) + sizeof(data2), ic.size());
    EXPECT_EQ(sizeof(data1) + sizeof(data2), coap_te::buffer_size(ic));
    {
      int c = 0;
      for (auto i = ic.begin(); i != ic.end(); ++i) {
        EXPECT_EQ(*i, c++);
      }
    }
    {
      for (std::size_t c = 0; c < ic.size(); ++c) {
        EXPECT_EQ(c, ic[c]);
      }
    }
  }
  {
    auto buf1 = coap_te::buffer(data1);
    buf1 += 2;
    std::vector<coap_te::const_buffer> buf{buf1,
                                            coap_te::buffer(data2)};
    EXPECT_TRUE(coap_te::is_const_buffer_sequence_v<decltype(buf)>);
    coap_te::iterator_container ic(buf);
    EXPECT_EQ(sizeof(data1) + sizeof(data2) - 2, ic.size());
    EXPECT_EQ(sizeof(data1) + sizeof(data2) - 2, coap_te::buffer_size(ic));
    {
      int c = 2;
      for (auto i = ic.begin(); i != ic.end(); ++i) {
        EXPECT_EQ(*i, c++);
      }
    }
    {
      for (std::size_t c = 2; c < ic.size(); ++c) {
        EXPECT_EQ(c, ic[c - 2]);
      }
    }
    //////////////////////////////////////////////
    // {
    //   auto buf1 = coap_te::buffer(data1);
    //   buf1 += 2;
    //   std::vector<coap_te::const_buffer> buf{buf1,
    //                                           coap_te::buffer(data2)};
    //   EXPECT_TRUE(coap_te::is_const_buffer_sequence_v<decltype(buf)>);
    //   coap_te::iterator_container ic(buf);
    //   coap_te::iterator_container ic2(buf);
    //   coap_te::buffer_copy(ic, ic2);
    // }
    // {
    //   auto buf1 = coap_te::buffer(data1);
    //   std::vector<coap_te::const_buffer> buf{buf1,
    //                                           coap_te::buffer(data2)};
    //   EXPECT_TRUE(coap_te::is_const_buffer_sequence_v<decltype(buf)>);
    //   coap_te::iterator_container ic(buf);
    //   coap_te::iterator_container ic2(buf1);
    //   coap_te::buffer_copy(ic, ic2);
    // }
    {
      std::uint8_t dataa[]{0, 1, 2, 3, 4};
      std::uint8_t datab[]{5, 6, 7, 8, 9};
      std::uint8_t datac[5];
      std::uint8_t datad[5];

      std::vector<coap_te::const_buffer> ssource{
        coap_te::buffer(dataa),
        coap_te::buffer(datab)
      };
      std::vector<coap_te::mutable_buffer> starget{
        coap_te::buffer(datac),
        coap_te::buffer(datad)
      };
      coap_te::iterator_container source(ssource);
      coap_te::iterator_container target(starget);
      auto size = coap_te::buffer_copy(target, source);
      EXPECT_EQ(size, 10);
      EXPECT_EQ(0, coap_te::buffer_compare(starget, ssource));
    }
  }
}
