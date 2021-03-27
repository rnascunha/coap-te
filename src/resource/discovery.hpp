#ifndef COAP_TE_RESOURCE_DISCOVERY_HPP__
#define COAP_TE_RESOURCE_DISCOVERY_HPP__

#include <cstdint>
#include "error.hpp"
#include "internal/list.hpp"

namespace CoAP{
namespace Resource{

enum class attr{
	resource_type,
	interface_description,
	maximum_size_est,
};

enum class attr_type{
	string = 0,
	uint
};

struct attr_config{
	attr		attribute;
	const char* name;
	attr_type	type;
};

static constexpr const attr_config config[] = {
		{attr::resource_type,	 		"rt", attr_type::string},
		{attr::interface_description,	"if", attr_type::string},
		{attr::maximum_size_est,		"sz", attr_type::uint}
};

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

}//Resource
}//CoAP

#include "impl/discovery_impl.hpp"

#endif /* COAP_TE_RESOURCE_DISCOVERY_HPP__ */
