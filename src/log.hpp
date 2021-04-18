#ifndef COAP_TE_LOG_HPP__
#define COAP_TE_LOG_HPP__

#include "error.hpp"

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

static constexpr Tree_Trunks::type_config<type> const type_config[] = {
	{type::error, 		"ERROR", 		"ERRO",	FG_RED},
	{type::warning, 	"WARNING", 		"WARN",	FG_YELLOW},
	{type::deprecated, 	"DEPRECATED", 	"DEPR",	FG_BRIG_YELLOW},
	{type::status, 		"STATUS", 		"STAT", FG_GREEN},
	{type::debug, 		"DEBUG", 		"DEBG",	FG_BLUE}
};

static constexpr const Tree_Trunks::config<type> config = {
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
	.use_color				= false,
#else /* defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) */
	.use_color 				= true,
#endif /* defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) */
	.time 					= true,
	.module 				= true,
	.ignore_module_level 	= false,
	.log_name 				= false,
	.log_sname 				= true,
	.name 					= nullptr,
	.max_level 				= type::debug,
	.tp_config 				= type_config
};

#include "tt/functions_conv.hpp"

/**
 * Convenience functions
 */
template<eolt EOL = eolt::nl_rs, typename ...Args>
constexpr std::size_t debug(Args&& ... args) noexcept
{
	return log<type::debug, EOL>(std::forward<Args>(args)...);
}

template<eolt EOL = eolt::nl_rs, typename ...Args>
constexpr std::size_t status(Args&& ... args) noexcept
{
	return log<type::status, EOL>(std::forward<Args>(args)...);
}

template<eolt EOL = eolt::nl_rs, typename ...Args>
constexpr std::size_t deprecated(Args&& ... args) noexcept
{
	return log<type::deprecated, EOL>(std::forward<Args>(args)...);
}

template<eolt EOL = eolt::nl_rs, typename ...Args>
constexpr std::size_t warning(Args&& ... args) noexcept
{
	return log<type::warning, EOL>(std::forward<Args>(args)...);
}

template<eolt EOL = eolt::nl_rs, typename ...Args>
constexpr std::size_t error(Args&& ... args) noexcept
{
	return log<type::error, EOL>(std::forward<Args>(args)...);
}

template<eolt EOL = eolt::nl_rs, typename ...Args>
constexpr std::size_t none(Args&& ... args) noexcept
{
	return log<type::none, EOL>(std::forward<Args>(args)...);
}

std::size_t error(CoAP::Error& ec, const char* what = nullptr) noexcept
{
	return log<type::error>("[%d] %s (%s)", ec.value(), ec.message(), what ? what : "");
}

std::size_t error(module const& mod, CoAP::Error& ec, const char* what = nullptr) noexcept
{
	return log<type::error>(mod, "[%d] %s (%s)", ec.value(), ec.message(), what ? what : "");
}

}//Log
}//CoAP

#include "tt/uncolor.hpp"

#endif /* COAP_TE_LOG_HPP__ */
