/**
 * @file utility.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief Utility functions used across all library
 * @version 0.1
 * @date 2022-11-21
 * 
 * @copyright Copyright (c) 2022
 * 
 * 
 */

#ifndef COAP_TE_CORE_UTILITY_HPP_
#define COAP_TE_CORE_UTILITY_HPP_

#include "coap-te/core/traits.hpp"

namespace coap_te {
namespace core {

/**
 * @brief
 * 
 * @precondition must be not-decreasing container
 * 
 * @tparam BidiIt 
 * @tparam T 
 * @param begin
 * @param end 
 * @param value 
 * @return constexpr BidiIt 
 */
template<typename RandomAccessIt, typename T>
[[nodiscard]] constexpr RandomAccessIt
binary_search(RandomAccessIt begin, RandomAccessIt end,
              T const& value) noexcept;

}  // namespace core
}  // namespace coap_te

#include "impl/utility.ipp"

#endif  // COAP_TE_CORE_UTILITY_HPP_
