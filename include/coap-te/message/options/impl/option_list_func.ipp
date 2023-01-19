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
#ifndef COAP_TE_MESSAGE_OPTIONS_IMPL_OPTION_LIST_FUNC_IPP_
#define COAP_TE_MESSAGE_OPTIONS_IMPL_OPTION_LIST_FUNC_IPP_

#include <algorithm>
#include <type_traits>
#include <utility>

#include "coap-te/core/error.hpp"
#include "coap-te/core/traits.hpp"
#include "coap-te/message/options/config.hpp"
#include "coap-te/message/options/parse.hpp"
#include "coap-te/message/options/option.hpp"
#include "coap-te/message/options/traits.hpp"
#include "coap-te/message/options/option_func.hpp"
#include "coap-te/message/options/vector_options.hpp"

namespace coap_te {
namespace message {
namespace options {

template<typename OptionList>
[[nodiscard]] constexpr std::size_t
count(const OptionList& list) noexcept {
  static_assert(is_option_list_v<OptionList>, "Must be a option list");
  if constexpr (is_option_vector_v<OptionList>) {
    return list.count();
  } else {
    return list.size();
  }
}

template<typename OptionList>
[[nodiscard]] constexpr std::size_t
size(const OptionList& list) noexcept {
  static_assert(is_option_list_v<OptionList>, "Must be a option list");

  if constexpr (is_option_vector_v<OptionList>) {
    return list.size();
  } else {
    std::size_t s = 0;
    number prev = number::invalid;
    for (auto const& op : list) {
      s += size(coap_te::core::forward_second_if_pair(op), prev);
      prev = coap_te::core::forward_second_if_pair(op).option_number();
    }
    return s;
  }
}

[[nodiscard]] constexpr std::pair<std::size_t, coap_te::error_code>
option_list_size(
  const coap_te::const_buffer& buf) noexcept {   // NOLINT
  coap_te::const_buffer cbuf(buf);
  number_type prev = invalid;
  coap_te::error_code ec;

  while (cbuf.size() != 0 &&
         *static_cast<const uint8_t*>(cbuf.data()) !=
         coap_te::message::payload_marker) {
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
insert(OptionList& list, Option&& op) noexcept {      // NOLINT
  static_assert(coap_te::core::is_container_v<OptionList>,
                "Must be a container");
  static_assert(is_option_v<Option>,
                "Must be a option");

  using OptionListComp = coap_te::core::remove_cvref_t<OptionList>;

  if constexpr (coap_te::core::is_map_v<OptionListComp>) {
    list.insert({op.option_number(), std::forward<Option>(op)});
  } else if constexpr (coap_te::core::is_set_v<OptionListComp> ||       // NOLINT
                       coap_te::core::is_sorted_list<OptionListComp>::value) {
    list.insert(std::forward<Option>(op));
  } else {
    // inserting in a ordered way
    auto it = std::upper_bound(list.begin(), list.end(), op);
    list.insert(it,
                std::forward<Option>(op));
  }
}

}  // namespace options
}  // namespace message
}  // namespace coap_te

#endif  // COAP_TE_MESSAGE_OPTIONS_IMPL_OPTION_LIST_FUNC_IPP_

