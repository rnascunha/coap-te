/**
 * @file utility.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief Utility functions used across all library
 * @version 0.1
 * @date 2022-11-21
 * 
 * @copyright Copyright (c) 2022
 */

#ifndef COAP_TE_CORE_UTILITY_HPP_
#define COAP_TE_CORE_UTILITY_HPP_

#include <cstdint>
#include <utility>

#include "coap-te/core/traits.hpp"

namespace coap_te {
namespace core {

/**
 * @defgroup utility Utility
 * Utility functions used all over CoAP-te library
 * 
 * @ingroup core
 * @{
 */

/**
 * @brief Computer de distrance of 2 pointer in bytes
 * 
 * @warning begin must be smaller or equal to end. No checks
 * are done.
 * 
 * @param begin pointer at the begin of the interval
 * @param end pointer at the end of the interval
 * @return std::size_t distance of the pointers
 */
[[nodiscard]] inline std::size_t
pointer_distance(const void* begin,
                 const void* end) noexcept {
  return static_cast<std::size_t>(
    reinterpret_cast<const std::uint8_t*>(end) -
    reinterpret_cast<const std::uint8_t*>(begin));
}

/**
 * @brief Makes a binary search in a container
 * 
 * @note The container must be sorted strictly increasing
 * @note The defered value of the iterator must be comparable with T
 * 
 * @tparam RandomAccessIt Iterator type (must be random access iterator)
 * @tparam T    Type of value to be searched
 * 
 * @param begin Begining of the container
 * @param end   End of the container
 * @param value Value to be searched
 * @return      the iterator to the value found or end
 */
template<typename RandomAccessIt, typename T>
[[nodiscard]] constexpr RandomAccessIt
binary_search(RandomAccessIt begin, RandomAccessIt end,
              T const& value) noexcept;

/**
 * @brief Converts enum to underlying type
 * 
 * @link https://stackoverflow.com/a/8357462
 * 
 * @tparam E enum type to be converted
 * @param e enum value to be converted
 * @return std::underlying_type<E>::type underlying type of enum
 */
template <typename E>
constexpr typename std::underlying_type<E>::type
to_underlying(E e) noexcept {
    return static_cast<typename std::underlying_type<E>::type>(e);
}

/**
 * @brief Trait to overload lambadas to be used with std::variant/std::visit
 * 
 * @tparam Ts Lambadas types tha will be overloaded
 */
template<class... Ts>
struct overloaded : Ts... {
  using Ts::operator()...;
};

template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

/**
 * @brief 
 * 
 * @tparam Type 
 * @param t 
 * @return constexpr auto 
 */
template<typename Type>
constexpr decltype(auto)
forward_second_if_pair(Type&& t) noexcept {
  if constexpr (!is_pair_v<std::decay_t<Type>>)
    return std::forward<Type>(t);
  else
    return t.second;
}

/** @} */  // end of utility group

}  // namespace core
}  // namespace coap_te

#include "impl/utility.ipp"

#endif  // COAP_TE_CORE_UTILITY_HPP_
