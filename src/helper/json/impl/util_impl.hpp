#ifndef AGRO_MESH_MESSAGE_JSON_UTIL_IMPL_HPP__
#define AGRO_MESH_MESSAGE_JSON_UTIL_IMPL_HPP__

#if COAP_TE_JSON_HELPER == 1

#include "../util.hpp"
#include "rapidjson/writer.h" // for stringify JSON
#include "rapidjson/prettywriter.h" // for stringify JSON

namespace CoAP{
namespace Helper{

template<bool Pretty /* = false */>
std::string stringify(rapidjson::Document& doc) noexcept
{
	rapidjson::StringBuffer sb;

	if constexpr(Pretty)
	{
		rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
		doc.Accept(writer);

		return std::string{sb.GetString()};
	}
	else
	{
		rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
		doc.Accept(writer);

		return std::string{sb.GetString()};
	}
}

}//Helper
}//CoAP

#endif /* COAP_TE_JSON_HELPER == 1 */

#endif /* AGRO_MESH_MESSAGE_JSON_UTIL_IMPL_HPP__ */
