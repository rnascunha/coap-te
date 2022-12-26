/**
 * @file alias.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-16
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef COAP_TE_ALIAS_HPP_
#define COAP_TE_ALIAS_HPP_

#include "coap-te/core/sorted_list.hpp"
#include "coap-te/core/sorted_no_alloc_list.hpp"
#include "coap-te/message/config.hpp"
#include "coap-te/message/code.hpp"
#include "coap-te/message/message.hpp"
#include "coap-te/message/options/option.hpp"
#include "coap-te/message/options/vector_options.hpp"

namespace coap_te {

// message
using type = coap_te::message::type;
using code = coap_te::message::code;

// options
using number = coap_te::message::options::number;
using accept = coap_te::message::options::accept;
using content = coap_te::message::options::content;

using option = coap_te::message::options::option;

using option_list =
          coap_te::core::sorted_list<option>;
using request = coap_te::message::message<option_list>;

using option_list_response = coap_te::message::options::vector_options<>;
using response = coap_te::message::message<option_list_response>;

namespace no_alloc {

using option_list =
          coap_te::core::sorted_no_alloc_list<option>;
using option = option_list::node;
using request = coap_te::message::message<option_list>;

}  // namespace no_alloc

}  // namespace coap_te

#endif  // COAP_TE_ALIAS_HPP_
