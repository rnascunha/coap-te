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

#include "coap-te/buffer/mutable_buffer.hpp"
#include "coap-te/buffer/const_buffer.hpp"
#include "coap-te/buffer/buffer.hpp"
#include "coap-te/buffer/buffers_iterator.hpp"
#include "coap-te/buffer/traits.hpp"

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
}
