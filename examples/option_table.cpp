/**
 * @file option_table.cpp
 * @author Rafael Cunha (rnascunha@gmail.com)
 * 
 * @brief Example to print options table like shown at RFC7252 and others
 * 
 * @version 0.1
 * @date 2022-11-17
 * 
 * @copyright Copyright (c) 2022
 */

#include <cstdio>       //std::printf
#include <iterator>     //std::begin, std::end
#include <algorithm>    //std::for_each

#include "coap-te.hpp"

int main() {
    //Namespace where all options are defined
    using namespace coap_te::message::options;

    //Drawing header
    std::printf("+-----+---+---+---+---+-----------------+---------+--------+\n");
    std::printf("| No. | C | U | N | R | Name            | Format  | Length |\n");
    std::printf("+-----+---+---+---+---+-----------------+---------+--------+\n");
    //Iterating options definitions
    std::for_each(std::begin(definitions), 
                std::end(definitions), 
                [](auto const& def) {
                    if (!def) return;
                    auto const& type = to_string(def.type);
                    std::printf("|%4u | %c | %c | %c | %c | %-15.*s | %-7.*s | %u-%-4u |\n",
                            static_cast<number_type>(def.op),
                            is_critical(def.op) ? 'x' : ' ',
                            is_unsafe_to_forward(def.op) ? 'x' : ' ',
                            is_unsafe_to_forward(def.op) ? '-' : (is_no_cache_key(def.op) ? 'x' : ' '),
                            def.repeatable ? 'x' : ' ',
                            static_cast<int>(def.name.size()), def.name.data(),
                            static_cast<int>(type.size()), type.data(),
                            def.length_min, 
                            def.length_max);
                });
    // Drawing footer
    std::printf("+-----+---+---+---+---+-----------------+---------+--------+\n");
    std::printf("     C=Critical, U=Unsafe, N=NoCachaKey, R=Repeatable\n");

    return EXIT_SUCCESS;
}