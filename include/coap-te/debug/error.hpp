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
#ifndef COAP_TE_DEBUG_ERROR_HPP_
#define COAP_TE_DEBUG_ERROR_HPP_

#include <iostream>
#include "coap-te/core/error.hpp"

std::ostream&
operator<<(std::ostream& os,
           const coap_te::error_code& op) noexcept {
  os << op.name() << ':' << op.value();
  return os;
}

#endif  // COAP_TE_DEBUG_ERROR_HPP_
