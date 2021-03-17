#ifndef COAP_TE_TRANSMISSION_ENGINE_HPP__
#define COAP_TE_TRANSMISSION_ENGINE_HPP__

#include <cstdint>

#include "error.hpp"

#include "types.hpp"
#include "request.hpp"
#include "response.hpp"
#include "resource/types.hpp"
#include "resource/node.hpp"

namespace CoAP{
namespace Transmission{

template<profile Profile,
	typename Connection,
	typename MessageID,
	typename TransactionList,
	typename Callback_Resource_Functor>
class engine
{
		using empty = struct{};
	public:
		using transaction_list = TransactionList;
		using transaction_t = typename TransactionList::transaction_t;
		using transaction_cb = typename transaction_t::transaction_cb;
		using endpoint = typename Connection::endpoint;
		using request = Request<endpoint, transaction_cb>;
		using response = Response<endpoint>;
		using resource = CoAP::Resource::resource<Callback_Resource_Functor>;
		using resource_root = typename std::conditional<Profile == profile::server,
				typename CoAP::Resource::resource_root<Callback_Resource_Functor>, empty>::type;
		using resource_node = typename std::conditional<Profile == profile::server,
				typename CoAP::Resource::resource_root<Callback_Resource_Functor>::node_t, empty>::type;
		using Async_response = async_response<endpoint>;

		static constexpr const unsigned packet_size = transaction_t::max_packet_size();

		engine(Connection&& conn, MessageID&& message_id)
		: conn_(std::move(conn)), mid_(std::move(message_id)){}

		engine(Connection&& conn, MessageID&& message_id, configure& config)
			: conn_(std::move(conn)), mid_(std::move(message_id)), config_(config){}

		static constexpr profile get_profile()
		{
			return Profile;
		}

		static constexpr unsigned max_packet_size()
		{
			return packet_size;
		}

		constexpr Connection& connection() noexcept
		{
			return conn_;
		}

		template<bool UseInternalBufferNon = false,
				bool SortOptions = true,
				bool CheckOpOrder = !SortOptions,
				bool CheckOpRepeat = true,
				std::size_t BufferSize,
				typename Message_ID>
		std::size_t send(endpoint&,
				CoAP::Message::Factory<BufferSize, Message_ID> const&,
				transaction_cb func, void* data,
				CoAP::Error&) noexcept;

		template<bool UseInternalBufferNon = false,
				bool SortOptions = true,
				bool CheckOpOrder = !SortOptions,
				bool CheckOpRepeat = true,
				std::size_t BufferSize,
				typename Message_ID>
		std::size_t send(endpoint&,
				CoAP::Message::Factory<BufferSize, Message_ID> const&,
				std::uint16_t mid,
				transaction_cb func, void* data,
				CoAP::Error&) noexcept;

		template<bool UseInternalBufferNon = false,
				bool SortOptions = true,
				bool CheckOpOrder = !SortOptions,
				bool CheckOpRepeat = true>
		std::size_t send(request&,
						std::uint16_t mid,
						CoAP::Error&) noexcept;

		template<bool UseInternalBufferNon = false,
				bool SortOptions = true,
				bool CheckOpOrder = !SortOptions,
				bool CheckOpRepeat = true>
		std::size_t send(request&,
						CoAP::Error&) noexcept;

		template<bool UseInternalBufferNon = false,
				bool SortOptions = true,
				bool CheckOpOrder = !SortOptions,
				bool CheckOpRepeat = true,
				std::size_t BufferSize,
				typename Message_ID>
		std::size_t send(endpoint&,
				configure const&,
				CoAP::Message::Factory<BufferSize, Message_ID> const&,
				transaction_cb func, void* data,
				CoAP::Error&) noexcept;

		template<bool UseInternalBufferNon = false,
				bool SortOptions = true,
				bool CheckOpOrder = !SortOptions,
				bool CheckOpRepeat = true,
				std::size_t BufferSize,
				typename Message_ID>
		std::size_t send(endpoint&,
				configure const&,
				CoAP::Message::Factory<BufferSize, Message_ID> const&,
				std::uint16_t mid,
				transaction_cb func, void* data,
				CoAP::Error&) noexcept;

		template<bool UseInternalBufferNon = false,
				bool SortOptions = true,
				bool CheckOpOrder = !SortOptions,
				bool CheckOpRepeat = true>
		std::size_t send(request&,
					configure const&,
					std::uint16_t mid,
					CoAP::Error&) noexcept;

		template<bool UseInternalBufferNon = false,
				bool SortOptions = true,
				bool CheckOpOrder = !SortOptions,
				bool CheckOpRepeat = true>
		std::size_t send(request&,
				configure const&,
				CoAP::Error&) noexcept;

		std::size_t send(endpoint&,
				const void* buffer, std::size_t buffer_len,
				CoAP::Error&) noexcept;

		resource& root() noexcept;
		resource_root& root_node() noexcept;

		std::uint16_t mid() noexcept;

		template<bool UseEndpointTransMatch = false>
		void process(endpoint& ep,
				std::uint8_t const* buffer, std::size_t buffer_len,
				CoAP::Error& ec) noexcept;

		void check_transactions() noexcept;
		bool run(CoAP::Error& ec);
		bool operator()(CoAP::Error& ec) noexcept;
	private:
		template<bool CheckEndpoint = true, bool CheckToken = false>
		void process_response(endpoint& ep, CoAP::Message::message&) noexcept;
		void process_request(endpoint& ep, CoAP::Message::message&,
				std::uint8_t const* buffer,
				CoAP::Error& ec) noexcept;

		transaction_list list_;

		resource_root	resource_root_;

		Connection		conn_;
		MessageID		mid_;
		std::uint8_t	buffer_[packet_size];

		configure		config_;
};

}//Transmission
}//CoAP

#include "impl/engine_send_impl.hpp"
#include "impl/engine_impl.hpp"

#endif /* COAP_TE_TRANSMISSION_ENGINE_HPP__ */
