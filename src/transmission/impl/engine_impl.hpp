#ifndef COAP_TE_TRANSMISSION_ENGINE_IMPL_HPP__
#define COAP_TE_TRANSMISSION_ENGINE_IMPL_HPP__

#include "../engine.hpp"

#include "log.hpp"
#include "debug/print_message.hpp"

namespace CoAP{
namespace Transmission{

static constexpr CoAP::Log::module engine_mod = {
		.name = "ENGINE",
		.max_level = CoAP::Log::type::debug,
		.enable = true
};

template<typename Connection,
	configure const& Config,
	unsigned MaxTransactionNumber,
	unsigned MaxPacketSize,
	typename Callback_Functor,
	typename Callback_Resource_Functor>
template<bool UseInternalBufferNon,
		std::size_t BufferSize,
		typename MessageID,
		bool SortOptions,
		bool CheckOpOrder,
		bool CheckOpRepeat>
std::size_t
engine<Connection, Config, MaxTransactionNumber, MaxPacketSize, Callback_Functor, Callback_Resource_Functor>::
send(endpoint& ep,
		CoAP::Message::Factory<BufferSize, MessageID>& fac,
		Callback_Functor func, void* data,
		CoAP::Error& ec) noexcept
{
	std::size_t size = 0;
	if constexpr(UseInternalBufferNon)
	{
		if(fac.type() == CoAP::Message::type::nonconfirmable)
		{
			size = fac.template serialize<SortOptions, CheckOpOrder, CheckOpRepeat>(buffer_, MaxPacketSize, ec);
			if(ec) return size;
			conn_.send(buffer_, size, ep, ec);
			return size;
		}
	}

	transaction_t* ts = list_.find_free_slot();
	if(!ts)
	{
		ec = CoAP::errc::no_free_slots;
		return size;
	}

	size = ts->template serialize<BufferSize, MessageID, SortOptions, CheckOpOrder, CheckOpRepeat>(fac, ec);
	if(ec) return size;

	conn_.send(ts->buffer(), ts->buffer_used(), ep, ec);
	if(ec) return size;
	ts->init(Config, ep, func, data, ec);

	return size;
}

template<typename Connection,
	configure const& Config,
	unsigned MaxTransactionNumber,
	unsigned MaxPacketSize,
	typename Callback_Functor,
	typename Callback_Resource_Functor>
template<bool UseInternalBufferNon,
		std::size_t BufferSize,
		typename MessageID,
		bool SortOptions,
		bool CheckOpOrder,
		bool CheckOpRepeat>
std::size_t
engine<Connection, Config, MaxTransactionNumber, MaxPacketSize, Callback_Functor, Callback_Resource_Functor>::
send(endpoint& ep,
		CoAP::Message::Factory<BufferSize, MessageID> const& fac,
		std::uint16_t mid,
		Callback_Functor func, void* data,
		CoAP::Error& ec) noexcept
{
	std::size_t size = 0;
	if constexpr(UseInternalBufferNon)
	{
		if(fac.type() == CoAP::Message::type::nonconfirmable)
		{
			size = fac.template serialize<SortOptions, CheckOpOrder, CheckOpRepeat>(buffer_, MaxPacketSize, mid, ec);
			if(ec) return size;
			conn_.send(buffer_, size, ep, ec);
			return size;
		}
	}

	transaction_t* ts = list_.find_free_slot();
	if(!ts)
	{
		ec = CoAP::errc::no_free_slots;
		return size;
	}

	size = ts->template serialize<BufferSize, MessageID, SortOptions, CheckOpOrder, CheckOpRepeat>(fac, mid, ec);
	if(ec) return size;

	conn_.send(ts->buffer(), ts->buffer_used(), ep, ec);
	if(ec) return size;
	ts->init(Config, ep, func, data, ec);

	return size;
}

template<typename Connection,
	configure const& Config,
	unsigned MaxTransactionNumber,
	unsigned MaxPacketSize,
	typename Callback_Functor,
	typename Callback_Resource_Functor>
template<bool UseInternalBufferNon,
		bool SortOptions,
		bool CheckOpOrder,
		bool CheckOpRepeat>
std::size_t
engine<Connection, Config, MaxTransactionNumber, MaxPacketSize, Callback_Functor, Callback_Resource_Functor>::
send(Request<Callback_Functor>& req,
	std::uint16_t mid,
	CoAP::Error& ec) noexcept
{
	return send<UseInternalBufferNon, 0, void*, SortOptions, CheckOpOrder, CheckOpRepeat>
						(req.endpoint(), req.factory(), mid, req.callback(), req.data(), ec);
}

template<typename Connection,
	configure const& Config,
	unsigned MaxTransactionNumber,
	unsigned MaxPacketSize,
	typename Callback_Functor,
	typename Callback_Resource_Functor>
template<bool UseEndpointTransMatch>
void
engine<Connection, Config, MaxTransactionNumber, MaxPacketSize, Callback_Functor, Callback_Resource_Functor>::
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
	CoAP::Debug::print_message(msg);

	if(CoAP::Message::is_response(msg.mcode))
	{
		transaction_t* trans;
		if constexpr(UseEndpointTransMatch)
		{
			 trans = list_.check_all_response(ep, msg);
		}
		else
		{
			 trans = list_.check_all_response(ep, msg);
		}

		if(!trans)
		{
			warning(engine_mod, "Transaction NOT found [%04X]", msg.mid);
			return;
		}
		debug(engine_mod, "Transaction found [%04X]", msg.mid);
	}
	else //is_request;
	{
		resource const* res = resource_root_.search(msg);
		if(!res)
		{
			error(engine_mod, "Resource not found!");
		}
		else
		{
			debug(engine_mod, "Found resource %s", res->path());
			Response response(ep, msg, buffer_, MaxPacketSize);
			if(res->call(msg.mcode, msg, response))
			{
				status(engine_mod, "Method found");
				if(!response.error_code() && response.buffer_used() > 0)
				{
					char buf_print[20];
					debug(engine_mod, "Sending response [%lu]: %s:%u", response.buffer_used(), ep.address(buf_print), ep.port());
					conn_.send(buffer_, response.buffer_used(), ep, ec);
					if(ec)
					{
						error(engine_mod, ec, "send");
					}
				}
			}
			else
			{
				status(engine_mod, "Method NOT found");
			}
		}
	}
}

template<typename Connection,
	configure const& Config,
	unsigned MaxTransactionNumber,
	unsigned MaxPacketSize,
	typename Callback_Functor,
	typename Callback_Resource_Functor>
void
engine<Connection, Config, MaxTransactionNumber, MaxPacketSize, Callback_Functor, Callback_Resource_Functor>::
check_transactions() noexcept
{
	int i = 0;
	transaction_t* trans;
	CoAP::Error ec;
	while((trans = list_[i++]))
	{
		if(trans->check(Config))
		{
			//Must retransmit
			debug(engine_mod, "[%04X] Retransmiting...", trans->mid());
			conn_.send(trans->buffer(), trans->buffer_used(), trans->endpoint(), ec);
			if(ec)
				error(engine_mod, "[%04X]Error sending...", trans->mid());
			trans->retransmit();
		}
	}
}

template<typename Connection,
	configure const& Config,
	unsigned MaxTransactionNumber,
	unsigned MaxPacketSize,
	typename Callback_Functor,
	typename Callback_Resource_Functor>
bool
engine<Connection, Config, MaxTransactionNumber, MaxPacketSize, Callback_Functor, Callback_Resource_Functor>::
run(CoAP::Error& ec)
{
	endpoint ep;
	std::size_t size = conn_.receive(buffer_, MaxPacketSize, ep, ec);
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
	configure const& Config,
	unsigned MaxTransactionNumber,
	unsigned MaxPacketSize,
	typename Callback_Functor,
	typename Callback_Resource_Functor>
bool
engine<Connection, Config, MaxTransactionNumber, MaxPacketSize, Callback_Functor, Callback_Resource_Functor>::
operator()(CoAP::Error& ec) noexcept
{
	return run(ec);
}

}//Transmission
}//CoAP

#endif /* COAP_TE_TRANSMISSION_ENGINE_IMPL_HPP__ */
