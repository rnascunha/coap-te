#ifndef COAP_TE_INTERNAL_ENCODE_HPP__
#define COAP_TE_INTERNAL_ENCODE_HPP__

#include <cstddef>

namespace CoAP{
namespace Helper{

template<typename Functor>
std::size_t percent_encoded_size(const char* buffer, std::size_t buffer_used,
								Functor,
								std::size_t* changes = nullptr);
template<typename Functor>
int percent_encode(char* buffer, std::size_t buffer_used, std::size_t buffer_len_total,
					Functor);

std::size_t percent_encoded_size(const char* buffer, std::size_t buffer_used,
								char const * const encoded_list, std::size_t encoded_list_len,
								std::size_t* changes = nullptr);
int percent_encode(char* buffer, std::size_t buffer_used, std::size_t buffer_len_total,
					char const* const encode_list, std::size_t encode_list_len);

}//Helper
}//CoAP

#include "impl/encoder_impl.hpp"

#endif /* COAP_TE_INTERNAL_ENCODE_HPP__ */
