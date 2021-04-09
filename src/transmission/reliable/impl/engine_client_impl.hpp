#ifndef COAP_TE_TRANSMISSION_RELIABLE_ENGINE_CLIENT_IMPL_HPP__
#define COAP_TE_TRANSMISSION_RELIABLE_ENGINE_CLIENT_IMPL_HPP__

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

static constexpr CoAP::Log::module engine_mod = {
		.name = "ENGINE",
		.max_level = CoAP::Log::type::debug,
		.enable = true
};

template<typename Connection,
	csm_configure const& Config,
	typename TransactionList,
	typename CallbackDefaultFunctor,
	typename Resource>
engine_client<Connection, Config, TransactionList, CallbackDefaultFunctor, Resource>::
engine_client(){}

template<typename Connection,
	csm_configure const& Config,
	typename TransactionList,
	typename CallbackDefaultFunctor,
	typename Resource>
engine_client<Connection, Config, TransactionList, CallbackDefaultFunctor, Resource>::
~engine_client()
{
	close<false>();
}

template<typename Connection,
	csm_configure const& Config,
	typename TransactionList,
	typename CallbackDefaultFunctor,
	typename Resource>
bool
engine_client<Connection, Config, TransactionList, CallbackDefaultFunctor, Resource>::
open(endpoint& ep, CoAP::Error& ec) noexcept
{
	conn_.open(ep, ec);
	if(ec) return false;

	std::size_t size = make_csm_message<Connection::set_length>(Config, buffer_, Config.max_message_size, ec);
	if(ec) return false;

	send(buffer_, size, ec);
	return ec ? false : true;
}

template<typename Connection,
	csm_configure const& Config,
	typename TransactionList,
	typename CallbackDefaultFunctor,
	typename Resource>
template<bool SendAbortMessage>
void
engine_client<Connection, Config, TransactionList, CallbackDefaultFunctor, Resource>::
close(const char* payload /* = nullptr */ [[maybe_unused]]) noexcept
{
	if(!conn_.is_open()) return;

	if constexpr(has_transaction_list)
	{
		list_.cancel_all();
	}

	if constexpr(SendAbortMessage)
	{
		CoAP::Error ec;
		send_abort(payload, ec);
	}

	conn_.close();
}

template<typename Connection,
	csm_configure const& Config,
	typename TransactionList,
	typename CallbackDefaultFunctor,
	typename Resource>
csm_configure const&
engine_client<Connection, Config, TransactionList, CallbackDefaultFunctor, Resource>::
server_csm() const noexcept
{
	return server_csm_;
}

template<typename Connection,
	csm_configure const& Config,
	typename TransactionList,
	typename CallbackDefaultFunctor,
	typename Resource>
void
engine_client<Connection, Config, TransactionList, CallbackDefaultFunctor, Resource>::
process(std::uint8_t const* buffer, std::size_t buffer_len,
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
				conn_.send(buffer_, bu, ec);
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
			process_signaling(msg);
		else if(CoAP::Message::is_response(msg.mcode))
			process_response(msg);
		else //is_request;
		{
			if constexpr(get_profile() == profile::server)
				process_request(msg, ec);
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
				conn_.send(buffer_, bu, ec);
			}
		}

		total_size += size;
		if(total_size >= buffer_len) break;
	}
}

template<typename Connection,
	csm_configure const& Config,
	typename TransactionList,
	typename CallbackDefaultFunctor,
	typename Resource>
void
engine_client<Connection, Config, TransactionList, CallbackDefaultFunctor, Resource>::
process_response(CoAP::Message::Reliable::message const& msg) noexcept
{
	if constexpr(has_transaction_list)
		if(list_.template check_all_response(conn_.native(), msg)) return;
	if constexpr(has_default_callback)
		if(default_cb_) default_cb_(conn_.native(), msg, this);
}

template<typename Connection,
	csm_configure const& Config,
	typename TransactionList,
	typename CallbackDefaultFunctor,
	typename Resource>
void
engine_client<Connection, Config, TransactionList, CallbackDefaultFunctor, Resource>::
process_request(CoAP::Message::Reliable::message const& request,
							CoAP::Error& ec) noexcept
{
	resource const* res = resource_root_.search(request);
	if(!res)
	{
		std::size_t bu = make_response_code_error<set_length>(
				request, buffer_, Config.max_message_size,
				CoAP::Message::code::not_found);
		conn_.send(buffer_, bu, ec);
	}
	else
	{
		debug(engine_mod, "Found resource %s", res->path());
		response response(conn_.native(),
				request.token, request.token_len,
				buffer_, Config.max_message_size);
		if(res->call(request.mcode, request, response, this))
		{
			debug(engine_mod, "Method found");
			if(!response.error() && response.buffer_used() > 0)
			{
				conn_.send(response.buffer(), response.buffer_used(), ec);
			}
		}
		else
		{
			std::size_t bu = make_response_code_error<set_length>(
					request, buffer_, Config.max_message_size,
					CoAP::Message::code::method_not_allowed);
			conn_.send(buffer_, bu, ec);
		}
	}
}

template<typename Connection,
	csm_configure const& Config,
	typename TransactionList,
	typename CallbackDefaultFunctor,
	typename Resource>
void
engine_client<Connection, Config, TransactionList, CallbackDefaultFunctor, Resource>::
process_signaling(CoAP::Message::Reliable::message const& msg) noexcept
{
	switch(msg.mcode)
	{
		using namespace CoAP::Message;
		case code::csm:
			process_signaling_csm(msg);
			break;
		case code::ping:
			process_signaling_ping(msg);
			break;
		case code::pong:
			process_signaling_pong(msg);
			break;
		case code::release:
			process_signaling_release(msg);
			break;
		case code::abort:
			process_signaling_abort(msg);
			break;
		default:
			return;
	}
	if constexpr(has_default_callback)
		if(default_cb_) default_cb_(conn_.native(), msg, this);
}

template<typename Connection,
	csm_configure const& Config,
	typename TransactionList,
	typename CallbackDefaultFunctor,
	typename Resource>
void
engine_client<Connection, Config, TransactionList, CallbackDefaultFunctor, Resource>::
process_signaling_csm(CoAP::Message::Reliable::message const& msg) noexcept
{
	CoAP::Transmission::Reliable::process_signaling_csm(server_csm_, msg);
}

template<typename Connection,
	csm_configure const& Config,
	typename TransactionList,
	typename CallbackDefaultFunctor,
	typename Resource>
void
engine_client<Connection, Config, TransactionList, CallbackDefaultFunctor, Resource>::
process_signaling_ping(CoAP::Message::Reliable::message const& msg) noexcept
{
	using namespace CoAP::Message;

	Option::option_ping opt;

	request<code::pong> req{conn_.native()};

	Option::node_pong custody_op{Option::ping_pong::custody};
	if(Option::get_option(msg, opt, Option::ping_pong::custody))
		req.add_option(custody_op);

	CoAP::Error ec;
	send<false, false, false, false>(req, ec);
}

template<typename Connection,
	csm_configure const& Config,
	typename TransactionList,
	typename CallbackDefaultFunctor,
	typename Resource>
void
engine_client<Connection, Config, TransactionList, CallbackDefaultFunctor, Resource>::
process_signaling_pong(CoAP::Message::Reliable::message const&) noexcept
{
	debug(engine_mod, "Pong message received");
}

template<typename Connection,
	csm_configure const& Config,
	typename TransactionList,
	typename CallbackDefaultFunctor,
	typename Resource>
void
engine_client<Connection, Config, TransactionList, CallbackDefaultFunctor, Resource>::
process_signaling_release(CoAP::Message::Reliable::message const&) noexcept
{
	debug(engine_mod, "Release message received");
}

template<typename Connection,
	csm_configure const& Config,
	typename TransactionList,
	typename CallbackDefaultFunctor,
	typename Resource>
void
engine_client<Connection, Config, TransactionList, CallbackDefaultFunctor, Resource>::
process_signaling_abort(CoAP::Message::Reliable::message const&) noexcept
{
	debug(engine_mod, "Abort message received");
	close<false>();
}

template<typename Connection,
	csm_configure const& Config,
	typename TransactionList,
	typename CallbackDefaultFunctor,
	typename Resource>
typename engine_client<Connection, Config, TransactionList, CallbackDefaultFunctor, Resource>::resource&
engine_client<Connection, Config, TransactionList, CallbackDefaultFunctor, Resource>::
root() noexcept
{
	static_assert(get_profile() == profile::server, "Resource just available at 'server' profile");
	return resource_root_.root();
}

template<typename Connection,
	csm_configure const& Config,
	typename TransactionList,
	typename CallbackDefaultFunctor,
	typename Resource>
typename engine_client<Connection, Config, TransactionList, CallbackDefaultFunctor, Resource>::resource_root&
engine_client<Connection, Config, TransactionList, CallbackDefaultFunctor, Resource>::
root_node() noexcept
{
	static_assert(get_profile() == profile::server, "Resource just available at 'server' profile");
	return resource_root_;
}

template<typename Connection,
	csm_configure const& Config,
	typename TransactionList,
	typename CallbackDefaultFunctor,
	typename Resource>
void
engine_client<Connection, Config, TransactionList, CallbackDefaultFunctor, Resource>::
default_cb(default_response_cb cb) noexcept
{
	static_assert(has_default_callback, "Default callback NOT set");
	default_cb_ = cb;
}

template<typename Connection,
	csm_configure const& Config,
	typename TransactionList,
	typename CallbackDefaultFunctor,
	typename Resource>
bool
engine_client<Connection, Config, TransactionList, CallbackDefaultFunctor, Resource>::
run(CoAP::Error& ec) noexcept
{
	std::size_t size = conn_.receive(buffer_, packet_size, ec);
	if(ec)
	{
		error(engine_mod, ec, "read");
		close<false>();
		return false;
	}

	if(size)
	{
		CoAP::Log::debug(engine_mod, "Received %zu bytes", size);
		process(buffer_, size, ec);
	}

	check_transactions();

	return true;
}

template<typename Connection,
	csm_configure const& Config,
	typename TransactionList,
	typename CallbackDefaultFunctor,
	typename Resource>
void
engine_client<Connection, Config, TransactionList, CallbackDefaultFunctor, Resource>::
check_transactions() noexcept
{
	int i = 0;
	transaction_t* trans;
	CoAP::Error ec;
	while((trans = list_[i++]))
	{
		trans->check();
	}
}

template<typename Connection,
	csm_configure const& Config,
	typename TransactionList,
	typename CallbackDefaultFunctor,
	typename Resource>
bool
engine_client<Connection, Config, TransactionList, CallbackDefaultFunctor, Resource>::
operator()(CoAP::Error& ec) noexcept
{
	return run(ec);
}

}//Reliable
}//Transmission
}//CoAP

#endif /* COAP_TE_TRANSMISSION_RELIABLE_TRANSACTION_IMPL_HPP__ */

