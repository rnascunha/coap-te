#ifndef COAP_TE_TRANSMISSION_TRANSACTION_IMPL_HPP__
#define COAP_TE_TRANSMISSION_TRANSACTION_IMPL_HPP__

#include "log.hpp"
#include "../functions.hpp"
#include "port/port.hpp"
#include "message/parser.hpp"
#include "../transaction.hpp"

namespace CoAP{
namespace Transmission{

static constexpr CoAP::Log::module transaction_mod = {
		.name = "TRANS",
		.max_level = CoAP::Log::type::debug,
		.enable = true
};

template<unsigned MaxPacketSize,
		typename Callback_Functor,
		typename Endpoint>
void
transaction<MaxPacketSize, Callback_Functor, Endpoint>::
clear() noexcept
{
	cb_ = nullptr;
	data_ = nullptr;
	max_span_timeout_ = 0;
	expiration_time_factor_ = 0;
	next_expiration_time_ = 0;
	retransmission_remaining_ = 0;
	buffer_used_ = 0;
	status_ = status_t::none;

	if constexpr(is_external_storage)
		buffer_ = nullptr;
}

template<unsigned MaxPacketSize,
		typename Callback_Functor,
		typename Endpoint>
template<bool CheckMaxSpan>
bool
transaction<MaxPacketSize, Callback_Functor, Endpoint>::
check() noexcept
{
	if(status_ != status_t::sending)
		return false;

	double ftime = static_cast<double>(CoAP::time());
	if constexpr(CheckMaxSpan)
	{
		if(ftime > max_span_timeout_)
		{
			status_ = status_t::timeout;
			call_cb(nullptr);
			return false;
		}
	}

	if(ftime > next_expiration_time_)
	{
		CoAP::Log::debug(transaction_mod, "[%04X] Transaciton expired [%.02f]", request_.mid, next_expiration_time_);
		if(retransmission_remaining_ <= 0)
		{
			CoAP::Log::status(transaction_mod, "[%04X] Max retransmission reached [%u]",
					request_.mid, retransmission_remaining_);
			status_ = status_t::timeout;
			call_cb(nullptr);
			clear();
			return false;
		}
		else //message must be retransmitted
			return true;
	}
	return false;
}

template<unsigned MaxPacketSize,
		typename Callback_Functor,
		typename Endpoint>
void
transaction<MaxPacketSize, Callback_Functor, Endpoint>::
retransmit() noexcept
{
	retransmission_remaining_--;
	CoAP::Log::status(transaction_mod, "[%04X] Retransmit remaining = %u",
						request_.mid, retransmission_remaining_);
	expiration_time_factor_ *= 2;
	next_expiration_time_ = static_cast<double>(CoAP::time()) + expiration_time_factor_;
	CoAP::Log::debug(transaction_mod, "[%04X] New expiration time = %.2f (diff=%.2f)",
											request_.mid,
											next_expiration_time_,
											next_expiration_time_ - static_cast<double>(CoAP::time()));
}

template<unsigned MaxPacketSize,
		typename Callback_Functor,
		typename Endpoint>
template<bool CheckEndpoint, bool CheckToken>
bool
transaction<MaxPacketSize, Callback_Functor, Endpoint>::
check_response(endpoint_t const& ep [[maybe_unused]],
		CoAP::Message::message const& response) noexcept
{
	if(status_ != status_t::sending) return false;
	if(request_.mid != response.mid) return false;
	if constexpr(CheckEndpoint)
		if(ep != ep_) return false;
	if constexpr(CheckToken)
	{
		if(request_.token_len != response.token_len)
			return false;
		if(std::memcmp(request_.token, response.token, request_.token_len) != 0)
			return false;
	}

	status_ = response.mcode == CoAP::Message::code::empty ?
			status_t::empty : status_t::success;

	CoAP::Log::debug(transaction_mod, "[%04X] Response arrived", response.mid);
	call_cb(&response);

	return true;
}

template<unsigned MaxPacketSize,
		typename Callback_Functor,
		typename Endpoint>
CoAP::Message::message const&
transaction<MaxPacketSize, Callback_Functor, Endpoint>::
request() const noexcept
{
	return request_;
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
std::size_t
transaction<MaxPacketSize, Callback_Functor, Endpoint>::
buffer_used() const noexcept
{
	return buffer_used_;
}

template<unsigned MaxPacketSize,
		typename Callback_Functor,
		typename Endpoint>
Endpoint&
transaction<MaxPacketSize, Callback_Functor, Endpoint>::
endpoint() noexcept
{
	return ep_;
}

template<unsigned MaxPacketSize,
		typename Callback_Functor,
		typename Endpoint>
Endpoint
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
init(configure const& config,
	endpoint_t const& ep,
	std::uint8_t* buffer, std::size_t size,
	Callback_Functor func, void* data, CoAP::Error& ec) noexcept
{
	static_assert(is_external_storage, "Must use external storage");

	return init_impl(config, ep, buffer, size, func, data, ec);
}

template<unsigned MaxPacketSize,
		typename Callback_Functor,
		typename Endpoint>
bool
transaction<MaxPacketSize, Callback_Functor, Endpoint>::
init_impl(configure const& config,
		endpoint_t const& ep,
		std::uint8_t* buffer, std::size_t size,
		Callback_Functor func, void* data, CoAP::Error& ec) noexcept
{
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

	max_span_timeout_ = static_cast<double>(CoAP::time()) + max_transmit_span(config);
	retransmission_remaining_ = config.max_restransmission;
	expiration_time_factor_ = expiration_timeout(config);
	next_expiration_time_ = static_cast<double>(CoAP::time()) + expiration_time_factor_;
	CoAP::Log::debug(transaction_mod, "[%04X] Expiration time = %.2f (diff=%.2f/factor=%.2f)",
								request_.mid,
								next_expiration_time_,
								next_expiration_time_ - static_cast<double>(CoAP::time()),
								expiration_time_factor_);

	ep_ = ep;
	cb_ = func;
	data_ = data;
	if constexpr(is_external_storage)
		buffer_ = buffer;
	buffer_used_ = size;

	return true;
}

template<unsigned MaxPacketSize,
		typename Callback_Functor,
		typename Endpoint>
template<bool SortOptions,
		bool CheckOpOrder,
		bool CheckOpRepeat,
		std::size_t BufferSize,
		typename MessageID>
std::size_t
transaction<MaxPacketSize, Callback_Functor, Endpoint>::
serialize(CoAP::Message::Factory<BufferSize, MessageID>& factory, CoAP::Error& ec) noexcept
{
	static_assert(!is_external_storage, "Must use internal storage");

	std::size_t size = factory.template serialize<SortOptions, CheckOpOrder, CheckOpRepeat>(
								buffer_, MaxPacketSize, ec);
	if(!ec) buffer_used_ = size;
	return size;
}

template<unsigned MaxPacketSize,
		typename Callback_Functor,
		typename Endpoint>
template<bool SortOptions,
		bool CheckOpOrder,
		bool CheckOpRepeat,
		std::size_t BufferSize,
		typename MessageID>
std::size_t
transaction<MaxPacketSize, Callback_Functor, Endpoint>::
serialize(CoAP::Message::Factory<BufferSize, MessageID> const& factory,
		std::uint16_t mid,
		CoAP::Error& ec) noexcept
{
	static_assert(!is_external_storage, "Must use internal storage");

	std::size_t size = factory.template serialize<SortOptions, CheckOpOrder, CheckOpRepeat>(
			buffer_, MaxPacketSize, mid, ec);
	if(!ec) buffer_used_ = size;
	return size;
}

template<unsigned MaxPacketSize,
		typename Callback_Functor,
		typename Endpoint>
bool
transaction<MaxPacketSize, Callback_Functor, Endpoint>::
init(configure const& config,
		endpoint_t const& ep,
		Callback_Functor func, void* data,
		CoAP::Error& ec) noexcept
{
	static_assert(!is_external_storage, "Must use internal storage");
	if(!buffer_used_)
	{
		ec = CoAP::errc::buffer_empty;
		return false;
	}
	return init_impl(config, ep, buffer_, buffer_used_, func, data, ec);
}

/**
 *
 */
template<unsigned MaxPacketSize,
		typename Callback_Functor,
		typename Endpoint>
status_t
transaction<MaxPacketSize, Callback_Functor, Endpoint>::
status() const noexcept
{
	return status_;
}

template<unsigned MaxPacketSize,
		typename Callback_Functor,
		typename Endpoint>
transaction_param
transaction<MaxPacketSize, Callback_Functor, Endpoint>::
transaction_parameters() const noexcept
{
	return transaction_param{
		.max_span = max_span_timeout_,
		.next_expiration = next_expiration_time_,
		.expiration_factor = expiration_time_factor_,
		.retransmission_remaining = retransmission_remaining_
	};
}

template<unsigned MaxPacketSize,
		typename Callback_Functor,
		typename Endpoint>
bool
transaction<MaxPacketSize, Callback_Functor, Endpoint>::
is_busy() const noexcept
{
	return status_ != status_t::none;
}

template<unsigned MaxPacketSize,
		typename Callback_Functor,
		typename Endpoint>
void
transaction<MaxPacketSize, Callback_Functor, Endpoint>::
cancel() noexcept
{
	if(status_ != status_t::sending) return;

	status_ = status_t::canceled;
	call_cb(nullptr);
}

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
