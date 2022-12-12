/**
 * @file serialize.ipp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-11-26
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef COAP_TE_MESSAGE_OPTIONS_IMPL_SERIALIZE_IPP_
#define COAP_TE_MESSAGE_OPTIONS_IMPL_SERIALIZE_IPP_

#include <system_error>   //NOLINT
#include <string_view>
#include <utility>
#include <cstring>        // std::memcpy
#include <cstdint>

#include "coap-te/core/const_buffer.hpp"
#include "coap-te/core/byte_order.hpp"
#include "coap-te/message/options/config.hpp"
#include "coap-te/message/options/checks.hpp"

namespace coap_te {
namespace message {
namespace options {

namespace detail {

struct header {
  std::uint16_t data_extend = 0;
  std::uint16_t size = 0;
  std::uint8_t  byte_op = static_cast<std::uint8_t>(extend::no_extend);
};

constexpr void
serialize_option_header(std::size_t size,
                       header& hdr) noexcept {  //NOLINT
  hdr.byte_op = static_cast<std::uint8_t>(extend::no_extend);
  hdr.data_extend = 0;
  hdr.size = 0;

  if (size < 13) {
    hdr.byte_op = static_cast<std::uint8_t>(size);
  } else if (size < 269) {
    hdr.byte_op = static_cast<std::uint8_t>(extend::one_byte);
    hdr.data_extend = static_cast<std::uint16_t>(size - 13);
    hdr.size = 1;
  } else {
    hdr.byte_op = static_cast<std::uint8_t>(extend::two_bytes);
    hdr.data_extend = static_cast<std::uint16_t>(size - 269);
    hdr.size = 2;
  }
}

template<typename ConstBuffer,
         typename MutableBuffer>
[[nodiscard]] constexpr
std::size_t
serialize(number_type before,
          number_type op,
          const ConstBuffer& input,
          MutableBuffer& output,              // NOLINT
          std::error_code& ec) noexcept {     // NOLINT
  static_assert(coap_te::core::is_const_buffer_type_v<ConstBuffer>, "Must be const_buffer type");
  static_assert(coap_te::core::is_mutable_buffer_type_v<MutableBuffer>, "Must be mutable_buffer type");

  header delta{};
  serialize_option_header(op - before, delta);

  header length{};
  serialize_option_header(input.size(), length);

  std::size_t size = 1 + delta.size + length.size + input.size();
  if (size > output.size()) {
    ec = std::make_error_code(std::errc::no_buffer_space);
    return 0;
  }

  output[0] = (delta.byte_op << 4) | length.byte_op;
  output += 1;

  for (const header& h : {delta, length}) {
    switch (static_cast<extend>(h.byte_op)) {
      case extend::one_byte:
        output[0] = static_cast<std::uint8_t>(h.data_extend);
        break;
      case extend::two_bytes: {
          auto [v, s] = coap_te::core::to_small_big_endian(h.data_extend);
          std::memcpy(output.data(), &v, 2);
        }
        break;
      default:
        break;
    }
    output += h.size;
  }

  std::memcpy(output.data(), input.data(), input.size());
  output += input.size();

  return size;
}

}  // namespace detail

template<typename CheckOptions,
         typename ConstBuffer,
         typename MutableBuffer>
[[nodiscard]] constexpr
std::enable_if_t<
  coap_te::core::is_const_buffer_type_v<ConstBuffer>, std::size_t>
serialize(number_type before,
          number_type op,
          const ConstBuffer& input,
          MutableBuffer& output,              // NOLINT
          std::error_code& ec) noexcept {     // NOLINT
  if constexpr (CheckOptions::check_any()) {
    ec = check<CheckOptions>(before, op, {format::opaque, format::string}, input.size());
    if (ec)
      return 0;
  }
  return detail::serialize(before, op,
                          input,
                          output,
                          ec);
}

template<typename CheckOptions /* = check_all */,
         typename MutableBuffer,
         typename UnsignedType>
[[nodiscard]] constexpr
std::enable_if_t<std::is_unsigned_v<UnsignedType>, std::size_t>
serialize(number_type before,
          number_type op,
          UnsignedType input,
          MutableBuffer& output,              // NOLINT
          std::error_code& ec) noexcept {     // NOLINT
  static_assert(std::is_unsigned_v<UnsignedType>, "Must be unsigned");

  auto [value, size] = ::coap_te::core::to_small_big_endian(input);

  if constexpr (CheckOptions::check_any()) {
    ec = check<CheckOptions>(before, op, format::uint, size);
    if (ec)
      return 0;
  }

  return detail::serialize(before, op,
                          ::coap_te::const_buffer{&value, size},
                          output,
                          ec);
}

template<typename CheckOptions /* = check_all */,
         typename MutableBuffer>
[[nodiscard]] constexpr std::size_t
serialize(number_type before,
          number_type op,
          MutableBuffer& output,              // NOLINT
          std::error_code& ec) noexcept {     // NOLINT
  if constexpr (CheckOptions::check_any()) {
    ec = check<CheckOptions>(before, op, format::empty, 0u);
    if (ec)
      return 0;
  }
  return detail::serialize(before, op, ::coap_te::const_buffer{}, output, ec);
}

/**
 * Serialize list
 */
template<typename CheckOptions /* = check_all */,
         typename ForwardIt,
         typename MutableBuffer>
[[nodiscard]] constexpr std::size_t
serialize(ForwardIt begin,
          ForwardIt end,
          MutableBuffer& output,               // NOLINT
          std::error_code& ec) noexcept {      // NOLINT
  static_assert(coap_te::core::is_mutable_buffer_type_v<MutableBuffer>,
                "Must be mutable buffer");
  std::size_t size = 0;
  number prev = number::invalid;
  while (begin != end) {
    size += begin->serialize(prev, output, ec);
    if (ec)
      break;
    prev = begin->option_number();
    ++begin;
  }
  return size;
}

template<typename CheckOptions /* = check_all */,
         typename Container,
         typename MutableBuffer>
[[nodiscard]] constexpr std::size_t
serialize(const Container& option_list,
          MutableBuffer& output,              // NOLINT
          std::error_code& ec) noexcept {      // NOLINT
  static_assert(coap_te::core::is_mutable_buffer_type_v<MutableBuffer>,
                "Must be mutable buffer");
  return serialize(option_list.begin(),
                   option_list.end(),
                   output,
                   ec);
}

}  // namespace options
}  // namespace message
}  // namespace coap_te

#endif  // COAP_TE_MESSAGE_OPTIONS_IMPL_SERIALIZE_IPP_
