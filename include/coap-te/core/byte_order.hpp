/**
 * @file byte_order.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-11-28
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef COAP_TE_CORE_BYTE_ORDER_HPP_
#define COAP_TE_CORE_BYTE_ORDER_HPP_

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
to_big_endian(UnsignedType& value) noexcept;    // NOLINT

/**
 * @brief Calculate big endian value and copy to buffer
 * 
 * Copies sizeof(value) bytes to buffer
 * 
 * @warning It's responsability of the user that the buffer is of
 * size sizeof(value).
 * 
 * @tparam UnsignedType Unsigned type to be converted
 * @param value Value to be converted
 * @return std::size_t void
 */
template<typename UnsignedType>
constexpr void
to_big_endian(UnsignedType value, std::uint8_t* buffer) noexcept;

/** @} */  // end of byte_order

}  // namespace core
}  // namespace coap_te

#include "impl/byte_order.ipp"

#endif  // COAP_TE_CORE_BYTE_ORDER_HPP_
