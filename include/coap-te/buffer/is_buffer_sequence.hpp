/**
 * @file is_buffer_sequence.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-28
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef COAP_TE_BUFFER_IS_BUFFER_SEQUENCE_HPP_
#define COAP_TE_BUFFER_IS_BUFFER_SEQUENCE_HPP_

#include "coap-te/buffer/buffer_sequence.hpp"

namespace coap_te {
namespace detail {

template<typename>
char buffer_sequence_begin_helper(...);

template<typename T>
char (&buffer_sequence_begin_helper(T* t,
      std::enable_if_t<
        !std::is_same_v<
          decltype(coap_te::buffer_sequence_begin(*t)),
          void>>*))[2];

template<typename>
char buffer_sequence_end_helper(...);

template<typename T>
char (&buffer_sequence_end_helper(T* t,
      std::enable_if_t<
        !std::is_same_v<
          decltype(coap_te::buffer_sequence_end(*t)),
          void>>*))[2];

template<typename, typename>
char (&buffer_sequence_element_type_helper(...))[2];

template<typename T, typename Buffer>
char buffer_sequence_element_type_helper(T* t,
  std::enable_if_t<std::is_convertible_v<
      decltype(*coap_te::buffer_sequence_begin(*t)),
        Buffer>>*);

template<typename T, typename Buffer>
struct is_buffer_sequence_class : std::bool_constant<
  sizeof(buffer_sequence_begin_helper<T>(0, 0)) != 1 &&
  sizeof(buffer_sequence_end_helper<T>(0, 0)) != 1 &&
  sizeof(buffer_sequence_element_type_helper<T, Buffer>(0, 0)) == 1>
{};

template<typename T, typename Buffer>
struct is_buffer_sequence : std::conditional_t<
  std::is_class_v<T>,
  is_buffer_sequence_class<T, Buffer>,
  std::false_type>{};

template <>
struct is_buffer_sequence<mutable_buffer, mutable_buffer>
  : std::true_type{};

template <>
struct is_buffer_sequence<mutable_buffer, const_buffer>
  : std::true_type{};

template <>
struct is_buffer_sequence<const_buffer, const_buffer>
  : std::true_type{};

template <>
struct is_buffer_sequence<const_buffer, mutable_buffer>
  : std::false_type{};

}  // namespace detail

template<typename T>
struct is_mutable_buffer_sequence
  : detail::is_buffer_sequence<T, mutable_buffer>{};

template<typename T>
static constexpr bool
is_mutable_buffer_sequence_v = is_mutable_buffer_sequence<T>::value;

template<typename T>
struct is_const_buffer_sequence
  : detail::is_buffer_sequence<T, const_buffer>{};

template<typename T>
static constexpr bool
is_const_buffer_sequence_v = is_const_buffer_sequence<T>::value;

}  // namespace coap_te

#endif  // COAP_TE_BUFFER_IS_BUFFER_SEQUENCE_HPP_
