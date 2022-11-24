/**
 * @file formats.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-11-22
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef COAP_TE_MESSAGE_OPTIONS_FORMATS_HPP_
#define COAP_TE_MESSAGE_OPTIONS_FORMATS_HPP_

#include <error_code>

#include "coap-te/message/options/config.hpp"

namespace coap_te {
namespace message {
namespace options {

template<typename Derived, format Format>
class base {
 public:
  static constexpr format op_type = Format;

  template<typename Buffer>
  virtual size_format
  serialize(number, Buffer, error_code&) const noexcept = 0;
  template<typename Buffer>
  virtual size_format
  serialize(number, Buffer) const = 0;

  number_type
  number() const noexcept {
    return number_;
  }
 private:
  number_type number_;
};

/**
 * @brief Type that defines a empty option value
 * 
 */
struct empty_format {
  static constexpr format op_type = format::empty;
};

struct opaque_format {
  static constexpr format op_type = format::opaque;
};

struct uint_format {
  static constexpr format op_type = format::uint;
};

struct string_format {
  static constexpr format op_type = format::string;
};

}  // namespace options
}  // namespace message
}  // namespace coap_te

#endif  // COAP_TE_MESSAGE_OPTIONS_FORMATS_HPP_
