#include <algorithm>

#include "coap-te.hpp"

#include <gtest/gtest.h>

TEST(Options, Traits) {
    using namespace coap_te::message::options;

    //Check if definitions options container is sorted increseing (required)
    ASSERT_TRUE(std::is_sorted(std::begin(definitions), std::end(definitions)));

    //Check if find options definitions correctly
    EXPECT_EQ(get_definition(number::if_match), number::if_match);
    EXPECT_EQ(get_definition(number::uri_host), number::uri_host);
    EXPECT_EQ(get_definition(number::etag), number::etag);
    EXPECT_EQ(get_definition(number::if_none_match), number::if_none_match);
#if COAP_TE_MESSAGE_OPTION_OBSERVABLE_RESOURCE == 1
    EXPECT_EQ(get_definition(number::observe), number::observe);
#endif /* COAP_TE_MESSAGE_OPTION_OBSERVABLE_RESOURCE == 1 */
    EXPECT_EQ(get_definition(number::uri_port), number::uri_port);
    EXPECT_EQ(get_definition(number::location_path), number::location_path);
    EXPECT_EQ(get_definition(number::uri_path), number::uri_path);
    EXPECT_EQ(get_definition(number::content_format), number::content_format);
    EXPECT_EQ(get_definition(number::max_age), number::max_age);
    EXPECT_EQ(get_definition(number::uri_query), number::uri_query);
#if COAP_TE_MESSAGE_OPTION_HOP_LIMIT == 1
    EXPECT_EQ(get_definition(number::hop_limit), number::hop_limit);
#endif /* COAP_TE_MESSAGE_OPTION_HOP_LIMIT == 1 */
    EXPECT_EQ(get_definition(number::accept), number::accept);
    EXPECT_EQ(get_definition(number::location_query), number::location_query);
#if	COAP_TE_MESSAGE_OPTION_BLOCKWISE_TRANSFER == 1
    EXPECT_EQ(get_definition(number::block2), number::block2);
    EXPECT_EQ(get_definition(number::block1), number::block1);
    EXPECT_EQ(get_definition(number::size2), number::size2);
#endif /* COAP_TE_MESSAGE_OPTION_BLOCKWISE_TRANSFER == 1 */
    EXPECT_EQ(get_definition(number::proxy_uri), number::proxy_uri);
    EXPECT_EQ(get_definition(number::proxy_scheme), number::proxy_scheme);
    EXPECT_EQ(get_definition(number::size1), number::size1);
#if COAP_TE_MESSAGE_OPTION_NO_RESPONSE == 1
    EXPECT_EQ(get_definition(number::no_response), number::no_response);
#endif /* COAP_TE_MESSAGE_OPTION_NO_RESPONSE == 1 */

    //Invalid options must return invalid definition
    EXPECT_EQ(get_definition(static_cast<number>(500)), number::invalid);
    EXPECT_EQ(get_definition(static_cast<number>(1000)), number::invalid);
    EXPECT_EQ(get_definition(static_cast<number>(1500)), number::invalid);
}