#include "template_class.hpp"
#include "no_response.hpp"

namespace CoAP{
namespace Message{
namespace Option{

template<>
option_template<code>::option_template(content_format const& value)
{
	create(*this, value);
}

template<>
option_template<code>::option_template(accept const& value)
{
	create(*this, value);
}

#if COAP_TE_OPTION_NO_RESPONSE == 1
template<>
option_template<code>::option_template(suppress const& value)
{
	create(*this, value);
}
#endif /* COAP_TE_OPTION_NO_RESPONSE == 1 */

}//Option
}//Message
}//CoAP
