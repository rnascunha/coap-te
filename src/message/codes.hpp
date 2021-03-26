#ifndef COAP_TE_CODES_HPP__
#define COAP_TE_CODES_HPP__

#include "defines/defaults.hpp"
#include <cstdint>

namespace CoAP{
namespace Message{

//https://tools.ietf.org/html/rfc7252#section-5.9
enum code_class : std::uint8_t
{
	request 		= 0,
	//Response
	success 		= 2,
	client_error 	= 4,
	server_error 	= 5
};

constexpr std::uint8_t make_code(code_class code_class, uint8_t detail)
{
	return code_class << 5 | detail;
}

enum class code : std::uint8_t
{
	empty					= 0,									//0.00 Empty
	//request - https://tools.ietf.org/html/rfc7252#section-12.1.1
	get 					= make_code(code_class::request, 1),	//0.01 GET
	post					= make_code(code_class::request, 2),	//0.02 POST
	put						= make_code(code_class::request, 3),	//0.03 PUT
	cdelete					= make_code(code_class::request, 4),	//0.04 DELETE
	//response - https://tools.ietf.org/html/rfc7252#section-12.1.2
	//success
	success					= make_code(code_class::success, 0),	//2.00 Success
	created 				= make_code(code_class::success, 1),	//2.01 Created
	deleted 				= make_code(code_class::success, 2),	//2.02 Deleted
	valid 					= make_code(code_class::success, 3),	//2.03 Valid
	changed 				= make_code(code_class::success, 4),	//2.04 Changed
	content 				= make_code(code_class::success, 5),	//2.05 Content
#if	COAP_TE_BLOCKWISE_TRANSFER == 1
	ccontinue				= make_code(code_class::success, 31),	//2.31 Continue
#endif /* COAP_TE_BLOCKWISE_TRANSFER == 1 */
	//Client Error
	bad_request 			= make_code(code_class::client_error, 0),	//4.00 Bad Request
	unauthorized 			= make_code(code_class::client_error, 1),	//4.01 Unauthorized
	bad_option 				= make_code(code_class::client_error, 2),	//4.02 Bad Option
	forbidden 				= make_code(code_class::client_error, 3),	//4.03 Forbidden
	not_found 				= make_code(code_class::client_error, 4),	//4.04 Not Found
	method_not_allowed 		= make_code(code_class::client_error, 5),	//4.05 Method Not Allowed
	not_accpetable 			= make_code(code_class::client_error, 6),	//4.06 Not Acceptable
#if	COAP_TE_BLOCKWISE_TRANSFER == 1
	request_entity_incomplete = make_code(code_class::client_error, 8),	//4.08 Request Entity Incomplete
#endif /* COAP_TE_BLOCKWISE_TRANSFER == 1 */
	precondition_failed 	= make_code(code_class::client_error, 12),	//4.12 Precondition Failed
	request_entity_too_large = make_code(code_class::client_error, 13), //4.13 Request Entity Too Large
	unsupported_content_format = make_code(code_class::client_error, 15),	//4.15 Unsupported Content-Format
	//Server Error
	internal_server_error 	= make_code(code_class::server_error, 0),	//5.00 Internal Server Error
	not_implemented 		= make_code(code_class::server_error, 1),	//5.01 Not Implemented
	bad_gateway 			= make_code(code_class::server_error, 2),	//5.02 Bad Gateway
	service_unavaiable 		= make_code(code_class::server_error, 3),	//5.03 Service Unavailable
	gateway_timeout 		= make_code(code_class::server_error, 4),	//5.04 Gateway Timeout
	proxying_not_supported 	= make_code(code_class::server_error, 5),	//5.05 Proxying Not Supported
};

bool check_code(code) noexcept;
bool is_request(code) noexcept;
bool is_response(code) noexcept;
bool is_success(code) noexcept;
bool is_client_error(code) noexcept;
bool is_server_error(code) noexcept;
bool is_error(code) noexcept;

}//Message
}//CoAP

#endif /* COAP_TE_CODES_HPP__ */
