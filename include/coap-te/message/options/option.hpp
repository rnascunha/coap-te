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

#if COAP_TE_ENABLE_DYNAMIC_ALLOC_SUPPORT == 1
#include <vector>
#endif  // COAP_TE_ENABLE_DYNAMIC_ALLOC_SUPPORT == 1

#include "coap-te/core/error.hpp"
#include "coap-te/core/utility.hpp"
#include "coap-te/core/byte_order.hpp"
#include "coap-te/core/sorted_no_alloc_list.hpp"
#include "coap-te/buffer/buffer.hpp"
#include "coap-te/message/options/config.hpp"
#include "coap-te/message/options/checks.hpp"

namespace coap_te {
namespace message {
namespace options {

struct empty_format{};

template<typename Derived>
class option_base {
 public:
  using unsigned_type = unsigned;

  constexpr
  option_base() noexcept = default;

  constexpr
  explicit option_base(number op) noexcept
    : op_{op} {}

  [[nodiscard]] constexpr number
  option_number() const noexcept {
    return op_;
  }

  [[nodiscard]] constexpr friend bool
  operator==(const option_base& lhs, const option_base& rhs) noexcept {
    return lhs.op_ == rhs.op_;
  }

  [[nodiscard]] constexpr friend bool
  operator<(const option_base& lhs, const option_base& rhs) noexcept {
    return lhs.op_ < rhs.op_;
  }

  [[nodiscard]] constexpr friend bool
  operator==(const option_base& op, number num) noexcept {
    return op.op_ == num;
  }

  [[nodiscard]] constexpr friend bool
  operator<(const option_base& op, number num) noexcept {
    return op.op_ < num;
  }

 private:
  number op_ = number::invalid;
};

/**
 * @brief Checks if a type is of type option
 */
template<typename, typename = void>
struct is_raw_option : std::false_type{};

template<typename T>
struct is_raw_option<T> : std::bool_constant<
    std::is_base_of<option_base<coap_te::core::remove_cvref_t<T>>,
                    coap_te::core::remove_cvref_t<T>>::value>{};

template<typename Option>
static constexpr bool
is_raw_option_v = is_raw_option<Option>::value;

/**
 * @brief 
 * 
 */
class option : public option_base<option> {
 public:
  using value_type = std::variant<
                      std::monostate,
                      empty_format,
                      unsigned_type,
                      coap_te::const_buffer>;

  constexpr
  option() noexcept = default;

  constexpr
  explicit option(number op) noexcept
    : option_base{op}, data_(empty_format{}) {}

  constexpr
  option(number op, unsigned_type value) noexcept
    : option_base{op},
      data_(coap_te::core::to_small_big_endian(value).first)
  {}

  constexpr
  option(number op, std::string_view str) noexcept
    : option_base{op}, data_(coap_te::buffer(str)) {}

  constexpr
  option(number op, const const_buffer& value) noexcept
    : option_base{op}, data_{value} {}

  constexpr
  explicit option(content value) noexcept
    : option_base{number::content_format},
      data_(static_cast<unsigned_type>(value)) {}

  constexpr
  explicit option(accept value) noexcept
    : option_base{number::accept},
      data_(static_cast<unsigned_type>(value)) {}

  template<typename Op,
           typename = std::enable_if_t<!std::is_same_v<Op, option> &&
                                        is_raw_option_v<Op>>>
  option(const Op& op) noexcept       // NOLINT
    : option_base{op.option_number()},
      data_{coap_te::const_buffer{op.data(), op.data_size()}} {}

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

 private:
  value_type  data_;
};

/**
 * @brief 
 */
class option_view : public option_base<option_view> {
 public:
  using value_type = coap_te::const_buffer;

  constexpr
  option_view() noexcept = default;

  constexpr
  explicit option_view(number op) noexcept
    : option_base{op} {}

  constexpr
  option_view(number op, unsigned_type& value) noexcept
    : option_base{op} {
    auto [v, size] = coap_te::core::to_small_big_endian(value);
    value = v;
    data_ = {&value, size};
  }

  constexpr
  option_view(number op, std::string_view str) noexcept
    : option_base{op}, data_{coap_te::buffer(str)} {}

  constexpr
  option_view(number op, const const_buffer& value) noexcept
    : option_base{op}, data_{value} {}

  constexpr
  explicit option_view(content& value) noexcept   // NOLINT
    : option_base{number::content_format} {
      auto [v, size] = coap_te::core::to_small_big_endian(
                        coap_te::core::to_underlying(value));
      value = static_cast<content>(v);
      data_ = {&value, size};
  }

  constexpr
  explicit option_view(accept& value) noexcept    // NOLINT
    : option_base{number::accept} {
    auto [v, size] = coap_te::core::to_small_big_endian(
                          coap_te::core::to_underlying(value));
    value = static_cast<accept>(v);
    data_ = {&value, size};
  }

  template<typename Op,
           typename = std::enable_if_t<!std::is_same_v<Op, option> &&
                                        is_raw_option_v<Op>>>
  explicit option_view(const Op& op) noexcept
    : option_base{op.option_number()}, data_{op.data(), op.data_size()}
  {}

  [[nodiscard]] constexpr std::size_t
  data_size() const noexcept {
    return data_.size();
  }

  [[nodiscard]] constexpr const void*
  data() const noexcept {
    return data_.data();
  }

 private:
  value_type  data_;
};

#if COAP_TE_ENABLE_DYNAMIC_ALLOC_SUPPORT == 1
/**
 * @brief 
 * 
 */
class option_container : public option_base<option_container> {
 public:
  using value_type = std::vector<std::uint8_t>;

  option_container() noexcept = default;

  explicit option_container(number op) noexcept
    : option_base{op} {}

  option_container(number op, unsigned_type value) noexcept
    : option_base{op} {
    auto [v, size] = coap_te::core::to_small_big_endian(value);
    std::uint8_t* ptr = reinterpret_cast<uint8_t*>(&v);
    data_ = {ptr, ptr + size};
  }

  option_container(number op, std::string_view str) noexcept
    : option_base{op}, data_{str.begin(), str.end()} {}

  option_container(number op, const const_buffer& value) noexcept
    : option_base{op},
      data_{reinterpret_cast<const std::uint8_t*>(value.data()),
            reinterpret_cast<const std::uint8_t*>(value.data()) + value.size()},
  {}

  explicit option_container(content value) noexcept   // NOLINT
    : option_base{number::content_format} {
      auto [v, size] = coap_te::core::to_small_big_endian(
                        coap_te::core::to_underlying(value));
      std::uint8_t* ptr = reinterpret_cast<uint8_t*>(&v);
      data_ = {ptr, ptr + size};
  }

  explicit option_container(accept value) noexcept    // NOLINT
    : option_base{number::accept} {
    auto [v, size] = coap_te::core::to_small_big_endian(
                          coap_te::core::to_underlying(value));
    std::uint8_t* ptr = reinterpret_cast<uint8_t*>(&v);
    data_ = {ptr, ptr + size};
  }

  template<typename Op,
           typename = std::enable_if_t<!std::is_same_v<Op, option> &&
                                        is_raw_option_v<Op>>>
  explicit option_container(const Op& op) noexcept
    : option_base{op.option_number()},
      data_{reinterpret_cast<const std::uint8_t*>(op.data()),
            reinterpret_cast<const std::uint8_t*>(op.data()) + op.data_size()}
  {}

  [[nodiscard]] std::size_t
  data_size() const noexcept {
    return data_.size();
  }

  [[nodiscard]] const void*
  data() const noexcept {
    return data_.data();
  }

 private:
  value_type  data_;
};

#endif  // COAP_TE_ENABLE_DYNAMIC_ALLOC_SUPPORT == 1

/**
 * @brief Comparsion funtions between different options
 * 
 */
template<typename Option1,
         typename Option2,
         typename = std::enable_if_t<is_raw_option_v<Option1> &&
                                     is_raw_option_v<Option2>>>
[[nodiscard]] constexpr bool
operator==(const Option1& lhs, const Option2& rhs) noexcept {
  return lhs.option_number() == rhs.option_number();
}

template<typename Option1,
         typename Option2,
         typename = std::enable_if_t<is_raw_option_v<Option1> &&
                                     is_raw_option_v<Option2>>>
[[nodiscard]] constexpr bool
operator<(const Option1& lhs, const Option2& rhs) noexcept {
  return lhs.option_number() < rhs.option_number();
}

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
