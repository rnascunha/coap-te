/**
 * @file buffer_sequence.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-04
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef COAP_TE_CORE_BUFFER_SEQUENCE_HPP_
#define COAP_TE_CORE_BUFFER_SEQUENCE_HPP_

#include <tuple>

namespace coap_te {

template<typename ...Args>
class buffer_sequence {
 public:
  static_assert(sizeof...(Args) > 1, "It must exist at least one buffer");
 private:
  std::tuple<Args...> buffers_;
};

}  // namespace coap_te

#endif  // COAP_TE_CORE_BUFFER_SEQUENCE_HPP_
