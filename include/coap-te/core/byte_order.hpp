/**
 * @file byte_order.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @copyright Copyright (c) 2022
 * @brief Calculate the unsigned options value as defined at RFC7252
 * 
 * The unsigned value must be sent in network byte order (big endian),
 * in the smallest possible bytes. So '0' should send no bytes, '1' to
 * '255' in one byte, and so on.
 * 
 * @version 0.1
 * @date 2022-11-28
 * 
 */
#ifndef COAP_TE_CORE_BYTE_ORDER_HPP_
#define COAP_TE_CORE_BYTE_ORDER_HPP_

#include <utility>

namespace coap_te {
namespace core {

/** 
 * @defgroup byte_order Byte Order
 * Defines function to deal with protocol byte order requirements
 * 
 * @ingroup byte_order
 * @{
 */

/**
 * @brief Gets size of unsigned value in big endian format
 * 
 * @tparam Unsigned Value type to calculate size
 * @return std::size_t size computed
 */
template<typename Unsigned>
[[nodiscard]] constexpr std::size_t
small_big_endian_size(Unsigned) noexcept;

/**
 * @brief Converts from a little endian to small big-endian.
 * 
 * @note Small big endian is the value in the small possible bytes
 * been represented.
 * 
 * @tparam Unsigned type value to be converted
 * @param value value to be converted
 * @return std::pair<Unsigned, std::size_t> The first value of the pair is
 * used as a storage of the bytes, and the second how many bytes of this
 * storage should be considered
 */
template<typename Unsigned>
[[nodiscard]] constexpr std::pair<Unsigned, std::size_t>
to_small_big_endian(Unsigned value) noexcept;

/**
 * @brief Converts a buffer representing a small big-endian to a 
 * little endian number
 * 
 * @warning if sizeof(Unsigned) < size, the converted value will be truncated.
 * 
 * @tparam Unsigned Type to be converted to.
 * @param value Buffer values to be conveted
 * @param size Size of the buffer
 * @return Unsigned Converted little-endian value 
 */
template<typename Unsigned = unsigned>
[[nodiscard]] constexpr Unsigned
from_small_big_endian(const std::uint8_t* value, std::size_t size) noexcept;

/** @} */  // end of byte_order

}  // namespace core
}  // namespace coap_te

#include "impl/byte_order.ipp"

#endif  // COAP_TE_CORE_BYTE_ORDER_HPP_
