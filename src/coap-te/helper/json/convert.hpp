#ifndef COAP_TE_HELPER_JSON_HPP__
#define COAP_TE_HELPER_JSON_HPP__

#if COAP_TE_JSON_HELPER == 1

#include <string>
#include "coap-te.hpp"
#include "rapidjson/document.h"

namespace CoAP{
namespace Helper{

void header_to_json(rapidjson::Document& doc, CoAP::Message::message const& msg) noexcept;
void header_to_json(rapidjson::Document& doc, CoAP::Message::Reliable::message const& msg) noexcept;
void token_to_json(rapidjson::Document& doc, const void* token, std::size_t token_len) noexcept;

template<typename Message>
void options_to_json(rapidjson::Document& doc, Message const& msg) noexcept;

void payload_to_json(rapidjson::Document& doc,
		const void* payload, std::size_t payload_len) noexcept;

template<typename Message>
void to_json(rapidjson::Document& doc, Message const& msg) noexcept;

template<bool Pretty = false, typename Message>
std::string to_json(Message const& msg) noexcept;

}//Helper
}//CoAP

#include "impl/convert_impl.hpp"

#endif /* COAP_TE_JSON_HELPER */

#endif /* COAP_TE_HELPER_JSON_HPP__ */
