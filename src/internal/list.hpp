#ifndef COAP_TE_INTERNAL_LIST_HPP__
#define COAP_TE_INTERNAL_LIST_HPP__

#include <utility>

namespace CoAP{

template<typename T>
class list{
	public:
		struct node{
			template<typename ...Args>
			node(Args&&... args) : value(std::forward<Args>(args)...){}

			node(){}

			T		value;
			node*	next = nullptr;
		};

		node* head() const noexcept{ return head_; }

		void clear() noexcept{ head_ = nullptr; }

		template<bool AddSorted = true>
		void add(list::node& node) noexcept
		{
			if(head_ == nullptr)
			{
				head_ = &node;
				head_->next = nullptr;
				return;
			}

			if constexpr(AddSorted)
			{
				list::node *n = head_, *prev = nullptr;
				for(; n != nullptr; n = n->next)
				{
					if(node.value < n->value)
					{
						node.next = n;
						if(prev) prev->next = &node;
						else head_ = &node;
						break;
					}
					if(n->next == nullptr)
					{
						n->next = &node;
						node.next = nullptr;
						break;
					}
					prev = n;
				}
			}
			else
			{
				list::node* n = head_;
				for(; n->next != nullptr; n = n->next);
				n->next = &node;
				node.next = nullptr;
			}
		}
	private:
		node* head_ = nullptr;
};

}//CoAP

#endif /* COAP_TE_INTERNAL_LIST_HPP__ */
