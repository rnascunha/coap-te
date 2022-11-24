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

template<class T>
struct is_buffer_type_impl
{
    template<class U>
    static auto test(U*) 
      -> std::tuple<
          decltype(static_cast<const void*>(
            std::declval<U&>().data())), 
            decltype(std::declval<U&>().size())
          >;
    template<typename>
    static auto test(...) -> std::false_type;

    using type = typename std::is_same<
                            std::tuple<const void*, std::size_t>, 
                            decltype(test<T>(0))>::type;
};

}  // namespace detail 
}  // namespace cre
}  // namespace coap_te

#endif  // COAP_TE_CORE_IMPL_TRAIT_IPP_