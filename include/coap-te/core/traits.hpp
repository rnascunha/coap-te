/**
 * @file traits.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief Trait functions used at all library
 * @version 0.1
 * @date 2022-11-21
 * 
 * @copyright Copyright (c) 2022
 * 
 * @defgroup core Core
 * Definitions and inplementations using among all library
 */

#ifndef COAP_TE_CORE_TRAITS_HPP_
#define COAP_TE_CORE_TRAITS_HPP_

#include <type_traits>
#include <iterator>
#include <utility>

#include "impl/traits.ipp"

namespace coap_te {
/**
 * @ingroup core
 */
namespace core {

/** 
 * @defgroup coretraits Type Traits
 * Type traits use on all CoAP-te library
 * 
 * @ingroup core
 * @{
 */

/**
 * @brief Checks if a iterator class is equal or convertible
 * to another category
 * 
 * @tparam Iter iterator class to check if can be converted
 * @tparam IterCategory iterator category required
 */
template<typename Iter, typename IterCategory>
struct is_iterator_type :
    std::bool_constant<
        std::is_convertible_v<
            typename std::iterator_traits<Iter>::iterator_category,
            IterCategory>>
{};

/**
 * @brief Helper template of @ref is_iterator_type
 */
template<typename Iter, typename IterCategory>
static constexpr bool
is_iterator_type_v = is_iterator_type<Iter, IterCategory>::value;

/**
 * @brief Checks if a iterator is bidirectional
 * 
 * @tparam Iter iterator to check
 */
template<typename Iter>
struct is_forward_iterator :
    is_iterator_type<
        Iter,
        std::forward_iterator_tag>
{};

/**
 * @brief Helper template of @ref is_bidirectional_iterator
 * 
 * @tparam Iter 
 */
template<typename Iter>
static constexpr bool
is_forward_iterator_v = is_forward_iterator<Iter>::value;

/**
 * @brief Checks if a iterator is bidirectional
 * 
 * @tparam Iter iterator to check
 */
template<typename Iter>
struct is_bidirectional_iterator :
    is_iterator_type<
        Iter,
        std::bidirectional_iterator_tag>
{};

/**
 * @brief Helper template of @ref is_bidirectional_iterator
 * 
 * @tparam Iter 
 */
template<typename Iter>
static constexpr bool
is_bidirectional_iterator_v = is_bidirectional_iterator<Iter>::value;

/**
 * @brief Checks if a iterator is random access
 * 
 * @tparam Iter iterator to check
 */
template<typename Iter>
struct is_random_access_iterator :
    is_iterator_type<
        Iter,
        std::random_access_iterator_tag>
{};

/**
 * @brief Helper template of @ref is_random_access_iterator
 * 
 * @tparam Iter 
 */
template<typename Iter>
static constexpr bool
is_random_access_iterator_v = is_random_access_iterator<Iter>::value;

/**
 * @brief Checks if types are equal comparable
 * 
 * Checks if operator== is defined to type T and EqualTo
 * 
 * @tparam T        Left hand side of operator==
 * @tparam EqualTo  Right hand side of operator==
 */
template<class T, class EqualTo = T>
struct is_equal_comparable :
  detail::is_equal_comparable_impl<T, EqualTo>::type {};

/**
 * @brief Helper template of @ref is_equal_comparable
 * 
 * @tparam T        Left hand side of operator==
 * @tparam EqualTo  Right hand side of operator==
 */
template<class T, class EqualTo = T>
static constexpr bool
is_equal_comparable_v = is_equal_comparable<T, EqualTo>::value;

/**
 * @brief Checks if types are less comparable
 * 
 * Checks if operator< is defined to type T and EqualTo
 * 
 * @tparam T        Left hand side of operator<
 * @tparam LessTo   Right hand side of operator<
 */
template<class T, class LessTo = T>
struct is_less_comparable :
  detail::is_less_comparable_impl<T, LessTo>::type {};

/**
 * @brief Helper template of @ref is_less_comparable
 * 
 * @tparam T        Left hand side of operator<
 * @tparam LessTo   Right hand side of operator<
 */
template<class T, class LessTo = T>
static constexpr bool
is_less_comparable_v = is_less_comparable<T, LessTo>::value;

/**
 * @brief Checks if a type is instantiated from a specific template
 * 
 * The template arguments must be all bools.
 * 
 * @link https://stackoverflow.com/a/69019297
 */
template <class, template <bool...> class>
struct is_instance : std::false_type {};

template <bool ...Bs, template <bool...> class U>
struct is_instance<U<Bs...>, U> : std::true_type {};

/**
 * @brief Remove reference and const/volatile qualifier from
 * type
 * 
 * @tparam T type to remove qualifies
 */
template< class T >
struct remove_cvref {
    typedef std::remove_cv_t<std::remove_reference_t<T>> type;
};

/**
 * @brief Convenint call to @ref remove_cvref
 */
template<typename T>
using remove_cvref_t = typename remove_cvref<T>::type;

/**
 * @brief Checks if the type is a pair
 * 
 */
template <typename>
struct is_pair : std::false_type {};

template <typename T, typename U>
struct is_pair<std::pair<T, U>> : std::true_type {};

template<typename T>
static constexpr bool
is_pair_v = is_pair<T>::value;

/**
 * @brief Checks if a type is a container
 * 
 * @link https://stackoverflow.com/a/31207079
 */
template<typename, typename = void>
struct is_container : std::false_type {};

template<typename... Ts>
struct is_container_helper {};

template<typename T>
struct is_container<
        T,
        std::conditional_t<
            false,
            is_container_helper<
                typename T::value_type,
                // typename T::size_type,
                // typename T::allocator_type,
                // typename T::iterator,
                // typename T::const_iterator,
                decltype(std::declval<T>().size()),
                decltype(std::declval<T>().begin()),
                decltype(std::declval<T>().end())   //,
                // decltype(std::declval<T>().cbegin()),
                // decltype(std::declval<T>().cend())
                >,
            void
            >
        > : public std::true_type {};

template<typename T>
static constexpr bool
is_container_v = is_container<T>::value;

/**
 * @brief Checks if mapped_type is defined at class
 */
template<typename, typename = void>
struct has_mapped_type : std::false_type{};

template<typename T>
struct has_mapped_type<T, std::void_t<typename T::mapped_type>> :
      std::true_type{};

template<typename T>
static constexpr bool
has_mapped_type_v = has_mapped_type<T>::value;

/**
 * @brief Checks if key_type is defined at class
 */
template<typename, typename = void>
struct has_key_type : std::false_type{};

template<typename T>
struct has_key_type<T, std::void_t<typename T::key_type>> : std::true_type{};

template<typename T>
static constexpr bool
has_key_type_v = has_key_type<T>::value;

/**
 * @brief Check if is a container of map type
 */
template<typename C>
struct is_map : std::bool_constant<
    is_container_v<C> &&
    has_key_type_v<C> &&
    has_mapped_type_v<C>>{};

template<typename T>
static constexpr bool
is_map_v = is_map<T>::value;

/**
 * @brief Check if is a container of set type
 */
template<typename C>
struct is_set : std::bool_constant<
    is_container_v<C> &&
    has_key_type_v<C> &&
    !has_mapped_type_v<C>>{};

template<typename T>
static constexpr bool
is_set_v = is_set<T>::value;

/**
 * @brief Checks if a type is sorted list
 */
template<typename, typename = void>
struct is_sorted_list : std::false_type{};

template<typename T>
struct is_sorted_list<T, std::void_t<decltype(T::is_sorted_list)>> :
        std::bool_constant<T::is_sorted_list>{};

/**
 * @brief If T is a pair, forward the second value, otherwise foward T
 * 
 */
template<typename T, typename = void>
struct value_type_if_pair {
    using type = T;
};

template<typename T>
struct value_type_if_pair<T, std::enable_if_t<is_pair_v<T>>> {
    using type = typename T::second_type;
};

template<typename T>
using value_type_if_pair_t = typename value_type_if_pair<T>::type;

/** @} */  // end of CoreTraits

}  // namespace core
}  // namespace coap_te

#endif  // COAP_TE_CORE_TRAITS_HPP_
