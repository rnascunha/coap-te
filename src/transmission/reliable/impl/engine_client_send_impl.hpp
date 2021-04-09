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
		expiration_time_type time_ex [[maybe_unused]],
		transaction_cb func [[maybe_unused]], void* data [[maybe_unused]],
		CoAP::Error& ec) noexcept
{
	if constexpr(has_transaction_list)
	{
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

		/**
		 * As signals are processed, they would be never associated with a transaction
		 * (or for some does't even expect a response)
		 */
		if(CoAP::Message::is_signaling(fac.code())) time_ex = no_transaction;
		trans->init(conn_.native(), func, data, time_ex, ec);

		return size;
	}
	return send<false, SortOptions, CheckOpOrder, CheckOpRepeat>(fac, ec);
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
	if constexpr(UseTransaction && has_transaction_list)
	{
		return send<SortOptions, CheckOpOrder, CheckOpRepeat>(
				req.factory(), default_expiration, req.callback(), req.data(), ec);
	}

	return send<false, SortOptions, CheckOpOrder, CheckOpRepeat>(
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
	if constexpr(UseTransaction && has_transaction_list)
	{
		return send<SortOptions, CheckOpOrder, CheckOpRepeat>(fac, no_transaction, nullptr, nullptr, ec);
	}

	std::size_t size =  fac.template serialize<set_length, SortOptions, CheckOpOrder, CheckOpRepeat>(
			buffer_, packet_size, ec);
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

template<typename Connection,
	csm_configure const& Config,
	typename TransactionList,
	typename CallbackDefaultFunctor,
	typename Resource>
std::size_t
engine_client<Connection, Config, TransactionList, CallbackDefaultFunctor, Resource>::
send_abort(const char* payload, CoAP::Error& ec) noexcept
{
	CoAP::Message::Option::option_abort op;
	return send_abort(op, payload, ec);
}


template<typename Connection,
	csm_configure const& Config,
	typename TransactionList,
	typename CallbackDefaultFunctor,
	typename Resource>
std::size_t
engine_client<Connection, Config, TransactionList, CallbackDefaultFunctor, Resource>::
send_abort(CoAP::Message::Option::option_abort& bad_csm_option, const char* payload, CoAP::Error& ec) noexcept
{
	std::size_t size = make_abort_message<set_length>(bad_csm_option, payload,
			buffer_, Config.max_message_size, ec);
	if(ec) return size;

	return send(buffer_, size, ec);
}

}//Reliable
}//Transmission
}//CoAP

#endif /* COAP_TE_TRANSMISSION_RELIABLE_TRANSACTION_IMPL_HPP__ */
