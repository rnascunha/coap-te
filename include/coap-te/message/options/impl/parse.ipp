/**
 * @file parse.ipp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-11-29
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef COAP_TE_MESSAGE_OPTIONS_IMPL_PARSE_IPP_
#define COAP_TE_MESSAGE_OPTIONS_IMPL_PARSE_IPP_

#include <cstdint>

#include "coap-te/core/error.hpp"
#include "coap-te/core/traits.hpp"
#include "coap-te/message/config.hpp"
#include "coap-te/message/options/config.hpp"
#include "coap-te/message/options/checks.hpp"

namespace coap_te {
namespace message {
namespace options {

namespace {   // NOLINT

struct header_parse {
  std::uint16_t data_extend = 0;
  std::uint16_t size = 0;
  extend        byte_op = extend::no_extend;
};

constexpr void
parse_option_header(std::uint8_t op,
                    header_parse& hdr) noexcept {  //NOLINT
  hdr.size = 0;
  hdr.byte_op = extend::no_extend;

  if (op < 13) {
    hdr.byte_op = static_cast<extend>(op);
  } else if (op == 13) {
    hdr.byte_op = extend::one_byte;
    hdr.size = 1;
  } else if (op == 14) {
    hdr.byte_op = extend::two_bytes;
    hdr.size = 2;
  } else {
    hdr.byte_op = extend::error;
  }
}

}  // namespace

template<typename CheckOptions /* = check_all */,
         typename ConstBuffer>
constexpr std::size_t
parse(number_type before,
      ConstBuffer& input,                // NOLINT
      number_type& current,              // NOLINT
      ConstBuffer& output,               // NOLINT
      coap_te::error_code& ec) noexcept {    // NOLINT
  static_assert(core::is_const_buffer_v<ConstBuffer>, "Must be const buffer");

  if (input.size() == 0) {
    ec = coap_te::errc::no_buffer_space;
    return 0;
  }

  header_parse delta;
  parse_option_header(input[0] >> 4, delta);
  if (delta.byte_op == extend::error) {
    ec = coap_te::errc::invalid_option_header;
    return 0;
  }

  header_parse length;
  parse_option_header(input[0] & 0x0F, length);
  if (length.byte_op == extend::error) {
    ec = coap_te::errc::invalid_option_header;
    return 0;
  }

  std::size_t size = 1 + delta.size + length.size;
  if (input.size() < size) {
    ec = coap_te::errc::no_buffer_space;
    return 0;
  }

  input += 1;
  for (header_parse* h : {&delta, &length}) {
    switch (h->byte_op) {
      case extend::one_byte:
        h->data_extend = input[0] + 13;
        break;
      case extend::two_bytes:
        h->data_extend = coap_te::core::from_small_big_endian<std::uint16_t>(
                          reinterpret_cast<const std::uint8_t*>(input.data()), 2)     // NOLINT
                          + 269;
        break;
      default:
        h->data_extend = static_cast<uint16_t>(h->byte_op);
        break;
    }
    input += h->size;
  }
  current = before + delta.data_extend;

  if (input.size() < length.data_extend) {
    ec = coap_te::errc::no_buffer_space;
    return size;
  }
  
  output = {input.data(), length.data_extend};
  input += length.data_extend;

  // As a sequence of bytes can be any type, we cannot check it
  using check_n = check_type<CheckOptions::sequence,
                             false,
                             CheckOptions::length>;
  if constexpr (check_n::check_any()) {
    ec = check<check_n>(before, current, format::empty, length.data_extend);
  }

  return size + length.data_extend;
}

template<typename CheckOptions /* = check_all */,
         typename ConstBuffer>
constexpr std::size_t
parse(number_type before,
      ConstBuffer& input,                // NOLINT
      option& output,                    // NOLINT
      coap_te::error_code& ec) noexcept {    // NOLINT
  number_type current = invalid;
  coap_te::const_buffer buf;
  auto size = parse<CheckOptions>(before, input, current, buf, ec);

  if (!ec) {
    output = create<check_none, false>(static_cast<number>(current), buf);
  }
  return size;
}

}  // namespace options
}  // namespace message
}  // namespace coap_te

#endif  // COAP_TE_MESSAGE_OPTIONS_IMPL_PARSE_IPP_
