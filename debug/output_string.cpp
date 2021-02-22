#include "output_string.hpp"

namespace CoAP{
namespace Debug{

const char* option_string(CoAP::Message::option_code op)
{
	switch(op)
	{
		using namespace CoAP::Message;
		case option_code::content_format:	return "Content-Format";
		case option_code::etag: 			return "ETag";
		case option_code::location_path: 	return "Location-Path";
		case option_code::location_query: 	return "Location-Query";
		case option_code::max_age: 			return "Max-Age";
		case option_code::proxy_uri: 		return "Proxy-Uri";
		case option_code::proxy_scheme: 	return "Proxy-Scheme";
		case option_code::uri_host: 		return "Uri-Host";
		case option_code::uri_path: 		return "Uri-Path";
		case option_code::uri_port: 		return "Uri-Port";
		case option_code::uri_query: 		return "Uri-Query";
		case option_code::accept: 			return "Accept";
		case option_code::if_match: 		return "If-Match";
		case option_code::if_none_match: 	return "If-None-Match";
		case option_code::size1: 			return "Size1";
		default:
			break;
	}
	return "undefined";
}

const char* response_string(CoAP::Message::code code)
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

}//Debug
}//CoAP
