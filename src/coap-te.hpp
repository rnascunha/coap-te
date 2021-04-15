#ifndef COAP_TE_MAIN_HPP__
#define COAP_TE_MAIN_HPP__

//Port
#include "port/port.hpp"

//Defines
#include "defines/defaults.hpp"

//Message
#include "message/types.hpp"
#include "message/codes.hpp"
#include "message/options/functions.hpp"
#include "message/options/functions2.hpp"
#include "message/options/options.hpp"
#include "message/options/parser.hpp"
#include "message/serialize.hpp"
#include "message/factory.hpp"
#include "message/parser.hpp"
#include "message/message_id.hpp"
#if COAP_TE_RELIABLE_CONNECTION == 1
#include "message/reliable/serialize.hpp"
#include "message/reliable/factory.hpp"
#include "message/reliable/parser.hpp"
#endif /* COAP_TE_RELIABLE_CONNECTION == 1 */
#if COAP_TE_OPTION_NO_RESPONSE == 1
#include "message/options/no_response.hpp"
#endif /* COAP_TE_OPTION_NO_RESPONSE == 1 */

//Resource
#include "resource/types.hpp"
#include "resource/resource.hpp"
#include "resource/node.hpp"
#include "resource/discovery.hpp"

//URI
#include "uri/types.hpp"
#include "uri/compose.hpp"
#include "uri/decompose.hpp"

//Transmission
#include "transmission/types.hpp"
#include "transmission/functions.hpp"
#include "transmission/request.hpp"
#include "transmission/response.hpp"
#include "transmission/transaction_list.hpp"
#include "transmission/transaction.hpp"
#include "transmission/engine.hpp"
#if COAP_TE_RELIABLE_CONNECTION == 1
#include "transmission/reliable/types.hpp"
#include "transmission/reliable/functions.hpp"
#include "transmission/reliable/request.hpp"
#include "transmission/reliable/response.hpp"
#include "transmission/reliable/containers/transaction.hpp"
#include "transmission/reliable/containers/transaction_list_empty.hpp"
#include "transmission/reliable/containers/transaction_list.hpp"
#include "transmission/reliable/containers/connection.hpp"
#include "transmission/reliable/containers/connection_list_empty.hpp"
#include "transmission/reliable/containers/connection_list.hpp"
#include "transmission/reliable/engine_client.hpp"
#include "transmission/reliable/engine_server.hpp"
#endif /* COAP_TE_RELIABLE_CONNECTION == 1 */

#if COAP_TE_OBSERVABLE_RESOURCE == 1
#include "observe/types.hpp"
#include "observe/functions.hpp"
#include "observe/observer.hpp"
#include "observe/list.hpp"
#endif /* COAP_TE_OBSERVABLE_RESOURCE == 1 */

#endif /* COAP_TE_MAIN_HPP__ */
