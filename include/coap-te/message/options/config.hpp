/**
 * @file config.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-11-21
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef COAP_TE_MESSAGE_OPTIONS_CONFIG_HPP_
#define COAP_TE_MESSAGE_OPTIONS_CONFIG_HPP_

#include <string_view>
#include <cstdint>

namespace coap_te {
namespace message {
namespace options {

using number_type = std::uint16_t;
static constexpr const number_type invalid = 0;

enum class format {
  empty     = 0,
  opaque,
  uint,
  string  //NOLINT
};
using size_format = std::size_t;

struct empty_format_tag{};
struct opaque_format_tag{};
struct uint_format_tag{};
struct string_format_tag{};

enum class number : number_type;

struct definition {
  number            op;
  std::string_view  name;
  bool              repeatable;
  format            type;
  unsigned          length_min;
  unsigned          length_max;

  operator number_type() const noexcept {
    return static_cast<number_type>(op);
  }

  operator std::string_view() const noexcept {
    return name;
  }

  operator bool() const noexcept {
    return static_cast<number_type>(op) != invalid;
  }

  friend constexpr bool
  operator<(const definition& lhs, const definition& rhs) noexcept {
    return lhs.op < rhs.op;
  }

  friend constexpr bool
  operator==(const definition& lhs, const definition& rhs) noexcept {
    return lhs.op == rhs.op;
  }

  friend constexpr bool
  operator<(const definition& lhs, number op) noexcept {
    return lhs.op < op;
  }

  friend constexpr bool
  operator==(const definition& lhs, number op) noexcept {
    return lhs.op == op;
  }
};

enum class extend : std::uint8_t {
  no_extend = 0,
  one_byte  = 13,
  two_bytes = 14,
  error     = 15
};


template<bool IsContentFormat>
struct content_format {
  enum class type : std::uint16_t {
    text_plain      = 0,    // text/plain;charset=utf-8
    link_format     = 40,   // application/link-format
    xml             = 41,   // application/xml
    octet_stream    = 42,   // application/octet-stream
    exi             = 47,   // application/exi
    json            = 50,   // application/json
#if COAP_TE_ENABLE_FETCH_PATCH_VERBS == 1
    // https://tools.ietf.org/html/rfc8132#section-6
    json_patch_json = 51,   // application/json-patch+json
    merge_patch_json = 52   // application/merge-patch+json
#endif /* COAP_TE_ENABLE_FETCH_PATCH_VERBS == 1 */
  };
};

using content = content_format<true>::type;
using accept = content_format<false>::type;

#if COAP_TE_MESSAGE_OPTION_NO_RESPONSE == 1
enum class suppress : std::uint8_t{
  none          = 0b0'0000,
  success       = 0b0'0010,
  client_error  = 0b0'1000,
  server_error  = 0b1'0000
};
#endif /* COAP_TE_MESSAGE_OPTION_NO_RESPONSE == 1 */

/** Constants */

static constexpr const
number_type no_cache_key = 0b11100;

static constexpr const
unsigned max_age_default = 60;  // seconds

#if COAP_TE_MESSAGE_OPTION_NO_RESPONSE == 1
static constexpr const
suppress suppress_default = suppress::none;
#endif /* COAP_TE_MESSAGE_OPTION_NO_RESPONSE == 1 */

}  // namespace options
}  // namespace message
}  // namespace coap_te

#endif  // COAP_TE_MESSAGE_OPTIONS_CONFIG_HPP_
