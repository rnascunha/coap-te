/**
 * @file sorted_no_alloc_list.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief Provides a container where all nodes are inserted
 * in order and no allocation is needed
 * @version 0.1
 * @date 2022-12-03
 * 
 * @copyright Copyright (c) 2022
 * 
 * As RFC7252 requests that all options must be inserted in
 * increase order, this container aims to be used to hold
 * all options in order to be serialized. Also, to prevent
 * the need of dynamic allocation, the options must be nodes
 * of the container, that will contain all the necessary structure
 * main the container.
 * 
 */
#ifndef COAP_TE_CORE_SORTED_NO_ALLOC_LIST_HPP_
#define COAP_TE_CORE_SORTED_NO_ALLOC_LIST_HPP_

#include <utility>

#include "coap-te/core/traits.hpp"

namespace coap_te {
namespace core {

/**
 * @brief Container that provides no need to dynamic
 * allocation and holds all values in order.
 * 
 * To maintain a stable ordered structure, type T requires 
 * to be defined operators '==' and '<'. The container is
 * implemented as a single linked list.
 * 
 * All instances inserted of equal value are append as the
 * last instance of same value, maintaining a stable order.
 * 
 * @tparam T type of the container
 */
template<typename T>
class sorted_no_alloc_list {
 public:
  static_assert(is_equal_comparable_v<T> &&
                is_less_comparable_v<T>,
                "T must be '<' and '==' comparable");

  using value_type = T;
  using pointer = T*;
  using const_pointer = const T*;
  using reference = T&;

  /**
   * @brief Type node that holds the value and the
   * pointer to the next element.
   * 
   * Is the node of the linked list. As is a no allocator list,
   * to use this container you must declare values not of type T,
   * but of type node<T>. The lifetime of the values is responsability
   * of the user.
   * 
   */
  struct node {
    using value_type = T;
    T     value;

    /**
     * @brief Construct a new node object
     * 
     * @tparam Args Argument types to contruct T
     * @param args values to construct T. The values are
     * forward to T constructor.
     */
    template<typename ...Args>
    node(Args&&... args) :    //NOLINT
      value(std::forward<Args>(args)...) {}

    /**
     * @brief Equality operator
     * 
     * @tparam V Type to be comparable. Must be comparable to T
     * @param rhs Value to compare
     */
    template<typename V>
    [[nodiscard]] bool
    operator==(const V& rhs) const noexcept {
      static_assert(is_equal_comparable_v<T, V>,
                    "Types must be equal comparable");
      return value == rhs;
    }

    [[nodiscard]] bool
    operator==(const node& rhs) const noexcept {
      return value == rhs.value;
    }

    /**
     * @brief Less operator
     * 
     * @tparam V Type to be comparable. Must be less comparable to T.
     * @param rhs Value to compare
     */
    template<typename V>
    [[nodiscard]] bool
    operator<(const V& rhs) const noexcept {
      static_assert(is_less_comparable_v<T, V>,
                    "Types must be less comparable");
      return value < rhs;
    }

    [[nodiscard]] bool
    operator<(const node& rhs) const noexcept {
      return value < rhs.value;
    }

    [[nodiscard]] bool
    operator<=(const node& rhs) const noexcept {
      return *this == rhs.value || *this < rhs.value;
    }

   private:
      friend class sorted_no_alloc_list;
      node* next_ = nullptr;
  };

  /**
   * @brief Iterator used to iterate the list
   */
  class iterator {
   public:
    using value_type = sorted_no_alloc_list::value_type;
    using pointer = sorted_no_alloc_list::value_type*;
    using reference = sorted_no_alloc_list::value_type&;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::forward_iterator_tag;

    iterator() = default;
    iterator(node& n) noexcept   // NOLINT
      : ptr_(&n) {}

    iterator& operator++() noexcept {
      if (ptr_ == nullptr)
        return *this;
      ptr_ = ptr_->next_;
      return *this;
    }

    iterator operator++(int) noexcept {
      if (ptr_ == nullptr)
        return iterator{};
      auto temp = iterator(*ptr_);
      ptr_ = ptr_->next_;
      return temp;
    }

    reference operator*() noexcept {
      return ptr_->value;
    }

    pointer operator->() noexcept {
      return &ptr_->value;
    }

    bool operator==(iterator const& other) const noexcept {
      return ptr_ == other.ptr_;
    }

    bool operator!=(iterator const& other) const noexcept {
      return !(*this == other);
    }

    operator bool() const noexcept {
      return ptr_ != nullptr;
    }

   private:
    friend class sorted_no_alloc_list;
    node* ptr_ = nullptr;
  };

  sorted_no_alloc_list() = default;

  template<typename Iterator>
  sorted_no_alloc_list(Iterator begin, Iterator end) {
    while (begin != end) {
      add(*begin++);
    }
  }

  /**
   * @brief Clear list
   */
  void clear() noexcept { head_ = nullptr; }

  /**
   * @brief Checks if list is empty
   */
  [[nodiscard]] bool
  empty() const noexcept {
    return head_ == nullptr;
  }

  /** Return the size of the list;
  */
  [[nodiscard]] std::size_t
  size() const noexcept {
    node* n = head_;
    std::size_t size = 0;
    while (n != nullptr) {
      ++size;
      n = n->next_;
    }
    return size;
  }

  /**
   * @brief Adds a new element to the list.
   * 
   * The element is inserted in a stable order away
   * 
   * @param n Element to be inserted
   * @return sorted_no_alloc_list& Returns this list
   */
  sorted_no_alloc_list&
  add(node& n) noexcept {    // NOLINT
    node* curr = head_;
    node* prev = nullptr;
    while (curr != nullptr && *curr <= n) {
      prev = curr;
      curr = curr->next_;
    }

    if (prev != nullptr) {
      prev->next_ = &n;
    } else {
      head_ = &n;
    }
    n.next_ = curr;

    return *this;
  }

  /**
   * @brief Removes a element based on the value and index
   * 
   * If no value be found, or doesn't have values equal to index,
   * the end iterator is returned.
   * 
   * @tparam V Type to be compared
   * @param value value to compare
   * @param index Index of the value
   * @return iterator iterator containing the value or the end
   * iterator
   */
  template<typename V>
  iterator
  remove(const V& value, std::size_t index = 0) noexcept {
    static_assert(is_equal_comparable_v<T, V> &&
                  is_less_comparable_v<T, V>,
                  "Types must be comparable");

    for (auto it = begin(), prev = end();
         it != end();
         prev = it++) {
      if (*it < value) {
        continue;
      }
      if (*it == value) {
        if (index == 0) {
          if (prev != end()) {
            prev.ptr_->next_ = it.ptr_->next_;
          } else {
            head_ = it.ptr_->next_;
          }
          it.ptr_->next_ = nullptr;
          return it;
        }
        --index;
        continue;
      }
      break;
    }
    return end();
  }

  /**
   * @brief Removes and return the first element
   * 
   * If no element is present, returns the end iterator;
   * 
   * @return iterator The element remove
   */
  iterator
  pop_front() noexcept {
    node* node = head_;
    if (head_ != nullptr) {
      head_ = head_->next_;
      node->next_ = nullptr;
    }
    return iterator(*node);
  }

  /**
   * @brief Returns element based on value and index
   * 
   * If no value or index not present, return the end
   * iterator.
   * 
   * @tparam V type to be compared
   * @param value value to compare
   * @param index index of the value
   * @return iterator The iterator of the value
   */
  template<typename V>
  [[nodiscard]] iterator
  get(const V& value, std::size_t index = 0) noexcept {
    static_assert(is_equal_comparable_v<T, V> &&
                  is_less_comparable_v<T, V>,
                  "Types must be comparable");

    for (auto it = begin(); it != end(); ++it) {
      if (*it < value) {
        continue;
      }
      if (*it == value) {
        if (index == 0)
          return it;
        --index;
        continue;
      }
      break;
    }
    return end();
  }

  /**
   * @brief Returns first value of list
   * 
   * If no value is presented returns end iterator
   * 
   * @return iterator First value of the list
   */
  [[nodiscard]] iterator
  front() noexcept {
    if (head_ == nullptr)
      return iterator{};

    return iterator(*head_);
  }

  // Iterator interface

  iterator begin() noexcept {
    return iterator(*head_);
  }

  iterator end() noexcept {
    return iterator{};
  }

  iterator begin() const noexcept {
    return iterator(*head_);
  }

  iterator end() const noexcept {
    return iterator{};
  }

 private:
  node*   head_ = nullptr;
};

}  // namespace core
}  // namespace coap_te

#endif  // COAP_TE_CORE_SORTED_NO_ALLOC_LIST_HPP_
