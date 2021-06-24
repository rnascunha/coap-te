#ifndef COAP_TE_DEBUG_PRINT_RESOURCE_HPP__
#define COAP_TE_DEBUG_PRINT_RESOURCE_HPP__

#include <cstdlib>
#include "../internal/tree.hpp"
#include "../resource/resource.hpp"

namespace CoAP{
namespace Debug{

template<typename Resource>
void print_resource_branch(CoAP::branch<Resource>& br,
		int level = 0) noexcept;

void print_link_format(const char* buffer, std::size_t length) noexcept;

}//Resource
}//Debug

#include "impl/print_resource_impl.hpp"

#endif /* COAP_TE_DEBUG_PRINT_RESOURCE_HPP__ */
