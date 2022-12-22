/**
 * @file error.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-21
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef COAP_TE_CORE_ERROR_HPP_
#define COAP_TE_CORE_ERROR_HPP_

#include <string_view>
#if COAP_TE_ENABLE_STD_ERROR_CODE == 1
#include <system_error>     // NOLINT
#include <string>
#include <type_traits>
#endif  // COAP_TE_ENABLE_STD_ERROR_CODE == 1

namespace coap_te {

static constexpr const char*
category_name = "coap-te";

enum errc {
  // system
  no_buffer_space = 10,
  // options
  invalid_option = 20,
  option_repeated,
  option_type,
  option_length,
};

[[nodiscard]] constexpr std::string_view
error_message(errc ec) noexcept {
#if COAP_TE_ERROR_SHOW_MESSAGE == 1
  switch (ec) {
    case errc::no_buffer_space:
      return "no buffer space";
    case errc::invalid_option:
      return "invalid option";
    case errc::option_repeated:
      return "option repeated error";
    case errc::option_type:
      return "option type error";
    case errc::option_length:
      return "option length error";
    default:
      break;
  }
  return "unknown";
#else  // COAP_TE_ERROR_SHOW_MESSAGE == 1
  return "";
#endif  // COAP_TE_ERROR_SHOW_MESSAGE == 1
}

class error_code {
 public:
  constexpr
  error_code() noexcept = default;

  constexpr
  error_code(errc ec) noexcept                 // NOLINT
    : ec_(ec) {}

  constexpr
  error_code(const error_code& ec) noexcept = default;

  [[nodiscard]] constexpr const char*
  name() const noexcept {
    return category_name;
  }

  [[nodiscard]] constexpr int
  value() const noexcept {
    return static_cast<int>(ec_);
  }

  [[nodiscard]] constexpr errc
  error() const noexcept {
    return ec_;
  }

  [[nodiscard]] constexpr std::string_view
  message() const noexcept {
    return error_message(ec_);
  }

  [[nodiscard]] constexpr bool
  operator==(const error_code& other) const noexcept {
    return ec_ == other.ec_;
  }

  [[nodiscard]] constexpr bool
  operator==(errc ec) const noexcept {
    return ec_ == ec;
  }

  [[nodiscard]] constexpr bool
  operator!=(errc ec) const noexcept {
    return !(ec_ == ec);
  }

  constexpr error_code&
  operator=(errc ec) noexcept {
    ec_ = ec;
    return *this;
  }

  constexpr error_code&
  operator=(const error_code&) noexcept = default;

  [[nodiscard]] constexpr
  operator bool() const noexcept {
    return static_cast<int>(ec_) != 0;
  }

 private:
  errc ec_ = static_cast<errc>(0);
};

}  // namespace coap_te

#if COAP_TE_ENABLE_STD_ERROR_CODE == 1

namespace coap_te {

namespace detail {

struct coap_te_error_category : public std::error_category {
  const char*
  name() const noexcept override {
    return category_name;
  }

  std::string
  message(int ev) const override {
    return std::string{error_message(static_cast<errc>(ev))};
  }
};

const coap_te_error_category the_coap_te_error_category {};

}  // namespace detail

std::error_code make_error_code(errc e) {
  return {static_cast<int>(e), detail::the_coap_te_error_category};
}

}  // namespace coap_te

namespace std {
  template <>
  struct is_error_code_enum<coap_te::errc> : std::true_type {};
}  // namespace std

#endif  // COAP_TE_ENABLE_STD_ERROR_CODE == 1

#endif  // COAP_TE_CORE_ERROR_HPP_
