#if COAP_TE_JSON_HELPER == 1

#include "convert.hpp"
#include "../../debug/output_string.hpp"
#include "util.hpp"

namespace CoAP{
namespace Helper{

void payload_to_json(rapidjson::Document& doc,
		const void* payload, std::size_t payload_len) noexcept
{
	rapidjson::Value vpayload;
	array_to_json(vpayload, doc, payload, payload_len);
	doc.AddMember("payload", vpayload, doc.GetAllocator());
}

void token_to_json(rapidjson::Document& doc, const void* token, std::size_t token_len) noexcept
{
	//Token
	rapidjson::Value vtoken;
	array_to_json(vtoken, doc, token, token_len);
	doc.AddMember("token", vtoken, doc.GetAllocator());
}

void header_to_json(rapidjson::Document& doc, CoAP::Message::message const& msg) noexcept
{
	doc.AddMember("type", static_cast<unsigned>(msg.mtype), doc.GetAllocator());
	doc.AddMember("type_name",
			STRING_TO_JSON_VALUE(CoAP::Debug::type_string(msg.mtype), doc),
			doc.GetAllocator());
	doc.AddMember("code", static_cast<unsigned>(msg.mcode), doc.GetAllocator());
	doc.AddMember("code_name",
				STRING_TO_JSON_VALUE(CoAP::Debug::code_string(msg.mcode), doc),
				doc.GetAllocator());
	doc.AddMember("mid", static_cast<unsigned>(msg.mid), doc.GetAllocator());
	token_to_json(doc, msg.token, msg.token_len);
}

void header_to_json(rapidjson::Document& doc, CoAP::Message::Reliable::message const& msg) noexcept
{
	doc.AddMember("length", msg.len, doc.GetAllocator());
	doc.AddMember("code", static_cast<unsigned>(msg.mcode), doc.GetAllocator());
	doc.AddMember("code_name",
					STRING_TO_JSON_VALUE(CoAP::Debug::code_string(msg.mcode), doc),
					doc.GetAllocator());
	token_to_json(doc, msg.token, msg.token_len);
}

}//Helper
}//CoAP

#endif /* COAP_TE_JSON_HELPER */
