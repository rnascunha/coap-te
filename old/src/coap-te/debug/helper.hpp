#ifndef COAP_TE_DEBUG_HELPER_HPP__
#define COAP_TE_DEBUG_HELPER_HPP__

#include <cstddef>

namespace CoAP{
namespace Debug{

int print_array(const void* const buffer, std::size_t len, unsigned separate = 2);
void print_string(const void* const buffer, std::size_t len);
void print_array_as_string(const void* const buffer, std::size_t len) noexcept;

}//Message
}//CoAP



#endif /* COAP_TE_DEBUG_HELPER_HPP__ */
