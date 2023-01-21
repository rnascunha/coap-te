/**
 * @file buffer.cpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-01-13
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <cstring>
#include <string_view>
#include <vector>
#include <array>
#include <limits>
#include <iostream>

#include "coap-te/buffer/buffer.hpp"
#include "coap-te/debug/to_string.hpp"

#define TEST_RUN_SINGLE_BUFFERS     1
#define TEST_RUN_MULTIPLE_BUFFERS   1
#define TEST_RUN_BUFFER_RANGE       1

#define TEST_RUN_PRINT_BUFFER       1
#define TEST_RUN_COMPARE            1
#define TEST_RUN_COPY               1
#define TEST_RUN_COPY_PRINT_AFTER   1

template<typename ConstBufferSequence>
void print_buffer(const ConstBufferSequence& buf,
                  std::string_view name,
                  std::string_view pref = "\t",
                  std::size_t max_size
                    = std::numeric_limits<std::size_t>::max()) noexcept {
  std::cout << pref << name
            << "[" << (std::min)(coap_te::buffer_size(buf), max_size)
            << "]: ["
            << coap_te::debug::to_hex(buf, " ", max_size)
            << "]\n";
}

template<typename ConstBufferSequence, std::size_t N>
void print_buffers(const ConstBufferSequence& buffers,
                   const std::array<std::string_view, N>& names,
                   std::string_view pref = "\t") noexcept {
  for (std::size_t i = 0; i < buffers.size(); ++i) {
    print_buffer(buffers[i], names[i], pref);
  }
}

template<typename ConstBufferSequence1,
         typename ConstBufferSequence2>
void compare_buffer(const ConstBufferSequence1& buf1, std::string_view name1,
                    const ConstBufferSequence2& buf2, std::string_view name2,
                    std::string_view pref = "\t") noexcept {
  auto res = coap_te::buffer_compare(buf1, buf2);
  std::cout << pref << name1
            << (res == 0 ? " = " : (res < 0 ? " < " : " > "))
            << name2
            << "\n";
}

template<typename ConstBufferSequence,
         std::size_t N>
void compare_all_buffers(const ConstBufferSequence& buffers,
                   const std::array<std::string_view, N>& names,
                   std::string_view pref = "\t") {
  for (std::size_t i = 0; i < buffers.size(); ++i) {
    for (std::size_t j = 0; j < buffers.size(); ++j) {
      compare_buffer(buffers[i], names[i],
                     buffers[j], names[j],
                     pref);
    }
  }
}

template<typename MutableBufferSequence,
         typename ConstBufferSequence>
void copy_buffer_check(const MutableBufferSequence& target,
                 const ConstBufferSequence& source,
                 std::string_view sname,
                 std::size_t max_size
                  = std::numeric_limits<std::size_t>::max()) noexcept {
  auto tsize = coap_te::buffer_size(target);
  auto ssize = coap_te::buffer_size(source);
  std::string_view tname = "data";
  std::cout << "{\n";
  std::cout << "\tCopying "
            << sname << "[" << ssize << "]"
            << " >> "
            << tname << "[" << tsize << "]\n";

  auto rsize = coap_te::buffer_copy(target, source, max_size);
  auto should_size = (std::min)({tsize, ssize, max_size});
  std::cout << "\tCopied " << rsize << " / "
            << should_size << "\n";
  auto res = coap_te::buffer_compare(target, source);
  std::cout << "\tCompare: " << (res == 0 ? "OK" : "ERROR");
  std::cout << "\n";

#if TEST_RUN_COPY_PRINT_AFTER == 1
  std::cout << "\t{\n";
  print_buffer(target, "  target", "\t", rsize);
  print_buffer(source, "  source", "\t", rsize);
  std::cout << "\t}\n";
#endif  // TEST_RUN_COPY_PRINT_AFTER == 1

  std::cout << "}\n";
}

int main() {
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
  std::array<std::string_view, 7> sbuffers_names = {
    "buf0", "buf1", "buf2", "buf3", "buf4", "buf5", "buf6"
  };

#if TEST_RUN_MULTIPLE_BUFFERS == 1 || TEST_RUN_BUFFER_RANGE == 1
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
  std::array<std::string_view, 9> mbuffers_name = {
    "buf00", "buf01", "buf02", "buf12",
    "buf012", "buf123", "buf456", "buf136",
    "buf0123456"
  };
#endif  // TEST_RUN_MULTIPLE_BUFFERS == 1 || TEST_RUN_BUFFER_RANGE == 1

#if TEST_RUN_SINGLE_BUFFERS == 1
  std::cout << "\t\tSingle Buffers\n";

#if TEST_RUN_PRINT_BUFFER == 1
  std::cout << "Buffers:\n";
  print_buffers(sbuffers, sbuffers_names);
#endif  //  TEST_RUN_PRINT_BUFFER == 1

#if TEST_RUN_COMPARE == 1
  std::cout << "Compare:\n";
  compare_all_buffers(sbuffers, sbuffers_names);
#endif  // TEST_RUN_COMPARE == 1

#if TEST_RUN_COPY == 1
  std::cout << "Copy:\n";
  {
    // same size target
    std::uint8_t data[10];
    copy_buffer_check(coap_te::buffer(data), sbuffers[2], sbuffers_names[2]);
  }
  {
    // bigger target
    std::vector<std::uint8_t> data(10);
    copy_buffer_check(coap_te::buffer(data), sbuffers[0], sbuffers_names[0]);
  }
  {
    // smaller target
    std::array<std::uint8_t, 3> data;
    copy_buffer_check(coap_te::buffer(data), sbuffers[1], sbuffers_names[1]);
  }
#endif  // TEST_RUN_COPY == 1

#endif  // TEST_RUN_SINGLE_BUFFERS == 1

#if TEST_RUN_MULTIPLE_BUFFERS == 1
  std::cout << "\n\t\tMultiple Buffers\n";

#if TEST_RUN_PRINT_BUFFER == 1
  std::cout << "Buffers:\n";
  print_buffers(mbuffers, mbuffers_name);
#endif  // TEST_RUN_PRINT_BUFFER == 1

#if TEST_RUN_COMPARE == 1
  std::cout << "Compare:\n";
  compare_all_buffers(mbuffers, mbuffers_name);
#endif  // TEST_RUN_COMPARE == 1

#if TEST_RUN_COPY == 1
  std::cout << "Copy:\n";
  std::cout << ">>>> multiple source, single target\n";
  {
    // same size
    std::array<std::uint8_t, 10> arr;
    copy_buffer_check(coap_te::buffer(arr), mbuffers[1], mbuffers_name[1]);
  }
  {
    // source bigger size
    std::vector<std::uint8_t> arr(8);
    copy_buffer_check(coap_te::buffer(arr), mbuffers[2], mbuffers_name[2]);
  }
  {
    // target bigger size
    std::uint8_t arr[12];
    copy_buffer_check(coap_te::buffer(arr), mbuffers[3], mbuffers_name[3]);
  }
  std::cout << ">>>> single source, multiple target\n";
  {
    // same size
    std::array<std::uint8_t, 3> arr0;
    std::uint8_t arr1[2];
    std::vector<coap_te::mutable_buffer> data_buf{coap_te::buffer(arr0),
                                              coap_te::buffer(arr1)};
    copy_buffer_check(data_buf, sbuffers[1], sbuffers_names[1]);
  }
  {
    // source bigger
    std::array<std::uint8_t, 1> arr0;
    std::uint8_t arr1[2];
    std::array<coap_te::mutable_buffer, 2> data_buf{coap_te::buffer(arr0),
                                              coap_te::buffer(arr1)};
    copy_buffer_check(data_buf, sbuffers[2], sbuffers_names[2]);
  }
  {
    // target bigger
    std::vector<std::uint8_t> arr0(3);
    std::uint8_t arr1[2];
    std::array<std::uint8_t, 4> arr2;
    coap_te::mutable_buffer data_buf[]{coap_te::buffer(arr0),
                                              coap_te::buffer(arr1),
                                              coap_te::buffer(arr2)};
    copy_buffer_check(data_buf, sbuffers[2], sbuffers_names[2]);
  }
  std::cout << ">>>> multiple buffers, multiple sources\n";
  {
    // same size
    std::array<std::uint8_t, 6> arr0;
    std::uint8_t arr1[7];
    std::vector<std::uint8_t> arr2(2);
    std::array<coap_te::mutable_buffer, 3> arr{
      coap_te::buffer(arr0),
      coap_te::buffer(arr1),
      coap_te::buffer(arr2)
    };
    copy_buffer_check(coap_te::buffer(arr), mbuffers[4], mbuffers_name[4]);
  }
  {
    // bigger source
    std::array<std::uint8_t, 3> arr0;
    std::uint8_t arr1[10];
    std::vector<std::uint8_t> arr2(8);
    std::array<coap_te::mutable_buffer, 3> arr{
      coap_te::buffer(arr0),
      coap_te::buffer(arr1),
      coap_te::buffer(arr2)
    };
    copy_buffer_check(arr, mbuffers[8], mbuffers_name[8]);
  }
  {
    // bigger target
    std::array<std::uint8_t, 4> arr0;
    std::uint8_t arr1[10];
    std::vector<std::uint8_t> arr2(12);
    coap_te::mutable_buffer arr[]{
      coap_te::buffer(arr0),
      coap_te::buffer(arr1),
      coap_te::buffer(arr2)
    };
    copy_buffer_check(arr, mbuffers[8], mbuffers_name[8]);
  }
#endif  // TEST_RUN_COPY == 1

#endif  // TEST_RUN_MULTIPLE_BUFFERS == 1

#if TEST_RUN_BUFFER_RANGE == 1
  std::cout << "\n\t\tIterator containers\n";
  coap_te::buffer_range ic0(buf01);
  coap_te::buffer_range ic1(coap_te::buffers_begin(buf01) + 2,
                                  coap_te::buffers_end(buf01));
  coap_te::buffer_range ic2(coap_te::buffers_begin(buf01) + 2,
                                  coap_te::buffers_end(buf01) - 2);
  coap_te::buffer_range ic3(coap_te::buffers_begin(buf0123456) + 5,
                                  coap_te::buffers_end(buf0123456) - 10);

  std::array<std::string_view, 4> ics_name = {"ic0", "ic1", "ic2", "ic3"};

#if TEST_RUN_PRINT_BUFFER == 1
  std::cout << "Buffers:\n";
  print_buffer(ic0, ics_name[0]);
  print_buffer(ic1, ics_name[1]);
  print_buffer(ic2, ics_name[2]);
  print_buffer(ic3, ics_name[3]);
#endif  // TEST_RUN_PRINT_BUFFER == 1

#if TEST_RUN_COMPARE == 1
  std::cout << "Compare:\n";
  compare_buffer(ic0, ics_name[0], ic1, ics_name[1]);
  compare_buffer(ic0, ics_name[0], ic2, ics_name[2]);
  compare_buffer(ic1, ics_name[1], ic2, ics_name[2]);
  {
    std::uint8_t data[]{2, 3, 4};
    compare_buffer(ic2, ics_name[2], coap_te::buffer(data), "data");
  }
#endif  // TEST_RUN_COMPARE == 1

#if TEST_RUN_COPY == 1
  std::cout << "Copy:\n";
  std::cout << ">>>> From iterator container\n";
  {
    // single buffer, same size
    std::uint8_t data[6];
    copy_buffer_check(coap_te::buffer(data), ic2, ics_name[2]);
  }
  {
    // single buffer, buffer small
    std::uint8_t data[7];
    copy_buffer_check(coap_te::buffer(data), ic0, ics_name[0]);
  }
  {
    // single buffer, buffer big
    std::uint8_t data[12];
    copy_buffer_check(coap_te::buffer(data), ic3, ics_name[3]);
  }
  {
    // multiple buffer, small buffer
    std::uint8_t data_1[3];
    std::uint8_t data_2[2];
    std::uint8_t data_3[2];
    std::array<coap_te::mutable_buffer, 3> buf{ coap_te::buffer(data_1),
                                     coap_te::buffer(data_2),
                                     coap_te::buffer(data_3)};
    copy_buffer_check(buf, ic1, ics_name[1]);
  }
  {
    // multiple buffer, big buffer
    std::uint8_t data_1[3];
    std::uint8_t data_2[3];
    std::uint8_t data_3[5];
    std::array<coap_te::mutable_buffer, 3> buf{ coap_te::buffer(data_1),
                                     coap_te::buffer(data_2),
                                     coap_te::buffer(data_3)};
    copy_buffer_check(buf, ic1, ics_name[1]);
  }
  std::cout << ">>>> To iterator container\n";
  {
    // single buffer, same size
    std::uint8_t data_1[3];
    std::uint8_t data_2[2];
    std::vector<coap_te::mutable_buffer> buf{coap_te::buffer(data_1),
                                             coap_te::buffer(data_2)};
    copy_buffer_check(coap_te::buffer_range(buf),
                      buf0, sbuffers_names[0]);
  }
  {
    // single buffer, buffer small
    std::uint8_t data_1[2];
    std::uint8_t data_2[1];
    std::uint8_t data_3[4];
    std::array<coap_te::mutable_buffer, 3> buf{coap_te::buffer(data_1),
                                            coap_te::buffer(data_2),
                                            coap_te::buffer(data_3)};
    copy_buffer_check(coap_te::buffer_range(buf),
                      buf1, sbuffers_names[1]);
  }
  {
    // single buffer, buffer big
    std::uint8_t data_1[2];
    std::uint8_t data_2[1];
    std::array<coap_te::mutable_buffer, 3> buf{coap_te::buffer(data_1),
                                            coap_te::buffer(data_2)};
    copy_buffer_check(coap_te::buffer_range(buf),
                      buf5, sbuffers_names[5]);
  }
  {
    // multiple buffer, same size
    std::uint8_t data_1[2];
    std::uint8_t data_2[4];
    std::uint8_t data_3[4];
    std::array<coap_te::mutable_buffer, 3> buf{coap_te::buffer(data_1),
                                               coap_te::buffer(data_2),
                                               coap_te::buffer(data_3)};
    copy_buffer_check(coap_te::buffer_range(buf),
                      buf01, mbuffers_name[1]);
  }
  {
    // multiple buffer, buffer small
    std::uint8_t data_1[2];
    std::uint8_t data_2[4];
    std::array<coap_te::mutable_buffer, 2> buf{coap_te::buffer(data_1),
                                            coap_te::buffer(data_2)};
    copy_buffer_check(coap_te::buffer_range(buf),
                      buf00, mbuffers_name[0]);
  }
  {
    // multiple buffer, buffer big
    std::uint8_t data_1[6];
    std::uint8_t data_2[4];
    std::uint8_t data_3[7];
    std::vector<coap_te::mutable_buffer> buf{coap_te::buffer(data_1),
                                             coap_te::buffer(data_2),
                                             coap_te::buffer(data_3)};
    copy_buffer_check(coap_te::buffer_range(buf),
                        buf0123456, mbuffers_name[8]);
  }
  std::cout << ">>>> From iterator container to iterator container\n";
  {
    // same size
    std::uint8_t data_0[3];
    std::array<std::uint8_t, 5> data_1;
    std::vector<std::uint8_t> data_2(2);
    std::vector<coap_te::mutable_buffer> buf{
      coap_te::buffer(data_0),
      coap_te::buffer(data_1),
      coap_te::buffer(data_2)
    };
    coap_te::buffer_range icm(buf);
    copy_buffer_check(icm, ic3, ics_name[3]);
  }
  {
    // small target
    std::uint8_t data_0[2];
    std::array<std::uint8_t, 3> data_1;
    std::vector<std::uint8_t> data_2(2);
    std::vector<coap_te::mutable_buffer> buf{
      coap_te::buffer(data_0),
      coap_te::buffer(data_1),
      coap_te::buffer(data_2)
    };
    coap_te::buffer_range icm(buf);
    copy_buffer_check(icm, ic1, ics_name[1]);
  }
  {
    // bigger target
    std::uint8_t data_0[5];
    std::array<std::uint8_t, 3> data_1;
    std::vector<std::uint8_t> data_2(7);
    std::vector<coap_te::mutable_buffer> buf{
      coap_te::buffer(data_0),
      coap_te::buffer(data_1),
      coap_te::buffer(data_2)
    };
    coap_te::buffer_range icm(buf);
    copy_buffer_check(icm, ic3, ics_name[3]);
  }
#endif  // TEST_RUN_COPY == 1

#endif  // TEST_RUN_BUFFER_RANGE == 1
  return 0;
}
