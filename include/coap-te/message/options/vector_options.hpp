/**
 * @file vector_options.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-08
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef COAP_TE_MESSAGE_OPTIONS_VECTOR_OPTIONS_HPP_
#define COAP_TE_MESSAGE_OPTIONS_VECTOR_OPTIONS_HPP_

#include "coap-te/core/error.hpp"
#include "coap-te/core/traits.hpp"
#include "coap-te/buffer/const_buffer.hpp"
#include "coap-te/message/config.hpp"
#include "coap-te/message/options/option.hpp"
#include "coap-te/message/options/parse.hpp"
#include "coap-te/message/options/option_func.hpp"
#include "coap-te/message/options/traits.hpp"

namespace coap_te {
namespace message {
namespace options {

template<typename Option = option_view>
class vector_options {
 public:
  static_assert(is_raw_option_v<Option>, "Must be option type");

  using value_type = Option;

  class const_iterator {
   public:
    using value_type = vector_options::value_type;

    constexpr
    const_iterator(const const_iterator& buf) noexcept = default;

    constexpr
    explicit const_iterator(const const_buffer& buf) noexcept
      : buf_(buf) {}

    constexpr
    const_iterator(number_type prev, const const_buffer& buf) noexcept
      : prev_(prev), buf_(buf) {}

    constexpr const_iterator&
    operator++() noexcept {
      value_type op;
      coap_te::error_code ec;
      auto size = parse(prev_, buf_, op, ec);
      buf_ += size;
      prev_ = static_cast<number_type>(op.option_number());

      return *this;
    }

    constexpr const_iterator
    operator++(int) noexcept {
      const_iterator temp(*this);
      this->operator++();
      return temp;
    }

    [[nodiscard]] constexpr bool
    is_end() noexcept {
      return buf_.size() == 0 ||
             *static_cast<const std::uint8_t*>(buf_.data()) ==
             coap_te::message::payload_marker;
    }

    [[nodiscard]] constexpr
    operator bool() noexcept {
      return is_end();
    }

    [[nodiscard]] constexpr value_type
    operator*() noexcept {
      value_type op;
      coap_te::error_code ec;
      coap_te::const_buffer copy_buf(buf_);
      parse(prev_, copy_buf, op, ec);

      return op;
    }

    [[nodiscard]] constexpr const const_buffer&
    buffer() const noexcept {
      return buf_;
    }

   private:
    number_type prev_ = invalid;
    coap_te::const_buffer buf_;
  };

  constexpr
  vector_options() noexcept = default;

  constexpr
  vector_options(const void* data, std::size_t size) noexcept
    : buf_{data, size} {}

  constexpr void
  update(const const_buffer& buf) noexcept {
    buf_ = buf;
  }

  [[nodiscard]] constexpr std::size_t
  size() const noexcept {
    return buf_.size();
  }

  [[nodiscard]] constexpr std::size_t
  count() const noexcept {
    std::size_t s = 0;
    for (auto it = begin(); it != end(); ++it)
      ++s;
    return s;
  }

  [[nodiscard]] constexpr bool
  empty() const noexcept {
    return buf_.size() == 0;
  }

  [[nodiscard]] constexpr value_type
  front() const noexcept {
    return *begin();
  }

  [[nodiscard]] constexpr const_iterator
  get(number n, std::size_t index = 0) const noexcept {
    for (auto it = begin(); it; ++it) {
      auto op = *it;
      if (op < n) {
        continue;
      }
      if (op == n) {
        if (index == 0) {
          return it;
        }
        --index;
      }
    }
    return const_iterator({nullptr, 0});
  }

  // iterator interface
  [[nodiscard]] constexpr const_iterator
  begin() const noexcept {
    return const_iterator(buf_);
  }

  [[nodiscard]] constexpr const_iterator
  end() const noexcept {
    return const_iterator(
      const_buffer((const std::uint8_t*)buf_.data() + buf_.size(), 0));
  }

 private:
  coap_te::const_buffer buf_;
};

/**
 * @brief Checks if a type is a vector_options type
 */
template<typename>
struct is_option_vector : std::false_type{};

template<typename Option>
struct is_option_vector<vector_options<Option>> : std::true_type{};

template<typename T>
static constexpr bool
is_option_vector_v = is_option_vector<T>::value;

}  // namespace options
}  // namespace message
}  // namespace coap_te

#endif  // COAP_TE_MESSAGE_OPTIONS_VECTOR_OPTIONS_HPP_
