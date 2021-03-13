#ifndef COAP_TE_TRANSMISSION_TRANSACTION_HPP__
#define COAP_TE_TRANSMISSION_TRANSACTION_HPP__

#include <cstdint>
#include <cstdlib>
#include <type_traits>

#include "transmission/types.hpp"
#include "message/factory.hpp"
#include "message/types.hpp"

namespace CoAP{
namespace Transmission{

template<unsigned MaxPacketSize = 0,
		typename Callback_Functor = transaction_cb,
		typename Endpoint = CoAP::endpoint>
class transaction
{
	using buffer_type = typename std::conditional<MaxPacketSize == 0,
									std::uint8_t*,
									std::uint8_t[MaxPacketSize]>::type;
	public:
		static bool constexpr const is_external_storage = MaxPacketSize == 0;
		using endpoint_t = Endpoint;

		transaction(){}

		/**
		 * To be used with external buffer
		 */
		bool init(configure const& config,
				endpoint_t const& ep,
				std::uint8_t* buffer, std::size_t size,
				Callback_Functor func, void* data, CoAP::Error& ec) noexcept;

		/**
		 * To be used with internal buffer
		 */
		template<std::size_t BufferSize,
				typename MessageID,
				bool SortOptions = true,
				bool CheckOpOrder = !SortOptions,
				bool CheckOpRepeat = true>
		std::size_t serialize(CoAP::Message::Factory<BufferSize, MessageID>&,
				CoAP::Error&) noexcept;

		template<std::size_t BufferSize,
				typename MessageID,
				bool SortOptions = true,
				bool CheckOpOrder = !SortOptions,
				bool CheckOpRepeat = true>
		std::size_t serialize(CoAP::Message::Factory<BufferSize, MessageID> const&,
				std::uint16_t mid,
				CoAP::Error&) noexcept;

		bool init(configure const& config,
				endpoint_t const&,
				Callback_Functor, void*,
				CoAP::Error&) noexcept;

		/**
		 *
		 */
		void retransmit() noexcept;

		status_t status() const noexcept;
		bool is_busy() const noexcept;

		void cancel() noexcept;

		template<bool CheckMaxSpan = false>
		bool check(configure const& config) noexcept;

		template<bool CheckEndpoint = true, bool CheckToken = true>
		bool check_response(endpoint_t const& ep,
				CoAP::Message::message const& response) noexcept;

		CoAP::Message::message const& request() const noexcept;
		std::uint16_t mid() const noexcept;
		std::uint8_t* buffer() noexcept;
		std::size_t buffer_used() const noexcept;
		endpoint_t& endpoint() noexcept;

		transaction_param transaction_parameters() const noexcept;
	private:
		bool init_impl(configure const& config,
				endpoint_t const& ep,
				std::uint8_t* buffer, std::size_t size,
				Callback_Functor func, void* data, CoAP::Error& ec) noexcept;

		void call_cb(CoAP::Message::message const* response) noexcept;
		void clear() noexcept;

		CoAP::Message::message	request_;
		endpoint_t				ep_;

		Callback_Functor		cb_ = nullptr;
		void*					data_ = nullptr;

		double					max_span_timeout_ = 0;
		double					next_expiration_time_ = 0;
		double					expiration_time_factor_ = 0;
		unsigned int			retransmission_count_ = 0;

		buffer_type				buffer_;
		std::size_t				buffer_used_ = 0;

		status_t				status_ = status_t::none;
};

}//Transmission
}//CoAP

#include "impl/transaction_impl.hpp"

#endif /* COAP_TE_TRANSMISSION_TRANSACTION_HPP__ */
