#ifndef COAP_TE_TRANSMISSION_TRANSACTION_LIST_HPP__
#define COAP_TE_TRANSMISSION_TRANSACTION_LIST_HPP__

#include "transaction.hpp"

namespace CoAP{
namespace Transmission{

template<unsigned Size,
		unsigned MaxPacketSize,
		typename Callback_Functor>
class transaction_list{
	public:
		using transaction_t = transaction<MaxPacketSize, Callback_Functor>;
		struct node{
			transaction_t 	transaction;
			bool			valid = false;
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

}//Transmission
}//CoAP

#endif /* COAP_TE_TRANSMISSION_TRANSACTION_LIST_HPP__ */
