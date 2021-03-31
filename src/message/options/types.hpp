#ifndef COAP_TE_MESSAGE_OPTIONS_TYPES_HPP__
#define COAP_TE_MESSAGE_OPTIONS_TYPES_HPP__

namespace CoAP{
namespace Message{
namespace Option{

static constexpr const int invalid_option = 255;

template<typename OptionCode>
constexpr OptionCode invalid() noexcept{
	return static_cast<OptionCode>(invalid_option);
}

enum class type
{
	empty 			= 0,
	opaque,
	uint,
	string
};

template<typename OptionCode>
struct config
{
	OptionCode	ocode;
	bool		repeatable;
	type		otype;
};

enum class delta_special
{
	one_byte_extend 		= 13,
	two_byte_extend 		= 14,
	error		 			= 15
};

enum class length_special
{
	one_byte_extend 		= 13,
	two_byte_extend 		= 14,
	error		 			= 15
};

}//Options
}//Message
}//CoAP

#endif /* COAP_TE_MESSAGE_OPTIONS_TYPES_HPP__ */
