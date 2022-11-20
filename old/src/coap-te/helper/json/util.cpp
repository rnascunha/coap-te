#if COAP_TE_JSON_HELPER == 1

#include "util.hpp"

namespace CoAP{
namespace Helper{

void array_to_json(rapidjson::Value& varr,
					rapidjson::Document& doc,
				const void* array, std::size_t array_len) noexcept
{
	varr.SetArray();
	const std::uint8_t* array_8 = static_cast<const uint8_t*>(array);
	for(std::size_t i = 0; i < array_len; i++)
	{
		varr.PushBack(array_8[i], doc.GetAllocator());
	}
}

}//Helper
}//CoAP

#endif /* COAP_TE_JSON_HELPER */
