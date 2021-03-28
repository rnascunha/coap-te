#ifndef COAP_TE_OPTIONS_IMPL_HPP__
#define COAP_TE_OPTIONS_IMPL_HPP__

#include <cstdint>
#include <cstring>
#include "internal/helper.hpp"
#include "../options.hpp"

namespace CoAP{
namespace Message{
namespace Option{

static bool check_type(code code, type type) noexcept
{
	auto const* config = get_config(code);
	if(!config) return false;
	if(config->otype != type) return false;

	return true;
}

template<bool CheckType /* = true */>
bool create(option& option, code code) noexcept
{
	if constexpr (CheckType)
		if(!check_type(code, type::empty))
			return false;

	option.ocode = code;
	option.length = 0;
	option.value = nullptr;

	return true;
}

template<bool CheckType /* = true */>
bool create(option& option, code code, const char* value) noexcept
{
	if constexpr (CheckType)
		if(!check_type(code, type::string))
			return false;

	option.ocode = code;
	option.length = static_cast<unsigned>(std::strlen(value));
	option.value = value;

	return true;
}

template<bool CheckType /* = true */>
bool create(option& option, code code, unsigned& value) noexcept
{
	if constexpr (CheckType)
		if(!check_type(code, type::uint))
			return false;

	option.ocode = code;
	CoAP::Helper::make_short_unsigned(value, option.length);
	option.value = &value;

	return true;
}

template<bool CheckType /* = true */>
bool create(option& option, code code, const void* value, unsigned length) noexcept
{
	if constexpr (CheckType)
		if(!check_type(code, type::opaque))
			return false;

	option.ocode = code;
	option.length = length;
	option.value = value;

	return true;
}

}//Option
}//Message
}//CoAP

#endif /* COAP_TE_OPTIONS_IMPL_HPP__ */
