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

/**
 * @brief Converts a interger type to big endian order
 * 
 * @tparam Interger Interger type to convert
 * @param dest Destination buffer to convert. Is user responsability that the buffer fits number
 * @param value Value to convert the byte order
 * @param count Number of byte to consider convert.
 */
template<typename Interger>
constexpr void
to_big_endian(std::uint8_t* dest,
              Interger value,
              std::size_t count = sizeof(Interger)) noexcept;

/**
 * @brief Returns a unsigned value at network byte order
 * 
 * This functions will convert in place a unsigned value to big endian
 * byte order. It will remove any leading zeros, as described
 * at RFC7252 to unsigned type options
 * 
 * @tparam UnsignedType Unsigned type to be converted
 * @param value Value to be converted
 * @return std::size_t Size of converted type
 */
template<typename UnsignedType>
constexpr std::size_t
make_short_unsigned(UnsignedType& value) noexcept;    // NOLINT

/** @} */  // end of utility group

}  // namespace core
}  // namespace coap_te

#include "impl/utility.ipp"

#endif  // COAP_TE_CORE_UTILITY_HPP_
