/**
 * @file option_list_func.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-15
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef COAP_TE_MESSAGE_OPTIONS_OPTION_LIST_FUNC_HPP_
#define COAP_TE_MESSAGE_OPTIONS_OPTION_LIST_FUNC_HPP_

#include <utility>
#include <system_error>   // NOLINT

#include "coap-te/core/const_buffer.hpp"

namespace coap_te {
namespace message {
namespace options {

/**
 * @brief Counts the number of options at list
 * 
 * @tparam OptionList Option list type
 * @param list option list to count
 * @return std::size_t number of options
 */
template<typename OptionList>
[[nodiscard]] constexpr std::size_t
count(const OptionList& list) noexcept;

/**
 * @brief Calculate size of option list when seriliazed
 * 
 * @tparam OptionList Option list type
 * @param list list to calculate size
 * @return std::size_t size of option list
 */
template<typename OptionList>
[[nodiscard]] constexpr std::size_t
size(const OptionList& list) noexcept;

/**
 * @brief Calculates the size of the options in a buffer
 * 
 * @note the buffer argument must be the beginning of the message
 * options list
 * 
 * @param buf buffer to calculate
 * @return std::pair<std::size_t, std::error_code> the size calcuated 
 * or a error
 */
[[nodiscard]] std::pair<std::size_t, std::error_code>
option_list_size(const coap_te::const_buffer& buf) noexcept;

/**
 * @brief Inserts a option in a option container
 * 
 * This function is responsible to deal with the complexity
 * to add a option to the diferent kinds of containers that
 * are supported.
 * 
 * @tparam OptionList type of the container
 * @tparam Option type of option
 * @param op option to be inserted
 */
template<typename OptionList,
         typename Option>
void
insert(OptionList&, Option&& op) noexcept;

}  // namespace options
}  // namespace message
}  // namespace coap_te

#include "impl/option_list_func.ipp"

#endif  // COAP_TE_MESSAGE_OPTIONS_OPTION_LIST_FUNC_HPP_
