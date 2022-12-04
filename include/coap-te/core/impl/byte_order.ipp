#ifndef COAP_TE_CORE_IMPL_BYTE_ORDER_IPP_
#define COAP_TE_CORE_IMPL_BYTE_ORDER_IPP_

#include <cstring>

namespace coap_te {
namespace core {

template<typename Unsigned>
constexpr std::pair<Unsigned, std::size_t>
to_small_big_endian(Unsigned value) noexcept {
  static_assert(std::is_unsigned_v<Unsigned>, "Must be unsigned");

  std::uint8_t *i = reinterpret_cast<std::uint8_t*>(&value);
  std::size_t size = sizeof(value);
  std::uint8_t *f = i + size - 1;

  while (size != 0 && *f == 0) {
    --size; --f;
  }
  while (i < f) {
    std::swap(*i++, *f--);
  }

  return {value, size};
}

template<typename Unsigned /* = unsigned */>
constexpr Unsigned
from_small_big_endian(const std::uint8_t* value, std::size_t size) noexcept {
  static_assert(std::is_unsigned_v<Unsigned>, "Must be unsigned");

  Unsigned n_value = 0;
  while (size--) {
    n_value = (n_value << 8) | *value++;
  }

  return n_value;
}

}  // namespace core
}  // namespace coap_te

#endif  // COAP_TE_CORE_IMPL_BYTE_ORDER_IPP_