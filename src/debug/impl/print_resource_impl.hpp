#ifndef COAP_TE_DEBUG_PRINT_RESOURCE_IMPL_HPP__
#define COAP_TE_DEBUG_PRINT_RESOURCE_IMPL_HPP__


#include "../print_resource.hpp"
#include <cstdio>

namespace CoAP{
namespace Debug{

template<typename Resource>
void print_resource_branch(CoAP::branch<Resource>& br, int level /* = 0 */) noexcept
{
	std::printf("%d: %s:[", level, br.value().path() ? br.value().path() : "/");
	CoAP::branch<Resource>* n;
	for(unsigned i = 0; (n = br[i]); i++)
	{
		if(i != 0) std::printf(" ");
		std::printf("%s", n->value().path() ? n->value().path() : "/");
	}
	std::printf("]\n");

	++level;
	for(unsigned i = 0; (n = br[i]); i++)
	{
		print_resource_branch<Resource>(*n, level);
	}
}

}//Debug
}//CoAP

#endif /* COAP_TE_DEBUG_PRINT_RESOURCE_IMPL_HPP__ */
