#ifndef COAP_TE_TRANSMISSION_RELIABLE_TRANSACTION_IMPL_HPP__
#define COAP_TE_TRANSMISSION_RELIABLE_TRANSACTION_IMPL_HPP__

#include "log.hpp"
#include "../../functions.hpp"
#include "port/port.hpp"
#include "message/reliable/parser.hpp"
#include "../transaction.hpp"

namespace CoAP{
namespace Transmission{
namespace Reliable{

static constexpr CoAP::Log::module transaction_mod = {
		.name = "TRANS",
		.max_level = CoAP::Log::type::debug,
		.enable = true
};

template<unsigned MaxPacketSize,
		typename Callback_Functor>
void
transaction<MaxPacketSize, Callback_Functor>::
clear() noexcept
{
	cb_ = nullptr;
	data_ = nullptr;
	expiration_time_ = default_expiration;
	buffer_used_ = 0;
	status_ = status_t::none;
	socket_ = 0;

	if constexpr(is_external_storage)
		buffer_ = nullptr;
}

template<unsigned MaxPacketSize,
		typename Callback_Functor>
bool
transaction<MaxPacketSize, Callback_Functor>::
check() noexcept
{
	if(status_ != status_t::sending)
		return false;

	double ftime = static_cast<double>(CoAP::time());
	if(ftime > expiration_time_)
	{
		CoAP::Log::debug(transaction_mod, "Transaciton expired");
		status_ = status_t::timeout;
		call_cb(nullptr);
		clear();

		return true;
	}
	return false;
}

template<unsigned MaxPacketSize,
		typename Callback_Functor>
bool
transaction<MaxPacketSize, Callback_Functor>::
check_response(int socket,
		CoAP::Message::Reliable::message const& response) noexcept
{
	if(status_ != status_t::sending) return false;
	if(socket != socket_) return false;

	if(request_.token_len != response.token_len)
		return false;
	if(std::memcmp(request_.token, response.token, request_.token_len) != 0)
		return false;

	status_ = status_t::success;

	CoAP::Log::debug(transaction_mod, "Response arrived");
	call_cb(&response);

	return true;
}

template<unsigned MaxPacketSize,
		typename Callback_Functor>
CoAP::Message::Reliable::message const&
transaction<MaxPacketSize, Callback_Functor>::
request() const noexcept
{
	return request_;
}

template<unsigned MaxPacketSize,
		typename Callback_Functor>
std::uint8_t*
transaction<MaxPacketSize, Callback_Functor>::
buffer() noexcept
{
	return buffer_;
}

template<unsigned MaxPacketSize,
		typename Callback_Functor>
std::size_t
transaction<MaxPacketSize, Callback_Functor>::
buffer_used() const noexcept
{
	return buffer_used_;
}

template<unsigned MaxPacketSize,
		typename Callback_Functor>
int
transaction<MaxPacketSize, Callback_Functor>::
socket() const noexcept
{
	return socket_;
}

template<unsigned MaxPacketSize,
		typename Callback_Functor>
bool
transaction<MaxPacketSize, Callback_Functor>::
init(int socket,
	std::uint8_t* buffer, std::size_t size,
	Callback_Functor func, void* data,
	expiration_time_type time,
	CoAP::Error& ec) noexcept
{
	static_assert(is_external_storage, "Must use external storage");

	return init_impl(socket, buffer, size, func, data, time, ec);
}

template<unsigned MaxPacketSize,
		typename Callback_Functor>
bool
transaction<MaxPacketSize, Callback_Functor>::
init_impl(int socket,
		std::uint8_t* buffer, std::size_t size,
		Callback_Functor func, void* data,
		expiration_time_type time,
		CoAP::Error& ec) noexcept
{
	if(status_ != status_t::none)
	{
		CoAP::Log::error(transaction_mod, "Transaction been used");
		ec = CoAP::errc::no_free_slots;
		return false;
	}

	if(time == no_transaction) return true;

	CoAP::Message::Reliable::parse(request_, buffer, size, ec);
	if(ec) return false;

	status_ = status_t::sending;

	expiration_time_ = time;

	socket_ = socket;
	cb_ = func;
	data_ = data;
	if constexpr(is_external_storage)
		buffer_ = buffer;
	buffer_used_ = size;

	return true;
}

template<unsigned MaxPacketSize,
		typename Callback_Functor>
template<bool SetLength,
		bool SortOptions,
		bool CheckOpOrder,
		bool CheckOpRepeat,
		std::size_t BufferSize,
		CoAP::Message::code Code>
std::size_t
transaction<MaxPacketSize, Callback_Functor>::
serialize(CoAP::Message::Reliable::Factory<BufferSize, Code> const& factory, CoAP::Error& ec) noexcept
{
	static_assert(!is_external_storage, "Must use internal storage");

	std::size_t size = factory.template serialize<SetLength, SortOptions, CheckOpOrder, CheckOpRepeat>(
								buffer_, MaxPacketSize, ec);
	if(!ec) buffer_used_ = size;
	return size;
}

template<unsigned MaxPacketSize,
		typename Callback_Functor>
bool
transaction<MaxPacketSize, Callback_Functor>::
init(int socket,
	Callback_Functor func, void* data,
	expiration_time_type time,
	CoAP::Error& ec) noexcept
{
	static_assert(!is_external_storage, "Must use internal storage");
	if(!buffer_used_)
	{
		ec = CoAP::errc::buffer_empty;
		return false;
	}
	return init_impl(socket, buffer_, buffer_used_, func, data, time, ec);
}

/**
 *
 */
template<unsigned MaxPacketSize,
		typename Callback_Functor>
status_t
transaction<MaxPacketSize, Callback_Functor>::
status() const noexcept
{
	return status_;
}

template<unsigned MaxPacketSize,
		typename Callback_Functor>
expiration_time_type
transaction<MaxPacketSize, Callback_Functor>::
expiration_time() const noexcept
{
	return expiration_time_;
}

template<unsigned MaxPacketSize,
		typename Callback_Functor>
bool
transaction<MaxPacketSize, Callback_Functor>::
is_busy() const noexcept
{
	return status_ != status_t::none;
}

template<unsigned MaxPacketSize,
		typename Callback_Functor>
void
transaction<MaxPacketSize, Callback_Functor>::
cancel() noexcept
{
	if(status_ != status_t::sending) return;

	status_ = status_t::canceled;
	call_cb(nullptr);
}

template<unsigned MaxPacketSize,
		typename Callback_Functor>
void
transaction<MaxPacketSize, Callback_Functor>::
call_cb(CoAP::Message::Reliable::message const* response) noexcept
{
	if(cb_) cb_(this, response, data_);
	clear();
}

}//Reliable
}//Transmission
}//CoAP

#endif /* COAP_TE_TRANSMISSION_RELIABLE_TRANSACTION_IMPL_HPP__ */