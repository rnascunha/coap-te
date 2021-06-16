#ifndef COAP_TE_TRANSMISSION_ENGINE_IMPL_HPP__
#define COAP_TE_TRANSMISSION_ENGINE_IMPL_HPP__

#include "../engine.hpp"
#include "../functions.hpp"

#include "log.hpp"

namespace CoAP{
namespace Transmission{

static constexpr CoAP::Log::module engine_mod = {
		/*.name = */"ENGINE",
		/*.max_level = */CoAP::Log::type::debug,
		/*.enable = */true
};

template<typename Connection,
	typename MessageID,
	typename TransactionList,
	typename Callback_Default_Functor,
	typename Resource>
engine<Connection, MessageID, TransactionList, Callback_Default_Functor, Resource>::
engine(Connection&& conn, MessageID&& message_id)
: conn_(std::move(conn)), mid_(std::move(message_id))
{
	if constexpr(has_default_callback)
		default_cb_ = nullptr;
}

template<typename Connection,
	typename MessageID,
	typename TransactionList,
	typename Callback_Default_Functor,
	typename Resource>
engine<Connection, MessageID, TransactionList, Callback_Default_Functor, Resource>::
engine(Connection&& conn, MessageID&& message_id, configure const& tconfig)
	: conn_(std::move(conn)), mid_(std::move(message_id)), config_(tconfig)
{
	if constexpr(has_default_callback)
		default_cb_ = nullptr;
}

template<typename Connection,
	typename MessageID,
	typename TransactionList,
	typename Callback_Default_Functor,
	typename Resource>
void
engine<Connection, MessageID, TransactionList, Callback_Default_Functor, Resource>::
default_cb(default_response_cb cb) noexcept
{
	static_assert(has_default_callback, "Default callback NOT set");
	default_cb_ = cb;
}

template<typename Connection,
	typename MessageID,
	typename TransactionList,
	typename Callback_Default_Functor,
	typename Resource>
typename engine<Connection, MessageID, TransactionList, Callback_Default_Functor, Resource>::resource&
engine<Connection, MessageID, TransactionList, Callback_Default_Functor, Resource>::
root() noexcept
{
	static_assert(get_profile() == profile::server, "Resource just available at 'server' profile");
	return resource_root_.root();
}

template<typename Connection,
	typename MessageID,
	typename TransactionList,
	typename Callback_Default_Functor,
	typename Resource>
typename engine<Connection, MessageID, TransactionList, Callback_Default_Functor, Resource>::resource_root&
engine<Connection, MessageID, TransactionList, Callback_Default_Functor, Resource>::
root_node() noexcept
{
	static_assert(get_profile() == profile::server, "Resource just available at 'server' profile");
	return resource_root_;
}

template<typename Connection,
	typename MessageID,
	typename TransactionList,
	typename Callback_Default_Functor,
	typename Resource>
std::uint16_t
engine<Connection, MessageID, TransactionList, Callback_Default_Functor, Resource>::
mid() noexcept
{
	return mid_();
}

template<typename Connection,
	typename MessageID,
	typename TransactionList,
	typename Callback_Default_Functor,
	typename Resource>
template<bool UseEndpointTransMatch>
void
engine<Connection, MessageID, TransactionList, Callback_Default_Functor, Resource>::
process(endpoint& ep, std::uint8_t const* buffer, std::size_t buffer_len, CoAP::Error& ec) noexcept
{
	CoAP::Message::message msg;
	CoAP::Message::parse(msg, buffer, buffer_len, ec);

	if(ec)
	{
		if(ec == CoAP::errc::insufficient_buffer)
		{
			std::size_t bu = make_response_code_error(msg,
					buffer_,
					packet_size,
					CoAP::Message::code::request_entity_too_large);
			conn_.send(buffer_, bu, ep, ec);
		}
		return;
	}

	if(CoAP::Message::is_response(msg.mcode)
		|| msg.mcode == CoAP::Message::code::empty)
	{
		process_response(ep, msg, ec);
	}
	else //is_request;
	{
		if constexpr(get_profile() == profile::server)
			process_request(ep, msg, ec);
		else
			ec = CoAP::errc::request_not_supported;
	}
}

template<typename Connection,
	typename MessageID,
	typename TransactionList,
	typename Callback_Default_Functor,
	typename Resource>
template<bool CheckEndpoint, bool CheckToken>
void
engine<Connection, MessageID, TransactionList, Callback_Default_Functor, Resource>::
process_response(endpoint& ep, CoAP::Message::message const& msg, CoAP::Error& ec) noexcept
{
	if(list_.template check_all_response<CheckEndpoint, CheckToken>(ep, msg)) return;
	if constexpr(has_default_callback)
		if(default_cb_) default_cb_(ep, &msg, this);

	if(msg.mtype == CoAP::Message::type::confirmable)
	{
		request req{ep};
		req.header(
				CoAP::Message::type::acknowledgment,
				CoAP::Message::code::empty);

		send(req, msg.mid, ec);
	}
}

template<typename Connection,
	typename MessageID,
	typename TransactionList,
	typename Callback_Default_Functor,
	typename Resource>
void
engine<Connection, MessageID, TransactionList, Callback_Default_Functor, Resource>::
process_request(endpoint& ep,
		CoAP::Message::message const& request,
		CoAP::Error& ec) noexcept
{
	resource const* res = resource_root_.search(request);
	if(!res)
	{
		status(engine_mod, "Not resource");
		std::size_t bu = make_response_code_error(request, buffer_, packet_size, CoAP::Message::code::not_found);
		conn_.send(buffer_, bu, ep, ec);
	}
	else
	{
		debug(engine_mod, "Found resource %s", res->path() ? res->path() : "/");
		response response(ep,
				request.mtype,
				request.mtype == CoAP::Message::type::confirmable ?
						request.mid : mid_(),
				request.token, request.token_len,
				buffer_, packet_size);
		if(res->call(request.mcode, request, response, this))
		{
			debug(engine_mod, "Method found");
			if(!response.error() && response.buffer_used() > 0)
			{
				conn_.send(response.buffer(), response.buffer_used(), ep, ec);
			}
		}
		else
		{
			std::size_t bu = make_response_code_error(request, buffer_, packet_size, CoAP::Message::code::method_not_allowed);
			conn_.send(buffer_, bu, ep, ec);
		}
	}
}

template<typename Connection,
	typename MessageID,
	typename TransactionList,
	typename Callback_Default_Functor,
	typename Resource>
void
engine<Connection, MessageID, TransactionList, Callback_Default_Functor, Resource>::
check_transactions() noexcept
{
	int i = 0;
	transaction_t* trans;
	CoAP::Error ec;
	while((trans = list_[i++]))
	{
		if(trans->check())
		{
			//Must retransmit
			debug(engine_mod, "[%04X] Retransmitting...", trans->mid());
			conn_.send(trans->buffer(), trans->buffer_used(), trans->endpoint(), ec);
			if(ec)
			{
				error(engine_mod, ec, "Error sending...");// trans->mid());
				trans->cancel();
			}
			trans->retransmit();
		}
	}
}

template<typename Connection,
	typename MessageID,
	typename TransactionList,
	typename Callback_Default_Functor,
	typename Resource>
bool
engine<Connection, MessageID, TransactionList, Callback_Default_Functor, Resource>::
run(CoAP::Error& ec) noexcept
{
	endpoint ep;
	std::size_t size = conn_.receive(buffer_, packet_size, ep, ec);
	if(ec) return false;
	if(size)
	{
		char buf_print[20];
		debug(engine_mod, "From: %s:%u", ep.address(buf_print), ep.port());
		CoAP::Log::debug(engine_mod, "Received %d bytes", size);
		process(ep, buffer_, size, ec);
	}

	check_transactions();

	return true;
}

template<typename Connection,
	typename MessageID,
	typename TransactionList,
	typename Callback_Default_Functor,
	typename Resource>
template<int BlockTimeMs>
bool
engine<Connection, MessageID, TransactionList, Callback_Default_Functor, Resource>::
run(CoAP::Error& ec) noexcept
{
	endpoint ep;
	std::size_t size = conn_.template receive<BlockTimeMs>(buffer_, packet_size, ep, ec);
	if(ec) return false;
	if(size)
	{
		char buf_print[20];
		debug(engine_mod, "From: %s:%u", ep.address(buf_print), ep.port());
		CoAP::Log::debug(engine_mod, "Received %d bytes", size);
		process(ep, buffer_, size, ec);
	}

	check_transactions();

	return true;
}

template<typename Connection,
	typename MessageID,
	typename TransactionList,
	typename Callback_Default_Functor,
	typename Resource>
bool
engine<Connection, MessageID, TransactionList, Callback_Default_Functor, Resource>::
operator()(CoAP::Error& ec) noexcept
{
	return run(ec);
}

}//Transmission
}//CoAP

#endif /* COAP_TE_TRANSMISSION_ENGINE_IMPL_HPP__ */
