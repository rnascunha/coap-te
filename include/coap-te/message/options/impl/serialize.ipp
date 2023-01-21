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

#include <string_view>
#include <utility>
#include <cstdint>

#include "coap-te/core/error.hpp"
#include "coap-te/core/utility.hpp"
#include "coap-te/buffer/buffer.hpp"
#include "coap-te/message/detail/buffer.hpp"
#include "coap-te/message/options/config.hpp"
#include "coap-te/message/options/checks.hpp"
#include "coap-te/message/options/traits.hpp"
#include "coap-te/message/options/option_func.hpp"

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

template<typename ConstBufferSequence,
         typename MutableBufferOrRange>
[[nodiscard]] constexpr std::size_t
serialize(number_type before,
          number_type op,
          const ConstBufferSequence& input,
          const MutableBufferOrRange& output,
          coap_te::error_code& ec) noexcept {     // NOLINT
  static_assert(coap_te::is_const_buffer_sequence_v<ConstBufferSequence>,
                "ConstBufferSequence requirements not met");
  static_assert(coap_te::message::detail::
                is_mutable_buffer_or_range_v<
                  MutableBufferOrRange>,
                "Must be mutable_buffer type");
  header delta{};
  serialize_option_header(op - before, delta);

  auto input_size = buffer_size(input);
  header length{};
  serialize_option_header(input_size, length);

  std::size_t size = 1 + delta.size + length.size + input_size;
  if (size > output.size()) {
    ec = coap_te::errc::no_buffer_space;
    return 0;
  }

  std::uint8_t header_opt[5]{};
  std::size_t header_size = 1;
  header_opt[0] = (delta.byte_op << 4) | length.byte_op;

  for (const header& h : {delta, length}) {
    switch (static_cast<extend>(h.byte_op)) {
      case extend::one_byte:
        header_opt[header_size] = static_cast<std::uint8_t>(h.data_extend);
        break;
      case extend::two_bytes: {
          header_opt[header_size] = h.data_extend >> 8;
          header_opt[header_size + 1] = h.data_extend & 0xFF;
        }
        break;
      default:
        break;
    }
    header_size += h.size;
  }

  buffer_copy(output, coap_te::buffer(header_opt), header_size);
  buffer_copy(output + header_size, input, input_size);

  return size;
}

}  // namespace detail

template<typename CheckOptions,
         typename ConstBufferSequence,
         typename MutableBufferOrRange>
[[nodiscard]] constexpr
std::enable_if_t<
  coap_te::is_const_buffer_sequence_v<ConstBufferSequence>, std::size_t>
serialize(number_type before,
          number_type op,
          const ConstBufferSequence& input,
          const MutableBufferOrRange& output,
          coap_te::error_code& ec) noexcept {     // NOLINT
  if constexpr (CheckOptions::check_any()) {
    ec = check<CheckOptions>(before, op,
                            {format::opaque, format::string},
                            buffer_size(input));
    if (ec)
      return 0;
  }
  return detail::serialize(before, op,
                          input,
                          coap_te::message::detail::get_mutable_buffer(output),
                          ec);
}

template<typename CheckOptions /* = check_all */,
         typename MutableBufferSequence,
         typename UnsignedType>
[[nodiscard]] constexpr
std::enable_if_t<std::is_unsigned_v<UnsignedType>, std::size_t>
serialize(number_type before,
          number_type op,
          UnsignedType input,
          const MutableBufferSequence& output,
          coap_te::error_code& ec) noexcept {     // NOLINT
  static_assert(std::is_unsigned_v<UnsignedType>, "Must be unsigned");

  auto [value, size] = ::coap_te::core::to_small_big_endian(input);

  if constexpr (CheckOptions::check_any()) {
    ec = check<CheckOptions>(before, op, format::uint, size);
    if (ec)
      return 0;
  }

  return detail::serialize(before, op,
                          coap_te::const_buffer{&value, size},
                          output,
                          ec);
}

template<typename CheckOptions /* = check_all */,
         typename MutableBufferSequence>
[[nodiscard]] constexpr std::size_t
serialize(number_type before,
          number_type op,
          const MutableBufferSequence& output,
          coap_te::error_code& ec) noexcept {     // NOLINT
  if constexpr (CheckOptions::check_any()) {
    ec = check<CheckOptions>(before, op, format::empty, 0u);
    if (ec)
      return 0;
  }
  return detail::serialize(before, op,
                           coap_te::const_buffer{},
                           output, ec);
}

template<typename CheckOptions /* = check_sequence */,
         typename Option,
         typename MutableBufferSequence>
[[nodiscard]] constexpr std::size_t
serialize(number before,
          const Option& option,
          const MutableBufferSequence& output,
          coap_te::error_code& ec) noexcept {    //NOLINT
  static_assert(is_option_v<Option>, "Options requirements not met");
  using n_check = check_type<CheckOptions::sequence, false, false>;
  return serialize<n_check>(coap_te::core::to_underlying(before),
                          coap_te::core::to_underlying(option.option_number()),
                          coap_te::const_buffer{option.data(),
                                                option.data_size()},
                          output,
                          ec);
}

/**
 * Serialize list
 */
template<typename CheckOptions /* = check_all */,
         typename ForwardIt,
         typename MutableBufferSequence>
[[nodiscard]] constexpr std::size_t
serialize(ForwardIt begin,
          ForwardIt end,
          const MutableBufferSequence& output,
          coap_te::error_code& ec) noexcept {      // NOLINT
  std::size_t size = 0;
  number prev = number::invalid;
  auto buf = coap_te::message::detail::get_mutable_buffer(output);
  while (begin != end) {
    // the use of @ref forward_second_if_pair is because the container may
    // be a associative container
    std::size_t size_opt = serialize<CheckOptions>(prev,
                              coap_te::core::forward_second_if_pair(*begin),
                              buf,
                              ec);
    size += size_opt;
    buf += size_opt;
    if (ec)
      break;
    prev = coap_te::core::forward_second_if_pair(*begin).option_number();
    ++begin;
  }
  return size;
}

template<typename CheckOptions /* = check_all */,
         typename Container,
         typename MutableBufferSequence>
[[nodiscard]] constexpr std::size_t
serialize(const Container& option_list,
          const MutableBufferSequence& output,
          coap_te::error_code& ec) noexcept {      // NOLINT
  return serialize<CheckOptions>(option_list.begin(),
                   option_list.end(),
                   output,
                   ec);
}

}  // namespace options
}  // namespace message
}  // namespace coap_te

#endif  // COAP_TE_MESSAGE_OPTIONS_IMPL_SERIALIZE_IPP_
