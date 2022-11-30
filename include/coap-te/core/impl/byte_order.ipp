#ifndef COAP_TE_CORE_IMPL_BYTE_ORDER_IPP_
#define COAP_TE_CORE_IMPL_BYTE_ORDER_IPP_

#include <cstring>

namespace coap_te {
namespace core {

namespace detail {

template<typename Integer>
constexpr Integer
invert_byte_order(Integer value) noexcept {
  static_assert(std::is_integral_v<Integer>, "Must be integer");

  std::uint8_t* d = reinterpret_cast<std::uint8_t*>(&value);
  for (int i = 0, j = sizeof(value) - 1;
       i < j;
       ++i, --j) {
    std::swap(d[i], d[j]);
  }
  return *reinterpret_cast<Integer*>(d);
}

template<typename Unsigned>
constexpr std::size_t unsigned_size(Unsigned t) {
  std::size_t size = sizeof(Unsigned);
  std::uint8_t* d = reinterpret_cast<std::uint8_t*>(&t);
  while (size != 0 && *d++ == 0) {
      --size;
  }
  return size;
}

}  // namespace deltail

template<typename Unsigned>
constexpr std::pair<Unsigned, std::size_t>
to_small_big_endian(Unsigned value) noexcept {
  static_assert(std::is_unsigned_v<Unsigned>, "Must be unsigned");

  value = detail::invert_byte_order(value);
  auto size = detail::unsigned_size(value);
  value >>= (8 * (sizeof(Unsigned) - size));
  return {value, size};
}

template<typename Unsigned /* = unsigned */>
constexpr Unsigned
from_small_big_endian(const std::uint8_t* value, std::size_t size) noexcept {
  static_assert(std::is_unsigned_v<Unsigned>, "Must be unsigned");

  Unsigned n_value = 0;
  std::memcpy(&n_value, value, size);
  n_value = detail::invert_byte_order(n_value);
  n_value >>= (8 * (sizeof(Unsigned) - size));

  return n_value;
}

}  // namespace core
}  // namespace coap_te

#endif  // COAP_TE_CORE_IMPL_BYTE_ORDER_IPP_