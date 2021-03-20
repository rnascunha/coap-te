#include "output_string.hpp"

namespace CoAP{
namespace Debug{

const char* type_string(CoAP::Message::type type)
{
	switch(type)
	{
		using namespace CoAP::Message;
		case type::confirmable: 		return "Confirmable";
		case type::nonconfirmable:		return "Non-Confirmable";
		case type::acknowledgment:		return "Ack";
		case type::reset:				return "Reset";
		default:
			break;
	}
	return "undefined";
}

const char* code_string(CoAP::Message::code code)
{
	switch(code)
	{
		using namespace CoAP::Message;
		case code::empty:				return "0.00 Empty";
		//request
		case code::get: 				return "0.01 GET";
		case code::post: 				return "0.02 POST";
		case code::put: 				return "0.03 PUT";
		case code::cdelete: 			return "0.04 DELETE";
		//response
		//success
		case code::success:				return "2.00 Success";
		case code::created: 			return "2.01 Created";
		case code::deleted: 			return "2.02 Deleted";
		case code::valid: 				return "2.03 Valid";
		case code::changed: 			return "2.04 Changed";
		case code::content: 			return "2.05 Content";
		//Client Error
		case code::bad_request: 		return "4.00 Bad Request";
		case code::unauthorized: 		return "4.01 Unauthorized";
		case code::bad_option: 			return "4.02 Bad Option";
		case code::forbidden: 			return "4.03 Forbidden";
		case code::not_found: 			return "4.04 Not Found";
		case code::method_not_allowed: 	return "4.05 Method Not Allowed";
		case code::not_accpetable: 		return "4.06 Not Acceptable";
		case code::precondition_failed: return "4.12 Precondition Failed";
		case code::request_entity_too_large: return "4.13 Request Entity Too Large";
		case code::unsupported_content_format: return "4.15 Unsupported Content-Format";
		//Server Error
		case code::internal_server_error: return "5.00 Internal Server Error";
		case code::not_implemented: 	return "5.01 Not Implemented";
		case code::bad_gateway: 		return "5.02 Bad Gateway";
		case code::service_unavaiable: 	return "5.03 Service Unavailable";
		case code::gateway_timeout: 	return "5.04 Gateway Timeout";
		case code::proxying_not_supported: return "5.05 Proxying Not Supported";
		default:
			break;
	}
	return "undefiend";
}

const char* option_string(CoAP::Message::Option::code op)
{
	switch(op)
	{
		using namespace CoAP::Message::Option;
		case code::content_format:	return "Content-Format";
		case code::etag: 			return "ETag";
		case code::location_path: 	return "Location-Path";
		case code::location_query: 	return "Location-Query";
		case code::size2:			return "Size2";
		case code::max_age: 		return "Max-Age";
		case code::proxy_uri: 		return "Proxy-Uri";
		case code::proxy_scheme: 	return "Proxy-Scheme";
		case code::uri_host: 		return "Uri-Host";
		case code::uri_path: 		return "Uri-Path";
		case code::uri_port: 		return "Uri-Port";
		case code::uri_query: 		return "Uri-Query";
		case code::accept: 			return "Accept";
		case code::if_match: 		return "If-Match";
		case code::if_none_match: 	return "If-None-Match";
		case code::size1: 			return "Size1";
		default:
			break;
	}
	return "undefined";
}

const char* content_format_string(CoAP::Message::content_format format)
{
	switch(format)
	{
		using namespace CoAP::Message;
		case content_format::text_plain: 				return "text/plain;charset=utf-8";
		case content_format::application_link_format:	return "application/link-format";
		case content_format::application_xml: 			return "application/xml";
		case content_format::application_octet_stream: 	return "application/octet-stream";
		case content_format::application_exi: 			return "application/exi";
		case content_format::application_json: 			return "application/json";
		default: break;
	}
	return "undefined";
}

const char* transaction_status_string(CoAP::Transmission::status_t status)
{
	switch(status)
	{
		using namespace CoAP::Transmission;
		case status_t::none:		return "none";
		case status_t::sending:		return "sending";
		case status_t::canceled:	return "canceled";
		case status_t::success:		return "success";
		case status_t::timeout:		return "timeout";
		case status_t::empty:		return "empty";
		default: break;
	}
	return "undefined";
}

}//Debug
}//CoAP
