#ifndef COAP_TE_CORE_IMPL_TRAIT_IPP_
#define COAP_TE_CORE_IMPL_TRAIT_IPP_

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

template<class T, class R>
struct is_buffer_impl
{
    template<class U, class V>
    static auto test(U*) 
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
}  // namespace cre
}  // namespace coap_te

#endif  // COAP_TE_CORE_IMPL_TRAIT_IPP_