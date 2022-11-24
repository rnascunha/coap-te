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
#include <string_view>
#include <utility>

#include "coap_te/message/options/config.hpp"

namespace coap_te {
namespace message {
namespace options {

struct span {
  const void* data = nullptr;
  std::size_t size = 0;
};

class option_view {
 public:
  using value_type = std::variant<
                      std::monostate,
                      empty_format,
                      unsigned,
                      std::string_view,
                      span>;
 private:
  value_type data_;
};

}  // namespace options
}  // namespace message
}  // namespace coap_te

#endif  // COAP_TE_MESSAGE_OPTIONS_OPTION_VIEW_HPP_
