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
#ifndef COAP_TE_MESSAGE_OPTIONS_TRAITS_HPP_
#define COAP_TE_MESSAGE_OPTIONS_TRAITS_HPP_

#include <type_traits>

#include "coap-te/core/traits.hpp"
#include "coap-te/core/utility.hpp"

namespace coap_te {
namespace message {
namespace options {

/**
 * @brief Checks if a type is a option
 * 
 */
template<typename Option>
struct is_option
  : std::bool_constant<
    std::is_same_v<options::option, coap_te::core::remove_cvref_t<Option>> ||
    std::is_same_v<
      coap_te::core::sorted_no_alloc_list
        <options::option>::node,
        coap_te::core::remove_cvref_t<Option>>>{};

template<typename Option>
static constexpr bool
is_option_v = is_option<Option>::value;

/**
 * @brief Checks if a type is a container of options
 * 
 */
template<typename T>
struct is_option_list :
  std::bool_constant<
    coap_te::core::is_container_v<T> &&
    is_option_v<
      coap_te::core::value_type_if_pair_t<
        typename T::value_type
      >
    >
  >{};

template<typename T>
static constexpr bool
is_option_list_v = is_option_list<T>::value;

}  // namespace options
}  // namespace message
}  // namespace coap_te

#endif  // COAP_TE_MESSAGE_OPTIONS_TRAITS_HPP_
