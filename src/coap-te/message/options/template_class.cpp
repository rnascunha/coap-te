#include "template_class.hpp"
#include "no_response.hpp"

namespace CoAP{
namespace Message{
namespace Option{

template<>
option_template<code>::option_template(content_format const& value, bool is_request /* = false */)
{
	create(*this, value, is_request);
}

#if COAP_TE_OPTION_NO_RESPONSE == 1
template<>
option_template<code>::option_template(suppress& value)
{
	create(*this, value);
}
#endif /* COAP_TE_OPTION_NO_RESPONSE == 1 */

}//Option
}//Message
}//CoAP
