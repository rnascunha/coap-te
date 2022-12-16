/**
 * @file expected.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief A nothrow expect type.
 * @version 0.1
 * @date 2022-12-14
 * 
 * Based on @link https://en.cppreference.com/w/cpp/utility/expected
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef COAP_TE_CORE_EXPECTED_HPP_
#define COAP_TE_CORE_EXPECTED_HPP_

#include <variant>
#include <system_error>   // NOLINT

namespace coap_te {

template<typename T, typename E = std::error_code>
class expected {
 public:
  constexpr
  explict expected(T&& value) noexcept
    : value_(value) {}

  constexpr
  explicit expected(E&& error) noexcept
    : value_(error) {}

  constexpr
  bool has_value() const noexcept {
    std::holds_alternative<T>(value_);
  }

  // Value
  constexpr T& value() noexcept& {
    return std::get<T>(value_);
  }

  constexpr const T& value() const noexcept& {
    return std::get<T>(value_);
  }

  constexpr T&& value() noexcept&& {
    return std::get<T>(value_);
  }

  constexpr const T& value() const noexcept&& {
    return std::get<T>(value_);
  }

  // Error
  constexpr E& error() noexcept& {
    return std::get<E>(value_);
  }

  constexpr const E& error() const noexcept& {
    return std::get<T>(value_);
  }

  constexpr E&& error() noexcept&& {
    return std::get<E>(value_);
  }

  constexpr const E& error() const noexcept&& {
    return std::get<E>(value_);
  }

  // operators
  constexpr
  operator bool() const noexcept {
    return has_value();
  }

  constexpr
  T& operator*() noexcept {
    return value();
  }

  const T& operator*() const noexcept {
    return value();
  }

  constexpr
  T* operator*() noexcept {
    return has_value() ? &value() : nullptr;
  }

  const T* operator*() const noexcept {
    return has_value() ? &value() : nullptr;
  }

 private:
  std::variant<T, E> value_;
};

}  // namespace coap_te

#endif  // COAP_TE_CORE_EXPECTED_HPP_
