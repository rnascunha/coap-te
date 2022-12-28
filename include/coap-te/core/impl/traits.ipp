/**
 * @file traits.ipp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-24
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef COAP_TE_CORE_IMPL_TRAITS_IPP_
#define COAP_TE_CORE_IMPL_TRAITS_IPP_

#include <utility>

namespace coap_te {
namespace core {
namespace detail {

/**
 * @brief 
 * 
 * @tparam T 
 * @tparam EqualTo 
 * 
 * @link https://stackoverflow.com/a/35207812
 */
template<class T, class EqualTo>
struct is_equal_comparable_impl {
  template<class U, class V>
  static auto test(U*) -> decltype(std::declval<U>() == std::declval<V>());
  template<typename, typename>
  static auto test(...) -> std::false_type;

  using type = typename std::is_same<bool, decltype(test<T, EqualTo>(0))>::type;
};

template<class T, class LessTo = T>
struct is_less_comparable_impl {
  template<class U, class V>
  static auto test(U*) -> decltype(std::declval<U>() < std::declval<V>());
  template<typename, typename>
  static auto test(...) -> std::false_type;

  using type = typename std::is_same<bool, decltype(test<T, LessTo>(0))>::type;
};

}  // namespace detail
}  // namespace core
}  // namespace coap_te

#endif  // COAP_TE_CORE_IMPL_TRAITS_IPP_
