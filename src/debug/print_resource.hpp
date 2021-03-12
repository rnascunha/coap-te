#ifndef COAP_TE_DEBUG_PRINT_RESOURCE_HPP__
#define COAP_TE_DEBUG_PRINT_RESOURCE_HPP__

#include "internal/tree.hpp"
#include "resource/resource.hpp"

namespace CoAP{
namespace Debug{

template<typename Callback_Functor>
void print_resource_branch(CoAP::branch<CoAP::Resource::resource<Callback_Functor>>& br,
		int level = 0) noexcept;

}//Resource
}//Debug

#include "impl/print_resource_impl.hpp"

#endif /* COAP_TE_DEBUG_PRINT_RESOURCE_HPP__ */
