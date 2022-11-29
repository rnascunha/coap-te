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

/** @} */  // end of utility group

}  // namespace core
}  // namespace coap_te

#include "impl/utility.ipp"

#endif  // COAP_TE_CORE_UTILITY_HPP_
