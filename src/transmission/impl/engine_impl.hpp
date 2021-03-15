#ifndef COAP_TE_TRANSMISSION_ENGINE_IMPL_HPP__
#define COAP_TE_TRANSMISSION_ENGINE_IMPL_HPP__

#include "../engine.hpp"
#include "../functions.hpp"

#include "log.hpp"

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
	typename Callback_Resource_Functor>
typename engine<Profile, Connection, MessageID, TransactionList, Callback_Resource_Functor>::resource&
engine<Profile, Connection, MessageID, TransactionList, Callback_Resource_Functor>::
root() noexcept
{
	static_assert(Profile == profile::server, "Resource just available at 'server' profile");
	return resource_root_.root();
}

template<profile Profile,
	typename Connection,
	typename MessageID,
	typename TransactionList,
	typename Callback_Resource_Functor>
typename engine<Profile, Connection, MessageID, TransactionList, Callback_Resource_Functor>::resource_node&
engine<Profile, Connection, MessageID, TransactionList, Callback_Resource_Functor>::
root_node() noexcept
{
	static_assert(Profile == profile::server, "Resource just available at 'server' profile");
	return resource_root_.node();
}

template<profile Profile,
	typename Connection,
	typename MessageID,
	typename TransactionList,
	typename Callback_Resource_Functor>
template<bool UseEndpointTransMatch>
void
engine<Profile, Connection, MessageID, TransactionList, Callback_Resource_Functor>::
process(endpoint& ep, std::uint8_t const* buffer, std::size_t buffer_len, CoAP::Error& ec) noexcept
{
	CoAP::Message::message msg;
	parse(msg, buffer, buffer_len, ec);
	if(ec)
	{
		error(engine_mod, ec, "parsing response");
		return;
	}
	debug(engine_mod, "Packet received");

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
			error(engine_mod, "Profile 'client' doesn't support requests");
	}
}

template<profile Profile,
	typename Connection,
	typename MessageID,
	typename TransactionList,
	typename Callback_Resource_Functor>
std::uint16_t
engine<Profile, Connection, MessageID, TransactionList, Callback_Resource_Functor>::
mid() noexcept
{
	return mid_();
}

template<profile Profile,
	typename Connection,
	typename MessageID,
	typename TransactionList,
	typename Callback_Resource_Functor>
template<bool CheckEndpoint, bool CheckToken>
void
engine<Profile, Connection, MessageID, TransactionList, Callback_Resource_Functor>::
process_response(endpoint& ep [[maybe_unused]], CoAP::Message::message& msg) noexcept
{
	transaction_t* trans;
	trans = list_.template check_all_response<CheckEndpoint, CheckToken>(ep, msg);

	if(!trans)
	{
		warning(engine_mod, "Transaction NOT found [%04X]", msg.mid);
		return;
	}
	debug(engine_mod, "Transaction found [%04X]", msg.mid);
}

template<profile Profile,
	typename Connection,
	typename MessageID,
	typename TransactionList,
	typename Callback_Resource_Functor>
void
engine<Profile, Connection, MessageID, TransactionList, Callback_Resource_Functor>::
process_request(endpoint& ep,
		CoAP::Message::message& request,
		std::uint8_t const* buffer,
		CoAP::Error& ec) noexcept
{
	resource const* res = resource_root_.search(request);
	if(!res)
	{
		error(engine_mod, "Resource not found!");
		std::size_t bu = make_response_code_error(request, buffer_, packet_size, CoAP::Message::code::not_found);
		conn_.send(buffer_, bu, ep, ec);
	}
	else
	{
		debug(engine_mod, "Found resource %s", res->path());
		Response response(ep,
				request.mtype,
				request.mtype == CoAP::Message::type::confirmable ?
						request.mid : mid_(),
				request.token, request.token_len,
				buffer_, packet_size);
		if(res->call(request.mcode, request, response))
		{
			status(engine_mod, "Method found");
			if(!response.error() && response.buffer_used() > 0)
			{
				char buf_print[20];
				debug(engine_mod, "Sending response [%lu]: %s:%u", response.buffer_used(), ep.address(buf_print), ep.port());

				conn_.send(response.buffer(), response.buffer_used(), ep, ec);
				if(ec)
					error(engine_mod, ec, "send");
			}
		}
		else
		{
			status(engine_mod, "Method not allowed");
			std::size_t bu = make_response_code_error(request, buffer_, packet_size, CoAP::Message::code::method_not_allowed);
			conn_.send(buffer_, bu, ep, ec);
		}
	}
}

template<profile Profile,
	typename Connection,
	typename MessageID,
	typename TransactionList,
	typename Callback_Resource_Functor>
void
engine<Profile, Connection, MessageID, TransactionList, Callback_Resource_Functor>::
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
	typename Callback_Resource_Functor>
bool
engine<Profile, Connection, MessageID, TransactionList, Callback_Resource_Functor>::
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
	typename Callback_Resource_Functor>
bool
engine<Profile, Connection, MessageID, TransactionList, Callback_Resource_Functor>::
operator()(CoAP::Error& ec) noexcept
{
	return run(ec);
}

}//Transmission
}//CoAP

#endif /* COAP_TE_TRANSMISSION_ENGINE_IMPL_HPP__ */
