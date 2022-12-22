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
#include "coap-te/core/error.hpp"
#include "coap-te/core/traits.hpp"
#include "coap-te/core/utility.hpp"
#include "coap-te/core/byte_order.hpp"
#include "coap-te/core/sorted_list.hpp"
#include "coap-te/core/sorted_no_alloc_list.hpp"

// buffers
#include "coap-te/core/mutable_buffer.hpp"
#include "coap-te/core/const_buffer.hpp"

// message
#include "coap-te/message/config.hpp"
#include "coap-te/message/code.hpp"
#include "coap-te/message/serialize.hpp"
#include "coap-te/message/parse.hpp"
#include "coap-te/message/message.hpp"
#include "coap-te/message/token.hpp"
// message/options
#include "coap-te/message/options/config.hpp"
#include "coap-te/message/options/functions.hpp"
#include "coap-te/message/options/checks.hpp"
#include "coap-te/message/options/definitions.hpp"
#include "coap-te/message/options/serialize.hpp"
#include "coap-te/message/options/parse.hpp"
#include "coap-te/message/options/option.hpp"
#include "coap-te/message/options/vector_options.hpp"
#include "coap-te/message/options/option_list_func.hpp"

// alias
#include "coap-te/alias.hpp"

#endif  // COAP_TE_HPP_
