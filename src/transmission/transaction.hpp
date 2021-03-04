#ifndef COAP_TE_TRANSMISSION_TRANSACTION_HPP__
#define COAP_TE_TRANSMISSION_TRANSACTION_HPP__

#include <cstdint>
#include <cstdlib>

//#include "port/port.hpp"
#include "request.hpp"
#include "message/types.hpp"
#include "message/parser.hpp"

namespace CoAP{
namespace Transmission{

template<unsigned MaxPacketSize,
		typename Callback_Functor>
class transaction2
{
	public:
		transaction2() :
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

		std::uint8_t* buffer() noexcept { return buffer_; }

		template<bool SortOptions3,
				bool CheckOpOrder3,
				bool CheckOpRepeat3>
		std::size_t request2(Request2<Callback_Functor> const& req2,
				uint16_t mid,
				CoAP::Error& ec) noexcept
		{
//			std::size_t size = 0;
			std::size_t size = req2.serialize2<SortOptions3, CheckOpOrder3, CheckOpRepeat3>(
														buffer_, MaxPacketSize, mid, ec);
//			std::size_t size = req.serialize<SortOptions, CheckOpOrder, CheckOpRepeat>(
//											buffer_, MaxPacketSize, mid, ec);
			if(req2.factory().type() != CoAP::Message::type::confirmable) return size;
			mid_ = mid;
			transmission_init_ = CoAP::time();
			retransmission_count_ = 0;
			buffer_used_ = size;
			/**
			 * Parsing and serialize hold the message in different ways...
			 */
			CoAP::Message::parse(request_, buffer_, MaxPacketSize, ec);

			ep_ = req2.endpoint();
			cb_ = req2.callback();
			data_ = req2.data();

			req2.reset();

			return size;
		}
	private:
		CoAP::Message::message	request_;
		CoAP::endpoint			ep_;

		Callback_Functor		cb_ = nullptr;
		void*					data_ = nullptr;
		std::uint16_t			mid_;
		CoAP::time_t			transmission_init_;
		CoAP::time_t			expiration_time_;
		unsigned int			retransmission_count_;
		std::uint8_t			buffer_[MaxPacketSize];
		std::size_t				buffer_used_ = 0;
};

}//Transmission
}//CoAP

#endif /* COAP_TE_TRANSMISSION_TRANSACTION_HPP__ */
