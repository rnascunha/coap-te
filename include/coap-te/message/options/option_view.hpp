/**
 * @file option_view.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-11-22
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef COAP_TE_MESSAGE_OPTIONS_OPTION_VIEW_HPP_
#define COAP_TE_MESSAGE_OPTIONS_OPTION_VIEW_HPP_

#include <variant>

#include "coap-te/message/options/config.hpp"
#include "coap-te/core/const_buffer.hpp"


namespace coap_te {
namespace message {
namespace options {

struct empty_format{};

class option_view {
 public:
  using value_type = std::variant<
                      std::monostate,
                      empty_format,
                      unsigned,
                      coap_te::const_buffer>;

 private:
  number      op;
  value_type  data_;
};

}  // namespace options
}  // namespace message
}  // namespace coap_te

#endif  // COAP_TE_MESSAGE_OPTIONS_OPTION_VIEW_HPP_
