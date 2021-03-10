#ifndef COAP_TE_DEFINES_CONNECTION_HPP__
#define COAP_TE_DEFINES_CONNECTION_HPP__

#include <utility>
#include <cstdint>
#include "error.hpp"

namespace CoAP{

template<typename ConnType,
		typename Endpoint>
class connection_base{
	public:
		using endpoint = Endpoint;

		connection_base(ConnType&& conn) : conn_(std::move(conn)){}

		std::size_t send(const void* buffer, std::size_t buffer_len, Endpoint& ep, CoAP::Error& ec) noexcept
		{
			return conn_.send(buffer, buffer_len, ep, ec);
		}
		std::size_t receive(std::uint8_t* buffer, std::size_t buffer_len, Endpoint& ep, CoAP::Error& ec) noexcept
		{
			return conn_.receive(buffer, buffer_len, ep, ec);
		}

		ConnType& native_handler(){ return conn_; }
	private:
		ConnType conn_;
};

}//CoAP

#endif /* COAP_TE_DEFINES_CONNECTION_HPP__ */
