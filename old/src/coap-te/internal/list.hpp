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
			list::node *curr = &node, *next = node.next;
			while(true)
			{
				_add<AddSorted>(*curr);
				if(next == nullptr) break;
				curr = next;
				next = curr->next;
			}
		}

		unsigned size() const noexcept
		{
			if(!head_) return 0;

			list::node* n = head_;
			unsigned i = 0;
			for(; n!= nullptr; n = n->next, i++);

			return i;
		}

		node* operator[](unsigned index) noexcept
		{
			node* n = head_;
			for(;index-- && n; n = n->next);
			return n;
		}

		template<typename U>
		node* remove(U const& u) noexcept
		{
			if(!head_) return nullptr;

			node* n = head_;
			if(head_->value == u)
			{
				head_ = head_->next;
				n->next = nullptr;
				return n;
			}

			for(; n->next; n = n->next)
			{
				if(n->next->value == u)
				{
					node* res = n->next;
					n->next = n->next->next;
					res->next = nullptr;
					return res;
				}
			}

			return nullptr;
		}

		node* remove(node& u) noexcept
		{
			if(!head_) return nullptr;

			if(head_ == &u)
			{
				head_ = u.next;
				u.next = nullptr;
				return &u;
			}

			node* n = head_;
			for(; n->next; n = n->next)
			{
				if(n->next == &u)
				{
					n->next = u.next;
					u.next = nullptr;
					return &u;
				}
			}

			return nullptr;
		}
	private:
		template<bool AddSorted = true>
		void _add(list::node& node) noexcept
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

		node* head_ = nullptr;
};

}//CoAP

#endif /* COAP_TE_INTERNAL_LIST_HPP__ */
