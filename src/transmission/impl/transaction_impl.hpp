#ifndef COAP_TE_TRANSMISSION_TRANSACTION_IMPL_HPP__
#define COAP_TE_TRANSMISSION_TRANSACTION_IMPL_HPP__

#include "../transaction.hpp"

namespace CoAP{
namespace Transmission{

template<unsigned MaxPacketSize,
		typename Callback_Functor,
		typename Endpoint>
void
transaction<MaxPacketSize, Callback_Functor, Endpoint>::
clear() noexcept
{
	cb_ = nullptr;
	data_ = nullptr;
	transmission_init_ = 0;
	expiration_time_ = 0;
	retransmission_count_ = 0;
	buffer_used_ = 0;
	status_ = status_t::none;

	if constexpr(is_external_storage)
		buffer_ = nullptr;
}

template<unsigned MaxPacketSize,
		typename Callback_Functor,
		typename Endpoint>
bool
transaction<MaxPacketSize, Callback_Functor, Endpoint>::
check(configure const& config) noexcept
{
	if(status_ != status_t::sending)
	{
		CoAP::Log::warning(transaction_mod, "Transaction not waiting response...");
		return false;
	}

	if(CoAP::time() > expiration_time_)
	{
		CoAP::Log::debug(transaction_mod, "[%u] Transaciton expired", request_.mid);
		if(retransmission_count_ >= config.max_restransmission)
		{
			CoAP::Log::status(transaction_mod, "[%u] Max retransmission reached [%u]",
					request_.mid, config.max_restransmission);
			status_ = status_t::timeout;
			call_cb(nullptr);
			return false;
		}
		else
		{
			retransmission_count_++;
			CoAP::Log::status(transaction_mod, "[%u] Retransmit transaction = %u",
								request_.mid, retransmission_count_);
			expiration_time_ = CoAP::time() + 10;
			CoAP::Log::debug(transaction_mod, "[%u] New expiration time = %u",
													request_.mid, expiration_time_);
			return true;
		}
	}
	return false;
}

template<unsigned MaxPacketSize,
		typename Callback_Functor,
		typename Endpoint>
bool
transaction<MaxPacketSize, Callback_Functor, Endpoint>::
check(CoAP::Message::message const& response)
{
	if(request_.mid != response.mid) return false;
	status_ = status_t::success;

	CoAP::Log::debug(transaction_mod, "[%d] Response arrived");
	call_cb(&response);

	return true;
}

template<unsigned MaxPacketSize,
		typename Callback_Functor,
		typename Endpoint>
bool
transaction<MaxPacketSize, Callback_Functor, Endpoint>::
check(Endpoint const& ep,
		CoAP::Message::message const& response)
{
	if(ep != ep_) return false;
	check(response);

	return true;
}

template<unsigned MaxPacketSize,
		typename Callback_Functor,
		typename Endpoint>
std::uint16_t
transaction<MaxPacketSize, Callback_Functor, Endpoint>::
mid() const noexcept
{
	return request_.mid;
}

template<unsigned MaxPacketSize,
		typename Callback_Functor,
		typename Endpoint>
std::uint8_t*
transaction<MaxPacketSize, Callback_Functor, Endpoint>::
buffer() noexcept
{
	return buffer_;
}

template<unsigned MaxPacketSize,
		typename Callback_Functor,
		typename Endpoint>
Endpoint const&
transaction<MaxPacketSize, Callback_Functor, Endpoint>::
endpoint() const noexcept
{
	return ep_;
}

template<unsigned MaxPacketSize,
		typename Callback_Functor,
		typename Endpoint>
bool
transaction<MaxPacketSize, Callback_Functor, Endpoint>::
init(Endpoint const& ep, std::uint8_t const* buffer, std::size_t size,
				Callback_Functor func, void* data, CoAP::Error& ec)
{
	static_assert(is_external_storage, "***");

	if(status_ != status_t::none)
	{
		CoAP::Log::error(transaction_mod, "Transaction been used");
		ec = CoAP::errc::no_free_slots;
		return false;
	}

	CoAP::Message::parse(request_, buffer, size, ec);
	if(ec) return false;
	if(request_.mtype != CoAP::Message::type::confirmable)
	{
		CoAP::Log::warning(transaction_mod, "No need transaction to NOT confirmable message");
		return true;
	}

	status_ = status_t::sending;

	transmission_init_ = CoAP::time();
	retransmission_count_ = 0;
	expiration_time_ = transmission_init_ + 10;

	ep_ = ep;
	cb_ = func;
	data_ = data;
	buffer_ = buffer;
	buffer_used_ = size;

	return true;
}

/**
 *
 */
template<unsigned MaxPacketSize,
		typename Callback_Functor,
		typename Endpoint>
void
transaction<MaxPacketSize, Callback_Functor, Endpoint>::
call_cb(CoAP::Message::message const* response) noexcept
{
	if(cb_) cb_(this, response, data_);
	clear();
}

}//Transmission
}//CoAP

#endif /* COAP_TE_TRANSMISSION_TRANSACTION_IMPL_HPP__ */
