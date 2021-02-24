#ifndef COAP_TE_OPTIONS_IMPL_HPP__
#define COAP_TE_OPTIONS_IMPL_HPP__

#include <cstdint>
#include <cstring>
#include "other/helper.hpp"
#include "../options.hpp"

namespace CoAP{
namespace Message{

static bool check_type(option_code code, option_type type) noexcept
{
	auto const* config = get_option_config(code);
	if(!config) return false;
	if(config->type != type) return false;

	return true;
}

template<bool CheckType = true>
bool create_option(option& option, option_code code) noexcept
{
	if constexpr (CheckType)
		if(!check_type(code, option_type::empty))
			return false;

	option.code = code;
	option.length = 0;
	option.value = nullptr;

	return true;
}

template<bool CheckType = true>
bool create_option(option& option, option_code code, const char* value) noexcept
{
	if constexpr (CheckType)
		if(!check_type(code, option_type::string))
			return false;

	option.code = code;
	option.length = std::strlen(value);
	option.value = value;

	return true;
}

template<bool CheckType = true>
bool create_option(option& option, option_code code, unsigned& value) noexcept
{
	if constexpr (CheckType)
		if(!check_type(code, option_type::uint))
			return false;

	option.code = code;
	CoAP::Helper::make_short_unsigned(value, option.length);
	option.value = &value;

	return true;
}

template<bool CheckType = true>
bool create_option(option& option, option_code code, const void* value, unsigned length) noexcept
{
	if constexpr (CheckType)
		if(!check_type(code, option_type::opaque))
			return false;

	option.code = code;
	option.length = length;
	option.value = value;

	return true;
}

}//Message
}//CoAP

#endif /* COAP_TE_OPTIONS_IMPL_HPP__ */
