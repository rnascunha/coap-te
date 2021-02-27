#ifndef CoAP_TE_CACHE_HPP__
#define CoAP_TE_CACHE_HPP__

#include <ctime>
#include "message/codes.hpp"

using namespace CoAP::Message;

namespace CoAP{
namespace Cache{

static constexpr code cachable_response[] = {
	code::content,
	//Client Error
	code::bad_request,
	code::unauthorized,
	code::bad_option,
	code::forbidden,
	code::not_found,
	code::method_not_allowed,
	code::not_accpetable,
	code::precondition_failed,
	code::request_entity_too_large,
	code::unsupported_content_format,
	//Server Error
	code::internal_server_error,
	code::not_implemented,
	code::bad_gateway,
	code::service_unavaiable,
	code::gateway_timeout,
	code::proxying_not_supported
};

static constexpr code mark_not_fresh[] = {
	code::deleted,
	code::changed
};

using etag_type = char[8];

struct cache{
	bool			fresh_;
	etag_type		etag_;
	void const*		value_;
	std::time_t		fresh_until_;
};

}//Cache
}//CoAP

#endif /* CoAP_TE_CACHE_HPP__ */
