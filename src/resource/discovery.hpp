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

template<typename ResourceNode, typename Criteria>
std::size_t discovery(ResourceNode const& node,
		char* buffer, std::size_t buffer_size,
		unsigned max_depth, Criteria func,
		CoAP::Error& ec) noexcept;

/**
 * Resource Discovery criteria
 */
using no_criteria_type = void(*)() noexcept;
void no_criteria() noexcept{}
template<typename ResourceNode>
bool default_criteria(ResourceNode const&, path_list const&) noexcept;

}//Resource
}//CoAP

#include "impl/discovery_impl.hpp"

#endif /* COAP_TE_RESOURCE_DISCOVERY_HPP__ */
