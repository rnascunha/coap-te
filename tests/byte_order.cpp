/**
 * @file byte_order.cpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-11-28
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <gtest/gtest.h>

#include <cstdint>

#include "coap-te/core/byte_order.hpp"    // NOLINT

TEST(CoreByteOrder, UnsignedShortNetworkByteOrder) {
  // 0
  {
    std::uint8_t u = 0u;
    EXPECT_EQ(coap_te::core::to_big_endian(u), 0);
    EXPECT_EQ(u, 0);
  }
  {
    std::uint16_t u = 0u;
    EXPECT_EQ(coap_te::core::to_big_endian(u), 0);
    EXPECT_EQ(u, 0);
  }
  {
    std::uint32_t u = 0u;
    EXPECT_EQ(coap_te::core::to_big_endian(u), 0);
    EXPECT_EQ(u, 0);
  }
  {
    std::uint64_t u = 0u;
    EXPECT_EQ(coap_te::core::to_big_endian(u), 0);
    EXPECT_EQ(u, 0);
  }
  {
    unsigned u = 0u;
    EXPECT_EQ(coap_te::core::to_big_endian(u), 0);
    EXPECT_EQ(u, 0);
  }

  // 1
  {
    std::uint8_t u = 1u;
    EXPECT_EQ(coap_te::core::to_big_endian(u), 1);
    EXPECT_EQ(u, 1u);
  }
  {
    std::uint16_t u = 1u;
    EXPECT_EQ(coap_te::core::to_big_endian(u), 1);
    EXPECT_EQ(u, 0x0100);
  }
  {
    std::uint32_t u = 1u;
    EXPECT_EQ(coap_te::core::to_big_endian(u), 1);
    EXPECT_EQ(u, 0x0100'0000);
  }
  {
    std::uint64_t u = 1u;
    EXPECT_EQ(coap_te::core::to_big_endian(u), 1);
    EXPECT_EQ(u, 0x0100'0000'0000'0000);
  }
  {
    unsigned u = 1u;
    EXPECT_EQ(coap_te::core::to_big_endian(u), 1);
    EXPECT_EQ(u, 0x0100'0000);
  }

  // 255
  {
    std::uint8_t u = 255u;
    EXPECT_EQ(coap_te::core::to_big_endian(u), 1);
    EXPECT_EQ(u, 255);
  }
  {
    std::uint16_t u = 255u;
    EXPECT_EQ(coap_te::core::to_big_endian(u), 1);
    EXPECT_EQ(u, 0xFF00);
  }
  {
    std::uint32_t u = 255u;
    EXPECT_EQ(coap_te::core::to_big_endian(u), 1);
    EXPECT_EQ(u, 0xFF00'0000);
  }
  {
    std::uint64_t u = 255u;
    EXPECT_EQ(coap_te::core::to_big_endian(u), 1);
    EXPECT_EQ(u, 0xFF00'0000'0000'0000);
  }
  {
    unsigned u = 255u;
    EXPECT_EQ(coap_te::core::to_big_endian(u), 1);
    EXPECT_EQ(u, 0xFF00'0000);
  }

  // 256
  {
    std::uint16_t u = 256u;
    EXPECT_EQ(coap_te::core::to_big_endian(u), 2);
    EXPECT_EQ(u, 0x0001);
  }
  {
    std::uint32_t u = 256u;
    EXPECT_EQ(coap_te::core::to_big_endian(u), 2);
    EXPECT_EQ(u, 0x0001'0000);
  }
  {
    std::uint64_t u = 256u;
    EXPECT_EQ(coap_te::core::to_big_endian(u), 2);
    EXPECT_EQ(u, 0x0001'0000'0000'0000);
  }
  {
    unsigned u = 256u;
    EXPECT_EQ(coap_te::core::to_big_endian(u), 2);
    EXPECT_EQ(u, 0x0001'0000);
  }

  // 65535
  {
    std::uint16_t u = 65535u;
    EXPECT_EQ(coap_te::core::to_big_endian(u), 2);
    EXPECT_EQ(u, 0xFFFF);
  }
  {
    std::uint32_t u = 65535u;
    EXPECT_EQ(coap_te::core::to_big_endian(u), 2);
    EXPECT_EQ(u, 0xFFFF'0000);
  }
  {
    std::uint64_t u = 65535u;
    EXPECT_EQ(coap_te::core::to_big_endian(u), 2);
    EXPECT_EQ(u, 0xFFFF'0000'0000'0000);
  }
  {
    unsigned u = 65535u;
    EXPECT_EQ(coap_te::core::to_big_endian(u), 2);
    EXPECT_EQ(u, 0xFFFF'0000);
  }

  // 16777215
  {
    std::uint32_t u = 16777215u;
    EXPECT_EQ(coap_te::core::to_big_endian(u), 3);
    EXPECT_EQ(u, 0xFFFF'FF00);
  }
  {
    std::uint64_t u = 16777215u;
    EXPECT_EQ(coap_te::core::to_big_endian(u), 3);
    EXPECT_EQ(u, 0xFFFF'FF00'0000'0000);
  }
  {
    unsigned u = 16777215u;
    EXPECT_EQ(coap_te::core::to_big_endian(u), 3);
    EXPECT_EQ(u, 0xFFFF'FF00);
  }

  // 16777216
  {
    std::uint32_t u = 16777216u;
    EXPECT_EQ(coap_te::core::to_big_endian(u), 4);
    EXPECT_EQ(u, 0x0000'0001);
  }
  {
    std::uint64_t u = 16777216u;
    EXPECT_EQ(coap_te::core::to_big_endian(u), 4);
    EXPECT_EQ(u, 0x0000'0001'0000'0000);
  }
  {
    unsigned u = 16777216u;
    EXPECT_EQ(coap_te::core::to_big_endian(u), 4);
    EXPECT_EQ(u, 0x0000'0001);
  }

  // 4294967295
  {
    std::uint32_t u = 4294967295u;
    EXPECT_EQ(coap_te::core::to_big_endian(u), 4);
    EXPECT_EQ(u, 0xFFFF'FFFF);
  }
  {
    std::uint64_t u = 4294967295u;
    EXPECT_EQ(coap_te::core::to_big_endian(u), 4);
    EXPECT_EQ(u, 0xFFFF'FFFF'0000'0000);
  }
  {
    unsigned u = 4294967295u;
    EXPECT_EQ(coap_te::core::to_big_endian(u), 4);
    EXPECT_EQ(u, 0xFFFF'FFFF);
  }

  // 4294967296
  {
    std::uint64_t u = 4294967296u;
    EXPECT_EQ(coap_te::core::to_big_endian(u), 5);
    EXPECT_EQ(u, 0x0000'0000'0100'0000);
  }

  // 4294967296
  {
    std::uint64_t u = 1099511627775u;
    EXPECT_EQ(coap_te::core::to_big_endian(u), 5);
    EXPECT_EQ(u, 0xFFFF'FFFF'FF00'0000);
  }

  // 4294967297
  {
    std::uint64_t u = 1099511627776u;
    EXPECT_EQ(coap_te::core::to_big_endian(u), 6);
    EXPECT_EQ(u, 0x0000'0000'0001'0000);
  }

  // 281474976710655
  {
    std::uint64_t u = 281474976710655u;
    EXPECT_EQ(coap_te::core::to_big_endian(u), 6);
    EXPECT_EQ(u, 0xFFFF'FFFF'FFFF'0000);
  }

  // 281474976710656
  {
    std::uint64_t u = 281474976710656u;
    EXPECT_EQ(coap_te::core::to_big_endian(u), 7);
    EXPECT_EQ(u, 0x0000'0000'0000'0100);
  }

  // 72057594037927935
  {
    std::uint64_t u = 72057594037927935u;
    EXPECT_EQ(coap_te::core::to_big_endian(u), 7);
    EXPECT_EQ(u, 0xFFFF'FFFF'FFFF'FF00);
  }

  // 72057594037927935
  {
    std::uint64_t u = 72057594037927936u;
    EXPECT_EQ(coap_te::core::to_big_endian(u), 8);
    EXPECT_EQ(u, 0x0000'0000'0000'0001);
  }

  // 18446744073709551615
  {
    std::uint64_t u = 18446744073709551615u;
    EXPECT_EQ(coap_te::core::to_big_endian(u), 8);
    EXPECT_EQ(u, 0xFFFF'FFFF'FFFF'FFFF);
  }
}

template<typename UnsignedType>
void test_byte_order_copy_impl(UnsignedType value, UnsignedType expected) {
  std::uint8_t buf[sizeof(UnsignedType)];
  coap_te::core::to_big_endian(value, buf);
  coap_te::core::to_big_endian(expected);
  EXPECT_EQ(*reinterpret_cast<UnsignedType*>(buf), expected);
}

template<typename T, typename ...Ts>
void test_byte_order_copy(T value, T expected) {
  test_byte_order_copy_impl<T>(value, expected);
  if (sizeof...(Ts) > 0)
    test_byte_order_copy<Ts...>(value, expected);
}

TEST(CoreByteOrder, UnsignedNetworkByteOrderToBuffer) {
  test_byte_order_copy<std::uint8_t,
                       std::uint16_t,
                       std::uint32_t,
                       std::uint64_t,
                       unsigned>(0u, 0u);

  test_byte_order_copy<std::uint8_t,
                       std::uint16_t,
                       std::uint32_t,
                       std::uint64_t,
                       unsigned>(1u, 1u);

  test_byte_order_copy<std::uint8_t,
                       std::uint16_t,
                       std::uint32_t,
                       std::uint64_t,
                       unsigned>(255u, 255u);

  test_byte_order_copy<std::uint16_t,
                       std::uint32_t,
                       std::uint64_t,
                       unsigned>(256u, 256u);

  test_byte_order_copy<std::uint16_t,
                       std::uint32_t,
                       std::uint64_t,
                       unsigned>(65535u, 65535u);

  test_byte_order_copy<std::uint32_t,
                       std::uint64_t,
                       unsigned>(65536u, 65536u);

  test_byte_order_copy<std::uint32_t,
                       std::uint64_t,
                       unsigned>(16777215u, 16777215u);

  test_byte_order_copy<std::uint32_t,
                       std::uint64_t,
                       unsigned>(16777216u, 16777216u);

  test_byte_order_copy<std::uint32_t,
                       std::uint64_t,
                       unsigned>(4294967295u, 4294967295u);

  test_byte_order_copy<std::uint64_t>(
                        281474976710655u,
                        281474976710655u);
}
