#ifndef COAP_TE_MESSAGE_OPTIONS_TEMPLATE_CLASS_HPP__
#define COAP_TE_MESSAGE_OPTIONS_TEMPLATE_CLASS_HPP__

#include "../../defines/defaults.hpp"
#include "types.hpp"
#include "../types.hpp"

namespace CoAP{
namespace Message{
namespace Option{

template<typename OptionCode>
struct option_template
{
	option_template();
	option_template(const option_template&) = default;
	//Copy
	option_template(OptionCode, unsigned, const void*);
	option_template(OptionCode);
	option_template(OptionCode, const char*);
	option_template(OptionCode, unsigned&);
	option_template(content_format const&);
	option_template(accept const&);
#if COAP_TE_OPTION_NO_RESPONSE == 1
	option_template(suppress const&);
#endif /* COAP_TE_OPTION_NO_RESPONSE == 1 */
	//Opaque
	option_template(OptionCode, const void*, unsigned);

	bool is_critical() const noexcept;
	bool is_unsafe() const noexcept;
	bool is_no_cache_key() const noexcept;

	bool operator==(option_template<OptionCode> const& rhs) const noexcept;
	bool operator!=(option_template<OptionCode> const& rhs) const noexcept;
	bool operator<(option_template<OptionCode> const& rhs) const noexcept;
	bool operator>(option_template<OptionCode> const& rhs) const noexcept;
	bool operator<=(option_template<OptionCode> const& rhs) const noexcept;
	bool operator>=(option_template<OptionCode> const& rhs) const noexcept;

	bool operator==(OptionCode rhs) const noexcept;
	bool operator!=(OptionCode rhs) const noexcept;
	explicit operator bool() const noexcept;

	option_template& operator=(option_template<OptionCode> const&) noexcept = default;

	OptionCode	ocode = invalid<OptionCode>();//static_cast<OptionCode>(invalid);	//Option code
	unsigned	length = 0;					//Option length
	const void*	value = nullptr;			//Pointer to option value
};

template<typename OptionCode,
		bool CheckType = true>
bool create(option_template<OptionCode>&, OptionCode) noexcept;
template<typename OptionCode,
		bool CheckType = true>
bool create(option_template<OptionCode>&, OptionCode, const char*) noexcept;
template<typename OptionCode,
		bool CheckType = true>
bool create(option_template<OptionCode>&, OptionCode, unsigned&) noexcept;
template<typename OptionCode,
		bool CheckType = true>
bool create(option_template<OptionCode>&, OptionCode, const void*, unsigned) noexcept;

}//Option
}//Message
}//CoAP

#include "impl/template_class_impl.hpp"

#endif /* COAP_TE_MESSAGE_OPTIONS_TEMPLATE_CLASS_HPP__ */
