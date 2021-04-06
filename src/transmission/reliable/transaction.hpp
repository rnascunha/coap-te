#ifndef COAP_TE_TRANSMISSION_RELIABLE_TRANSACTION_HPP__
#define COAP_TE_TRANSMISSION_RELIABLE_TRANSACTION_HPP__

#include "types.hpp"
#include "../types.hpp"
#include <cstdint>
#include <type_traits>

namespace CoAP{
namespace Transmission{
namespace Reliable{

template<unsigned MaxPacketSize,
		typename CallbackFunctor>
class transaction
{
	using buffer_type = typename std::conditional<MaxPacketSize == 0,
									std::uint8_t*,
									std::uint8_t[MaxPacketSize]>::type;
	public:
		static bool constexpr const is_external_storage = MaxPacketSize == 0;
		using transaction_cb = CallbackFunctor;

		static constexpr unsigned max_packet_size()
		{
			return MaxPacketSize;
		}

		transaction(){}

		/**
		 * To be used with external buffer
		 */
		bool init(int socket,
				std::uint8_t* buffer, std::size_t size,
				CallbackFunctor func, void* data,
				expiration_time_type,
				CoAP::Error& ec) noexcept;

		/**
		 * To be used with internal buffer
		 */
		template<bool SetLength = true,
				bool SortOptions = true,
				bool CheckOpOrder = !SortOptions,
				bool CheckOpRepeat = true,
				std::size_t BufferSize,
				CoAP::Message::code Code>
		std::size_t serialize(CoAP::Message::Reliable::Factory<BufferSize, Code> const&,
				CoAP::Error&) noexcept;

		bool init(int socket,
				CallbackFunctor, void*,
				expiration_time_type,
				CoAP::Error&) noexcept;

		/**
		 *
		 */
		status_t status() const noexcept;
		bool is_busy() const noexcept;

		void cancel() noexcept;
		bool check() noexcept;

		bool check_response(int socket,
				CoAP::Message::Reliable::message const& response) noexcept;

		CoAP::Message::Reliable::message const& request() const noexcept;
		std::uint8_t* buffer() noexcept;
		std::size_t buffer_used() const noexcept;

		int socket() const noexcept;

		expiration_time_type expiration_time() const noexcept;
	private:
		bool init_impl(int socket,
				std::uint8_t* buffer, std::size_t size,
				CallbackFunctor func, void* data,
				expiration_time_type time,
				CoAP::Error& ec) noexcept;

		void call_cb(CoAP::Message::Reliable::message const* response) noexcept;
		void clear() noexcept;

		CoAP::Message::Reliable::message
								request_;

		int						socket_ = 0;

		CallbackFunctor			cb_ = nullptr;
		void*					data_ = nullptr;

		expiration_time_type	expiration_time_ = default_expiration;

		buffer_type				buffer_;
		std::size_t				buffer_used_ = 0;

		status_t				status_ = status_t::none;
};

}//CoAP
}//Transmission
}//Reliable

#include "impl/transaction_impl.hpp"

#endif /* COAP_TE_TRANSMISSION_RELIABLE_TRANSACTION_HPP__ */
