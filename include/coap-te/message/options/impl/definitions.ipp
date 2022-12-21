/**
 * @file definitions.ipp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-20
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef COAP_TE_MESSAGE_OPTIONS_IMPL_DEFINITIONS_IPP_
#define COAP_TE_MESSAGE_OPTIONS_IMPL_DEFINITIONS_IPP_

#include "coap-te/core/utility.hpp"

namespace coap_te {
namespace message {
namespace options {

[[nodiscard]] constexpr const definition&
get_definition(number n) noexcept {
  auto it = ::coap_te::core::binary_search(
                std::begin(definitions), std::end(definitions), n);
  if (it == std::end(definitions))
    return definitions[0];
  return *it;
}

}   // namespace options
}   // namespace message
}   // namespace coap_te

#endif  // COAP_TE_MESSAGE_OPTIONS_IMPL_DEFINITIONS_IPP_
