#include "codes.hpp"

namespace CoAP{
namespace Message{

bool check_code(code mcode) noexcept
{
	switch(mcode)
	{
		case code::empty:						//0.00 Empty
		//request
		case code::get:							//0.01 GET
		case code::post:						//0.02 POST
		case code::put:							//0.03 PUT
		case code::cdelete:						//0.04 DELETE
		//response
		//success
		case code::created:						//2.01 Created
		case code::deleted:						//2.02 Deleted
		case code::valid:						//2.03 Valid
		case code::changed:						//2.04 Changed
		case code::content:						//2.05 Content
		//Client Error
		case code::bad_request:					//4.00 Bad Request
		case code::unauthorized:				//4.01 Unauthorized
		case code::bad_option:					//4.02 Bad Option
		case code::forbidden:					//4.03 Forbidden
		case code::not_found:					//4.04 Not Found
		case code::method_not_allowed:			//4.05 Method Not Allowed
		case code::not_accpetable:				//4.06 Not Acceptable
		case code::precondition_failed:			//4.12 Precondition Failed
		case code::request_entity_too_large:	//4.13 Request Entity Too Large
		case code::unsupported_content_format:	//4.15 Unsupported Content-Format
		//Server Error
		case code::internal_server_error:		//5.00 Internal Server Error
		case code::not_implemented:				//5.01 Not Implemented
		case code::bad_gateway:					//5.02 Bad Gateway
		case code::service_unavaiable:			//5.03 Service Unavailable
		case code::gateway_timeout:				//5.04 Gateway Timeout
		case code::proxying_not_supported:		//5.05 Proxying Not Supported
			return true;
		default: break;
	}
	return false;
}

bool is_request(code mcode) noexcept
{
	return mcode != code::empty && ((static_cast<std::uint8_t>(mcode) & 0b11100000) == code_class::request);
}

bool is_response(code mcode) noexcept
{
	return !is_request(mcode) && mcode != code::empty;
}

}//Message
}//CoAP
