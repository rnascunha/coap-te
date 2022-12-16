/**
 * @file constants.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-07
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef COAP_TE_MESSAGE_CONFIG_HPP_
#define COAP_TE_MESSAGE_CONFIG_HPP_

#include <cstdint>

namespace coap_te {
namespace message {

// https://www.rfc-editor.org/rfc/rfc7252#section-3

static constexpr std::uint8_t version = 0b01;

enum class type : std::uint8_t {
  confirmable     = 0,
  nonconfirmable  = 1,
  ack             = 2,
  reset           = 3
};


static constexpr std::size_t minimum_header_size = 4;
static constexpr std::size_t max_token_size = 8;

using message_id = std::uint16_t;

static constexpr std::uint8_t payload_marker = 0xFF;

}  // namespace message
}  // namespace coap_te

#endif  // COAP_TE_MESSAGE_CONFIG_HPP_
