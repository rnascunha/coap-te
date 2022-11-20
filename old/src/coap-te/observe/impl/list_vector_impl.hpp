#ifndef COAP_TE_OBSERVE_LIST_VECTOR_IMPL_HPP__
#define COAP_TE_OBSERVE_LIST_VECTOR_IMPL_HPP__

#include "../list_vector.hpp"
#include "../functions.hpp"
#include <algorithm>

namespace CoAP{
namespace Observe{

template<typename Observe>
list_vector<Observe>::list_vector(){}

template<typename Observe>
std::size_t
list_vector<Observe>::size() const noexcept
{
	return list_.size();
}

template<typename Observe>
template<typename Message,
		typename ...Args>
bool list_vector<Observe>::
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

template<typename Observe>
template<typename Message,
		typename ...Args>
bool list_vector<Observe>::
add(endpoint const& ep, Message const& msg, Args&& ...args) noexcept
{
	for(auto& obs : list_)
	{
		if(obs.check(ep, msg))
		{
			obs.set(ep, msg, std::forward<Args>(args)...);
			return true;
		}
	}
	auto& o = list_.emplace_back();
	o.set(ep, msg, std::forward<Args>(args)...);

	return true;
}

template<typename Observe>
template<typename Message>
bool list_vector<Observe>::
remove(endpoint const& ep, Message const& msg) noexcept
{
	auto it = std::remove_if(list_.begin(), list_.end(),
				[&ep, &msg](Observe& obs){ return  obs.check(ep, msg); });
	bool ret = it != list_.end();
	list_.erase(it, list_.end());

	return ret;
}

template<typename Observe>
void
list_vector<Observe>::
cancel() noexcept
{
	list_.clear();
}

template<typename Observe>
void
list_vector<Observe>::
cancel(endpoint const& ep) noexcept
{
	auto it = std::remove_if(list_.begin(), list_.end(),
					[&ep](Observe& obs){ return obs.endpoint() == ep; });
	list_.erase(it, list_.end());
}

template<typename Observe>
Observe* list_vector<Observe>::
operator[](unsigned index) noexcept
{
	return index >= list_.size() ? nullptr : &list_[index];
}

}//Observe
}//CoAP

#endif /* COAP_TE_OBSERVE_LIST_VECTOR_IMPL_HPP__ */
