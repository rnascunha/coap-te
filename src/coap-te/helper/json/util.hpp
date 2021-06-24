#ifndef COAP_TE_HELPER_JSON_UTIL_HPP__
#define COAP_TE_HELPER_JSON_UTIL_HPP__

#if COAP_TE_JSON_HELPER == 1

#include <string>
#include "rapidjson/document.h"

namespace CoAP{
namespace Helper{

#define STRING_TO_JSON_VALUE(str, doc)	rapidjson::Value((str), doc.GetAllocator()).Move()

void array_to_json(rapidjson::Value& varr,
					rapidjson::Document& doc,
				const void* array, std::size_t array_len) noexcept;

template<bool Pretty = false>
std::string stringify(rapidjson::Document& doc) noexcept;

}//Helper
}//CoAP

#include "impl/util_impl.hpp"

#endif /* COAP_TE_JSON_HELPER */

#endif /* COAP_TE_HELPER_JSON_UTIL_HPP__ */
