/**
 * @file traits.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-19
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef COAP_TE_MESSAGE_TRAITS_HPP_
#define COAP_TE_MESSAGE_TRAITS_HPP_

#include <type_traits>

#include "coap-te/core/sorted_no_alloc_list.hpp"
#include "coap-te/message/message.hpp"
#include "coap-te/message/options/option.hpp"

namespace coap_te {
namespace message {

/**
 * @brief Checks if a type is a message
 */
template<typename>
struct is_message : std::false_type{};

template<typename O, typename T, typename P>
struct is_message<message<O, T, P>> : std::true_type{};

template<typename T>
static constexpr bool
is_message_v = is_message<T>::value;

}  // namespace message
}  // namespace coap_te

#endif  // COAP_TE_MESSAGE_TRAITS_HPP_
