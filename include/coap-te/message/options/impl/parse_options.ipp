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

#include "coap-te/core/const_buffer.hpp"
#include "coap-te/core/sorted_list.hpp"
#include "coap-te/message/options/option.hpp"
#include "coap-te/message/options/option_list_func.hpp"
#include "coap-te/message/options/vector_options.hpp"

namespace coap_te {
namespace message {
namespace options {

template<>
std::size_t
parse_options(const coap_te::const_buffer& buf,
              vector_options& list,               // NOLINT
              std::error_code& ec) noexcept {     // NOLINT
  auto [size, ecs] = option_list_size(buf);
  if (ecs) {
    ec = ecs;
    return 0;
  }

  list = {buf.data(), size};

  return size;
}

template<>
std::size_t
parse_options(const coap_te::const_buffer& buf,
              coap_te::core::sorted_list<coap_te::message::options::option>& list,   // NOLINT
              std::error_code& ec) noexcept {             // NOLINT
  coap_te::message::options::vector_options list_v;
  auto size = parse_options(buf, list_v, ec);
  if (ec) {
    return size;
  }

  for (auto op : list_v) {
    list.add(std::move(op));
  }
  return size;
}

template<typename OptionList>
std::size_t
parse_options(const coap_te::const_buffer& buf,
              OptionList& list,   // NOLINT
              std::error_code& ec) noexcept {             // NOLINT
  coap_te::message::options::vector_options list_v;
  auto size = parse_options(buf, list_v, ec);
  if (ec) {
    return size;
  }

  for (auto op : list_v) {
    list.push_back(op);
  }
  return size;
}

}  // namespace options 
}  // namespace message
}  // namespace coap-te

#endif  // COAP_TE_MESSAGE_OPTIONS_IMPL_PARSE_OPTIONS_IPP_