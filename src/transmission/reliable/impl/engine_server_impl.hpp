#ifndef COAP_TE_TRANSMISSION_RELIABLE_ENGINE_SERVER_IMPL_HPP__
#define COAP_TE_TRANSMISSION_RELIABLE_ENGINE_SERVER_IMPL_HPP__

#include <functional>

#include "log.hpp"
#include "message/reliable/parser.hpp"
#include "message/options/options.hpp"
#include "message/options/functions2.hpp"
#include "../engine_client.hpp"
#include "../types.hpp"
#include "../functions.hpp"

namespace CoAP{
namespace Transmission{
namespace Reliable{

template<typename Connection,
	csm_configure const& Config,
	typename ConnectionList,
	typename TransactionList,
	typename CallbackDefaultFunctor,
	typename Resource>
engine_server<Connection, Config, ConnectionList, TransactionList, CallbackDefaultFunctor, Resource>::
engine_server(){}

template<typename Connection,
	csm_configure const& Config,
	typename ConnectionList,
	typename TransactionList,
	typename CallbackDefaultFunctor,
	typename Resource>
engine_server<Connection, Config, ConnectionList, TransactionList, CallbackDefaultFunctor, Resource>::
~engine_server()
{
	close<false>();
}

template<typename Connection,
	csm_configure const& Config,
	typename ConnectionList,
	typename TransactionList,
	typename CallbackDefaultFunctor,
	typename Resource>
bool
engine_server<Connection, Config, ConnectionList, TransactionList, CallbackDefaultFunctor, Resource>::
open(endpoint& ep, CoAP::Error& ec) noexcept
{
	debug(engine_mod, "Opening");

	conn_.open(ep, ec);
	if(ec) return false;
	return true;
}

template<typename Connection,
	csm_configure const& Config,
	typename ConnectionList,
	typename TransactionList,
	typename CallbackDefaultFunctor,
	typename Resource>
template<bool SendAbortMessage>
void
engine_server<Connection, Config, ConnectionList, TransactionList, CallbackDefaultFunctor, Resource>::
close() noexcept
{
	debug(engine_mod, "Closing");

	if(!conn_.is_open()) return;

	if constexpr(has_connection_list)
	{
		for(unsigned i = 0; i < conn_list_.size(); i++)
			close_client<SendAbortMessage>(conn_list_[i]->socket());
	}
	conn_.close();
}

template<typename Connection,
	csm_configure const& Config,
	typename ConnectionList,
	typename TransactionList,
	typename CallbackDefaultFunctor,
	typename Resource>
template<bool SendAbortMessage>
void
engine_server<Connection, Config, ConnectionList, TransactionList, CallbackDefaultFunctor, Resource>::
close_client(socket sock) noexcept
{
	close_client<SendAbortMessage>(sock, nullptr);
}

template<typename Connection,
	csm_configure const& Config,
	typename ConnectionList,
	typename TransactionList,
	typename CallbackDefaultFunctor,
	typename Resource>
template<bool SendAbortMessage>
void
engine_server<Connection, Config, ConnectionList, TransactionList, CallbackDefaultFunctor, Resource>::
close_client(socket sock, const char* payload [[maybe_unused]]) noexcept
{
	debug(engine_mod, "Closing client [%d]", sock);
	if(sock == 0) return;

	if constexpr(has_transaction_list)
	{
		list_.cancel_all(sock);
	}

	if constexpr(has_connection_list)
	{
		debug(engine_mod, "Removing list [%d]", sock);
		conn_list_.close(sock);
	}

	if constexpr(SendAbortMessage)
	{
		send_abort(sock, payload);
	}

	conn_.close_client(sock);
}

template<typename Connection,
	csm_configure const& Config,
	typename ConnectionList,
	typename TransactionList,
	typename CallbackDefaultFunctor,
	typename Resource>
void
engine_server<Connection, Config, ConnectionList, TransactionList, CallbackDefaultFunctor, Resource>::
process(socket sock, std::uint8_t
		const* buffer, std::size_t buffer_len,
				CoAP::Error& ec) noexcept
{
	debug(engine_mod, "Processing buffer");
	CoAP::Message::Reliable::message msg;

	std::size_t total_size = 0;
	while(true)
	{
		msg.reset();
		std::size_t size = CoAP::Message::Reliable::parse(msg,
				buffer  + total_size, buffer_len - total_size,
				ec);

		if(ec)
		{
			debug(engine_mod, "Error parsing received message...");
			if(ec == CoAP::errc::insufficient_buffer)
			{
				std::size_t bu = make_response_code_error<set_length>(msg,
						buffer_, Config.max_message_size,
						CoAP::Message::code::request_entity_too_large);
				conn_.send(sock, buffer_, bu, ec);
			}
			return;
		}

		/**
		 * https://tools.ietf.org/html/rfc8323#section-3.4
		 *
		 * Empty messages (Code 0.00) can always be sent and MUST be ignored by
		 * the recipient.  This provides a basic keepalive function that can
		 * refresh NAT bindings.
		 */
		if(CoAP::Message::is_empty(msg.mcode))
		{
			debug(engine_mod, "Empty message received... Ignoring");
			return;
		}

		if(CoAP::Message::is_signaling(msg.mcode))
			process_signaling(sock, msg);
		else if(CoAP::Message::is_response(msg.mcode))
			process_response(sock, msg);
		else //is_request;
		{
			if constexpr(get_profile() == profile::server)
				process_request(sock, msg, ec);
			else
			{
				/**
				 * https://tools.ietf.org/html/rfc8323#section-3.3
				 *
				 * If one side does not implement a CoAP server, an error response
				 * MUST be returned for all CoAP requests from the other side. The
				 * simplest approach is to always return 5.01 (Not Implemented)
				 */
				std::size_t bu = make_response_code_error<set_length>(msg,
								buffer_, Config.max_message_size,
								CoAP::Message::code::not_implemented);
				conn_.send(sock, buffer_, bu, ec);
			}
		}

		total_size += size;
		if(total_size >= buffer_len) break;
	}
}

template<typename Connection,
	csm_configure const& Config,
	typename ConnectionList,
	typename TransactionList,
	typename CallbackDefaultFunctor,
	typename Resource>
void
engine_server<Connection, Config, ConnectionList, TransactionList, CallbackDefaultFunctor, Resource>::
process_response(socket sock, CoAP::Message::Reliable::message const& msg) noexcept
{
	debug(engine_mod, "Received resposne");
	if constexpr(has_transaction_list)
		if(list_.template check_all_response(sock, msg)) return;
	if constexpr(has_default_callback)
		if(default_cb_) default_cb_(sock, msg, this);
}

template<typename Connection,
	csm_configure const& Config,
	typename ConnectionList,
	typename TransactionList,
	typename CallbackDefaultFunctor,
	typename Resource>
void
engine_server<Connection, Config, ConnectionList, TransactionList, CallbackDefaultFunctor, Resource>::
process_request(socket sock,
			CoAP::Message::Reliable::message const& request,
			CoAP::Error& ec) noexcept
{
	debug(engine_mod, "Process request [%d]", sock);
	resource const* res = resource_root_.search(request);
	if(!res)
	{
		std::size_t bu = make_response_code_error<set_length>(
				request, buffer_, Config.max_message_size,
				CoAP::Message::code::not_found);
		conn_.send(sock, buffer_, bu, ec);
	}
	else
	{
		debug(engine_mod, "Found resource %s", res->path());
		response response(sock,
				request.token, request.token_len,
				buffer_, Config.max_message_size);
		if(res->call(request.mcode, request, response, this))
		{
			debug(engine_mod, "Method found");
			if(!response.error() && response.buffer_used() > 0)
			{
				conn_.send(sock, response.buffer(), response.buffer_used(), ec);
			}
		}
		else
		{
			std::size_t bu = make_response_code_error<set_length>(
					request, buffer_, Config.max_message_size,
					CoAP::Message::code::method_not_allowed);
			conn_.send(sock, buffer_, bu, ec);
		}
	}
}

template<typename Connection,
	csm_configure const& Config,
	typename ConnectionList,
	typename TransactionList,
	typename CallbackDefaultFunctor,
	typename Resource>
void
engine_server<Connection, Config, ConnectionList, TransactionList, CallbackDefaultFunctor, Resource>::
process_signaling(socket sock, CoAP::Message::Reliable::message const& msg) noexcept
{
	debug(engine_mod, "Signaling message received");

	switch(msg.mcode)
	{
		using namespace CoAP::Message;
		case code::csm:
			process_signaling_csm(sock, msg);
			break;
		case code::ping:
			process_signaling_ping(sock, msg);
			break;
		case code::pong:
			process_signaling_pong(sock, msg);
			break;
		case code::release:
			process_signaling_release(sock, msg);
			break;
		case code::abort:
			process_signaling_abort(sock, msg);
			break;
		default:
			return;
	}
	if constexpr(has_default_callback)
		if(default_cb_) default_cb_(sock, msg, this);
}

template<typename Connection,
	csm_configure const& Config,
	typename ConnectionList,
	typename TransactionList,
	typename CallbackDefaultFunctor,
	typename Resource>
void
engine_server<Connection, Config, ConnectionList, TransactionList, CallbackDefaultFunctor, Resource>::
process_signaling_csm(socket sock, CoAP::Message::Reliable::message const& msg) noexcept
{
	debug(engine_mod, "CSM message received");

	if constexpr(!has_connection_list) return;

	using namespace CoAP::Message;

	connection_hold_t* conn = conn_list_.find(sock);
	if(!conn) return;

	CoAP::Transmission::Reliable::process_signaling_csm(conn->csm(), msg);
}

template<typename Connection,
	csm_configure const& Config,
	typename ConnectionList,
	typename TransactionList,
	typename CallbackDefaultFunctor,
	typename Resource>
void
engine_server<Connection, Config, ConnectionList, TransactionList, CallbackDefaultFunctor, Resource>::
process_signaling_ping(socket sock, CoAP::Message::Reliable::message const& msg) noexcept
{
	debug(engine_mod, "Ping message received");

	using namespace CoAP::Message;

	Option::option_ping opt;

	request<code::pong> req{sock};

	Option::node_pong custody_op{Option::ping_pong::custody};
	if(Option::get_option(msg, opt, Option::ping_pong::custody))
		req.add_option(custody_op);

	CoAP::Error ec;
	send<false, false, false, false>(req, ec);
}

template<typename Connection,
	csm_configure const& Config,
	typename ConnectionList,
	typename TransactionList,
	typename CallbackDefaultFunctor,
	typename Resource>
void
engine_server<Connection, Config, ConnectionList, TransactionList, CallbackDefaultFunctor, Resource>::
process_signaling_pong(socket, CoAP::Message::Reliable::message const&) noexcept
{
	debug(engine_mod, "Pong message received");
}

template<typename Connection,
	csm_configure const& Config,
	typename ConnectionList,
	typename TransactionList,
	typename CallbackDefaultFunctor,
	typename Resource>
void
engine_server<Connection, Config, ConnectionList, TransactionList, CallbackDefaultFunctor, Resource>::
process_signaling_release(socket, CoAP::Message::Reliable::message const&) noexcept
{
	debug(engine_mod, "Release message received");
}

template<typename Connection,
	csm_configure const& Config,
	typename ConnectionList,
	typename TransactionList,
	typename CallbackDefaultFunctor,
	typename Resource>
void
engine_server<Connection, Config, ConnectionList, TransactionList, CallbackDefaultFunctor, Resource>::
process_signaling_abort(socket sock, CoAP::Message::Reliable::message const&) noexcept
{
	debug(engine_mod, "Abort message received");
	close_client<false>(sock);
}

template<typename Connection,
	csm_configure const& Config,
	typename ConnectionList,
	typename TransactionList,
	typename CallbackDefaultFunctor,
	typename Resource>
typename engine_server<Connection, Config, ConnectionList, TransactionList, CallbackDefaultFunctor, Resource>::resource&
engine_server<Connection, Config, ConnectionList, TransactionList, CallbackDefaultFunctor, Resource>::
root() noexcept
{
	static_assert(get_profile() == profile::server, "Resource just available at 'server' profile");
	return resource_root_.root();
}

template<typename Connection,
	csm_configure const& Config,
	typename ConnectionList,
	typename TransactionList,
	typename CallbackDefaultFunctor,
	typename Resource>
typename engine_server<Connection, Config, ConnectionList, TransactionList, CallbackDefaultFunctor, Resource>::resource_root&
engine_server<Connection, Config, ConnectionList, TransactionList, CallbackDefaultFunctor, Resource>::
root_node() noexcept
{
	static_assert(get_profile() == profile::server, "Resource just available at 'server' profile");
	return resource_root_;
}

template<typename Connection,
	csm_configure const& Config,
	typename ConnectionList,
	typename TransactionList,
	typename CallbackDefaultFunctor,
	typename Resource>
void
engine_server<Connection, Config, ConnectionList, TransactionList, CallbackDefaultFunctor, Resource>::
default_cb(default_response_cb cb) noexcept
{
	static_assert(has_default_callback, "Default callback NOT set");
	default_cb_ = cb;
}

template<typename Connection,
	csm_configure const& Config,
	typename ConnectionList,
	typename TransactionList,
	typename CallbackDefaultFunctor,
	typename Resource>
template<int BlockTimeMs /* = 0 */,
		unsigned MaxEvents /* = 32 */>
bool
engine_server<Connection, Config, ConnectionList, TransactionList, CallbackDefaultFunctor, Resource>::
run(CoAP::Error& ec) noexcept
{
	using engine = engine_server<Connection, Config, ConnectionList, TransactionList, CallbackDefaultFunctor, Resource>;
	using namespace std::placeholders;
	conn_.template receive<BlockTimeMs, MaxEvents>(
			buffer_, packet_size, ec,
			std::bind(&engine::on_read, this, _1, _2, _3),
			std::bind(&engine::on_open, this, _1),
			std::bind(&engine::on_close, this, _1));

	if(ec)
	{
		error(engine_mod, ec, "read");
		close<false>();
		return false;
	}

	check_transactions();

	return true;
}

template<typename Connection,
	csm_configure const& Config,
	typename ConnectionList,
	typename TransactionList,
	typename CallbackDefaultFunctor,
	typename Resource>
void
engine_server<Connection, Config, ConnectionList, TransactionList, CallbackDefaultFunctor, Resource>::
on_read(socket sock, void* buffer, std::size_t size) noexcept
{
	debug(engine_mod, "Received %zu bytes [%d]", size, sock);
	CoAP::Error ec;
	process(sock, buffer_, size, ec);
}

template<typename Connection,
	csm_configure const& Config,
	typename ConnectionList,
	typename TransactionList,
	typename CallbackDefaultFunctor,
	typename Resource>
void
engine_server<Connection, Config, ConnectionList, TransactionList, CallbackDefaultFunctor, Resource>::
on_open(socket sock) noexcept
{
	debug(engine_mod, "Opened socket[%d]", sock);
	if constexpr(has_connection_list)
	{
		debug(engine_mod, "Has conn[%d]", sock);
		connection_hold_t* conn = conn_list_.find_free_slot();
		if(!conn)
		{
			debug(engine_mod, "Conn not found[%d][%u/%u]", sock, conn_list_.ocupied(), conn_list_.size());
			CoAP::Error ec;

			send_abort(sock, "max connection reached", ec);
			close_client(sock);

			return;
		}
		debug(engine_mod, "Init conn[%d/%u]", sock, conn_list_.size());
		conn->init(sock);
	}

	debug(engine_mod, "Making CSM");
	CoAP::Error ec;
	std::size_t size = make_csm_message<set_length>(Config, buffer_, Config.max_message_size, ec);
	if(ec) return;

	debug(engine_mod, "Sending CSM[%lu]", size);
	send(sock, buffer_, size, ec);
}

template<typename Connection,
	csm_configure const& Config,
	typename ConnectionList,
	typename TransactionList,
	typename CallbackDefaultFunctor,
	typename Resource>
void
engine_server<Connection, Config, ConnectionList, TransactionList, CallbackDefaultFunctor, Resource>::
on_close(socket sock) noexcept
{
	debug(engine_mod, "Closed socket[%d/%u]", sock, conn_list_.size());
	close_client<false>(sock);
	debug(engine_mod, "Closed [%u]", conn_list_.size());
}

template<typename Connection,
	csm_configure const& Config,
	typename ConnectionList,
	typename TransactionList,
	typename CallbackDefaultFunctor,
	typename Resource>
void
engine_server<Connection, Config, ConnectionList, TransactionList, CallbackDefaultFunctor, Resource>::
check_transactions() noexcept
{
	if constexpr(has_transaction_list)
		list_.check_all();
}

template<typename Connection,
	csm_configure const& Config,
	typename ConnectionList,
	typename TransactionList,
	typename CallbackDefaultFunctor,
	typename Resource>
template<int BlockTimeMs /* = 0 */,
		unsigned MaxEvents /* = 32 */>
bool
engine_server<Connection, Config, ConnectionList, TransactionList, CallbackDefaultFunctor, Resource>::
operator()(CoAP::Error& ec) noexcept
{
	return run<BlockTimeMs, MaxEvents>(ec);
}

}//Reliable
}//Transmission
}//CoAP

#endif /* COAP_TE_TRANSMISSION_RELIABLE_TRANSACTION_IMPL_HPP__ */
