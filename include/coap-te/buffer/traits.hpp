/**
 * @file traits.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-28
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef COAP_TE_BUFFER_TRAITS_HPP_
#define COAP_TE_BUFFER_TRAITS_HPP_

#include <tuple>
#include <type_traits>

namespace coap_te {

namespace detail {

template<class T, class R>
struct is_buffer_impl {
    template<class U, class V>
    static auto test(U*)    // NOLINT
      -> std::tuple<
          decltype(static_cast<V>(
            std::declval<U&>().data())),
            decltype(std::declval<U&>().size())
          >;
    template<typename, typename>
    static auto test(...) -> std::false_type;

    using type = typename std::is_same<
                            std::tuple<R, std::size_t>,
                            decltype(test<T, R>(0))>::type;
};

}  // namespace detail

/**
 * @brief Checks if T is of buffer type
 * 
 * Here, buffer type is defined as class that have defined the
 * _data_ and _size_ method.
 * 
 * @tparam T type to check if is buffer
 */
template<typename T>
struct is_const_buffer :
  detail::is_buffer_impl<T, const void*>::type{};

/**
 * @brief Helper template of @ref is_buffer_type
 * 
 * @tparam T type to check if is buffer
 */
template<typename T>
static constexpr bool
is_const_buffer_v = is_const_buffer<T>::value;

/**
 * @brief Checks if T is of buffer type
 * 
 * Here, buffer type is defined as class that have defined the
 * _data_ and _size_ method.
 * 
 * @tparam T type to check if is buffer
 */
template<typename T>
struct is_mutable_buffer :
  detail::is_buffer_impl<T, void*>::type{};

/**
 * @brief Helper template of @ref is_buffer_type
 * 
 * @tparam T type to check if is buffer
 */
template<typename T>
static constexpr bool
is_mutable_buffer_v = is_mutable_buffer<T>::value;

}  // namespace coap_te

#endif  // COAP_TE_BUFFER_TRAITS_HPP_
