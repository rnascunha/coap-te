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
#if	COAP_TE_BLOCKWISE_TRANSFER == 1
		case code::ccontinue:					//2.31 Continue
#endif /* COAP_TE_BLOCKWISE_TRANSFER == 1 */
		//Client Error
		case code::bad_request:					//4.00 Bad Request
		case code::unauthorized:				//4.01 Unauthorized
		case code::bad_option:					//4.02 Bad Option
		case code::forbidden:					//4.03 Forbidden
		case code::not_found:					//4.04 Not Found
		case code::method_not_allowed:			//4.05 Method Not Allowed
		case code::not_accpetable:				//4.06 Not Acceptable
#if	COAP_TE_BLOCKWISE_TRANSFER == 1
		case code::request_entity_incomplete:	//4.08 Request Entity Incomplete
#endif /* COAP_TE_BLOCKWISE_TRANSFER == 1 */
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
#if COAP_TE_RELIABLE_CONNECTION == 1
		case code::csm:							//7.01 CSM
		case code::ping:						//7.02 Ping
		case code::pong:						//7.03 Pong
		case code::release:						//7.04 Release
		case code::abort:						//7.05 Abort
#endif /* COAP_TE_RELIABLE_CONNECTION == 1 */
			return true;
		default: break;
	}
	return false;
}

bool is_empty(code mcode) noexcept
{
	return mcode == code::empty;
}

bool is_request(code mcode) noexcept
{
	return mcode != code::empty && ((static_cast<std::uint8_t>(mcode) & 0b11100000) == code_class::request);
}

bool is_response(code mcode) noexcept
{
	return !is_request(mcode) && mcode != code::empty;
}

bool is_success(code mcode) noexcept
{
	return mcode != code::empty && (static_cast<int>(mcode) >> 5) == code_class::success;
}

bool is_client_error(code mcode) noexcept
{
	return mcode != code::empty && (static_cast<int>(mcode) >> 5) == code_class::client_error;
}

bool is_server_error(code mcode) noexcept
{
	return mcode != code::empty && (static_cast<int>(mcode) >> 5) == code_class::server_error;
}

bool is_error(code mcode) noexcept
{
	return is_server_error(mcode) || is_client_error(mcode);
}

#if COAP_TE_RELIABLE_CONNECTION == 1
bool is_signaling(code mcode) noexcept
{
	return mcode != code::empty && ((static_cast<std::uint8_t>(mcode) >> 5) == code_class::signaling);
}
#endif /* COAP_TE_RELIABLE_CONNECTION == 1 */

}//Message
}//CoAP
