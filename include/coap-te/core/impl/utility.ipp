#ifndef COAP_TE_CORE_IMPL_UTILITY_IPP_
#define COAP_TE_CORE_IMPL_UTILITY_IPP_

#include <cstring>

namespace coap_te {
namespace core {

template<typename RandomAccessIt, typename T>
[[nodiscard]] constexpr RandomAccessIt
binary_search(RandomAccessIt begin, RandomAccessIt end,
              T const& value) noexcept {
  static_assert(is_random_access_iterator_v<RandomAccessIt>,
                "Wrong iterator type. Must be random access");
  //https://stackoverflow.com/a/44522730
  static_assert(is_equal_comparable_v<std::remove_reference_t<decltype(*begin)>, T>);

  if (begin >= end) {
    return end;
  }

  auto rend = end;
  while (begin <= end) {
    auto mid = begin + (end - begin) / 2;
    if (*mid == value)
      return mid;
    if (*mid < value)
      begin = mid + 1;
    else
      end = mid - 1;
  }
  return rend;
}

template<typename Interger>
constexpr void
to_big_endian(std::uint8_t* dest,
              Interger value,
              std::size_t count /* = sizeof(Interger) */) noexcept {
  static_assert(std::is_integral<Interger>::value, "Interger required");

  for (unsigned i = 0; i < count; i++) {
    dest[i] = static_cast<std::uint8_t>(value >> ((count - (i + 1)) * 8));
  }
}

template<typename UnsignedType>
[[nodiscard]] constexpr std::size_t
make_short_unsigned(UnsignedType& value) noexcept {     //NOLINT
  static_assert(std::is_unsigned_v<UnsignedType>, "Must be unsigned");

  if (value == 0)
    return 0;

  bool skip = false;
  std::size_t size = 0;
  std::uint8_t d[sizeof(UnsignedType)];
  for (UnsignedType i = 0; i < sizeof(UnsignedType); ++i) {
    int v = value >> ((sizeof(UnsignedType) - (i + 1)) * 8);
    std::uint8_t nv = static_cast<std::uint8_t>(v);
    if (!skip && nv == 0)
      continue;
    d[size++] = nv;
    skip = true;
  }
  std::memcpy(reinterpret_cast<uint8_t*>(&value), d, size);
  return size;
}

}  // namespace core
}  // namespace coap_te

#endif  // COAP_TE_CORE_IMPL_UTILITY_IPP_