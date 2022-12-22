#ifndef COAP_TE_MESSAGES_TYPES_DETAIL_HPP__
#define COAP_TE_MESSAGES_TYPES_DETAIL_HPP__

#include "../defines/defaults.hpp"

namespace CoAP{
namespace Message{

namespace detail{

template<typename T>
struct format_type : T{
	enum class format : unsigned
	{
		text_plain				= 0,	//text/plain;charset=utf-8
		application_link_format = 40,	//application/link-format
		application_xml			= 41, 	//application/xml
		application_octet_stream = 42, 	//application/octet-stream
		application_exi			= 47, 	//application/exi
		application_json		= 50,	//application/json
	#if COAP_TE_ENABLE_FETCH_PATCH_VERBS == 1
		//https://tools.ietf.org/html/rfc8132#section-6
		 application_json_patch_json = 51,	//application/json-patch+json
		 application_merge_patch_json = 52	//application/merge-patch+json
	#endif /* COAP_TE_ENABLE_FETCH_PATCH_VERBS == 1 */
	};
};

struct content_type{};
struct accept_type{};

using content_format = format_type<content_type>::format;
using accept = format_type<accept_type>::format;

}//detail

}//Message
}//CoAP

#endif /* COAP_TE_MESSAGES_TYPES_DETAIL_HPP__ */
