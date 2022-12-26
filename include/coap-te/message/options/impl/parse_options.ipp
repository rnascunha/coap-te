/**
 * @file parse_options.ipp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-16
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef COAP_TE_MESSAGE_OPTIONS_IMPL_PARSE_OPTIONS_IPP_
#define COAP_TE_MESSAGE_OPTIONS_IMPL_PARSE_OPTIONS_IPP_

#include <utility>      // std::move

#include "coap-te/core/traits.hpp"
#include "coap-te/core/const_buffer.hpp"
#include "coap-te/core/sorted_list.hpp"
#include "coap-te/message/options/option.hpp"
#include "coap-te/message/options/option_list_func.hpp"
#include "coap-te/message/options/vector_options.hpp"
#include "coap-te/message/options/traits.hpp"

namespace coap_te {
namespace message {
namespace options {

namespace detail {

template<typename Option>
std::size_t
parse_options(const coap_te::const_buffer& buf,
              vector_options<Option>& list,               // NOLINT
              coap_te::error_code& ec) noexcept {     // NOLINT
  auto [size, ecs] = option_list_size(buf);
  if (ecs) {
    ec = ecs;
    return 0;
  }

  list = {buf.data(), size};

  return size;
}

}  // namespace detail

template<typename OptionList>
std::size_t
parse_options(const coap_te::const_buffer& buf,
              OptionList& list,                           // NOLINT
              coap_te::error_code& ec) noexcept {         // NOLINT
  if constexpr (is_option_vector_v<OptionList>) {
    return detail::parse_options(buf, list, ec);
  } else {
    using Option = coap_te::core::value_type_if_pair_t<
                        typename OptionList::value_type>;
    vector_options<Option> list_v;
    auto size = detail::parse_options(buf, list_v, ec);
    if (ec) {
      return size;
    }

    for (auto op : list_v) {
      insert(list, std::move(op));
    }
    return size;
  }
}

}  // namespace options
}  // namespace message
}  // namespace coap_te

#endif  // COAP_TE_MESSAGE_OPTIONS_IMPL_PARSE_OPTIONS_IPP_
