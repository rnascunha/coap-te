/**
 * @file cardinality.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-01-05
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef COAP_TE_BUFFER_DETAIL_CARDINALITY_HPP_
#define COAP_TE_BUFFER_DETAIL_CARDINALITY_HPP_

#include <type_traits>

namespace coap_te {

class mutable_buffer;
class const_buffer;

namespace detail {

// Tag types used to select appropriately optimised overloads.
struct one_buffer {};
struct multiple_buffers {};

// Helper trait to detect single buffers.
template <typename BufferSequence>
struct buffer_sequence_cardinality :
  std::conditional_t<
    std::is_same_v<BufferSequence, mutable_buffer>
    || std::is_same_v<BufferSequence, const_buffer>,
    one_buffer, multiple_buffers> {};

}  // namespace detail
}  // namespace coap_te

#endif   // COAP_TE_BUFFER_DETAIL_CARDINALITY_HPP_