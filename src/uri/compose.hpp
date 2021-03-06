#ifndef COAP_TE_URI_COMPOSE_HPP__
#define COAP_TE_URI_COMPOSE_HPP__

#include <cstdint>
#include "message/parser.hpp"
#include "error.hpp"

namespace CoAP{
namespace URI{

std::size_t compose_host(char* buffer, std::size_t buffer_len,
		const char* host, std::size_t host_len, CoAP::Error&);
std::size_t compose_port(char* buffer, std::size_t buffer_len,
		unsigned port, CoAP::Error&);
std::size_t compose_path(char* buffer, std::size_t buffer_len,
		const char* path, std::size_t path_len, CoAP::Error& );
std::size_t compose_query(char* buffer, std::size_t buffer_len,
		const char* query, std::size_t query_len, bool is_first, CoAP::Error&);
std::size_t compose(char* buffer, std::size_t buffer_len,
		CoAP::Message::Option_Parser&,
		CoAP::Error&);

}//URI
}//CoAP

#endif /* COAP_TE_URI_COMPOSE_HPP__ */
