/**
 * @file checks.hpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-11-22
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef COAP_TE_MESSAGE_OPTIONS_CHECKS_HPP_
#define COAP_TE_MESSAGE_OPTIONS_CHECKS_HPP_

#include <initializer_list>

#include "coap-te/core/error.hpp"
#include "coap-te/message/options/config.hpp"

namespace coap_te {
namespace message {
namespace options {

/**
 * @brief Type to define what kinds of checks should be
 * done to options
 * 
 * All options have requirements of order, type, length and others.
 * This type defines which of this requirements should be checked
 * when data is been serialized to send, or when parsing when
 * received.
 * 
 * @tparam CheckSequence true to check order of options
 * @tparam CheckFormat   true to check if options is the correct format to the option
 * @tparam CheckLength   true to check the data length of the option
 */
template<bool CheckSequence,
         bool CheckFormat,
         bool CheckLength>
struct check_type {
  static constexpr bool sequence    = CheckSequence;
  static constexpr bool format      = CheckFormat;
  static constexpr bool length      = CheckLength;

  /** Checks if any of the checks should be made */
  static bool constexpr
  check_any() noexcept {
    return sequence || format || length;
  }
};

/** Convenint type to check all requirements
 */
using check_all = check_type<true, true , true>;
/** Convenint type to not check any requirements
 */
using check_none = check_type<false, false, false>;
/** Convenint type to check only sequence requirement
 */
using check_sequence = check_type<true, false, false>;
/** Convenint type to check only format requirement
 */
using check_format = check_type<false, true, false>;
/** Convenint type to check only length requirement
 */
using check_length = check_type<false, false, true>;


/**
 * @brief Make all the checks requested
 * 
 * @tparam CheckOptions Defines which checks should be done
 * @param before The option number before this
 * @param op The current option to be checked
 * @param type The type that was passed
 * @param opt_length The length of the option passed
 * @return coap_te::error_code error code returned
 * @retval coap_te::errc::no_protocol_option Invalid option (not found)
 * @retval coap_te::errc::protocol_error Sequence option error
 * @retval coap_te::errc::wrong_protocol_type Wrong type sent with option
 * @retval coap_te::errc::argument_out_of_domain Argument length out of bound
 */
template<typename CheckOptions>
coap_te::error_code
check(number_type before,
      number_type op,
      format type,
      std::size_t opt_length) noexcept;

/**
 * @brief Make all the checks requested
 * 
 * See @ref check
 * 
 * @param types list of types to be checked
 */
template<typename CheckOptions>
coap_te::error_code
check(number_type before,
      number_type op,
      const std::initializer_list<format>& types,
      std::size_t opt_length) noexcept;

}  // namespace options
}  // namespace message
}  // namespace coap_te

#include "impl/checks.ipp"

#endif  // COAP_TE_MESSAGE_OPTIONS_CHECKS_HPP_
