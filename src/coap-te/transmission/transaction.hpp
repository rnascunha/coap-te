#ifndef COAP_TE_TRANSMISSION_TRANSACTION_HPP__
#define COAP_TE_TRANSMISSION_TRANSACTION_HPP__

#include <cstdint>
#include <cstdlib>
#include <type_traits>

#include "types.hpp"
#include "../message/factory.hpp"
#include "../message/types.hpp"
#include "../internal/meta.hpp"

namespace CoAP{
namespace Transmission{

template<unsigned MaxPacketSize,
		typename Callback_Functor,
		typename Endpoint>
class transaction
{
	using buffer_type = typename buffer_or_pointer_type<MaxPacketSize>::type;
	public:
		static bool constexpr const is_external_storage = MaxPacketSize == 0;
		using endpoint_t = Endpoint;
		using transaction_cb = Callback_Functor;

		static constexpr unsigned max_packet_size()
		{
			return MaxPacketSize;
		}

		transaction() = default;

		/**
		 * Copy constructable and copy assigable is a requirement to use
		 * std::vector as a container
		 */
		transaction(transaction const&);
		transaction& operator=(transaction const&) noexcept;

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
		template<bool SortOptions = true,
				bool CheckOpOrder = !SortOptions,
				bool CheckOpRepeat = true,
				std::size_t BufferSize,
				typename MessageID>
		std::size_t serialize(CoAP::Message::Factory<BufferSize, MessageID>&,
				CoAP::Error&) noexcept;

		template<bool SortOptions = true,
				bool CheckOpOrder = !SortOptions,
				bool CheckOpRepeat = true,
				std::size_t BufferSize,
				typename MessageID>
		std::size_t serialize(CoAP::Message::Factory<BufferSize, MessageID> const&,
				std::uint16_t mid,
				CoAP::Error&) noexcept;

		bool init(configure const&,
				endpoint_t const&,
				Callback_Functor, void*,
				CoAP::Error&) noexcept;

		/**
		 *
		 */
		template<unsigned BackoffTimeFactor = 2>
		void retransmit() noexcept;

		status_t status() const noexcept;
		bool is_busy() const noexcept;

		void cancel() noexcept;

		void lock() noexcept;
		void release() noexcept;

		template<bool CheckMaxSpan = false>
		bool check() noexcept;

		template<bool CheckEndpoint = true, bool CheckToken = true>
		bool check_response(endpoint_t const& ep,
				CoAP::Message::message const& response) noexcept;

		CoAP::Message::message const& request() const noexcept;
		std::uint16_t mid() const noexcept;
		std::uint8_t* buffer() noexcept;
		std::size_t buffer_used() const noexcept;
		endpoint_t& endpoint() noexcept;
		endpoint_t endpoint() const noexcept;

		transaction_param transaction_parameters() const noexcept;
	private:
		void clear() noexcept;

		bool init_impl(configure const& config,
				endpoint_t const& ep,
				std::uint8_t* buffer, std::size_t size,
				Callback_Functor func, void* data, CoAP::Error& ec) noexcept;

		void call_cb(CoAP::Message::message const* response) noexcept;

		CoAP::Message::message	request_;
		endpoint_t				ep_;

		Callback_Functor		cb_ = nullptr;
		void*					data_ = nullptr;

		double					max_span_timeout_ = 0;
		double					next_expiration_time_ = 0;
		double					expiration_time_factor_ = 0;
		unsigned int			retransmission_remaining_ = 0;

		buffer_type				buffer_;
		std::size_t				buffer_used_ = 0;

		status_t				status_ = status_t::none;
};

}//Transmission
}//CoAP

#include "impl/transaction_impl.hpp"

#endif /* COAP_TE_TRANSMISSION_TRANSACTION_HPP__ */
