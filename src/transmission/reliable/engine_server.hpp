#ifndef COAP_TE_TRANSMISSION_RELIABLE_ENGINE_SERVER_HPP__
#define COAP_TE_TRANSMISSION_RELIABLE_ENGINE_SERVER_HPP__

#include "defines/defaults.hpp"
#include "types.hpp"
#include "../types.hpp"
#include "resource/resource.hpp"
#include "resource/node.hpp"
#include "response.hpp"
#include "request.hpp"

#include <type_traits>

#include <cstdio>

namespace CoAP{
namespace Transmission{
namespace Reliable{

template<typename Connection,
	csm_configure const& Config,
	typename ConnectionList,
	typename TransactionList,
	typename CallbackDefaultFunctor,
	typename Resource>
class engine_server
{
		using empty = struct{};
	public:
		using socket = typename Connection::handler;
		using connection = Connection;
		static constexpr const bool set_length = Connection::set_length;
		using endpoint = typename Connection::endpoint;

		/**
		 * Connection type
		 */
		static constexpr const bool has_connection_list =
				!std::is_same<ConnectionList, CoAP::disable>::value;
		using connection_list_type = typename std::conditional<
				has_connection_list,
					ConnectionList, connection_list_empty>::type;
		using connection_hold_t = typename connection_list_type::connection_t;

		/**
		 * Transaction type
		 */
		static constexpr const bool has_transaction_list =
				!std::is_same<TransactionList, CoAP::disable>::value;
		using transaction_list_type = typename std::conditional<
				has_transaction_list,
					TransactionList, transaction_list_empty>::type;
		using transaction_t = typename transaction_list_type::transaction_t;
		using transaction_cb = typename transaction_t::transaction_cb;

		template<CoAP::Message::code Code = CoAP::Message::code::get>
		using request = Request<transaction_cb, Code>;
		using response = Response;
		using resource = Resource;
		using resource_root = typename std::conditional<
				!std::is_same<Resource, CoAP::disable>::value,
					typename CoAP::Resource::resource_root<resource>, empty>::type;
		using resource_node = typename std::conditional<
				!std::is_same<Resource, CoAP::disable>::value,
					typename CoAP::Resource::resource_root<resource>::node_t, empty>::type;
		using async_response = separate_response;

		static constexpr const unsigned max_packet_size = Config.max_message_size;

		static constexpr const bool has_default_callback =
						std::is_invocable< // @suppress("Symbol is not resolved")
							CallbackDefaultFunctor,
								socket&,
								CoAP::Message::Reliable::message const&, void*>::value;

		using default_response_cb = typename std::conditional<has_default_callback,
				CallbackDefaultFunctor, empty>::type;

		engine_server();
		~engine_server();

		bool open(endpoint& ep, CoAP::Error&) noexcept;
		template<bool SendAbortMessage = false>
		void close() noexcept;

		template<bool SendAbortMessage = false>
		void close_client(socket) noexcept;
		template<bool SendAbortMessage = false>
		void close_client(socket, const char* payload) noexcept;

		static constexpr profile get_profile()
		{
			return std::is_same<Resource, void*>::value ? profile::client : profile::server;
		}

		constexpr Connection& get_connection() noexcept
		{
			return conn_;
		}

		constexpr ConnectionList& get_connection_list() noexcept
		{
			return conn_list_;
		}

		template<bool SortOptions = true,
				bool CheckOpOrder = !SortOptions,
				bool CheckOpRepeat = true,
				std::size_t BufferSize,
				CoAP::Message::code Code>
		std::size_t send(socket, CoAP::Message::Reliable::Factory<BufferSize, Code> const&,
				transaction_cb func, void* data,
				CoAP::Error&) noexcept;

		template<bool SortOptions = true,
				bool CheckOpOrder = !SortOptions,
				bool CheckOpRepeat = true,
				std::size_t BufferSize,
				CoAP::Message::code Code>
		std::size_t send(socket, CoAP::Message::Reliable::Factory<BufferSize, Code> const&,
				expiration_time_type,
				transaction_cb func, void* data,
				CoAP::Error&) noexcept;

		template<bool UseTransaction = true,
				bool SortOptions = true,
				bool CheckOpOrder = !SortOptions,
				bool CheckOpRepeat = true,
				CoAP::Message::code Code>
		std::size_t send(request<Code>&,
						CoAP::Error&) noexcept;

		template<bool SortOptions = true,
				bool CheckOpOrder = !SortOptions,
				bool CheckOpRepeat = true,
				CoAP::Message::code Code>
		std::size_t send(request<Code>&,
					expiration_time_type,
					CoAP::Error&) noexcept;

		template<bool UseTransaction = true,
				bool SortOptions = true,
				bool CheckOpOrder = !SortOptions,
				bool CheckOpRepeat = true,
				std::size_t BufferSize,
				CoAP::Message::code Code>
		std::size_t send(socket, CoAP::Message::Reliable::Factory<BufferSize, Code> const&,
						CoAP::Error& ec) noexcept;

		std::size_t send(socket, const void* buffer, std::size_t buffer_len,
				CoAP::Error&) noexcept;

		std::size_t send_abort(socket,
				const char* payload, CoAP::Error& ec) noexcept;
		std::size_t send_abort(socket, CoAP::Message::Option::option_abort&,
				const char* payload, CoAP::Error& ec) noexcept;

		resource& root() noexcept;
		resource_root& root_node() noexcept;

		void default_cb(default_response_cb cb) noexcept;

		void process(socket, std::uint8_t const* buffer, std::size_t buffer_len,
				CoAP::Error& ec) noexcept;

		void check_transactions() noexcept;

		template<int BlockTimeMs = 0,
				unsigned MaxEvents = 32>
		bool run(CoAP::Error& ec) noexcept;
		template<int BlockTimeMs = 0,
				unsigned MaxEvents = 32>
		bool operator()(CoAP::Error& ec) noexcept;
	private:
		void process_response(socket, CoAP::Message::Reliable::message const&) noexcept;
		void process_request(socket, CoAP::Message::Reliable::message const&,
							CoAP::Error& ec) noexcept;
		void process_signaling(socket, CoAP::Message::Reliable::message const&) noexcept;
		void process_signaling_csm(socket, CoAP::Message::Reliable::message const&) noexcept;
		void process_signaling_ping(socket, CoAP::Message::Reliable::message const&) noexcept;
		void process_signaling_pong(socket, CoAP::Message::Reliable::message const&) noexcept;
		void process_signaling_release(socket, CoAP::Message::Reliable::message const&) noexcept;
		void process_signaling_abort(socket, CoAP::Message::Reliable::message const&) noexcept;

		void on_read(socket, void*, std::size_t) noexcept;
		void on_open(socket) noexcept;
		void on_close(socket) noexcept;

		resource_root		resource_root_;

		transaction_list_type 	list_;
		connection_list_type	conn_list_;

		Connection			conn_;
		std::uint8_t		buffer_[max_packet_size];

		default_response_cb default_cb_;
};

}//CoAP
}//Transmission
}//Reliable

#include "impl/engine_server_send_impl.hpp"
#include "impl/engine_server_impl.hpp"

#endif /* COAP_TE_TRANSMISSION_RELIABLE_ENGINE_SERVER_HPP__ */
