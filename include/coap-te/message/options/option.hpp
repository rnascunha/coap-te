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
#endif  // COAP_TE_ENABLE_DYNAMIC_ALLOC_SUPPORT = 1

#include "coap-te/core/error.hpp"
#include "coap-te/core/utility.hpp"
#include "coap-te/core/const_buffer.hpp"
#include "coap-te/core/byte_order.hpp"
#include "coap-te/core/sorted_no_alloc_list.hpp"
#include "coap-te/message/options/config.hpp"
#include "coap-te/message/options/checks.hpp"

namespace coap_te {
namespace message {
namespace options {

struct empty_format{};

class option_base {
 public:
  using unsigned_type = unsigned;
  static constexpr unsigned_type
  invalid_unsigned = std::numeric_limits<unsigned_type>::max();

  // virtual number
  // option_number() const noexcept = 0;

  // virtual std::size_t
  // data_size() const noexcept = 0;

  // virtual const void*
  // data() const noexcept = 0;

  // [[nodiscard]] friend bool
  // operator==(const option_base& lhs, const option_base& rhs) noexcept {
  //   return lhs.option_number() == rhs.option_number();
  // }

  // [[nodiscard]] friend bool
  // operator<(const option_base& lhs, const option_base& rhs) noexcept {
  //   return lhs.option_number() < rhs.option_number();
  // }

  // [[nodiscard]] friend bool
  // operator==(const option_base& op, number num) noexcept {
  //   return op.option_number() == num;
  // }

  // [[nodiscard]] friend bool
  // operator<(const option_base& op, number num) noexcept {
  //   return op.option_number() < num;
  // }
};

class option : public option_base {
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

  template<typename Op,
           typename = std::enable_if_t<!std::is_same_v<Op, option> &&
                                        std::is_base_of_v<option_base, Op>>>
  option(const Op& op) noexcept       // NOLINT
    : data_{coap_te::const_buffer{op.data(), op.data_size()}},
      op_{op.option_number()}
  {}

  [[nodiscard]] constexpr number
  option_number() const noexcept {
    return op_;
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

  [[nodiscard]] friend constexpr bool
  operator==(const option& lhs, const option& rhs) noexcept {
    return lhs.option_number() == rhs.option_number();
  }

  [[nodiscard]] friend constexpr bool
  operator<(const option& lhs, const option& rhs) noexcept {
    return lhs.option_number() < rhs.option_number();
  }

  [[nodiscard]] friend constexpr bool
  operator==(const option& op, number num) noexcept {
    return op.option_number() == num;
  }

  [[nodiscard]] friend constexpr bool
  operator<(const option& op, number num) noexcept {
    return op.option_number() < num;
  }

 private:
  value_type  data_;
  number      op_  = number::invalid;
};

/**
 * @brief 
 * 
 */
class option_view : public option_base {
 public:
  using value_type = coap_te::const_buffer;

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
  explicit option_view(content& value) noexcept   // NOLINT
    : op_{number::content_format} {
      auto [v, size] = coap_te::core::to_small_big_endian(
                        coap_te::core::to_underlying(value));
      value = static_cast<content>(v);
      data_ = {&value, size};
  }

  constexpr
  explicit option_view(accept& value) noexcept    // NOLINT
    : op_{number::accept} {
    auto [v, size] = coap_te::core::to_small_big_endian(
                          coap_te::core::to_underlying(value));
    value = static_cast<accept>(v);
    data_ = {&value, size};
  }

  template<typename Op,
           typename = std::enable_if_t<!std::is_same_v<Op, option_view> &&
                                        std::is_base_of_v<option_base, Op>>>
  explicit option_view(const Op& op) noexcept
    : data_{op.data(), op.data_size()}, op_{op.option_number()}
  {}

  [[nodiscard]] constexpr number
  option_number() const noexcept {
    return op_;
  }

  [[nodiscard]] constexpr std::size_t
  data_size() const noexcept {
    return data_.size();
  }

  [[nodiscard]] constexpr const void*
  data() const noexcept {
    return data_.data();
  }

  [[nodiscard]] friend constexpr bool
  operator==(const option_view& lhs, const option_view& rhs) noexcept {
    return lhs.option_number() == rhs.option_number();
  }

  [[nodiscard]] friend constexpr bool
  operator<(const option_view& lhs, const option_view& rhs) noexcept {
    return lhs.option_number() < rhs.option_number();
  }

  [[nodiscard]] friend constexpr bool
  operator==(const option_view& op, number num) noexcept {
    return op.option_number() == num;
  }

  [[nodiscard]] friend constexpr bool
  operator<(const option_view& op, number num) noexcept {
    return op.option_number() < num;
  }

 private:
  value_type  data_;
  number      op_  = number::invalid;
};


#if COAP_TE_ENABLE_DYNAMIC_ALLOC_SUPPORT == 1

/**
 * @brief 
 * 
 */
class option_container : public option_base {
 public:
  using value_type = std::vector<std::uint8_t>;

  option_container() noexcept = default;

  explicit option_container(number op) noexcept
    : op_(op) {}

  option_container(number op, unsigned_type value) noexcept
    : op_(op) {
    auto [v, size] = coap_te::core::to_small_big_endian(value);
    std::uint8_t* ptr = reinterpret_cast<uint8_t*>(&v);
    data_ = {ptr, ptr + size};
  }

  option_container(number op, std::string_view str) noexcept
    : data_{str.begin(), str.end()}, op_(op) {}

  option_container(number op, const const_buffer& value) noexcept
    : data_{reinterpret_cast<const std::uint8_t*>(value.data()),
            reinterpret_cast<const std::uint8_t*>(value.data()) + value.size()},
            op_(op) {}

  explicit option_container(content value) noexcept   // NOLINT
    : op_{number::content_format} {
      auto [v, size] = coap_te::core::to_small_big_endian(
                        coap_te::core::to_underlying(value));
      std::uint8_t* ptr = reinterpret_cast<uint8_t*>(&v);
      data_ = {ptr, ptr + size};
  }

  explicit option_container(accept value) noexcept    // NOLINT
    : op_{number::accept} {
    auto [v, size] = coap_te::core::to_small_big_endian(
                          coap_te::core::to_underlying(value));
    std::uint8_t* ptr = reinterpret_cast<uint8_t*>(&v);
    data_ = {ptr, ptr + size};
  }

  template<typename Op,
           typename = std::enable_if_t<!std::is_same_v<Op, option_container> &&
                                        std::is_base_of_v<option_base, Op>>>
  explicit option_container(const Op& op) noexcept
    : data_{op.data(), op.data_size()}, op_{op.option_number()}
  {}

  [[nodiscard]] constexpr number
  option_number() const noexcept {
    return op_;
  }

  [[nodiscard]] std::size_t
  data_size() const noexcept {
    return data_.size();
  }

  [[nodiscard]] const void*
  data() const noexcept {
    return data_.data();
  }

  [[nodiscard]] friend constexpr bool
  operator==(const option_container& lhs, const option_view& rhs) noexcept {
    return lhs.option_number() == rhs.option_number();
  }

  [[nodiscard]] friend constexpr bool
  operator<(const option_container& lhs, const option_view& rhs) noexcept {
    return lhs.option_number() < rhs.option_number();
  }

  [[nodiscard]] friend constexpr bool
  operator==(const option_container& op, number num) noexcept {
    return op.option_number() == num;
  }

  [[nodiscard]] friend constexpr bool
  operator<(const option_container& op, number num) noexcept {
    return op.option_number() < num;
  }

 private:
  value_type  data_;
  number      op_  = number::invalid;
};

#endif  // COAP_TE_ENABLE_DYNAMIC_ALLOC_SUPPORT == 1

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
