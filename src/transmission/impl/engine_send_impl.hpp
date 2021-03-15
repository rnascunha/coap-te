#ifndef COAP_TE_TRANSMISSION_ENGINE_SEND_IMPL_HPP__
#define COAP_TE_TRANSMISSION_ENGINE_SEND_IMPL_HPP__

#include "../engine.hpp"

namespace CoAP{
namespace Transmission{

template<profile Profile,
	typename Connection,
	typename MessageID,
	typename TransactionList,
	typename Callback_Resource_Functor>
template<bool UseInternalBufferNon,
	bool SortOptions,
	bool CheckOpOrder,
	bool CheckOpRepeat,
	std::size_t BufferSize,
	typename Message_ID>
std::size_t
engine<Profile, Connection, MessageID, TransactionList, Callback_Resource_Functor>::
send(endpoint& ep,
		configure const& config,
		CoAP::Message::Factory<BufferSize, Message_ID> const& fac,
		std::uint16_t mid,
		transaction_cb func, void* data,
		CoAP::Error& ec) noexcept
{
	std::size_t size = 0;
	if constexpr(UseInternalBufferNon)
	{
		if(fac.type() == CoAP::Message::type::nonconfirmable)
		{
			size = fac.template serialize<SortOptions, CheckOpOrder, CheckOpRepeat>(buffer_, packet_size, mid, ec);
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

	size = ts->template serialize<SortOptions, CheckOpOrder, CheckOpRepeat, BufferSize, Message_ID>(fac, mid, ec);
	if(ec) return size;

	conn_.send(ts->buffer(), ts->buffer_used(), ep, ec);
	if(ec) return size;
	ts->init(config, ep, func, data, ec);

	return size;
}

template<profile Profile,
	typename Connection,
	typename MessageID,
	typename TransactionList,
	typename Callback_Resource_Functor>
template<bool UseInternalBufferNon,
		bool SortOptions,
		bool CheckOpOrder,
		bool CheckOpRepeat,
		std::size_t BufferSize,
		typename Message_ID>
std::size_t
engine<Profile, Connection, MessageID, TransactionList, Callback_Resource_Functor>::
send(endpoint& ep,
		CoAP::Message::Factory<BufferSize, Message_ID> const& fac,
		transaction_cb func, void* data,
		CoAP::Error& ec) noexcept
{
	return send(ep, config_, fac, mid_(), data, func, ec);
}

template<profile Profile,
	typename Connection,
	typename MessageID,
	typename TransactionList,
	typename Callback_Resource_Functor>
template<bool UseInternalBufferNon,
		bool SortOptions,
		bool CheckOpOrder,
		bool CheckOpRepeat,
		std::size_t BufferSize,
		typename Message_ID>
std::size_t
engine<Profile, Connection, MessageID, TransactionList, Callback_Resource_Functor>::
send(endpoint& ep,
		CoAP::Message::Factory<BufferSize, Message_ID> const& fac,
		std::uint16_t mid,
		transaction_cb func, void* data,
		CoAP::Error& ec) noexcept
{
	return send(ep, config_, fac, mid, data, func, ec);
}

template<profile Profile,
	typename Connection,
	typename MessageID,
	typename TransactionList,
	typename Callback_Resource_Functor>
template<bool UseInternalBufferNon,
		bool SortOptions,
		bool CheckOpOrder,
		bool CheckOpRepeat>
std::size_t
engine<Profile, Connection, MessageID, TransactionList, Callback_Resource_Functor>::
send(request& req,
	std::uint16_t mid,
	CoAP::Error& ec) noexcept
{
	return send<UseInternalBufferNon, SortOptions, CheckOpOrder, CheckOpRepeat, 0, void*>
						(req.endpoint(), config_, req.factory(), mid, req.callback(), req.data(), ec);
}

template<profile Profile,
	typename Connection,
	typename MessageID,
	typename TransactionList,
	typename Callback_Resource_Functor>
template<bool UseInternalBufferNon,
		bool SortOptions,
		bool CheckOpOrder,
		bool CheckOpRepeat>
std::size_t
engine<Profile, Connection, MessageID, TransactionList, Callback_Resource_Functor>::
send(request& req,
	CoAP::Error& ec) noexcept
{
	return send<UseInternalBufferNon, SortOptions, CheckOpOrder, CheckOpRepeat, 0, void*>
						(req.endpoint(), config_, req.factory(), mid_(), req.callback(), req.data(), ec);
}

template<profile Profile,
	typename Connection,
	typename MessageID,
	typename TransactionList,
	typename Callback_Resource_Functor>
template<bool UseInternalBufferNon,
	bool SortOptions,
	bool CheckOpOrder,
	bool CheckOpRepeat,
	std::size_t BufferSize,
	typename Message_ID>
std::size_t
engine<Profile, Connection, MessageID, TransactionList, Callback_Resource_Functor>::
send(endpoint& ep,
		configure const& config,
		CoAP::Message::Factory<BufferSize, Message_ID> const& fac,
		transaction_cb func, void* data,
		CoAP::Error& ec) noexcept
{
	return send(ep, config, fac, mid_(), func, data, ec);
}

template<profile Profile,
	typename Connection,
	typename MessageID,
	typename TransactionList,
	typename Callback_Resource_Functor>
template<bool UseInternalBufferNon,
		bool SortOptions,
		bool CheckOpOrder,
		bool CheckOpRepeat>
std::size_t
engine<Profile, Connection, MessageID, TransactionList, Callback_Resource_Functor>::
send(request& req,
			configure const& config,
			std::uint16_t mid,
			CoAP::Error& ec) noexcept
{
	return send<UseInternalBufferNon, SortOptions, CheckOpOrder, CheckOpRepeat, 0, void*>
			(req.endpoint(), config, req.factory(), mid, req.callback(), req.data(), ec);
}

template<profile Profile,
	typename Connection,
	typename MessageID,
	typename TransactionList,
	typename Callback_Resource_Functor>
template<bool UseInternalBufferNon,
		bool SortOptions,
		bool CheckOpOrder,
		bool CheckOpRepeat>
std::size_t
engine<Profile, Connection, MessageID, TransactionList, Callback_Resource_Functor>::
send(request& req,
		configure const& config,
		CoAP::Error& ec) noexcept
{
	return send<UseInternalBufferNon, SortOptions, CheckOpOrder, CheckOpRepeat, 0, void*>
			(req.endpoint(), config, req.factory(), mid_(), req.callback(), req.data(), ec);
}

template<profile Profile,
	typename Connection,
	typename MessageID,
	typename TransactionList,
	typename Callback_Resource_Functor>
std::size_t
engine<Profile, Connection, MessageID, TransactionList, Callback_Resource_Functor>::
send(endpoint& ep, const void* buffer, std::size_t buffer_len, CoAP::Error& ec) noexcept
{
	return conn_.send(buffer, buffer_len, ep, ec);
}

}//Transmission
}//CoAP

#endif /* COAP_TE_TRANSMISSION_ENGINE_SEND_IMPL_HPP__ */
