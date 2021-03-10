#ifndef COAP_TE_TRANSMISSION_ENGINE_IMPL_HPP__
#define COAP_TE_TRANSMISSION_ENGINE_IMPL_HPP__

#include "../engine.hpp"

#include "log.hpp"

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
	typename Callback_Functor>
template<bool UseInternalBufferNon,
		std::size_t BufferSize,
		typename MessageID,
		bool SortOptions,
		bool CheckOpOrder,
		bool CheckOpRepeat>
std::size_t
engine<Connection, Config, MaxTransactionNumber, MaxPacketSize, Callback_Functor>::
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
	typename Callback_Functor>
template<bool UseInternalBufferNon,
		std::size_t BufferSize,
		typename MessageID,
		bool SortOptions,
		bool CheckOpOrder,
		bool CheckOpRepeat>
std::size_t
engine<Connection, Config, MaxTransactionNumber, MaxPacketSize, Callback_Functor>::
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
	typename Callback_Functor>
template<bool UseInternalBufferNon,
		bool SortOptions,
		bool CheckOpOrder,
		bool CheckOpRepeat>
std::size_t
engine<Connection, Config, MaxTransactionNumber, MaxPacketSize, Callback_Functor>::
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
	typename Callback_Functor>
void
engine<Connection, Config, MaxTransactionNumber, MaxPacketSize, Callback_Functor>::
process(std::uint8_t const* buffer, std::size_t buffer_len, CoAP::Error& ec) noexcept
{
	Response response;
	parse(response, buffer, buffer_len, ec);
	if(ec)
	{
		error(engine_mod, ec, "parsing response");
		return;
	}
	debug(engine_mod, "Packet received");

	transaction_t* trans = list_.check_all_response(response);
	if(!trans)
	{
		warning(engine_mod, "Transaction %04X not found", response.mid);
		return;
	}
	debug(engine_mod, "Transaction found [%04X]", response.mid);
}

template<typename Connection,
	configure const& Config,
	unsigned MaxTransactionNumber,
	unsigned MaxPacketSize,
	typename Callback_Functor>
void
engine<Connection, Config, MaxTransactionNumber, MaxPacketSize, Callback_Functor>::
process(endpoint const& ep, std::uint8_t const* buffer, std::size_t buffer_len, CoAP::Error& ec) noexcept
{
	Response response;
	parse(response, buffer, buffer_len, ec);
	if(ec)
	{
		error(engine_mod, ec, "parsing response");
		return;
	}
	debug(engine_mod, "Packet received");

	transaction_t* trans = list_.check_all_response(ep, response);
	if(!trans)
	{
		warning(engine_mod, "Transaction %04X not found", response.mid);
		return;
	}
	debug(engine_mod, "Transaction found [%04X]", response.mid);
}

template<typename Connection,
	configure const& Config,
	unsigned MaxTransactionNumber,
	unsigned MaxPacketSize,
	typename Callback_Functor>
void
engine<Connection, Config, MaxTransactionNumber, MaxPacketSize, Callback_Functor>::
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
	typename Callback_Functor>
bool
engine<Connection, Config, MaxTransactionNumber, MaxPacketSize, Callback_Functor>::
run(CoAP::Error& ec)
{
	endpoint ep;
	std::size_t size = conn_.receive(buffer_, MaxPacketSize, ep, ec);
	if(ec) return false;
	if(size)
	{
		CoAP::Log::debug(engine_mod, "Received %d bytes", size);
		process(buffer_, size, ec);
	}

	check_transactions();

	return true;
}

template<typename Connection,
	configure const& Config,
	unsigned MaxTransactionNumber,
	unsigned MaxPacketSize,
	typename Callback_Functor>
bool
engine<Connection, Config, MaxTransactionNumber, MaxPacketSize, Callback_Functor>::
operator()(CoAP::Error& ec) noexcept
{
	return run(ec);
}

//template<typename Connection,
//	configure const& Config,
//	typename MessageID,
//	unsigned MaxTransactionNumber,
//	unsigned MaxPacketSize,
//	typename Callback_Functor>
//template<bool UseInternalBufferNon,
//		bool SortOptions,
//		bool CheckOpOrder,
//		bool CheckOpRepeat>
//std::size_t
//engine<Connection, Config, MessageID, MaxTransactionNumber, MaxPacketSize, Callback_Functor>::
//send(Request<Callback_Functor> const& req, CoAP::Error& ec) noexcept
//{
//	std::size_t size = 0;
//	if constexpr(UseInternalBufferNon)
//	{
//		if(req.type() == CoAP::Message::type::nonconfirmable)
//		{
//			size = req.serialize<SortOptions,
//					CheckOpOrder,
//					CheckOpRepeat>(buffer_, MaxPacketSize, mid_(), ec);
//			if(ec) return 0;
//			if(size)
//			{
//				conn_.send(buffer_, size, req.endpoint(), ec);
//				if(ec)
//					CoAP::Log::error(engine_mod, ec, "send");
//
//				return size;
//			}
//		}
//	}
//	else
//	{
//		transaction_t* ts = list_.check_free_slot();
//		if(!ts)
//		{
//			ec = CoAP::errc::no_free_slots;
//			return 0;
//		}
//		size = ts->template request<SortOptions, CheckOpOrder, CheckOpRepeat>(req, 10, ec);
//		conn_.send(ts->buffer(), size, ts->endpoint(), ec);
//	}
//	return size;
//}

}//Transmission
}//CoAP

#endif /* COAP_TE_TRANSMISSION_ENGINE_IMPL_HPP__ */
