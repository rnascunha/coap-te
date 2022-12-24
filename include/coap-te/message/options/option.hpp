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

#include <limits>
#include <variant>
#include <utility>
#include <string_view>

#include "coap-te/core/error.hpp"
#include "coap-te/core/utility.hpp"
#include "coap-te/core/const_buffer.hpp"
#include "coap-te/core/byte_order.hpp"
#include "coap-te/message/options/config.hpp"
#include "coap-te/message/options/checks.hpp"

namespace coap_te {
namespace message {
namespace options {

struct empty_format{};

class option {
 public:
  using unsigned_type = unsigned;
  using value_type = std::variant<
                      std::monostate,
                      empty_format,
                      unsigned_type,
                      coap_te::const_buffer>;
  static constexpr unsigned_type
  invalid_unsigned = std::numeric_limits<unsigned_type>::max();

  constexpr
  option() noexcept = default;

  constexpr
  explicit option(number op) noexcept
    : data_(empty_format{}), op_(op) {}

  constexpr
  option(number op, unsigned_type value) noexcept
    : data_(coap_te::core::to_small_big_endian(value).first),
      op_(op) {}

  constexpr
  option(number op, std::string_view str) noexcept
    : data_(const_buffer{str}), op_(op) {}

  constexpr
  option(number op, const const_buffer& value) noexcept
    : data_(const_buffer{value}), op_(op) {}

  constexpr
  explicit option(content value) noexcept
    : data_(static_cast<unsigned_type>(value)),
      op_(number::content_format) {}

  constexpr
  explicit option(accept value) noexcept
    : data_(static_cast<unsigned_type>(value)),
      op_(number::accept) {}

  [[nodiscard]] constexpr number
  option_number() const noexcept {
    return op_;
  }

  [[nodiscard]] constexpr std::size_t
  header_size(number previous) const noexcept {
    std::size_t size = 1;
    std::size_t diff = coap_te::core::to_underlying(op_) -
                coap_te::core::to_underlying(previous);

    for (std::size_t s : {diff, data_size()}) {
      if (s >= 269)
        size +=  2;
      else if (s >= 13)
        size += 1;
    }
    return size;
  }

  [[nodiscard]] constexpr std::size_t
  data_size() const noexcept {
    return std::visit(coap_te::core::overloaded {
      [](std::monostate) { return std::size_t(0); },
      [](empty_format) { return std::size_t(0); },
      [](unsigned_type data) {
        return coap_te::core::small_big_endian_size(data);
      },
      [](const coap_te::const_buffer&  data) { return data.size(); }
    }, data_);
  }

  [[nodiscard]] constexpr std::size_t
  size(number previous) const noexcept {
    return header_size(previous) + data_size();
  }

  [[nodiscard]] constexpr unsigned_type
  get_unsigned() const noexcept {
    return std::visit(coap_te::core::overloaded {
      [](auto) { return invalid_unsigned; },
      [](unsigned_type data) {
        return coap_te::core::from_small_big_endian<unsigned_type>(
                          reinterpret_cast<const std::uint8_t*>(&data),
                          coap_te::core::small_big_endian_size(data));
      },
      [](const const_buffer& data) {
        return coap_te::core::from_small_big_endian<unsigned_type>(
                            reinterpret_cast<const std::uint8_t*>(data.data()),
                            data.size());
      }
    }, data_);
  }

  [[nodiscard]] constexpr const void*
  data() const noexcept {
    return std::visit(coap_te::core::overloaded {
      [](auto) -> const void* { return nullptr; },
      [](const unsigned_type& data) {
        return reinterpret_cast<const void*>(&data);
      },
      [](const coap_te::const_buffer&  data) {
        return data.data();
      }
    }, data_);
  }

  [[nodiscard]] constexpr const value_type&
  raw_data() const noexcept {
    return data_;
  }

  /**
   * @brief Check if options is valid
   * 
   * @note Not the best API, but for now necessary for no-throw code
   * 
   * @return true Is in a valid state
   * @return false Is not in a not valid
   */
  // [[nodiscard]] constexpr bool
  // is_valid() const noexcept {
  //   return op_ != number::invalid;
  // }

  [[nodiscard]] constexpr bool
  operator==(const option& op) const noexcept {
    return op_ == op.op_;
  }

  [[nodiscard]] constexpr bool
  operator<(const option& op) const noexcept {
    return op_ < op.op_;
  }

  [[nodiscard]] friend constexpr bool
  operator==(const option& op, number num) noexcept {
    return op.op_ == num;
  }

  [[nodiscard]] friend constexpr bool
  operator<(const option& op, number num) noexcept {
    return op.op_ < num;
  }

 private:
  value_type  data_;
  number      op_  = number::invalid;
};

/**
 * @brief 
 * 
 */
class option_view {
 public:
  using unsigned_type = unsigned;
  using value_type = coap_te::const_buffer;

  static constexpr unsigned_type
  invalid_unsigned = std::numeric_limits<unsigned_type>::max();

  constexpr
  option_view() noexcept = default;

  constexpr
  explicit option_view(number op) noexcept
    : op_(op) {}

  constexpr
  option_view(number op, unsigned_type& value) noexcept
    : op_(op) {
    auto [v, size] = coap_te::core::to_small_big_endian(value);
    value = v;
    data_ = {&value, size};
  }

  constexpr
  option_view(number op, std::string_view str) noexcept
    : data_{str}, op_(op) {}

  constexpr
  option_view(number op, const const_buffer& value) noexcept
    : data_{value}, op_(op) {}

  constexpr
  explicit option_view(content& value) noexcept
    : op_{number::content_format} {
      auto [v, size] = coap_te::core::to_small_big_endian(coap_te::core::to_underlying(value));
      value = static_cast<content>(v);
      data_ = {&value, size};
  }

  constexpr
  explicit option_view(accept& value) noexcept
    : op_{number::accept} {
    auto [v, size] = coap_te::core::to_small_big_endian(coap_te::core::to_underlying(value));
    value = static_cast<accept>(v);
    data_ = {&value, size};
  }

  [[nodiscard]] constexpr number
  option_number() const noexcept {
    return op_;
  }

  [[nodiscard]] constexpr std::size_t
  header_size(number previous) const noexcept {
    std::size_t size = 1;
    std::size_t diff = coap_te::core::to_underlying(op_) -
                coap_te::core::to_underlying(previous);

    for (std::size_t s : {diff, data_size()}) {
      if (s >= 269)
        size +=  2;
      else if (s >= 13)
        size += 1;
    }
    return size;
  }

  [[nodiscard]] constexpr std::size_t
  data_size() const noexcept {
    return data_.size();
  }

  [[nodiscard]] constexpr std::size_t
  size(number previous) const noexcept {
    return header_size(previous) + data_size();
  }

  [[nodiscard]] constexpr unsigned_type
  get_unsigned() const noexcept {
    return coap_te::core::from_small_big_endian<unsigned_type>(
                            static_cast<const std::uint8_t*>(data_.data()),
                            data_.size());
  }

  [[nodiscard]] constexpr const void*
  data() const noexcept {
    return data_.data();
  }

  [[nodiscard]] constexpr const value_type&
  raw_data() const noexcept {
    return data_;
  }

  /**
   * @brief Check if options is valid
   * 
   * @note Not the best API, but for now necessary for no-throw code
   * 
   * @return true Is in a valid state
   * @return false Is not in a not valid
   */
  // [[nodiscard]] constexpr bool
  // is_valid() const noexcept {
  //   return op_ != number::invalid;
  // }

  [[nodiscard]] constexpr bool
  operator==(const option_view& op) const noexcept {
    return op_ == op.op_;
  }

  [[nodiscard]] constexpr bool
  operator<(const option_view& op) const noexcept {
    return op_ < op.op_;
  }

  [[nodiscard]] friend constexpr bool
  operator==(const option_view& op, number num) noexcept {
    return op.op_ == num;
  }

  [[nodiscard]] friend constexpr bool
  operator<(const option_view& op, number num) noexcept {
    return op.op_ < num;
  }

 private:
  value_type  data_;
  number      op_  = number::invalid;
};

/*
 * The functions above are a way to create options
 * in a more checkable way. You can check if the options
 * is of the correc type and in with lengths permited by
 * the option.
 * 
 * There is also the possibility to throw a exception when
 * the requiriments are not met.
 */


template<typename CheckOptions = check_all,
         bool ToThrow = false>
[[nodiscard]] constexpr option
create(number) noexcept(!ToThrow);

template<typename CheckOptions = check_all,
         bool ToThrow = false>
[[nodiscard]] constexpr option
create(number, option::unsigned_type) noexcept(!ToThrow);

template<typename CheckOptions = check_all,
         bool ToThrow = false>
[[nodiscard]] constexpr option
create(number, std::string_view) noexcept(!ToThrow);

template<typename CheckOptions = check_all,
         bool ToThrow = false>
[[nodiscard]] constexpr option
create(number, const const_buffer&) noexcept(!ToThrow);

}  // namespace options
}  // namespace message
}  // namespace coap_te

#include "impl/option.ipp"

#endif  // COAP_TE_MESSAGE_OPTIONS_OPTION_HPP_
