#ifndef COAP_TE_TRANSMISSION_RELIABLE_TRANSACTION_EMPTY_HPP__
#define COAP_TE_TRANSMISSION_RELIABLE_TRANSACTION_EMPTY_HPP__

#include "../types.hpp"
#include "../../types.hpp"
#include <cstdint>
#include <type_traits>

namespace CoAP{
namespace Transmission{
namespace Reliable{

class transaction_empty
{
	public:
		static bool constexpr const is_external_storage = 0;
		using transaction_cb = CoAP::Transmission::Reliable::transaction_cb;

		static constexpr unsigned max_packet_size()
		{
			return 0;
		}

		transaction_empty(){}

		/**
		 * To be used with external buffer
		 */
		bool init(int,
				std::uint8_t*, std::size_t,
				transaction_cb, void*,
				expiration_time_type,
				CoAP::Error&) noexcept{ return true; }

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
				CoAP::Error&) noexcept{ return 0; }

		bool init(int socket,
				transaction_cb, void*,
				expiration_time_type,
				CoAP::Error&) noexcept{ return true; }

		/**
		 *
		 */
		status_t status() const noexcept{ return status_t::none; }
		bool is_busy() const noexcept{ return false; }

		void cancel() noexcept{}
		bool check() noexcept{ return true; }

		bool check_response(int,
				CoAP::Message::Reliable::message const&) noexcept{ return true; }

		CoAP::Message::Reliable::message const& request() const noexcept
		{
			static CoAP::Message::Reliable::message msg;
			return msg;
		}

		std::uint8_t* buffer() noexcept{ return nullptr; }
		std::size_t buffer_used() const noexcept{ return 0; }

		int socket() const noexcept;

		expiration_time_type expiration_time() const noexcept{ return 0; }
};

}//CoAP
}//Transmission
}//Reliable

#endif /* COAP_TE_TRANSMISSION_RELIABLE_TRANSACTION_EMPTY_HPP__ */
