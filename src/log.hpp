#ifndef COAP_TE_LOG_HPP__
#define COAP_TE_LOG_HPP__

#include "error.hpp"

#include "tt/types.hpp"
#include "tt/log.hpp"
#include "tt/color.hpp"

#include <utility>

#if COAP_TE_LOG_COLOR == 0
#define COAP_TE_CONFIG_LOG_COLOR 	false
#else /* COAP_TE_LOG_COLOR == 0 */
#define COAP_TE_CONFIG_LOG_COLOR 	true
#endif /* COAP_TE_LOG_COLOR == 0 */

#if COAP_TE_LOG_LEVEL == 0
#define COAP_TE_CONFIG_LOG_LEVEL	type::none
#elif COAP_TE_LOG_LEVEL == 1
#define COAP_TE_CONFIG_LOG_LEVEL	type::error
#elif COAP_TE_LOG_LEVEL == 2
#define COAP_TE_CONFIG_LOG_LEVEL	type::warning
#elif COAP_TE_LOG_LEVEL == 3
#define COAP_TE_CONFIG_LOG_LEVEL	type::deprecated
#elif COAP_TE_LOG_LEVEL == 4
#define COAP_TE_CONFIG_LOG_LEVEL	type::status
#else
#define COAP_TE_CONFIG_LOG_LEVEL	type::debug
#endif

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
	/*.use_color				= */COAP_TE_CONFIG_LOG_COLOR,
	/*.time 					= */true,
	/*.module 					= */true,
	/*.ignore_module_level 		= */false,
	/*.log_name 				= */false,
	/*.log_sname 				= */true,
	/*.name 					= */nullptr,
	/*.max_level 				= */COAP_TE_CONFIG_LOG_LEVEL,
	/*.tp_config 				= */type_config
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
