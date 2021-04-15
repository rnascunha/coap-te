#ifndef COAP_TE_MESSAGE_OPTIONS_TEMPLATE_CLASS_IMPL_HPP__
#define COAP_TE_MESSAGE_OPTIONS_TEMPLATE_CLASS_IMPL_HPP__

#include <cstring>
#include <type_traits>

#include "../template_class.hpp"

#include "internal/helper.hpp"
#include "../functions.hpp"
#include "../options.hpp"

namespace CoAP{
namespace Message{
namespace Option{

template<typename OptionCode>
option_template<OptionCode>::option_template(){}

template<typename OptionCode>
option_template<OptionCode>::option_template(OptionCode code, unsigned len, const void* val) :
	ocode(code), length(len), value(val){}

template<typename OptionCode>
option_template<OptionCode>::option_template(OptionCode code)
{
	create<OptionCode, false>(*this, code);
}

template<typename OptionCode>
option_template<OptionCode>::option_template(OptionCode code, const char* value)
{
	create<OptionCode, false>(*this, code, value);
}

template<typename OptionCode>
option_template<OptionCode>::option_template(OptionCode code, unsigned& value)
{
	create<OptionCode, false>(*this, code, value);
}

template<typename OptionCode>
option_template<OptionCode>::option_template(content_format const& value, bool is_request /* = false */)
{
	static_assert(std::is_same<OptionCode, code>::value, "Must be of type 'Option::code'");

	create(*this, value, is_request);
}

#if COAP_TE_OPTION_NO_RESPONSE == 1
template<typename OptionCode>
option_template<OptionCode>::option_template(suppress& value)
{
	static_assert(std::is_same<OptionCode, code>::value, "Must be of type 'Option::code'");

	create(*this, value);
}
#endif /* COAP_TE_OPTION_NO_RESPONSE == 1 */


template<typename OptionCode>
option_template<OptionCode>::option_template(OptionCode code, const void* value, unsigned length)
{
	create<OptionCode, false>(*this, code, value, length);
}

template<typename OptionCode>
bool option_template<OptionCode>::operator==(option_template<OptionCode> const& rhs) const noexcept
{
	return ocode == rhs.ocode;
}

template<typename OptionCode>
bool option_template<OptionCode>::operator!=(option_template<OptionCode> const& rhs) const noexcept
{
	return !(*this == rhs);
}

template<typename OptionCode>
bool option_template<OptionCode>::operator<(option_template<OptionCode> const& rhs) const noexcept
{
	return ocode < rhs.ocode;
}

template<typename OptionCode>
bool option_template<OptionCode>::operator>(option_template<OptionCode> const& rhs) const noexcept
{
	return *this > rhs;
}

template<typename OptionCode>
bool option_template<OptionCode>::operator<=(option_template<OptionCode> const& rhs) const noexcept
{
	return !(*this > rhs);
}

template<typename OptionCode>
bool option_template<OptionCode>::operator>=(option_template<OptionCode> const& rhs) const noexcept
{
	return !(*this < rhs);
}

template<typename OptionCode>
bool option_template<OptionCode>::operator==(OptionCode rhs) const noexcept
{
	return ocode == rhs;
}

template<typename OptionCode>
bool option_template<OptionCode>::operator!=(OptionCode rhs) const noexcept
{
	return !(*this == rhs);
}

template<typename OptionCode>
option_template<OptionCode>::operator bool() const noexcept
{
	return ocode != invalid<OptionCode>();
}

template<typename OptionCode>
bool option_template<OptionCode>::is_critical() const noexcept
{
	return CoAP::Message::Option::is_critical(ocode);
}

template<typename OptionCode>
bool option_template<OptionCode>::is_unsafe() const noexcept
{
	return CoAP::Message::Option::is_unsafe(ocode);
}

template<typename OptionCode>
bool option_template<OptionCode>::is_no_cache_key() const noexcept
{
	return CoAP::Message::Option::is_no_cache_key(ocode);
}

template<typename OptionCode>
extern config<OptionCode> const* get_config(OptionCode) noexcept;

template<typename OptionCode>
[[maybe_unused]] static bool
check_type(OptionCode code, type type) noexcept
{
	auto const* config = get_config<OptionCode>(code);
	if(!config) return false;
	if(config->otype != type) return false;

	return true;
}

template<typename OptionCode,
		bool CheckType /* = true */>
bool create(option_template<OptionCode>& option, OptionCode code) noexcept
{
	if constexpr (CheckType)
		if(!check_type(code, type::empty))
			return false;

	option.ocode = code;
	option.length = 0;
	option.value = nullptr;

	return true;
}

template<typename OptionCode,
		bool CheckType /* = true */>
bool create(option_template<OptionCode>& option, OptionCode code, const char* value) noexcept
{
	if constexpr (CheckType)
		if(!check_type(code, type::string))
			return false;

	option.ocode = code;
	option.length = static_cast<unsigned>(std::strlen(value));
	option.value = value;

	return true;
}

template<typename OptionCode,
		bool CheckType /* = true */>
bool create(option_template<OptionCode>& option, OptionCode code, unsigned& value) noexcept
{
	if constexpr (CheckType)
		if(!check_type(code, type::uint))
			return false;

	option.ocode = code;
	CoAP::Helper::make_short_unsigned(value, option.length);
	option.value = &value;

	return true;
}

template<typename OptionCode,
		bool CheckType /* = true */>
bool create(option_template<OptionCode>& option, OptionCode code, const void* value, unsigned length) noexcept
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

#endif /* COAP_TE_MESSAGE_OPTIONS_TEMPLATE_CLASS_IMPL_HPP__ */
