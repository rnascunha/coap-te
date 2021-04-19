#ifndef COAP_TE_RESOURCE_DISCOVERY_IMPL_HPP__
#define COAP_TE_RESOURCE_DISCOVERY_IMPL_HPP__

#include <cstring>
#include "../discovery.hpp"

namespace CoAP{
namespace Resource{

template<typename Resource>
std::size_t description(Resource const& root,
		char* buffer,
		std::size_t buffer_size,
		CoAP::Error& ec) noexcept
{
	std::size_t offset = 0;
	unsigned size_path = root.path() ?
							std::strlen(root.path()) : 0;

	if(buffer_size < offset + size_path + 3)
	{
		ec = CoAP::errc::insufficient_buffer;
		return offset;
	}

	buffer[0] = '<';
	buffer[1] = '/';
	offset += 2;

	if(root.path())
	{
		std::memcpy(buffer + offset, root.path(), size_path);
		offset += size_path;
	}
	buffer[offset++] = '>';

	if constexpr(Resource::has_description)
	{
		unsigned size_desc = root.description() ?
									std::strlen(root.description()) : 0;

		if(size_desc && buffer_size < offset + size_desc + 1)
		{
			ec = CoAP::errc::insufficient_buffer;
			return offset;
		}

		if(size_desc)
		{
			buffer[offset++] = ';';
			std::memcpy(buffer + offset, root.description(), size_desc);
			offset += size_desc;
		}
	}

	if(offset < buffer_size)
		buffer[offset] = '\0';

	return offset;
}

/**
 * MSVC warning... don't understand...
 */ 
#if defined(_MSC_VER)
#pragma warning( push )
#pragma warning( disable : 4505 )
#endif /* defined(_MSC_VER) */

[[maybe_unused]]
static std::size_t make_parents_path(path_list const* parents_path,
		char* buffer, std::size_t buffer_size,
		CoAP::Error& ec) noexcept
{
	if(!parents_path) return 0;

	std::size_t offset = 0;
	for(path_node const* n = parents_path->head(); n; n = n->next)
	{
		std::size_t size = n->value ? std::strlen(n->value) : 0;
		if(buffer_size < (offset + size + 1))
		{
			ec = CoAP::errc::insufficient_buffer;
			return offset;
		}

		if(size)
		{
			std::memcpy(buffer + offset, n->value, size);
			offset += size;
		}

		buffer[offset++] = '/';
	}
	return offset;
}

#if defined(_MSC_VER)
#pragma warning( pop )
#endif /* defined(_MSC_VER) */

template<typename Resource>
std::size_t description(Resource const& root,
		path_list const* parents_path,
		char* buffer, std::size_t buffer_size,
		CoAP::Error& ec) noexcept
{
	if(buffer_size == 0)
	{
		ec = CoAP::errc::insufficient_buffer;
		return 0;
	}

	std::size_t offset = 0;

	buffer[offset] = '<';
	offset += 1;

	offset += make_parents_path(parents_path, buffer + offset, buffer_size - offset, ec);
	if(ec) return offset;

	std::size_t size_path = root.path() ?
							std::strlen(root.path()) : 0;

	if(buffer_size <  offset + size_path + 2)
	{
		ec = CoAP::errc::insufficient_buffer;
		return offset;
	}

	if(size_path)
	{
		std::memcpy(buffer + offset, root.path(), size_path);
		offset += size_path;
	}
	else
	{
		buffer[offset++] = '/';
	}
	buffer[offset++] = '>';

	if constexpr(Resource::has_description)
	{
		std::size_t size_desc = root.description() ?
									std::strlen(root.description()) : 0;

		if(size_desc && buffer_size < offset + size_desc + 1)
		{
			ec = CoAP::errc::insufficient_buffer;
			return offset;
		}

		if(size_desc)
		{
			buffer[offset++] = ';';
			std::memcpy(buffer + offset, root.description(), size_desc);
			offset += size_desc;
		}
	}

	if(offset < buffer_size)
		buffer[offset] = '\0';

	return offset;

}

template<typename ResourceNode>
bool default_criteria(ResourceNode const& node, path_list const& plist) noexcept
{
	if(plist.size() == 2
			&& plist.head()->next->value
			&& std::strcmp(plist.head()->next->value, ".well-known") == 0
			&& std::strcmp(node.value().path(), "core") == 0) return false;
	return node.value().has_callback();
}

template<typename ResourceNode, typename Criteria>
static std::size_t discovery_impl(ResourceNode const& node,
		char* buffer, std::size_t buffer_size,
		unsigned max_depth, unsigned depth, unsigned& count,
		path_list& list, Criteria func [[maybe_unused]],
		CoAP::Error& ec) noexcept
{
	if(max_depth && depth >= max_depth) return 0;
	std::size_t offset = 0;

	if constexpr(!std::is_same<decltype(func), no_criteria_type>::value)
	{
		if(func && func(node, list))
		{
			if(depth && count)
			{
				if(buffer_size > 0)
				{
					buffer[offset++] = ',';
				}
				else
				{
					ec = CoAP::errc::insufficient_buffer;
					return offset;
				}
			}
			offset += description(node.value(), &list, buffer + offset, buffer_size - offset, ec);
			count++;
		}
	}
	else
	{
		if(depth && count)
		{
			if(buffer_size > 0)
			{
				buffer[offset++] = ',';
			}
			else
			{
				ec = CoAP::errc::insufficient_buffer;
				return offset;
			}
		}
		offset += description(node.value(), &list, buffer + offset, buffer_size - offset, ec);
		count++;
	}
	if(ec) return offset;

	ResourceNode const* n_node = node.next();
	if(n_node)
	{
		offset += discovery_impl(*n_node, buffer + offset, buffer_size - offset, max_depth,
				depth, count, list, func, ec);
		if(ec) return offset;
	}

	path_node n{node.value().path()};
	list.add<false>(n);
	depth += 1;
	ResourceNode const* children = node.children();
	if(children)
	{
		offset += discovery_impl(*children, buffer + offset, buffer_size - offset, max_depth,
				depth, count, list, func, ec);
		if(ec) return offset;
		children = children->next();
	}
	list.remove(n);

	return offset;
}

template<typename ResourceNode>
std::size_t discovery(ResourceNode const& node,
		char* buffer, std::size_t buffer_size,
		unsigned max_depth,
		CoAP::Error& ec) noexcept
{
	path_list list;
	unsigned count = 0;
	return discovery_impl<ResourceNode, decltype(default_criteria<ResourceNode>)>(node, buffer, buffer_size,
			max_depth, 0, count,
			list, default_criteria, ec);
}

template<typename ResourceNode>
std::size_t discovery(ResourceNode const& node,
		char* buffer, std::size_t buffer_size,
		CoAP::Error& ec) noexcept
{
	return discovery(node, buffer, buffer_size, 0, ec);
}

template<typename ResourceNode, typename Criteria>
std::size_t discovery(ResourceNode const& node,
		char* buffer, std::size_t buffer_size,
		unsigned max_depth, Criteria func,
		CoAP::Error& ec) noexcept
{
	path_list list;
	unsigned count = 0;
	return discovery_impl<ResourceNode, Criteria>(node, buffer, buffer_size,
			max_depth, 0, count,
			list, func, ec);
}

}//Resource
}//CoAP

#endif /* COAP_TE_RESOURCE_DISCOVERY_IMPL_HPP__ */
