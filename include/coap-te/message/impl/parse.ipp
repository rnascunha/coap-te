/**
 * @file parse.ipp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-13
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef COAP_TE_MESSAGE_IMPL_PARSE_IPP_
#define COAP_TE_MESSAGE_IMPL_PARSE_IPP_

namespace coap_te {
namespace message {

template<typename ConstBuffer,
         typename Message>
std::size_t parse(const ConstBuffer&, //input,
                  Message&,
                  std::error_code&) noexcept {
  return 0;
}

}  // namespace message 
}  // namespace coap_te

#endif  // COAP_TE_MESSAGE_IMPL_PARSE_IPP_