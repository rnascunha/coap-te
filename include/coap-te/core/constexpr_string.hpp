/**
 * @file constexpr_string.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-01-21
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef COAP_TE_CORE_CONSTEXPR_STRING_HPP_
#define COAP_TE_CORE_CONSTEXPR_STRING_HPP_

#include <cstdlib>

namespace coap_te {

[[nodiscard]] constexpr std::size_t
strlen(const char* data) noexcept {
  const char* d = data;
  while (*d != '\0') ++d;
  return d - data;
}

}  // namespace coap_te

#endif  // COAP_TE_CORE_CONSTEXPR_STRING_HPP_
