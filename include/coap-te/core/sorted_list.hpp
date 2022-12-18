/**
 * @file sorted_list.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef COAP_TE_CORE_SORTED_LIST_HPP_
#define COAP_TE_CORE_SORTED_LIST_HPP_

#include <list>
#include <algorithm>
#include <utility>

#include "coap-te/core/traits.hpp"

namespace coap_te {
namespace core {

template<typename T>
class sorted_list {
 public:
  using value_type = T;
  using pointer = T*;
  using const_pointer = const T*;
  using reference = T&;

  using Container = std::list<T>;
  using iterator = typename Container::iterator;
  using const_iterator = typename Container::const_iterator;

  sorted_list() = default;
  sorted_list(std::initializer_list<T> list) {
    for (auto& t : list) {
      insert(t);
    }
  }

  template<typename Iterator>
  sorted_list(Iterator begin, Iterator end) {
    while (begin != end) {
      insert(*begin++);
    }
  }

  sorted_list&
  insert(const T& i) noexcept {
    auto it = std::upper_bound(v_.begin(), v_.end(), i);
    v_.insert(it, i);

    return *this;
  }

  /**
   * @brief Clear list
   */
  void clear() noexcept { v_.clear(); }

  /**
   * @brief Checks if list is empty
   */
  [[nodiscard]] bool
  empty() const noexcept {
    return v_.empty();
  }

  /** Return the size of the list;
   */
  [[nodiscard]] std::size_t
  size() const noexcept {
    return v_.size();
  }

  template<typename V>
  bool
  remove(const V& value, std::size_t index = 0) noexcept {
    static_assert(is_equal_comparable_v<T, V> &&
                  is_less_comparable_v<T, V>,
                  "Types must be comparable");

    auto it = get(value, index);
    if (it == v_.end())
      return false;

    v_.erase(it);
    return true;
  }

  void
  pop_front() noexcept {
    v_.pop_front();
  }

  template<typename V>
  [[nodiscard]] typename Container::iterator
  get(const V& value, std::size_t index = 0) noexcept {
    static_assert(is_equal_comparable_v<T, V> &&
                  is_less_comparable_v<T, V>,
                  "Types must be comparable");

    auto it = std::lower_bound(v_.begin(), v_.end(), value);
    if (it == v_.end() || !(*it == value))
      return v_.end();

    while (it != v_.end() && *it == value && index != 0) {
      ++it;
      --index;
    }

    if (index != 0 || !(*it == value))
      return v_.end();

    return it;
  }

  [[nodiscard]] reference
  front() noexcept {
    return *v_.begin();
  }

  // Iterator interface

  typename Container::iterator
  begin() noexcept {
    return v_.begin();
  }

  typename Container::iterator
  end() noexcept {
    return v_.end();
  }

  typename Container::const_iterator
  begin() const noexcept {
    return v_.begin();
  }

  typename Container::const_iterator
  end() const noexcept {
    return v_.end();
  }

 private:
    Container v_;
};

}  // namespace core
}  // namespace coap_te

#endif  // COAP_TE_CORE_SORTED_LIST_HPP_
