#ifndef COAP_TE_TRANSMISSION_HPP__
#define COAP_TE_TRANSMISSION_HPP__

#include <cstdint>
#include "message/types.hpp"
#include "message/parser.hpp"
#include "internal/helper.hpp"
#include "port/port.hpp"

namespace CoAP{
namespace Transmission{

//https://tools.ietf.org/html/rfc7252#section-4.8
struct configure{
	unsigned int	ack_timeout_seconds 			= 2;	//ACK_TIMEOUT
	float			ack_ramdom_factor 				= 1.5;	//ACK_RANDOM_FACTOR
	unsigned int	max_restransmission 			= 4;	//MAX_RETRANSMIT
	unsigned int	max_interaction 				= 1;	//NSTART
	unsigned int	default_leisure_seconds 		= 5;	//DEFAULT_LEISURE
	float			probing_rate_byte_per_seconds 	= 1;	//PROBING_RATE
	//Implementation
	unsigned int	max_packet_size = 1024;
};

using transaction_cb = void(CoAP::Message::message const*,
							CoAP::Message::message const*,
							void*) const;

template<unsigned MaxPacketSize, typename Callback_Functor = transaction_cb>
class transaction
{
	public:
		transaction() :
			mid_(0),
			transmission_init_(0),
			expiration_time_(0),
			retransmission_count_(0)
			{}

		void call_cb(CoAP::Message::message const* response) const noexcept
		{
			if(cb_) cb_(&request_, response, data_);
		}

		bool check(CoAP::time_t time)
		{
//			if(time > expiration_time_)
//			{
//				if(retransmission_count_ >= )
//			}
			return true;
		}

		std::uint16_t mid() const noexcept{ return mid_; }

		std::uint8_t const* buffer() noexcept { return buffer_; }
	private:
		CoAP::Message::message	request_;
		void*					data_ = nullptr;
		Callback_Functor		cb_ = nullptr;
		CoAP::endpoint			ep_;

		std::uint16_t			mid_;
		CoAP::time_t			transmission_init_;
		CoAP::time_t			expiration_time_;
		unsigned int			retransmission_count_;
		std::uint8_t			buffer_[MaxPacketSize];
		std::size_t				buffer_used = 0;
};

template<unsigned Size, unsigned MaxPacketSize>
class transaction_list{
	public:
		using transaction_t = transaction<MaxPacketSize>;
		struct node{
			transaction<MaxPacketSize> 	transaction;
			bool						valid = false;
		};

		transaction_list(){}
		transaction_t* mid(std::uint16_t mid)
		{
			for(unsigned i = 0; i < Size; i++)
				if(nodes_[i].valid && nodes_[i].transaction.mid() == mid)
					return &nodes_[i].transaction;

			return nullptr;
		}

		transaction_t* check_free_slot(bool ocupie = true)
		{
			for(unsigned i = 0; i < Size; i++)
				if(!nodes_[i].valid)
				{
					nodes_[i].valid = ocupie;
					return &nodes_[i].transaction;
				}

			return nullptr;
		}

		void clear_by_mid(std::uint16_t mid)
		{
			for(unsigned i = 0; i < Size; i++)
				if(nodes_[i].transaction.mid() == mid)
					nodes_[i].valid = false;
		}

		void clear_by_slot(unsigned slot)
		{
			if(slot >= Size) return;
			nodes_[slot].valid = false;
		}

		void checks(CoAP::time_t time)
		{
			for(unsigned i = 0; i < Size; i++)
				if(nodes_[i].valid)
					if(!nodes_[i].transaction.check(time))
						nodes_[i].valid = false;
		}

		constexpr unsigned size(){ return Size; }
	private:
		node	nodes_[Size];
};

template<configure Config>
class engine
{
	public:
		using transaction_t = transaction<Config.max_packet_size>;
		engine(connection&& conn) : conn_(std::move(conn)){}

		void process(std::uint8_t const* buffer, std::size_t buffer_len) noexcept
		{
			CoAP::Message::message response;
			CoAP::Error ec;
			parse(response, buffer, buffer_len, ec);
			if(ec) return;

			transaction_t* trans = list_.mid(response.mid);
			if(!trans) return;

			trans->call_cb(&response);
			list_.clear_by_mid(response.mid);
		}

		void check_transactions()
		{
			list_.checks(CoAP::time());
		}

		bool operator()(CoAP::Error& ec)
		{
			CoAP::endpoint ep;
			std::size_t size = conn_.receive(buffer_, Config.max_packet_size, ep, ec);
			if(ec) return false;
			process(buffer_, size);

			check_transactions();

			return true;
		}
	private:
		transaction_list<Config.max_interaction,
						 Config.max_packet_size>
						list_;
		connection		conn_;
		std::uint8_t	buffer_[Config.max_packet_size];
};

float
max_transmit_span(configure const& config)
{
	return config.ack_timeout_seconds *
			CoAP::Helper::pow(2, config.max_restransmission) *
			config.ack_ramdom_factor;
}

float
max_transmist_wait(configure const& config)
{
	return config.ack_timeout_seconds *
				(CoAP::Helper::pow(2, config.max_restransmission + 1) - 1) *
				config.ack_ramdom_factor;
}

static constexpr const unsigned int max_latency_seconds = 100;	//MAX_LATENCY

unsigned int
maximum_round_trip_time(unsigned int max_latency, unsigned int processing_delay)
{
	return (2* max_latency) * processing_delay;
}

float
exchange_lifetime(configure const& config, unsigned int max_latency, unsigned int processing_delay)
{
	return max_transmit_span(config) * (2 * max_latency) * processing_delay;
}

float
non_lifetime(configure const& config, unsigned int max_latency)
{
	return max_transmit_span(config) * (max_latency);
}

}//Transmission
}//CoAP

#endif /* COAP_TE_TRANSMISSION_HPP__ */
