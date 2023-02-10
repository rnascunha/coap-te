/**
 * @file code.cpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-02-08
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "emscripten/bind.h"

#include "coap-te/message/config.hpp"
#include "coap-te/message/code.hpp"

#include "coap-te/message/options/config.hpp"
#include "coap-te/message/options/definitions.hpp"

EMSCRIPTEN_BINDINGS(coap_enum) {
  namespace em = emscripten;
  namespace co = coap_te::message;

  em::constant("version", co::version);

  em::enum_<co::type>("type")
    .value("confirmable", co::type::confirmable)
    .value("nonconfirmable", co::type::nonconfirmable)
    .value("ack", co::type::ack)
    .value("reset", co::type::reset);

  em::constant("minimum_header_size", co::minimum_header_size);
  em::constant("max_token_size", co::max_token_size);
  em::constant("payload_marker", co::payload_marker);

  em::enum_<co::code>("code")
    .value("empty", co::code::empty)
    .value("get", co::code::get)
    .value("post", co::code::post)
    .value("put", co::code::put)
    .value("delete", co::code::cdelete)
    .value("fetch", co::code::fetch)
    .value("patch", co::code::patch)
    .value("ipatch", co::code::ipatch)
    .value("success", co::code::success)
    .value("created", co::code::created)
    .value("deleted", co::code::deleted)
    .value("changed", co::code::changed)
    .value("content", co::code::content)
    .value("continue", co::code::ccontinue)
    .value("bad_request", co::code::bad_request)
    .value("unauthorized", co::code::unauthorized)
    .value("bad_option", co::code::bad_option)
    .value("forbidden", co::code::forbidden)
    .value("not_found", co::code::not_found)
    .value("method_not_allowed", co::code::method_not_allowed)
    .value("not_accpetable", co::code::not_accpetable)
    .value("request_entity_incomplete", co::code::request_entity_incomplete)
    .value("precondition_failed", co::code::precondition_failed)
    .value("request_entity_too_large", co::code::request_entity_too_large)
    .value("unsupported_content_format", co::code::unsupported_content_format)
    .value("unprocessable_entity", co::code::unprocessable_entity)
    .value("internal_server_error", co::code::internal_server_error)
    .value("not_implemented", co::code::not_implemented)
    .value("bad_gateway", co::code::bad_gateway)
    .value("service_unavaiable", co::code::service_unavaiable)
    .value("gateway_timeout", co::code::gateway_timeout)
    .value("proxying_not_supported", co::code::proxying_not_supported)
    .value("csm", co::code::csm)
    .value("ping", co::code::ping)
    .value("pong", co::code::pong)
    .value("release", co::code::release)
    .value("abort", co::code::abort);

  em::function("code_class", &co::code_class);
  em::function("code_detail", &co::code_detail);

  em::function("is_empty", &co::is_empty);
  em::function("is_request", &co::is_request);
  em::function("is_response", &co::is_response);
  em::function("is_success", &co::is_success);
  em::function("is_client_error", &co::is_client_error);
  em::function("is_server_error", &co::is_server_error);
  em::function("is_error", &co::is_error);
  em::function("is_signaling", &co::is_signaling);

  em::function("check_code", &co::check_code);

  namespace op = co::options;
  em::constant("op_invalid", op::invalid);

  em::enum_<op::number>("op_number")
    .value("invalid", op::number::invalid)
    .value("if_match", op::number::if_match)
    .value("uri_host", op::number::uri_host)
    .value("etag", op::number::etag)
    .value("if_none_match", op::number::if_none_match)
    .value("observe", op::number::observe)
    .value("uri_port", op::number::uri_port)
    .value("location_path", op::number::location_path)
    .value("uri_path", op::number::uri_path)
    .value("content_format", op::number::content_format)
    .value("max_age", op::number::max_age)
    .value("uri_query", op::number::uri_query)
    .value("hop_limit", op::number::hop_limit)
    .value("accept", op::number::accept)
    .value("block2", op::number::block2)
    .value("block1", op::number::block1)
    .value("size2", op::number::size2)
    .value("proxy_uri", op::number::proxy_uri)
    .value("proxy_scheme", op::number::proxy_scheme)
    .value("size1", op::number::size1)
    .value("no_response", op::number::no_response);

  em::enum_<op::format>("format_op")
    .value("empty", op::format::empty)
    .value("opaque", op::format::opaque)
    .value("uint", op::format::uint)
    .value("string", op::format::string);

  em::enum_<op::content>("content")
    .value("text_plain", op::content::text_plain)
    .value("link_format", op::content::link_format)
    .value("xml", op::content::xml)
    .value("octet_stream", op::content::octet_stream)
    .value("exi", op::content::exi)
    .value("json", op::content::json)
    .value("json_patch_json", op::content::json_patch_json)
    .value("merge_patch_json", op::content::merge_patch_json);

  em::enum_<op::accept>("accept")
    .value("text_plain", op::accept::text_plain)
    .value("link_format", op::accept::link_format)
    .value("xml", op::accept::xml)
    .value("octet_stream", op::accept::octet_stream)
    .value("exi", op::accept::exi)
    .value("json", op::accept::json)
    .value("json_patch_json", op::accept::json_patch_json)
    .value("merge_patch_json", op::accept::merge_patch_json);

  em::enum_<op::suppress>("suppress")
    .value("node", op::suppress::none)
    .value("success", op::suppress::success)
    .value("client_error", op::suppress::client_error)
    .value("server_error", op::suppress::server_error);

  em::constant("no_cache_key", op::no_cache_key);
  em::constant("max_age_default", op::max_age_default);
}
