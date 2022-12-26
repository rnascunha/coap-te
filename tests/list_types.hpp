/**
 * @file list_types.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-25
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef COAP_TE_TESTS_LIST_TYPES_HPP_
#define COAP_TE_TESTS_LIST_TYPES_HPP_

#include <vector>
#include <list>
#include <set>
#include <map>

#include "coap-te/core/const_buffer.hpp"
#include "coap-te/core/sorted_list.hpp"
#include "coap-te/core/sorted_no_alloc_list.hpp"

#include "coap-te/message/message.hpp"
#include "coap-te/message/token.hpp"
#include "coap-te/message/options/option.hpp"

#define TOKEN_TYPES     coap_te::message::token,      \
                        coap_te::message::token_view, \
                        coap_te::const_buffer

#define OPTION_TYPES    coap_te::message::options::option \
                        coap_te::message::options::option_view



#endif  // COAP_TE_TESTS_LIST_TYPES_HPP_
