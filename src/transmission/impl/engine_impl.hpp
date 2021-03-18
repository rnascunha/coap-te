#ifndef COAP_TE_TRANSMISSION_ENGINE_IMPL_HPP__
#define COAP_TE_TRANSMISSION_ENGINE_IMPL_HPP__

#include "../engine.hpp"
#include "../functions.hpp"

#include "log.hpp"
//#include "debug/print_message.hpp"

namespace CoAP{
namespace Transmission{

static constexpr CoAP::Log::module engine_mod = {
		.name = "ENGINE",
		.max_level = CoAP::Log::type::debug,
		.enable = true
};

template<profile Profile,
	typename Connection,
	typename MessageID,
	typename TransactionList,
	typename Callback_Default_Functor,
	typename Callback_Resource_Functor>
engine<Profile, Connection, MessageID, TransactionList, Callback_Default_Functor, Callback_Resource_Functor>::
engine(Connection&& conn, MessageID&& message_id)
: conn_(std::move(conn)), mid_(std::move(message_id))
{
	if constexpr(has_default_callback)
		default_cb_ = nullptr;
}

template<profile Profile,
	typename Connection,
	typename MessageID,
	typename TransactionList,
	typename Callback_Default_Functor,
	typename Callback_Resource_Functor>
engine<Profile, Connection, MessageID, TransactionList, Callback_Default_Functor, Callback_Resource_Functor>::
engine(Connection&& conn, MessageID&& message_id, configure& config)
	: conn_(std::move(conn)), mid_(std::move(message_id)), config_(config)
{
	if constexpr(has_default_callback)
		default_cb_ = nullptr;
}

template<profile Profile,
	typename Connection,
	typename MessageID,
	typename TransactionList,
	typename Callback_Default_Functor,
	typename Callback_Resource_Functor>
void
engine<Profile, Connection, MessageID, TransactionList, Callback_Default_Functor, Callback_Resource_Functor>::
default_cb(default_response_cb cb) noexcept
{
	static_assert(has_default_callback, "Default callback NOT set");
	default_cb_ = cb;
}

template<profile Profile,
	typename Connection,
	typename MessageID,
	typename TransactionList,
	typename Callback_Default_Functor,
	typename Callback_Resource_Functor>
typename engine<Profile, Connection, MessageID, TransactionList, Callback_Default_Functor, Callback_Resource_Functor>::resource&
engine<Profile, Connection, MessageID, TransactionList, Callback_Default_Functor, Callback_Resource_Functor>::
root() noexcept
{
	static_assert(Profile == profile::server, "Resource just available at 'server' profile");
	return resource_root_.root();
}

template<profile Profile,
	typename Connection,
	typename MessageID,
	typename TransactionList,
	typename Callback_Default_Functor,
	typename Callback_Resource_Functor>
typename engine<Profile, Connection, MessageID, TransactionList, Callback_Default_Functor, Callback_Resource_Functor>::resource_root&
engine<Profile, Connection, MessageID, TransactionList, Callback_Default_Functor, Callback_Resource_Functor>::
root_node() noexcept
{
	static_assert(Profile == profile::server, "Resource just available at 'server' profile");
	return resource_root_;
}

template<profile Profile,
	typename Connection,
	typename MessageID,
	typename TransactionList,
	typename Callback_Default_Functor,
	typename Callback_Resource_Functor>
std::uint16_t
engine<Profile, Connection, MessageID, TransactionList, Callback_Default_Functor, Callback_Resource_Functor>::
mid() noexcept
{
	return mid_();
}

template<profile Profile,
	typename Connection,
	typename MessageID,
	typename TransactionList,
	typename Callback_Default_Functor,
	typename Callback_Resource_Functor>
template<bool UseEndpointTransMatch>
void
engine<Profile, Connection, MessageID, TransactionList, Callback_Default_Functor, Callback_Resource_Functor>::
process(endpoint& ep, std::uint8_t const* buffer, std::size_t buffer_len, CoAP::Error& ec) noexcept
{
	CoAP::Message::message msg;
	parse(msg, buffer, buffer_len, ec);

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
		process_response(ep, msg);
	}
	else //is_request;
	{
		if constexpr(Profile == profile::server)
			process_request(ep, msg, buffer, ec);
		else
			ec = CoAP::errc::request_not_supported;
	}
}

template<profile Profile,
	typename Connection,
	typename MessageID,
	typename TransactionList,
	typename Callback_Default_Functor,
	typename Callback_Resource_Functor>
template<bool CheckEndpoint, bool CheckToken>
void
engine<Profile, Connection, MessageID, TransactionList, Callback_Default_Functor, Callback_Resource_Functor>::
process_response(endpoint& ep, CoAP::Message::message const& msg) noexcept
{
	if(list_.template check_all_response<CheckEndpoint, CheckToken>(ep, msg)) return;
	if constexpr(has_default_callback)
		if(default_cb_) default_cb_(ep, msg, this);
}

template<profile Profile,
	typename Connection,
	typename MessageID,
	typename TransactionList,
	typename Callback_Default_Functor,
	typename Callback_Resource_Functor>
void
engine<Profile, Connection, MessageID, TransactionList, Callback_Default_Functor, Callback_Resource_Functor>::
process_request(endpoint& ep,
		CoAP::Message::message const& request,
		std::uint8_t const* buffer,
		CoAP::Error& ec) noexcept
{
	resource const* res = resource_root_.search(request);
	if(!res)
	{
		std::size_t bu = make_response_code_error(request, buffer_, packet_size, CoAP::Message::code::not_found);
		conn_.send(buffer_, bu, ep, ec);
	}
	else
	{
		debug(engine_mod, "Found resource %s", res->path());
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

template<profile Profile,
	typename Connection,
	typename MessageID,
	typename TransactionList,
	typename Callback_Default_Functor,
	typename Callback_Resource_Functor>
void
engine<Profile, Connection, MessageID, TransactionList, Callback_Default_Functor, Callback_Resource_Functor>::
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
				error(engine_mod, "[%04X]Error sending...", trans->mid());
			trans->retransmit();
		}
	}
}

template<profile Profile,
	typename Connection,
	typename MessageID,
	typename TransactionList,
	typename Callback_Default_Functor,
	typename Callback_Resource_Functor>
bool
engine<Profile, Connection, MessageID, TransactionList, Callback_Default_Functor, Callback_Resource_Functor>::
run(CoAP::Error& ec)
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

template<profile Profile,
	typename Connection,
	typename MessageID,
	typename TransactionList,
	typename Callback_Default_Functor,
	typename Callback_Resource_Functor>
bool
engine<Profile, Connection, MessageID, TransactionList, Callback_Default_Functor, Callback_Resource_Functor>::
operator()(CoAP::Error& ec) noexcept
{
	return run(ec);
}

}//Transmission
}//CoAP

#endif /* COAP_TE_TRANSMISSION_ENGINE_IMPL_HPP__ */
