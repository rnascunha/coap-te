#ifndef COAP_TE_INTERNAL_DECODE_HPP__
#define COAP_TE_INTERNAL_DECODE_HPP__

#include <cstdlib>
#include <cstdint>

namespace CoAP{
namespace Helper{

/*
 * Uses a external buffer to decode
 */
std::size_t percent_decode(char* buffer_out, std::size_t buffer_out_len,
					const char* buffer_in, std::size_t buffer_in_len) noexcept;

/**
 * Uses the same buffer to decode
 */
std::size_t percent_decode(char* buffer, std::size_t buffer_len) noexcept;
std::size_t percent_decode(char* buffer) noexcept;

}//Helper
}//CoAP

#endif /* COAP_TE_INTERNAL_DECODE_HPP__ */
