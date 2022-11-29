#ifndef COAP_TE_CORE_IMPL_BYTE_ORDER_IPP_
#define COAP_TE_CORE_IMPL_BYTE_ORDER_IPP_

#include <cstring>

namespace coap_te {
namespace core {

template<typename UnsignedType>
constexpr std::size_t
to_big_endian(UnsignedType& value) noexcept {     //NOLINT
  static_assert(std::is_unsigned_v<UnsignedType>, "Must be unsigned");

  // Invert bytes
  std::uint8_t* d = reinterpret_cast<std::uint8_t*>(&value);
  std::size_t n = sizeof(UnsignedType) >> 1;
  for (std::size_t i = 0; i < n; ++i) {
      auto temp = d[i];
      d[i] = d[sizeof(UnsignedType) - 1 - i];
      d[sizeof(UnsignedType) - 1 - i] = temp;
  }

  // Calulate size
  std::size_t size = 0;
  bool flag = false;
  for (std::size_t i = 0; i < sizeof(UnsignedType); ++i) {
    if (flag || d[i] != 0) {
        ++size;
        flag = true;
    }
  }
  return size;
}

template<typename UnsignedType>
constexpr void
to_big_endian(UnsignedType value, std::uint8_t* buffer) noexcept {
  static_assert(std::is_unsigned_v<UnsignedType>, "Must be unsigned");

  to_big_endian(value);
  std::memcpy(buffer, &value, sizeof(value));
}

}  // namespace core
}  // namespace coap_te

#endif  // COAP_TE_CORE_IMPL_BYTE_ORDER_IPP_