#ifndef COAP_TE_HELPER_JSON_IMPL_HPP__
#define COAP_TE_HELPER_JSON_IMPL_HPP__

#if COAP_TE_JSON_HELPER == 1

#include "../convert.hpp"
#include "../util.hpp"
#include "../../../debug/output_string.hpp"

namespace CoAP{
namespace Helper{

template<typename Message>
void options_to_json(rapidjson::Document& doc, Message const& msg) noexcept
{
	//Options
	rapidjson::Value options;
	options.SetArray();

	CoAP::Message::Option::Parser op_parser(msg);
	CoAP::Message::Option::option const* op;
	while((op = op_parser.next()))
	{
		rapidjson::Value vop;
		vop.SetObject();
		vop.AddMember("code", static_cast<unsigned>(op->ocode), doc.GetAllocator());
		vop.AddMember("name", STRING_TO_JSON_VALUE(CoAP::Debug::option_string(op->ocode), doc), doc.GetAllocator());

		CoAP::Message::Option::config<CoAP::Message::Option::code> const* cfg = CoAP::Message::Option::get_config(op->ocode);
		if(!cfg)
		{
			rapidjson::Value op_payload;
			array_to_json(op_payload, doc, op->value, op->length);
			vop.AddMember("value", op_payload, doc.GetAllocator());
			continue;
		}
		else
		{
			switch(cfg->otype)
			{
				using namespace CoAP::Message::Option;
				case type::empty:
					vop.AddMember("value", rapidjson::Value(), doc.GetAllocator());
					break;
				case type::uint:
				{
					unsigned value = CoAP::Message::Option::parse_unsigned(*op);
					vop.AddMember("value", value, doc.GetAllocator());
				}
					break;
				case type::string:
				{
					vop.AddMember("value",
							rapidjson::Value(static_cast<const char*>(op->value), op->length, doc.GetAllocator()).Move(),
							doc.GetAllocator());
				}
					break;
				case type::opaque:
				{
					rapidjson::Value op_payload;
					array_to_json(op_payload, doc, op->value, op->length);
					vop.AddMember("value", op_payload, doc.GetAllocator());
				}
					break;
				default:
					break;
			}
		}
		options.PushBack(vop, doc.GetAllocator());
	}
	doc.AddMember("options", options, doc.GetAllocator());
}

template<typename Message>
void to_json(rapidjson::Document& doc, Message const& msg) noexcept
{
	doc.SetObject();

	header_to_json(doc, msg);
	options_to_json(doc, msg);
	payload_to_json(doc, msg.payload, msg.payload_len);
}

template<bool Pretty /* = false */, typename Message>
std::string to_json(Message const& msg) noexcept
{
	rapidjson::Document doc;

	to_json(doc, msg);

	return stringify<Pretty>(doc);
}

}//Helper
}//CoAP

#endif /* COAP_TE_JSON_HELPER */

#endif /* COAP_TE_HELPER_JSON_IMPL_HPP__ */
