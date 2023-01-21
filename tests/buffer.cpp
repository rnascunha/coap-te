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
#include "coap-te/buffer/buffer_range.hpp"

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
      const char v[]{1, 2, 3, 4, 5, '\0'};
      auto buf = coap_te::buffer(v);
      // EXPECT_EQ(buf.size(), sizeof(v) / sizeof(v[0]));
      EXPECT_EQ(buf.size(), 5);
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

TEST(Buffer, BufferRange) {
  std::uint8_t data1[]{0, 1, 2, 3, 4};
  std::uint8_t data2[]{5, 6, 7, 8, 9};
  {
    auto buf = coap_te::buffer(data1);
    EXPECT_TRUE(coap_te::is_mutable_buffer_sequence_v<decltype(buf)>);
    coap_te::buffer_range ic(coap_te::buffers_begin(buf),
                             coap_te::buffers_end(buf));
    EXPECT_FALSE(decltype(ic)::is_multiple);
    EXPECT_EQ(sizeof(data1), ic.size());
    EXPECT_EQ(sizeof(data1), coap_te::buffer_size(ic));
  }
  {
    std::vector<coap_te::const_buffer> v{coap_te::buffer(data1),
                                         coap_te::buffer(data2)};
    EXPECT_FALSE(coap_te::is_mutable_buffer_sequence_v<decltype(v)>);
    EXPECT_TRUE(coap_te::is_const_buffer_sequence_v<decltype(v)>);
    coap_te::buffer_range ic(coap_te::buffers_begin(v),
                                 coap_te::buffers_end(v));
    EXPECT_TRUE(decltype(ic)::is_multiple);
    EXPECT_EQ(sizeof(data1) + sizeof(data2), ic.size());
    EXPECT_EQ(sizeof(data1) + sizeof(data2), coap_te::buffer_size(ic));
  }
  {
    auto buf = coap_te::buffer(data1);
    coap_te::buffer_range ic(coap_te::buffers_begin(buf),
                                  coap_te::buffers_end(buf));
    EXPECT_FALSE(decltype(ic)::is_multiple);
    EXPECT_EQ(sizeof(data1), ic.size());
    EXPECT_EQ(coap_te::buffer_size(ic), ic.size());
  }
  {
    coap_te::const_buffer buf[] = {coap_te::buffer(data1),
                                   coap_te::buffer(data2)};
    EXPECT_TRUE(coap_te::is_const_buffer_sequence_v<decltype(buf)>);
    coap_te::buffer_range ic(coap_te::buffers_begin(buf),
                                  coap_te::buffers_end(buf));
    EXPECT_TRUE(decltype(ic)::is_multiple);
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
    coap_te::buffer_range ic(buf);
    EXPECT_TRUE(decltype(ic)::is_multiple);
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
    std::vector<coap_te::const_buffer> buff{buf1,
                                          coap_te::buffer(data2)};
    EXPECT_TRUE(coap_te::is_const_buffer_sequence_v<decltype(buff)>);
    coap_te::buffer_range icc(buff);
    EXPECT_TRUE(decltype(icc)::is_multiple);
    EXPECT_EQ(sizeof(data1) + sizeof(data2) - 2, icc.size());
    EXPECT_EQ(sizeof(data1) + sizeof(data2) - 2, coap_te::buffer_size(icc));
    {
      int c = 2;
      for (auto i = icc.begin(); i != icc.end(); ++i) {
        EXPECT_EQ(*i, c++);
      }
    }
    {
      for (std::size_t c = 2; c < icc.size(); ++c) {
        EXPECT_EQ(c, icc[c - 2]);
      }
    }
    //////////////////////////////////////////////
    {
      auto buff1 = coap_te::buffer(data1);
      buff1 += 2;
      std::vector<coap_te::mutable_buffer> buff{buff1,
                                              coap_te::buffer(data2)};
      EXPECT_TRUE(coap_te::is_const_buffer_sequence_v<decltype(buff)>);
      coap_te::buffer_range ic(buff);
      coap_te::buffer_range ic2(buff);
      EXPECT_TRUE(decltype(ic)::is_multiple);
      EXPECT_TRUE(decltype(ic2)::is_multiple);
      coap_te::buffer_copy(ic, ic2);
    }
    {
      auto buff1 = coap_te::buffer(data1);
      std::vector<coap_te::mutable_buffer> buff{buff1,
                                              coap_te::buffer(data2)};
      EXPECT_TRUE(coap_te::is_const_buffer_sequence_v<decltype(buff)>);
      coap_te::buffer_range ic0(buff);
      coap_te::buffer_range ic2(buff1);
      EXPECT_TRUE(decltype(ic0)::is_multiple);
      EXPECT_FALSE(decltype(ic2)::is_multiple);
      coap_te::buffer_copy(ic0, ic2);
    }
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
      coap_te::buffer_range source(ssource);
      coap_te::buffer_range target(starget);
      EXPECT_TRUE(decltype(source)::is_multiple);
      EXPECT_TRUE(decltype(target)::is_multiple);
      auto size = coap_te::buffer_copy(target, source);
      EXPECT_EQ(size, 10);
      EXPECT_EQ(0, coap_te::buffer_compare(starget, ssource));
    }
  }
}

/**
 * The above tests are the same of example buffer_ex
 */

template<typename ConstBufferSequence1,
         typename ConstBufferSequence2>
void compare_buffer(const ConstBufferSequence1& buffer1,
                    const ConstBufferSequence2& buffer2,
                    int expect) {
  auto res = coap_te::buffer_compare(buffer1, buffer2);
  if (expect == 0)
    EXPECT_EQ(res, 0);
  else if (expect < 0)
    EXPECT_LT(res, 0);
  else
    EXPECT_GT(res, 0);
}

template<typename ConstBufferSequence>
void compare_all_buffers(const ConstBufferSequence& buffers) {
  for (std::size_t i = 0; i < buffers.size(); ++i) {
    for (std::size_t j = 0; j < buffers.size(); ++j) {
      compare_buffer(buffers[i], buffers[j], i == j ? 0 : (i < j ? -1 : 1));
    }
  }
}

template<typename MutableBufferSequence,
         typename ConstBufferSequence>
void copy_buffer_check(const MutableBufferSequence& target,
                 const ConstBufferSequence& source,
                 std::size_t max_size
                  = std::numeric_limits<std::size_t>::max()) noexcept {
  auto tsize = coap_te::buffer_size(target);
  auto ssize = coap_te::buffer_size(source);
  auto rsize = coap_te::buffer_copy(target, source, max_size);
  auto should_size = (std::min)({tsize, ssize, max_size});

  EXPECT_EQ(should_size , rsize);
  EXPECT_EQ(coap_te::buffer_compare(target, source), 0);
}

TEST(Buffer, BufferExample) {
  static constexpr std::uint8_t data0[] = {0, 1, 2, 3, 4};
  static constexpr std::uint8_t data1[] = {5, 6, 7, 8, 9};
  static constexpr std::uint8_t data2[] = {10, 11, 12, 13, 14};
  static constexpr std::uint8_t data3[] = {15, 16, 17};
  static constexpr std::uint8_t data4[] = {18, 19};
  static constexpr std::uint8_t data5[] = {20, 21, 22, 23};
  static constexpr std::uint8_t data6[] = {24};

  coap_te::const_buffer buf0 = coap_te::buffer(data0);
  coap_te::const_buffer buf1 = coap_te::buffer(data1);
  coap_te::const_buffer buf2 = coap_te::buffer(data2);
  coap_te::const_buffer buf3 = coap_te::buffer(data3);
  coap_te::const_buffer buf4 = coap_te::buffer(data4);
  coap_te::const_buffer buf5 = coap_te::buffer(data5);
  coap_te::const_buffer buf6 = coap_te::buffer(data6);

  std::array<coap_te::const_buffer, 7> sbuffers = {
    buf0, buf1, buf2, buf3, buf4, buf5, buf6
  };

  std::vector<coap_te::const_buffer> buf00 = {buf0};
  std::vector<coap_te::const_buffer> buf01 = {buf0, buf1};
  std::vector<coap_te::const_buffer> buf02 = {buf0, buf2};
  std::vector<coap_te::const_buffer> buf12 = {buf1, buf2};
  std::vector<coap_te::const_buffer> buf012 = {buf0, buf1, buf2};
  std::vector<coap_te::const_buffer> buf123 = {buf1, buf2, buf3};
  std::vector<coap_te::const_buffer> buf456 = {buf4, buf5, buf6};
  std::vector<coap_te::const_buffer> buf136 = {buf1, buf3, buf6};
  std::vector<coap_te::const_buffer> buf0123456
    = {buf0, buf1, buf2, buf3, buf4, buf5, buf6};

  std::vector<std::vector<coap_te::const_buffer>> mbuffers = {
    buf00, buf01, buf02, buf12,
    buf012, buf123, buf456, buf136,
    buf0123456
  };

  coap_te::buffer_range ic0(buf01);
  coap_te::buffer_range ic1(coap_te::buffers_begin(buf01) + 2,
                                  coap_te::buffers_end(buf01));
  coap_te::buffer_range ic2(coap_te::buffers_begin(buf01) + 2,
                                  coap_te::buffers_end(buf01) - 2);
  coap_te::buffer_range ic3(coap_te::buffers_begin(buf0123456) + 5,
                                  coap_te::buffers_end(buf0123456) - 10);
  EXPECT_TRUE(decltype(ic0)::is_multiple);
  EXPECT_TRUE(decltype(ic1)::is_multiple);
  EXPECT_TRUE(decltype(ic2)::is_multiple);
  EXPECT_TRUE(decltype(ic3)::is_multiple);

  {
    SCOPED_TRACE("Single buffers");
    compare_all_buffers(sbuffers);
    {
      SCOPED_TRACE("same size");
      std::uint8_t data[10];
      copy_buffer_check(coap_te::buffer(data), sbuffers[2]);
    }
    {
      SCOPED_TRACE("bigger target");
      std::vector<std::uint8_t> data(10);
      copy_buffer_check(coap_te::buffer(data), sbuffers[0]);
    }
    {
      SCOPED_TRACE("smaller target");
      std::array<std::uint8_t, 3> data;
      copy_buffer_check(coap_te::buffer(data), sbuffers[1]);
    }
  }
  {
    SCOPED_TRACE("Multiple buffers");
    compare_buffer(buf00, buf00, 0);
    compare_buffer(buf00, buf01, 0);
    compare_buffer(buf00, buf02, 0);
    compare_buffer(buf00, buf012, 0);
    compare_buffer(buf00, buf0123456, 0);
    compare_buffer(buf12, buf01, 1);
    compare_buffer(buf136, buf0123456, 1);
    compare_buffer(buf0123456, buf0123456, 0);
    {
      SCOPED_TRACE("multiple source, single target");
      {
        SCOPED_TRACE("same size");
        std::array<std::uint8_t, 10> arr;
        copy_buffer_check(coap_te::buffer(arr), mbuffers[1]);
      }
      {
        SCOPED_TRACE("source bigger size");
        std::vector<std::uint8_t> arr(8);
        copy_buffer_check(coap_te::buffer(arr), mbuffers[2]);
      }
      {
        SCOPED_TRACE("target bigger size");
        std::uint8_t arr[12];
        copy_buffer_check(coap_te::buffer(arr), mbuffers[3]);
      }
    }
    {
      SCOPED_TRACE("single source, multiple target");
      {
        SCOPED_TRACE("same size");
        std::array<std::uint8_t, 3> arr0;
        std::uint8_t arr1[2];
        std::vector<coap_te::mutable_buffer> data_buf{coap_te::buffer(arr0),
                                                  coap_te::buffer(arr1)};
        copy_buffer_check(data_buf, sbuffers[1]);
      }
      {
        SCOPED_TRACE("source bigger");
        std::array<std::uint8_t, 1> arr0;
        std::uint8_t arr1[2];
        std::array<coap_te::mutable_buffer, 2> data_buf{coap_te::buffer(arr0),
                                                  coap_te::buffer(arr1)};
        copy_buffer_check(data_buf, sbuffers[2]);
      }
      {
        SCOPED_TRACE("target bigger");
        std::vector<std::uint8_t> arr0(3);
        std::uint8_t arr1[2];
        std::array<std::uint8_t, 4> arr2;
        coap_te::mutable_buffer data_buf[]{coap_te::buffer(arr0),
                                                  coap_te::buffer(arr1),
                                                  coap_te::buffer(arr2)};
        copy_buffer_check(data_buf, sbuffers[2]);
      }
    }
    {
      SCOPED_TRACE("multiple buffers, multiple sources");
      {
        SCOPED_TRACE("same size");
        std::array<std::uint8_t, 6> arr0;
        std::uint8_t arr1[7];
        std::vector<std::uint8_t> arr2(2);
        std::array<coap_te::mutable_buffer, 3> arr{
          coap_te::buffer(arr0),
          coap_te::buffer(arr1),
          coap_te::buffer(arr2)
        };
        copy_buffer_check(coap_te::buffer(arr), mbuffers[4]);
      }
      {
        SCOPED_TRACE("bigger source");
        std::array<std::uint8_t, 3> arr0;
        std::uint8_t arr1[10];
        std::vector<std::uint8_t> arr2(8);
        std::array<coap_te::mutable_buffer, 3> arr{
          coap_te::buffer(arr0),
          coap_te::buffer(arr1),
          coap_te::buffer(arr2)
        };
        copy_buffer_check(arr, mbuffers[8]);
      }
      {
        SCOPED_TRACE("bigger target");
        std::array<std::uint8_t, 4> arr0;
        std::uint8_t arr1[10];
        std::vector<std::uint8_t> arr2(12);
        coap_te::mutable_buffer arr[]{
          coap_te::buffer(arr0),
          coap_te::buffer(arr1),
          coap_te::buffer(arr2)
        };
        copy_buffer_check(arr, mbuffers[8]);
      }
    }
  }
  {
    SCOPED_TRACE("Buffer ranges");
    compare_buffer(ic0, ic1, -1);
    compare_buffer(ic0, ic2, -1);
    compare_buffer(ic1, ic2, 0);
    {
      std::uint8_t data[]{2, 3, 4};
      compare_buffer(ic2, coap_te::buffer(data), 0);
    }
    {
      SCOPED_TRACE("From iterator container");
      {
        SCOPED_TRACE("single buffer, same size");
        std::uint8_t data[6];
        copy_buffer_check(coap_te::buffer(data), ic2);
      }
      {
        SCOPED_TRACE("single buffer, buffer small");
        std::uint8_t data[7];
        copy_buffer_check(coap_te::buffer(data), ic0);
      }
      {
        SCOPED_TRACE("single buffer, buffer big");
        std::uint8_t data[12];
        copy_buffer_check(coap_te::buffer(data), ic3);
      }
      {
        SCOPED_TRACE("multiple buffer, small buffer");
        std::uint8_t data_1[3];
        std::uint8_t data_2[2];
        std::uint8_t data_3[2];
        std::array<coap_te::mutable_buffer, 3> buf{ coap_te::buffer(data_1),
                                        coap_te::buffer(data_2),
                                        coap_te::buffer(data_3)};
        copy_buffer_check(buf, ic1);
      }
      {
        SCOPED_TRACE("multiple buffer, big buffer");
        std::uint8_t data_1[3];
        std::uint8_t data_2[3];
        std::uint8_t data_3[5];
        std::array<coap_te::mutable_buffer, 3> buf{ coap_te::buffer(data_1),
                                        coap_te::buffer(data_2),
                                        coap_te::buffer(data_3)};
        copy_buffer_check(buf, ic1);
      }
    }
    {
      SCOPED_TRACE("To iterator container");
      {
        SCOPED_TRACE("single buffer, same size");
        std::uint8_t data_1[3];
        std::uint8_t data_2[2];
        std::vector<coap_te::mutable_buffer> buf{coap_te::buffer(data_1),
                                                coap_te::buffer(data_2)};
        copy_buffer_check(coap_te::buffer_range(buf),
                          buf0);
      }
      {
        SCOPED_TRACE("single buffer, buffer small");
        std::uint8_t data_1[2];
        std::uint8_t data_2[1];
        std::uint8_t data_3[4];
        std::array<coap_te::mutable_buffer, 3> buf{coap_te::buffer(data_1),
                                                coap_te::buffer(data_2),
                                                coap_te::buffer(data_3)};
        copy_buffer_check(coap_te::buffer_range(buf),
                          buf1);
      }
      {
        SCOPED_TRACE("single buffer, buffer big");
        std::uint8_t data_1[2];
        std::uint8_t data_2[1];
        std::array<coap_te::mutable_buffer, 3> buf{coap_te::buffer(data_1),
                                                coap_te::buffer(data_2)};
        copy_buffer_check(coap_te::buffer_range(buf),
                          buf5);
      }
      {
        SCOPED_TRACE("multiple buffer, same size");
        std::uint8_t data_1[2];
        std::uint8_t data_2[4];
        std::uint8_t data_3[4];
        coap_te::mutable_buffer buf[]{coap_te::buffer(data_1),
                                      coap_te::buffer(data_2),
                                      coap_te::buffer(data_3)};
        copy_buffer_check(coap_te::buffer_range(buf),
                          buf01);
      }
      {
        SCOPED_TRACE("multiple buffer, buffer small");
        std::uint8_t data_1[2];
        std::uint8_t data_2[4];
        std::array<coap_te::mutable_buffer, 2> buf{coap_te::buffer(data_1),
                                                coap_te::buffer(data_2)};
        copy_buffer_check(coap_te::buffer_range(buf),
                          buf00);
      }
      {
        SCOPED_TRACE("multiple buffer, buffer big");
        std::uint8_t data_1[6];
        std::uint8_t data_2[4];
        std::uint8_t data_3[7];
        std::vector<coap_te::mutable_buffer> buf{coap_te::buffer(data_1),
                                                coap_te::buffer(data_2),
                                                coap_te::buffer(data_3)};
        copy_buffer_check(coap_te::buffer_range(buf),
                            buf0123456);
      }
    }
    {
      SCOPED_TRACE("From iterator container to iterator container");
      {
        SCOPED_TRACE("same size");
        std::uint8_t data_0[3];
        std::array<std::uint8_t, 5> data_1;
        std::vector<std::uint8_t> data_2(2);
        std::vector<coap_te::mutable_buffer> buf{
          coap_te::buffer(data_0),
          coap_te::buffer(data_1),
          coap_te::buffer(data_2)
        };
        coap_te::buffer_range icm(buf);
        copy_buffer_check(icm, ic3);
      }
      {
        SCOPED_TRACE("small target");
        std::uint8_t data_0[2];
        std::array<std::uint8_t, 3> data_1;
        std::vector<std::uint8_t> data_2(2);
        std::vector<coap_te::mutable_buffer> buf{
          coap_te::buffer(data_0),
          coap_te::buffer(data_1),
          coap_te::buffer(data_2)
        };
        coap_te::buffer_range icm(buf);
        copy_buffer_check(icm, ic1);
      }
      {
        SCOPED_TRACE("bigger target");
        std::uint8_t data_0[5];
        std::array<std::uint8_t, 3> data_1;
        std::vector<std::uint8_t> data_2(7);
        coap_te::mutable_buffer buf[]{
          coap_te::buffer(data_0),
          coap_te::buffer(data_1),
          coap_te::buffer(data_2)
        };
        coap_te::buffer_range icm(buf);
        copy_buffer_check(icm, ic3);
      }
    }
  }
}
