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

#include "coap-te/core/const_buffer.hpp"
#include "coap-te/core/byte_order.hpp"    // NOLINT

template<typename Unsigned>
void test_endianess_impl(
        Unsigned value,
        Unsigned value_expected,
        std::size_t size_expected) noexcept {
  namespace core = coap_te::core;

  auto [value_coverted,
        size_converted] = core::to_small_big_endian<Unsigned>(value);
  EXPECT_EQ(value_coverted, value_expected);
  EXPECT_EQ(size_converted, size_expected);

  Unsigned value2 = core::from_small_big_endian<Unsigned>(
                    reinterpret_cast<const std::uint8_t*>(&value_coverted),
                    size_converted);

  EXPECT_EQ(value2, value);
}

template<typename Arg, typename ...Args>
void test_endianess(
        Arg value,
        Arg value_expected,
        std::size_t size_expected) {
  test_endianess_impl<Arg>(value, value_expected, size_expected);
  if (sizeof...(Args) > 0)
    test_endianess<Args...>(value, value_expected, size_expected);
}

TEST(CoreByteOrder, ConvertBigLittleEndianCornerCases) {
  // 0x0
  test_endianess<std::uint8_t,
                 std::uint16_t,
                 std::uint32_t,
                 std::uint64_t,
                 unsigned>(0, 0, 0);
  // 0x1
  test_endianess<std::uint8_t,
                 std::uint16_t,
                 std::uint32_t,
                 std::uint64_t,
                 unsigned>(1, 1, 1);
  // 0xFF
  test_endianess<std::uint8_t,
                 std::uint16_t,
                 std::uint32_t,
                 std::uint64_t,
                 unsigned>(255, 255, 1);
  // 0x100
  test_endianess<std::uint16_t,
                 std::uint32_t,
                 std::uint64_t,
                 unsigned>(256, 1, 2);
  // 0xFFFF
  test_endianess<std::uint16_t,
                 std::uint32_t,
                 std::uint64_t,
                 unsigned>(65535, 65535, 2);
  // 0x10000
  test_endianess<std::uint32_t,
                 std::uint64_t,
                 unsigned>(65536, 1, 3);
  // 0xFFFFFF
  test_endianess<std::uint32_t,
                 std::uint64_t,
                 unsigned>(16777215, 16777215, 3);
  // 0x1000000
  test_endianess<std::uint32_t,
                 std::uint64_t,
                 unsigned>(16777216, 1, 4);
  // 0xFFFFFFFF
  test_endianess<std::uint32_t,
                 std::uint64_t,
                 unsigned>(4294967295u, 4294967295u, 4);
  // 0x100000000
  test_endianess<std::uint64_t>(4294967296, 1, 5);
  // 0xFFFFFFFFFF
  test_endianess<std::uint64_t>(1099511627775ULL, 1099511627775ULL, 5);
  // 0x10000000000
  test_endianess<std::uint64_t>(1099511627776ULL, 1, 6);
  // 0xFFFFFFFFFFFF
  test_endianess<std::uint64_t>(281474976710655ULL, 281474976710655ULL, 6);
  // 0x1000000000000
  test_endianess<std::uint64_t>(281474976710656ULL, 1, 7);
  // 0xFFFFFFFFFFFFFF
  test_endianess<std::uint64_t>(72057594037927935, 72057594037927935, 7);
  // 0x100000000000000
  test_endianess<std::uint64_t>(72057594037927936, 1, 8);
  // 0xFFFFFFFFFFFFFFFF
  test_endianess<std::uint64_t>(18446744073709551615ULL,
                                18446744073709551615ULL, 8);
}

TEST(CoreByteOrder, ConvertBigLittleEndian) {
  test_endianess<std::uint8_t,
                 std::uint16_t,
                 std::uint32_t,
                 std::uint64_t,
                 unsigned>(0x12, 0x12, 1);
  test_endianess<std::uint16_t,
                 std::uint32_t,
                 std::uint64_t,
                 unsigned>(0x1234, 0x3412, 2);
  test_endianess<std::uint32_t,
                 std::uint64_t,
                 unsigned>(0x123456, 0x563412, 3);
  test_endianess<std::uint32_t,
                 std::uint64_t,
                 unsigned>(0x12345678, 0x78563412, 4);
  test_endianess<std::uint64_t>(0x1234567890, 0x9078563412, 5);
  test_endianess<std::uint64_t>(0x1234567890AB, 0xAB9078563412, 6);
  test_endianess<std::uint64_t>(0x1234567890ABCD, 0xCDAB9078563412, 7);
  test_endianess<std::uint64_t>(0x1234567890ABCDEF, 0xEFCDAB9078563412, 8);
}
