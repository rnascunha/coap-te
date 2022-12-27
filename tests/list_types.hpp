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

#include <utility>
#include <vector>
#include <list>
#include <set>
#include <map>

#include "coap-te/core/mutable_buffer.hpp"
#include "coap-te/core/const_buffer.hpp"
#include "coap-te/core/sorted_list.hpp"
#include "coap-te/core/sorted_no_alloc_list.hpp"

#include "coap-te/message/config.hpp"
#include "coap-te/message/code.hpp"
#include "coap-te/message/message.hpp"
#include "coap-te/message/token.hpp"
#include "coap-te/message/serialize.hpp"
#include "coap-te/message/options/option.hpp"
#include "coap-te/message/options/vector_options.hpp"

using msg = coap_te::message;
using opts = msg::options;

#define TOKEN_TYPES     msg::token,      \
                        msg::token_view, \
                        coap_te::const_buffer

#define OPTION_TYPES    opts::option, \
                        opts::option_view

#define OPTION_SERIALIZE_LISTS    std::vector,    \
                                  std::list,      \
                                  std::multiset,  \
                                  std::multimap,  \
                                  coap_te::core::sorted_list

#define OPTION_PARSE_LISTS        std::vector,    \
                                  std::list,      \
                                  std::multiset,  \
                                  std::multimap,  \
                                  coap_te::core::sorted_list,  \
                                  opts::vector_options

#endif  // COAP_TE_TESTS_LIST_TYPES_HPP_
