#ifndef COAP_TE_TRANSMISSION_RELIABLE_ENGINE_CLIENT_HPP__
#define COAP_TE_TRANSMISSION_RELIABLE_ENGINE_CLIENT_HPP__

#include "defines/defaults.hpp"
#include "types.hpp"
#include "../types.hpp"
#include "resource/resource.hpp"
#include "resource/node.hpp"
#include "response.hpp"
#include "request.hpp"

#include <type_traits>

namespace CoAP{
namespace Transmission{
namespace Reliable{

template<typename Connection,
	csm_configure const& Config,
	typename TransactionList,
	typename CallbackDefaultFunctor,
	typename Resource>
class engine_client
{
		using empty = struct{};
	public:
		using socket = typename Connection::handler;
		using connection = Connection;
		static constexpr const bool set_length = Connection::set_length;
		using endpoint = typename Connection::endpoint;

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

		using message = CoAP::Message::Reliable::message;
		template<CoAP::Message::code Code = CoAP::Message::code::get>
		using request = Request<socket, transaction_cb, Code>;
		using response = Response<socket>;
		using resource = Resource;
		using resource_root = typename std::conditional<
				!std::is_same<Resource, CoAP::disable>::value,
					typename CoAP::Resource::resource_root<resource>, empty>::type;
		using resource_node = typename std::conditional<
				!std::is_same<Resource, CoAP::disable>::value,
					typename CoAP::Resource::resource_root<resource>::node_t, empty>::type;
		using async_response = separate_response<socket>;


		static constexpr const unsigned packet_size = Config.max_message_size;

		static constexpr const bool has_default_callback =
						std::is_invocable< // @suppress("Symbol is not resolved")
							CallbackDefaultFunctor,
								socket&,
								CoAP::Message::Reliable::message const*, void*>::value;

		using default_response_cb = typename std::conditional<has_default_callback,
				CallbackDefaultFunctor, empty>::type;

		engine_client();
		~engine_client();

		bool open(endpoint& ep, CoAP::Error&) noexcept;
		/**
		 * This is not really async. The emscripten sockets must be used
		 * async, so we are going to use async connection but block until
		 * it connect... Just for test purpose...
		 */
		bool async_open(endpoint& ep, CoAP::Error&) noexcept;

		template<bool SendAbortMessage = false>
		void close(const char* payload = nullptr) noexcept;

		static constexpr profile get_profile()
		{
			return std::is_same<Resource, void*>::value ? profile::client : profile::server;
		}

		constexpr Connection& get_connection() noexcept
		{
			return conn_;
		}

		template<bool SortOptions = true,
				bool CheckOpOrder = !SortOptions,
				bool CheckOpRepeat = true,
				std::size_t BufferSize,
				CoAP::Message::code Code>
		std::size_t send(CoAP::Message::Reliable::Factory<BufferSize, Code> const&,
				transaction_cb func, void* data,
				CoAP::Error&) noexcept;

		template<bool SortOptions = true,
				bool CheckOpOrder = !SortOptions,
				bool CheckOpRepeat = true,
				std::size_t BufferSize,
				CoAP::Message::code Code>
		std::size_t send(CoAP::Message::Reliable::Factory<BufferSize, Code> const&,
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
		std::size_t send(CoAP::Message::Reliable::Factory<BufferSize, Code> const&,
						CoAP::Error& ec) noexcept;

		std::size_t send(const void* buffer, std::size_t buffer_len,
				CoAP::Error&) noexcept;

		std::size_t send_abort(	const char* payload, CoAP::Error& ec) noexcept;
		std::size_t send_abort(CoAP::Message::Option::option_abort&,
				const char* payload, CoAP::Error& ec) noexcept;

		csm_configure const& server_csm() const noexcept;

		resource& root() noexcept;
		resource_root& root_node() noexcept;

		void default_cb(default_response_cb cb) noexcept;

		void process(std::uint8_t const* buffer, std::size_t buffer_len,
				CoAP::Error& ec) noexcept;

		void check_transactions() noexcept;

		bool run(CoAP::Error& ec) noexcept;
		template<int BlockTimeMs>
		bool run(CoAP::Error& ec) noexcept;
		bool operator()(CoAP::Error& ec) noexcept;
	private:
		template<int BlockTimeMs>
		bool read_packet(CoAP::Error& ec) noexcept;

		void process_response(CoAP::Message::Reliable::message const&) noexcept;
		void process_request(CoAP::Message::Reliable::message const&,
							CoAP::Error& ec) noexcept;
		void process_signaling(CoAP::Message::Reliable::message const&) noexcept;
		void process_signaling_csm(CoAP::Message::Reliable::message const&) noexcept;
		void process_signaling_ping(CoAP::Message::Reliable::message const&) noexcept;
		void process_signaling_pong(CoAP::Message::Reliable::message const&) noexcept;
		void process_signaling_release(CoAP::Message::Reliable::message const&) noexcept;
		void process_signaling_abort(CoAP::Message::Reliable::message const&) noexcept;

		resource_root		resource_root_;

		transaction_list_type list_;

		csm_configure		server_csm_;
		Connection			conn_;
		std::uint8_t		buffer_[packet_size];

		default_response_cb default_cb_;
};

}//CoAP
}//Transmission
}//Reliable

#include "impl/engine_client_send_impl.hpp"
#include "impl/engine_client_impl.hpp"

#endif /* COAP_TE_TRANSMISSION_RELIABLE_ENGINE_CLIENT_HPP__ */
