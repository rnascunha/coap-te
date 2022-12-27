/**
 * @file message_serialize_parse.cpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-26
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <gtest/gtest.h>

#include <cstring>

#include "list_types.hpp"


template<typename Option,
         typename OptionListSerialize,
         typename TokenTypeSerialize,
         typename OptionListParse,
         typaname TokenTypeParse>
constexpr void
serialize_parse_message(msg::type tp, msg::code co,
                  msg::message_id mid,
                  const coap_te::const_buffer& tk,
                  const std::initializer_list<Option>& opt_list,
                  const coap_te::const_buffer& pl) noexcept {
  using request = msg::message<OptionListSerialize, TokenTypeSerialize>;
  using response = msg::message<OptionListParse, TokenTypeParse>;

  std::vector<Option> storage(opt_list);

  request req(tp, co, tk);
  for (auto& op : storage) {
    req.add_option(op);
  }
  req.payload(pl);

  // Checks construct message
  EXPECT_EQ(opt_list.size(), req.count_options());

  std::uint8_t data[100];

  coap_te::mutable_buffer bufs{data};
  coap_te::error_code ecs;
  auto sizes = msg::serialize(req, mid, bufs, ecs);
  EXPECT_FALSE(ecs);

  coap_te::const_buffer bufp{data, sizes};
  response resp;
  coap_te::error_code ecp;
  auto sizep = msg::parse(bufp, resp, ecp);
  EXPECT_FALSE(ecp);
  EXPECT_EQ(sizes, sizep);
  // Header
  EXPECT_EQ(tp, resp.get_type());
  EXPECT_EQ(co, resp.get_code());
  EXPECT_EQ(mid, resp.mid());
  EXPECT_EQ(msg::clamp_token_size(tk.size()), resp.token().size());
  EXPECT_EQ(0, std::memcmp(resp.token().data(), tk.data(),
                            resp.token().size()));
  // Option
  EXPECT_EQ(opt_list.size(), resp.count_options());
  auto it = resp.begin();
  msg::options::number prev = msg::options::number::invalid;
  for (auto& op0 : req) {
    auto op = coap_te::core::forward_second_if_pair(op0);
    auto op2 = coap_te::core::forward_second_if_pair(*it++);
    EXPECT_EQ(op, op2);
    EXPECT_EQ(op.option_number(), op2.option_number());
    EXPECT_EQ(op.data_size(), op2.data_size());
    EXPECT_EQ(header_size(op, prev), header_size(op2, prev));
    EXPECT_EQ(size(op, prev), size(op2, prev));
    EXPECT_EQ(0, std::memcmp(op.data(), op2.data(), op.data_size()));
    prev = op.option_number();
  }
  // PAYLOAD
  EXPECT_EQ(pl.size(), resp.payload().size());
  EXPECT_EQ(0, std::memcmp(resp.payload().data(), pl.data(),
                            resp.payload().size()));
}

template<typename Option,
         typename OptionListSerialize,
         typename OptionList,
         typename ...OptionLists>
void call_all_parse(msg::type tp, msg::code co,
                    msg::message_id mid,
                    const coap_te::const_buffer& tk,
                    const std::initializer_list<Option>& opt_list,
                    const coap_te::const_buffer& pl) {
  serialize_parse_message<Option,
                          OptionListSerialize,
                          coap_te::const_buffer,
                          OptionList,
                          coap_te::const_buffer>(tp, co, mid, tk, opt_list, pl);

  if constexpr (sizeof...(OptionLists) > 0) {
    call_all_parse<Option,
                   OptionListSerialize,
                   OptionLists...>(tp, co, mid, tk, opt_list, pl);
  }
}

template<typename Option,
         typename OptionList,
         typename ...OptionLists>
void call_all(msg::type tp, msg::code co,
              msg::message_id mid,
              const coap_te::const_buffer& tk,
              const std::initializer_list<Option>& opt_list,
              const coap_te::const_buffer& pl) noexcept {
  call_all_parse<Option,
                 OptionList,
                 OptionsLists...>(tp, co, mid, tk, opt_list, pl);

  if constexpr (sizeof...(OptionLists) > 0) {
    call_all<Option,
             OptionLists...>(tp, co, mid, tk, opt_list, pl);
  }
}

int main() {
  call_all<opts::option>
}
