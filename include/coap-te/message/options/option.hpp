/**
 * @file option.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-11-21
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef COAP_TE_MESSAGE_OPTIONS_OPTION_HPP_
#define COAP_TE_MESSAGE_OPTIONS_OPTION_HPP_

#include <variant>
#include <utility>
#include <string_view>
#include <system_error>   // NOLINT

// #include "coap-te/core/traits.hpp"
#include "coap-te/core/utility.hpp"
#include "coap-te/core/const_buffer.hpp"
#include "coap-te/core/byte_order.hpp"
#include "coap-te/message/options/config.hpp"
#include "coap-te/message/options/checks.hpp"
#include "coap-te/message/options/serialize.hpp"


namespace coap_te {
namespace message {
namespace options {

struct empty_format{};

template<class... Ts>
struct overloaded : Ts... {
  using Ts::operator()...;
};
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

class option {
 public:
  using value_type = std::variant<
                      std::monostate,
                      empty_format,
                      unsigned,
                      coap_te::const_buffer>;

  constexpr
  option() noexcept = default;

  template<typename CheckOptions = check_all,
           bool ToThrow = false>
  static constexpr option
  create(number op) noexcept(!ToThrow) {
    option nop(op);
    check_constructor<CheckOptions, ToThrow>(nop, format::empty);
    return nop;
  }

  template<typename CheckOptions = check_all,
           bool ToThrow = false>
  static constexpr option
  create(number op, unsigned value) noexcept(!ToThrow) {
    option nop(op, value);
    check_constructor<CheckOptions, ToThrow>(nop, format::uint);
    return nop;
  }

  template<typename CheckOptions = check_all,
           bool ToThrow = false>
  static constexpr option
  create(number op, std::string_view value) noexcept(!ToThrow) {
    option nop(op, value);
    check_constructor<CheckOptions, ToThrow>(nop, format::string);
    return nop;
  }

  template<typename CheckOptions = check_all,
           bool ToThrow = false>
  static constexpr option
  create(number op, const const_buffer& value) noexcept(!ToThrow) {
    option nop(op, value);
    check_constructor<CheckOptions, ToThrow>(nop, format::opaque);
    return nop;
  }

  template<typename CheckOptions = check_sequence,
           typename MutableBuffer>
  std::size_t serialize(number before,
                        MutableBuffer& output,                  //NOLINT
                        std::error_code& ec) const noexcept {   //NOLINT
    static_assert(coap_te::core::is_mutable_buffer_type_v<MutableBuffer>,
                  "Must be mutable buffer type");
    using n_check = check_type<CheckOptions::sequence, false, false>;

    return std::visit(overloaded {
      [](auto) {
        return std::size_t(0);
      },
      [&](empty_format) {
        return serialize<n_check>(coap_te::core::to_underlying(before),
                                  coap_te::core::to_underlying(op_),
                                  output, ec);
      },
      [&](unsigned data) {
        return serialize<n_check>(coap_te::core::to_underlying(before),
                                  coap_te::core::to_underlying(op_),
                                  data, output, ec);
      },
      [&](const coap_te::const_buffer& data) {
        return serialize<n_check>(coap_te::core::to_underlying(before),
                                  coap_te::core::to_underlying(op_),
                                  data, ec);
      }
    }, data_);
  }

  template<typename CheckOptions = check_sequence,
           typename MutableBuffer>
  std::size_t serialize(number before,
                        MutableBuffer& output) const {   //NOLINT
    std::error_code ec;
    auto size = serialize<CheckOptions>(before, output, ec);
    if (ec) {
      throw std::system_error{ec};
    }
    return size;
  }

  // bool set(number op) noexcept(!ToThrow) {
  //   op_ = op;
  //   check_constructor(format::empty);
  // }

  // bool set(number op, unsigned value) noexcept(!ToThrow) {
  //   op_ = op;
  //   data_ = value;
  //   check_constructor(format::uint);
  // }

  // bool set(number op, std::string_view value) noexcept(!ToThrow) {
  //   op_ = op;
  //   data_ = coap_te::const_buffer{value};
  //   check_constructor(format::string);
  // }

  // bool set(number op, coap_te::const_buffer value) noexcept(!ToThrow) {
  //   op_ = op;
  //   data_ = coap_te::const_buffer{value};
  //   check_constructor(format::opaque);
  // }

  number option_number() const noexcept {
    return op_;
  }

  const value_type& value() const noexcept {
    return data_;
  }

  std::size_t size() const noexcept {
    return std::visit(overloaded {
      [](auto) { return std::size_t(0); },
      [](const coap_te::const_buffer&  data) { return data.size(); },
      [](unsigned data) { return coap_te::core::small_big_endian_size(data); }
    }, data_);
  }

  /**
   * @brief Check if options is valid
   * 
   * @note Not the best API, but for now necessary for no-throw code
   * 
   * @return true Is in a valid state
   * @return false Is not in a not valid
   */
  bool is_valid() const noexcept {
    return op_ != number::invalid;
  }

 private:
  template<typename Arg>
  void set_no_check(number op, Arg&& arg) noexcept {
    op_ = op;
    data_ = arg;
  }

  template<typename CheckOptions,
           bool ToThrow>
  static constexpr void
  check_constructor(option& nop,      // NOLINT
                    format type) noexcept(!ToThrow) {
    using n_check = check_type<false,
                               CheckOptions::format,
                               CheckOptions::length>;
    if constexpr (n_check::check_any()) {
      auto ec = check<n_check>(
                        coap_te::core::to_underlying(number::invalid),
                        coap_te::core::to_underlying(nop.option_number()),
                        type, nop.size());
      if (ec) {
        nop.set_no_check(number::invalid, std::monostate{});
        if constexpr (ToThrow) {
          throw std::system_error{ec};
        }
      }
    }
  }

  constexpr
  explicit option(number op) noexcept
    : op_(op) {}

  constexpr
  option(number op, unsigned value) noexcept
    : op_(op), data_(value) {}

  constexpr
  option(number op, std::string_view str) noexcept
    : op_(op), data_(const_buffer{str}) {}

  constexpr
  option(number op, const const_buffer& value) noexcept
    : op_(op), data_(const_buffer{value}) {}

  number      op_  = number::invalid;
  value_type  data_;
};

}  // namespace options
}  // namespace message
}  // namespace coap_te

#endif  // COAP_TE_MESSAGE_OPTIONS_OPTION_HPP_
