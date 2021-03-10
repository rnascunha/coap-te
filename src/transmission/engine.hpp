#ifndef COAP_TE_TRANSMISSION_ENGINE_HPP__
#define COAP_TE_TRANSMISSION_ENGINE_HPP__

#include <cstdint>

#include "error.hpp"

#include "types.hpp"
#include "transaction_list.hpp"
#include "transaction.hpp"
#include "request.hpp"

namespace CoAP{
namespace Transmission{

template<typename Connection,
	configure const& Config,
	unsigned MaxTransactionNumber,
	unsigned MaxPacketSize,
	typename Callback_Functor = transaction_cb>
class engine
{
	public:
		using endpoint = typename Connection::endpoint;
		using transaction_t = transaction<MaxPacketSize, Callback_Functor, endpoint>;
		using request = Request<Callback_Functor>;

		engine(Connection&& conn)
		: conn_(std::move(conn)){}

		template<bool UseInternalBufferNon,
				std::size_t BufferSize,
				typename MessageID,
				bool SortOptions = true,
				bool CheckOpOrder = !SortOptions,
				bool CheckOpRepeat = true>
		std::size_t send(endpoint&,
				CoAP::Message::Factory<BufferSize, MessageID>&,
				Callback_Functor func, void* data,
				CoAP::Error&) noexcept;

		template<bool UseInternalBufferNon,
				std::size_t BufferSize,
				typename MessageID,
				bool SortOptions = true,
				bool CheckOpOrder = !SortOptions,
				bool CheckOpRepeat = true>
		std::size_t send(endpoint&,
				CoAP::Message::Factory<BufferSize, MessageID> const&,
				std::uint16_t mid,
				Callback_Functor func, void* data,
				CoAP::Error&) noexcept;

		template<bool UseInternalBufferNon,
						bool SortOptions = true,
						bool CheckOpOrder = !SortOptions,
						bool CheckOpRepeat = true>
		std::size_t send(Request<Callback_Functor>&,
						std::uint16_t mid,
						CoAP::Error&) noexcept;

		void process(std::uint8_t const* buffer, std::size_t buffer_len,
				CoAP::Error& ec) noexcept;
		void process(endpoint const& ep,
				std::uint8_t const* buffer, std::size_t buffer_len,
				CoAP::Error& ec) noexcept;
		void check_transactions() noexcept;
		bool run(CoAP::Error& ec);
		bool operator()(CoAP::Error& ec) noexcept;
	private:
		transaction_list<MaxTransactionNumber,
						transaction_t> list_;
		Connection		conn_;
		std::uint8_t	buffer_[MaxPacketSize];
};

}//Transmission
}//CoAP

#include "impl/engine_impl.hpp"

#endif /* COAP_TE_TRANSMISSION_ENGINE_HPP__ */
