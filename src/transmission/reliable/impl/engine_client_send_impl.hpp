#ifndef COAP_TE_TRANSMISSION_RELIABLE_ENGINE_CLIENT_SEND_IMPL_HPP__
#define COAP_TE_TRANSMISSION_RELIABLE_ENGINE_CLIENT_SEND_IMPL_HPP__

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

template<bool SortOptions /* = true */,
		bool CheckOpOrder /* = !SortOptions */,
		bool CheckOpRepeat /* = true */,
		std::size_t BufferSize,
		CoAP::Message::code Code>
std::size_t send(CoAP::Message::Reliable::Factory<BufferSize, Code> const& fac,
		transaction_cb func, void* data,
		CoAP::Error& ec) noexcept
{
	return send<SortOptions, CheckOpOrder, CheckOpRepeat>(fac, default_expiration, func, data, ec);
}

template<typename Connection,
	csm_configure const& Config,
	typename TransactionList,
	typename CallbackDefaultFunctor,
	typename Resource>
template<bool SortOptions /* = true */,
		bool CheckOpOrder /* = !SortOptions */,
		bool CheckOpRepeat /* = true */,
		std::size_t BufferSize,
		CoAP::Message::code Code>
std::size_t
engine_client<Connection, Config, TransactionList, CallbackDefaultFunctor, Resource>::
send(CoAP::Message::Reliable::Factory<BufferSize, Code> const& fac,
		expiration_time_type time_ex,
		transaction_cb func, void* data,
		CoAP::Error& ec) noexcept
{
	static_assert(has_transaction_list, "Transaction list must be available");

	transaction_t* trans = list_.find_free_slot();
	if(!trans)
	{
		ec = CoAP::errc::transaction_ocupied;
		return 0;
	}
	std::size_t size = trans->template serialize<set_length, SortOptions, CheckOpOrder, CheckOpRepeat>(
			fac, ec);
	if(ec) return size;

	send(trans->buffer(), trans->buffer_used(), ec);
	if(ec) return size;

	trans->init(conn_.native(), func, data, time_ex, ec);

	return size;
}

template<typename Connection,
	csm_configure const& Config,
	typename TransactionList,
	typename CallbackDefaultFunctor,
	typename Resource>
template<bool UseTransaction /* = true */,
		bool SortOptions /* = true */,
		bool CheckOpOrder /* = !SortOptions */,
		bool CheckOpRepeat /* = true */,
		CoAP::Message::code Code>
std::size_t
engine_client<Connection, Config, TransactionList, CallbackDefaultFunctor, Resource>::
send(request<Code>& req,
		CoAP::Error& ec) noexcept
{
	static_assert(has_transaction_list, "Transaction list must be available");
	if constexpr(UseTransaction)
		return send<SortOptions, CheckOpOrder, CheckOpRepeat>(
				req.factory(), default_expiration, req.callback(), req.data(), ec);

	return send<SortOptions, CheckOpOrder, CheckOpRepeat>(
				req.factory(), ec);
}

template<typename Connection,
	csm_configure const& Config,
	typename TransactionList,
	typename CallbackDefaultFunctor,
	typename Resource>
template<bool SortOptions /* = true */,
		bool CheckOpOrder /* = !SortOptions */,
		bool CheckOpRepeat /* = true */,
		CoAP::Message::code Code>
std::size_t
engine_client<Connection, Config, TransactionList, CallbackDefaultFunctor, Resource>::
send(request<Code>& req,
	expiration_time_type time_ex,
	CoAP::Error& ec) noexcept
{
	static_assert(has_transaction_list, "Transaction list must be available");

	return send<SortOptions, CheckOpOrder, CheckOpRepeat>(
			req.factory(), time_ex, req.callback(), req.data(), ec);
}

template<typename Connection,
	csm_configure const& Config,
	typename TransactionList,
	typename CallbackDefaultFunctor,
	typename Resource>
template<bool UseTransaction,
		bool SortOptions /* = true */,
		bool CheckOpOrder /* = !SortOptions */,
		bool CheckOpRepeat /* = true */,
		std::size_t BufferSize,
		CoAP::Message::code Code>
std::size_t
engine_client<Connection, Config, TransactionList, CallbackDefaultFunctor, Resource>::
send(CoAP::Message::Reliable::Factory<BufferSize, Code> const& fac, CoAP::Error& ec) noexcept
{
	if constexpr(UseTransaction)
	{
		return send<SortOptions, CheckOpOrder, CheckOpRepeat>(fac, no_transaction, nullptr, nullptr, ec);
	}

	std::size_t size =  fac.template serialize<set_length, SortOptions, CheckOpOrder, CheckOpRepeat>(
			buffer_, max_packet_size, ec);
	if(ec) return size;

	return send(buffer_, size, ec);
}

template<typename Connection,
	csm_configure const& Config,
	typename TransactionList,
	typename CallbackDefaultFunctor,
	typename Resource>
std::size_t
engine_client<Connection, Config, TransactionList, CallbackDefaultFunctor, Resource>::
send(const void* buffer, std::size_t buffer_len,
		CoAP::Error& ec) noexcept
{
	return conn_.send(buffer, buffer_len, ec);
}

}//Reliable
}//Transmission
}//CoAP

#endif /* COAP_TE_TRANSMISSION_RELIABLE_TRANSACTION_IMPL_HPP__ */
