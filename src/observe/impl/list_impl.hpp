#ifndef COAP_TE_OBSERVE_LIST_IMPL_HPP__
#define COAP_TE_OBSERVE_LIST_IMPL_HPP__

#include "../list.hpp"
#include "../functions.hpp"

namespace CoAP{
namespace Observe{

template<typename Observe,
		unsigned Size>
list<Observe, Size>::list(){}

template<typename Observe,
	unsigned Size>
constexpr unsigned
list<Observe, Size>::size() const noexcept
{
	return Size;
}

template<typename Observe,
		unsigned Size>
template<typename Message,
		typename ...Args>
bool list<Observe, Size>::
process(endpoint const& ep, Message const& msg, Args&& ...args) noexcept
{
	message_status status = process_message(msg);

	if(status == message_status::register_) return add(ep, msg, std::forward<Args>(args)...);
	else if(status == message_status::deregister)
	{
			remove(ep, msg);
			return false;
	}
	return false;

}

template<typename Observe,
		unsigned Size>
template<typename Message,
		typename ...Args>
bool list<Observe, Size>::
add(endpoint const& ep, Message const& msg, Args&& ...args) noexcept
{
	unsigned slot = Size + 1;
	for(unsigned i = 0; i < Size; i++)
	{
		if(!list_[i].is_used())
		{
			if(slot > Size) slot = i;
			continue;
		}
		if(list_[i].check(ep, msg))
		{
			list_[slot].set(ep, msg, std::forward<Args>(args)...);
			return true;
		}
	}
	if(slot > Size) return false;

	list_[slot].set(ep, msg, std::forward<Args>(args)...);
	return true;
}

template<typename Observe,
		unsigned Size>
template<typename Message>
bool list<Observe, Size>::
remove(endpoint const& ep, Message const& msg) noexcept
{
	for(unsigned i = 0; i < Size; i++)
	{
		if(list_[i].check(ep, msg))
		{
			list_[i].clear();
			return true;
		}
	}
	return false;
}

template<typename Observe,
		unsigned Size>
Observe* list<Observe, Size>::
operator[](unsigned index) noexcept
{
	return index >= Size ? nullptr : &list_[index];
}

}//Observe
}//CoAP

#endif /* COAP_TE_OBSERVE_LIST_IMPL_HPP__ */
