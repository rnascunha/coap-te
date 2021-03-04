#ifndef COAP_TE_LOG_HPP__
#define COAP_TE_LOG_HPP__

#include "tt/types.hpp"
#include "tt/log.hpp"
#include "tt/color.hpp"

#include <utility>

namespace CoAP{
namespace Log{

enum class type{
	none = 0,
	error,
	warning,
	deprecated,
	status,
	debug,
};

using module = Tree_Trunks::module<type>;

static constexpr const Tree_Trunks::type_config<type> type_config[] = {
	{type::error, 		"ERROR", 		"ERRO",	FG_RED},
	{type::warning, 	"WARNING", 		"WARN",	FG_YELLOW},
	{type::deprecated, 	"DEPRECATED", 	"DEPR",	FG_BRIG_YELLOW},
	{type::status, 		"STATUS", 		"STAT", FG_GREEN},
	{type::debug, 		"DEBUG", 		"DEBG",	FG_BLUE}
};

static constexpr Tree_Trunks::config<type, 5> config = {
	.use_color 				= true,
	.time 					= true,
	.module 				= true,
	.ignore_module_level 	= false,
	.log_name 				= false,
	.log_sname 				= true,
	.name 					= nullptr,
	.max_level 				= type::debug,
	.tp_config 				= type_config
};

template<type MinType, typename ...Args>
constexpr std::size_t log(Args&& ... args) noexcept
{
	return Tree_Trunks::log<type, MinType, 5, config>(std::forward<Args>(args)...);
}

/**
 * Convenience functions
 */

template<typename ...Args>
constexpr std::size_t debug(Args&& ... args) noexcept
{
	return log<type::debug>(std::forward<Args>(args)...);
}

template<typename ...Args>
constexpr std::size_t status(Args&& ... args) noexcept
{
	return log<type::status>(std::forward<Args>(args)...);
}

template<typename ...Args>
constexpr std::size_t deprecated(Args&& ... args) noexcept
{
	return log<type::deprecated>(std::forward<Args>(args)...);
}

template<typename ...Args>
constexpr std::size_t warning(Args&& ... args) noexcept
{
	return log<type::warning>(std::forward<Args>(args)...);
}

template<typename ...Args>
constexpr std::size_t error(Args&& ... args) noexcept
{
	return log<type::error>(std::forward<Args>(args)...);
}

template<typename ...Args>
constexpr std::size_t none(Args&& ... args) noexcept
{
	return log<type::none>(std::forward<Args>(args)...);
}

}//Log
}//CoAP

#include "tt/uncolor.hpp"

#endif /* COAP_TE_LOG_HPP__ */
