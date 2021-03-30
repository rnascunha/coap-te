#ifndef COAP_TE_RESOURCE_DISCOVERY_HPP__
#define COAP_TE_RESOURCE_DISCOVERY_HPP__

#include <cstdint>
#include "error.hpp"
#include "internal/list.hpp"
#include "types.hpp"

namespace CoAP{
namespace Resource{

using path_list = CoAP::list<const char*>;
using path_node = path_list::node;

template<typename Resource>
std::size_t description(Resource const& root,
		char* buffer, std::size_t buffer_size,
		CoAP::Error& ec) noexcept;

template<typename Resource>
std::size_t description(Resource const& root,
		path_list const* parents_path,
		char* buffer, std::size_t buffer_size,
		CoAP::Error& ec) noexcept;

template<typename ResourceNode>
std::size_t discovery(ResourceNode const&,
		char* buffer, std::size_t buffer_size,
		unsigned max_depth,
		CoAP::Error& ec) noexcept;

template<typename ResourceNode>
std::size_t discovery(ResourceNode const&,
		char* buffer, std::size_t buffer_size,
		CoAP::Error& ec) noexcept;

}//Resource
}//CoAP

#include "impl/discovery_impl.hpp"

#endif /* COAP_TE_RESOURCE_DISCOVERY_HPP__ */
