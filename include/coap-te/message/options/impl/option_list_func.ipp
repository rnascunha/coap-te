/**
 * @file option_list_func.ipp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-15
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef COAP_TE_MESSAGE_OPTIONS_IMPL_OPTION_LIST_FUNC_HPP_
#define COAP_TE_MESSAGE_OPTIONS_IMPL_OPTION_LIST_FUNC_HPP_

#include <algorithm>
#include <type_traits>
#include <utility>
#include <vector>
#include <map>

#include "coap-te/message/options/config.hpp"
#include "coap-te/message/options/parse.hpp"
#include "coap-te/message/options/option.hpp"

namespace coap_te {
namespace message {
namespace options {

template<typename OptionList>
[[nodiscard]] constexpr std::size_t
count(const OptionList& list) noexcept {
  // static_assert(is_option_list_v<OptionList>, "Must be a option list");
  return list.size();
}

template<typename OptionList>
[[nodiscard]] constexpr std::size_t
size(const OptionList& list) noexcept {
  // static_assert(is_option_list_v<OptionList>, "Must be a option list");
  std::size_t s = 0;
  number prev = number::invalid;
  for (auto const& op : list) {
    s += op.size(prev);
    prev = op.option_number();
  }
  return s;
}

template<typename>
[[nodiscard]] std::size_t
size(const std::multimap<number, option>& list) noexcept {
  // static_assert(is_option_list_v<OptionList>, "Must be a option list");
  std::size_t s = 0;
  number prev = number::invalid;
  for (auto const& [_, op] : list) {
    s += op.size(prev);
    prev = op.option_number();
  }
  return s;
}

[[nodiscard]] std::pair<std::size_t, std::error_code>
option_list_size(
  const coap_te::const_buffer& buf) noexcept {   // NOLINT
  coap_te::const_buffer cbuf(buf);
  number_type prev = invalid;
  std::error_code ec;

  while (cbuf.size() != 0 && cbuf[0] != coap_te::message::payload_marker) {
    option op;
    parse(prev, cbuf, op, ec);
    if (ec) {
      return {0, ec};
    }
    prev = static_cast<number_type>(op.option_number());
  }
  return {coap_te::core::pointer_distance(buf.data(), cbuf.data()), ec};
}

template<typename OptionList,
         typename Option>
void
insert(OptionList& list, Option&& op) noexcept {
  if constexpr (std::is_same_v<std::vector<option>, OptionList> || 
                std::is_same_v<std::list<option>, OptionList>) {
    auto it = std::upper_bound(list.begin(), list.end(), op);
    list.insert(it, std::forward<Option>(op));
  } else if constexpr (std::is_same_v<std::multimap<number, option>, OptionList>) {
    list.insert({op.option_number(), std::forward<Option>(op)});
  } else
    list.insert(std::forward<Option>(op));
}

}  // namespace options 
}  // namespace message
}  // namespace coap_te

#endif // COAP_TE_MESSAGE_OPTIONS_IMPL_OPTION_LIST_FUNC_HPP_
