#ifndef COAP_TE_TRANSMISSION_ENGINE_HPP__
#define COAP_TE_TRANSMISSION_ENGINE_HPP__

#include <cstdint>
#include <type_traits>

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
	typename Callback_Default_Functor,
	typename Callback_Resource_Functor>
class engine
{
		using empty = struct{};
	public:
		using endpoint = typename Connection::endpoint;
		using transaction_list = TransactionList;
		using transaction_t = typename TransactionList::transaction_t;
		using transaction_cb = typename transaction_t::transaction_cb;
		using request = Request<endpoint, transaction_cb>;
		using response = Response<endpoint>;
		using resource = CoAP::Resource::resource<Callback_Resource_Functor>;
		using resource_root = typename std::conditional<Profile == profile::server,
				typename CoAP::Resource::resource_root<Callback_Resource_Functor>, empty>::type;
		using resource_node = typename std::conditional<Profile == profile::server,
				typename CoAP::Resource::resource_root<Callback_Resource_Functor>::node_t, empty>::type;
		using async_response = separate_response<endpoint>;

		static constexpr const bool has_default_callback =
						std::is_invocable< // @suppress("Symbol is not resolved")
										Callback_Default_Functor,
										endpoint const&,
										CoAP::Message::message const&, void*>::value;

		using default_response_cb = typename std::conditional<has_default_callback,
				Callback_Default_Functor, empty>::type;

		static constexpr const unsigned packet_size = transaction_t::max_packet_size();

		engine(Connection&& conn, MessageID&& message_id);
		engine(Connection&& conn, MessageID&& message_id, configure& config);

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

		void default_cb(default_response_cb cb) noexcept;

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
		void process_response(endpoint& ep, CoAP::Message::message const&) noexcept;
		void process_request(endpoint& ep, CoAP::Message::message const&,
				std::uint8_t const* buffer,
				CoAP::Error& ec) noexcept;

		transaction_list list_;

		resource_root	resource_root_;

		Connection		conn_;
		MessageID		mid_;
		std::uint8_t	buffer_[packet_size];

		default_response_cb default_cb_;

		configure		config_;
};

}//Transmission
}//CoAP

#include "impl/engine_send_impl.hpp"
#include "impl/engine_impl.hpp"

#endif /* COAP_TE_TRANSMISSION_ENGINE_HPP__ */
