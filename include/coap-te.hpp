/**
 * @file coap-te.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief Convenint header of CoAP-te library
 * @version 0.1
 * @date 2022-11-21
 * 
 * @copyright Copyright (c) 2022
 * 
 * This header adds all internal headers of the CoAP-te library
 */

#ifndef COAP_TE_HPP_
#define COAP_TE_HPP_

// core
#include "coap-te/core/traits.hpp"
#include "coap-te/core/utility.hpp"
#include "coap-te/core/byte_order.hpp"

// buffers
#include "coap-te/core/mutable_buffer.hpp"
#include "coap-te/core/const_buffer.hpp"

// message
#include "coap-te/message/options/config.hpp"
#include "coap-te/message/options/constants.hpp"
#include "coap-te/message/options/functions.hpp"
#include "coap-te/message/options/definitions.hpp"
#include "coap-te/message/options/to_string.hpp"
#include "coap-te/message/options/serialize.hpp"
#include "coap-te/message/options/parse.hpp"
#include "coap-te/message/options/option.hpp"

#endif  // COAP_TE_HPP_
