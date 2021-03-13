#ifndef COAP_TE_TRANSMISSION_ENGINE_HPP__
#define COAP_TE_TRANSMISSION_ENGINE_HPP__

#include <cstdint>

#include "error.hpp"

#include "types.hpp"
#include "transaction_list.hpp"
#include "transaction.hpp"
#include "request.hpp"
#include "response.hpp"
#include "resource/types.hpp"
#include "resource/node.hpp"
//#include "resource/list.hpp"

namespace CoAP{
namespace Transmission{

template<profile Profile,
	typename Connection,
	typename MessageID,
	configure const& Config,
	unsigned MaxTransactionNumber,
	unsigned MaxPacketSize,
	typename Callback_Functor = transaction_cb,
	typename Callback_Resource_Functor = CoAP::Resource::callback>
class engine
{
		using empty = struct{};
	public:
		using endpoint = typename Connection::endpoint;
		using transaction_t = transaction<MaxPacketSize, Callback_Functor, endpoint>;
		using request = Request<Callback_Functor>;
		using resource = CoAP::Resource::resource<Callback_Resource_Functor>;
		using resource_root = typename std::conditional<Profile == profile::server,
				typename CoAP::Resource::resource_node<Callback_Resource_Functor>, empty>::type;
		using resource_node = typename std::conditional<Profile == profile::server,
				typename CoAP::Resource::resource_node<Callback_Resource_Functor>::node_t, empty>::type;

		engine(Connection&& conn, MessageID&& message_id)
		: conn_(std::move(conn)), mid_(std::move(message_id)){}

		constexpr profile get_profile() const noexcept
		{
			return Profile;
		}

		constexpr configure const& get_configure() const noexcept
		{
			return Config;
		}

		constexpr unsigned max_transaction() const noexcept
		{
			return MaxTransactionNumber;
		}

		constexpr unsigned max_packet_size() const noexcept
		{
			return MaxPacketSize;
		}

		constexpr Connection& connection() noexcept
		{
			return conn_;
		}

		template<bool UseInternalBufferNon,
				bool SortOptions = true,
				bool CheckOpOrder = !SortOptions,
				bool CheckOpRepeat = true,
				std::size_t BufferSize,
				typename Message_ID>
		std::size_t send(endpoint&,
				CoAP::Message::Factory<BufferSize, Message_ID>&,
				Callback_Functor func, void* data,
				CoAP::Error&) noexcept;

		template<bool UseInternalBufferNon,
				bool SortOptions = true,
				bool CheckOpOrder = !SortOptions,
				bool CheckOpRepeat = true,
				std::size_t BufferSize,
				typename Message_ID>
		std::size_t send(endpoint&,
				CoAP::Message::Factory<BufferSize, Message_ID> const&,
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

		std::size_t send(endpoint&,
				const void* buffer, std::size_t buffer_len,
				CoAP::Error&) noexcept;

		resource& root() noexcept;
		resource_node& root_node() noexcept;

		std::uint16_t mid() noexcept;

		template<bool UseEndpointTransMatch = false>
		void process(endpoint& ep,
				std::uint8_t const* buffer, std::size_t buffer_len,
				CoAP::Error& ec) noexcept;

		void check_transactions() noexcept;
		bool run(CoAP::Error& ec);
		bool operator()(CoAP::Error& ec) noexcept;
	private:
		template<bool CheckEndpoint = true, bool CheckToken = true>
		void process_response(endpoint& ep, CoAP::Message::message&) noexcept;
		void process_request(endpoint& ep, CoAP::Message::message&,
				std::uint8_t const* buffer,
				CoAP::Error& ec) noexcept;

		transaction_list<MaxTransactionNumber,
						transaction_t> list_;

		resource_root	resource_root_;

		Connection		conn_;
		MessageID		mid_;
		std::uint8_t	buffer_[MaxPacketSize];
};

}//Transmission
}//CoAP

#include "impl/engine_impl.hpp"

#endif /* COAP_TE_TRANSMISSION_ENGINE_HPP__ */
