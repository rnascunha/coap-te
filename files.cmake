
set(SRC_DIR ./src/coap-te)
set(SRC_DIR_MESSAGE ${SRC_DIR}/message)
set(SRC_DIR_TRANSMISSION ${SRC_DIR}/transmission)
set(SRC_DIR_DEBUG ${SRC_DIR}/debug)
set(SRC_DIR_INTERNAL ${SRC_DIR}/internal)
set(SRC_DIR_PORT ${SRC_DIR}/port)
set(SRC_DIR_URI ${SRC_DIR}/uri)
set(SRC_DIR_RESOURCE ${SRC_DIR}/resource)
set(SRC_DIR_OBSERVE ${SRC_DIR}/observe)

set(MAIN_SRC ${SRC_DIR}/error.cpp
				${SRC_DIR_URI}/compose.cpp
				${SRC_DIR_URI}/decompose.cpp
				${SRC_DIR_INTERNAL}/helper.cpp
				${SRC_DIR_INTERNAL}/decoder.cpp
				${SRC_DIR_MESSAGE}/options/options.cpp
				${SRC_DIR_MESSAGE}/options/template_class.cpp
				${SRC_DIR_MESSAGE}/options/no_response.cpp
				${SRC_DIR_MESSAGE}/message_id.cpp
				${SRC_DIR_MESSAGE}/types.cpp
				${SRC_DIR_MESSAGE}/codes.cpp
				${SRC_DIR_MESSAGE}/parser.cpp
				${SRC_DIR_MESSAGE}/serialize.cpp
				${SRC_DIR_MESSAGE}/reliable/serialize.cpp
				${SRC_DIR_MESSAGE}/reliable/parser.cpp
				${SRC_DIR_TRANSMISSION}/functions.cpp
				${SRC_DIR_DEBUG}/helper.cpp
				${SRC_DIR_DEBUG}/output_string.cpp
				${SRC_DIR_DEBUG}/print_message.cpp
				${SRC_DIR_DEBUG}/print_resource.cpp
				${SRC_DIR_DEBUG}/print_uri.cpp
				${SRC_DIR_RESOURCE}/link_format.cpp
				${SRC_DIR_OBSERVE}/functions.cpp
				)

set(SRC_PORT_POSIX ${SRC_DIR_PORT}/posix/functions.cpp)
set(SRC_PORT_ESP_MESH ${SRC_DIR_PORT}/esp_mesh/function.cpp)

set(MAIN_INCLUDE_DIRS ./src ./third/tree_trunks/src/include)

set(JSON_SUPPORT_DIR	${SRC_DIR}/helper/json)
set(SRC_JSON_SUPPORT	${JSON_SUPPORT_DIR}/convert.cpp
						${JSON_SUPPORT_DIR}/util.cpp
						)
