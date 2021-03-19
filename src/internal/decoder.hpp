#ifndef COAP_TE_INTERNAL_DECODE_HPP__
#define COAP_TE_INTERNAL_DECODE_HPP__

#include <cstdint>

namespace CoAP{
namespace Helper{

bool percent_decode(char* buffer_out, std::size_t& buffer_out_len,
					const char* buffer_in, std::size_t buffer_in_len) noexcept;

}//Helper
}//CoAP

#endif /* COAP_TE_INTERNAL_DECODE_HPP__ */
