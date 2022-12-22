#ifndef COAP_TE_MAIN_HPP__
#define COAP_TE_MAIN_HPP__

//Port
#include "coap-te/port/port.hpp"

//Defines
#include "coap-te/defines/defaults.hpp"

//Error
#include "coap-te/error.hpp"

//Message
#include "coap-te/message/types.hpp"
#include "coap-te/message/codes.hpp"
#include "coap-te/message/options/functions.hpp"
#include "coap-te/message/options/functions2.hpp"
#include "coap-te/message/options/options.hpp"
#include "coap-te/message/options/parser.hpp"
#include "coap-te/message/serialize.hpp"
#include "coap-te/message/factory.hpp"
#include "coap-te/message/parser.hpp"
#include "coap-te/message/message_id.hpp"
#if COAP_TE_ENABLE_STREAM_CONNECTION == 1
#include "coap-te/message/reliable/serialize.hpp"
#include "coap-te/message/reliable/factory.hpp"
#include "coap-te/message/reliable/parser.hpp"
#endif /* COAP_TE_ENABLE_STREAM_CONNECTION == 1 */
#if COAP_TE_OPTION_NO_RESPONSE == 1
#include "coap-te/message/options/no_response.hpp"
#endif /* COAP_TE_OPTION_NO_RESPONSE == 1 */

//Resource
#include "coap-te/resource/types.hpp"
#include "coap-te/resource/resource.hpp"
#include "coap-te/resource/node.hpp"
#include "coap-te/resource/discovery.hpp"

//URI
#include "coap-te/uri/types.hpp"
#include "coap-te/uri/compose.hpp"
#include "coap-te/uri/decompose.hpp"

//Transmission
#include "coap-te/transmission/types.hpp"
#include "coap-te/transmission/functions.hpp"
#include "coap-te/transmission/request.hpp"
#include "coap-te/transmission/response.hpp"
#include "coap-te/transmission/transaction_list.hpp"
#include "coap-te/transmission/transaction.hpp"
#include "coap-te/transmission/engine.hpp"
#if COAP_TE_ENABLE_STREAM_CONNECTION == 1
#include "coap-te/transmission/reliable/types.hpp"
#include "coap-te/transmission/reliable/functions.hpp"
#include "coap-te/transmission/reliable/request.hpp"
#include "coap-te/transmission/reliable/response.hpp"
#include "coap-te/transmission/reliable/containers/transaction.hpp"
#include "coap-te/transmission/reliable/containers/transaction_list_empty.hpp"
#include "coap-te/transmission/reliable/containers/transaction_list.hpp"
#include "coap-te/transmission/reliable/containers/connection.hpp"
#include "coap-te/transmission/reliable/containers/connection_list_empty.hpp"
#include "coap-te/transmission/reliable/containers/connection_list.hpp"
#include "coap-te/transmission/reliable/engine_client.hpp"
#include "coap-te/transmission/reliable/engine_server.hpp"
#endif /* COAP_TE_ENABLE_STREAM_CONNECTION == 1 */

#if COAP_TE_OBSERVABLE_RESOURCE == 1
#include "coap-te/observe/types.hpp"
#include "coap-te/observe/functions.hpp"
#include "coap-te/observe/observer.hpp"
#include "coap-te/observe/list.hpp"
#endif /* COAP_TE_OBSERVABLE_RESOURCE == 1 */

#if COAP_TE_JSON_HELPER == 1
#include "coap-te/helper/json/convert.hpp"
#endif /* COAP_TE_JSON_HELPER == 1 */

#endif /* COAP_TE_MAIN_HPP__ */
