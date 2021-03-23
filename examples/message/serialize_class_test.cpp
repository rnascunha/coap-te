/**
 * Test for the 'Serialize' class
 */
#include <cstdint>
#include <cstdio>

#include "coap-te.hpp"
#include "coap-te-debug.hpp"

/**
 * Auxiliar function
 */
static void exit_error(const char* what)
{
	printf("ERROR! %s\n", what);
	exit(EXIT_FAILURE);
}

int main()
{
	std::uint8_t buffer[512];
	CoAP::Message::Serialize serial(buffer, 512);

	/**
	 * Setting header
	 */
	if(!serial.header(CoAP::Message::type::confirmable, CoAP::Message::code::content, 0x1234))
		exit_error("header");

	std::printf("header [%lu]:\n", serial.used());
	CoAP::Debug::print_message_str(serial.get_message());

	/**
	 * Adding token
	 */
	if(!serial.token("abcde"))
		exit_error("token");

	std::printf("add token [%lu]:\n", serial.used());
	CoAP::Debug::print_message_str(serial.get_message());

	/**
	 * Adding payload
	 */
	if(!serial.payload("0123456789"))
		exit_error("token");

	std::printf("add payload [%lu]:\n", serial.used());
	CoAP::Debug::print_message_str(serial.get_message());

	/**
	 * extending token
	 */
	if(!serial.token("jklmnopq"))
		exit_error("token");

	std::printf("extending token [%lu]:\n", serial.used());
	CoAP::Debug::print_message_str(serial.get_message());

	/**
	 * shorting token
	 */
	if(!serial.token("rts"))
		exit_error("token");

	std::printf("shorting token [%lu]:\n", serial.used());
	CoAP::Debug::print_message_str(serial.get_message());

	/**
	 * removing token
	 */
	if(!serial.remove_token())
		exit_error("token");

	std::printf("removing token [%lu]:\n", serial.used());
	CoAP::Debug::print_message_str(serial.get_message());

	/**
	 * Adding token again
	 */
	if(!serial.token("abcde"))
		exit_error("token");

	std::printf("add token again [%lu]:\n", serial.used());
	CoAP::Debug::print_message_str(serial.get_message());

	/**
	 * shorting payload
	 */
	if(!serial.payload("98765"))
		exit_error("payload");

	std::printf("shorting payload [%lu]:\n", serial.used());
	CoAP::Debug::print_message_str(serial.get_message());

	/**
	 * Adding payload
	 */
	if(!serial.payload("ABCDEFGHIJ"))
		exit_error("payload");

	std::printf("adding payload [%lu]:\n", serial.used());
	CoAP::Debug::print_message_str(serial.get_message());

	/**
	 * removing payload
	 */
	if(!serial.remove_payload())
		exit_error("payload");

	std::printf("removing payload [%lu]:\n", serial.used());
	CoAP::Debug::print_message_str(serial.get_message());

	/**
	 * New header
	 */
	if(!serial.header(CoAP::Message::type::acknowledgment, CoAP::Message::code::post, 0x5678))
		exit_error("header");

	std::printf("header [%lu]:\n", serial.used());
	CoAP::Debug::print_message_str(serial.get_message());

	/**
	 * Adding payload
	 */
	if(!serial.payload("ABCDEFGHIJ"))
		exit_error("payload");

	std::printf("adding payload [%lu]:\n", serial.used());
	CoAP::Debug::print_message_str(serial.get_message());

	/**
	 * Adding option
	 */
	if(!serial.add_option(CoAP::Message::Option::option{CoAP::Message::content_format::text_plain}))
		exit_error("option");

	std::printf("adding option [%lu]:\n", serial.used());
	CoAP::Debug::print_message_str(serial.get_message());

	/**
	 * Adding option
	 */
	if(!serial.add_option(CoAP::Message::Option::option{CoAP::Message::Option::code::uri_path, "teste"}))
		exit_error("option");

	std::printf("adding option [%lu]:\n", serial.used());
	CoAP::Debug::print_message_str(serial.get_message());

	/**
	 * Adding option
	 */
	if(!serial.add_option(CoAP::Message::Option::option{CoAP::Message::Option::code::uri_path, "other"}))
		exit_error("option");

	std::printf("adding option [%lu]:\n", serial.used());
	CoAP::Debug::print_message_str(serial.get_message());

	/**
	 * Adding option
	 */
	if(!serial.add_option(CoAP::Message::Option::option{CoAP::Message::Option::code::proxy_scheme, "proxy_scheme"}))
		exit_error("option");

	std::printf("adding option [%lu]:\n", serial.used());
	CoAP::Debug::print_message_str(serial.get_message());

	/**
	 * Adding option
	 */
	if(!serial.add_option(CoAP::Message::Option::option{CoAP::Message::Option::code::proxy_uri, "proxy_uri"}))
		exit_error("option");

	std::printf("adding option [%lu]:\n", serial.used());
	CoAP::Debug::print_message_str(serial.get_message());

	/**
	 * Adding option
	 */
	unsigned size2 = 128;
	if(!serial.add_option(CoAP::Message::Option::option{CoAP::Message::Option::code::size2, size2}))
		exit_error("option");

	std::printf("adding option [%lu]:\n", serial.used());
	CoAP::Debug::print_message_str(serial.get_message());

	/**
	 * Adding option
	 */
	if(!serial.add_option(CoAP::Message::Option::option{CoAP::Message::Option::code::uri_path, "uno_mas"}))
		exit_error("option");

	std::printf("adding option [%lu]:\n", serial.used());
	CoAP::Debug::print_message_str(serial.get_message());

	/**
	 * Adding option
	 */
	if(!serial.add_option(CoAP::Message::Option::option{CoAP::Message::Option::code::if_none_match}))
		exit_error("option");

	std::printf("adding option [%lu]:\n", serial.used());
	CoAP::Debug::print_message_str(serial.get_message());

	/**
	 * Adding option
	 */
	if(!serial.add_option(CoAP::Message::Option::option{CoAP::Message::Option::code::uri_query, "key=value"}))
		exit_error("option");

	std::printf("adding option [%lu]:\n", serial.used());
	CoAP::Debug::print_message_str(serial.get_message());

	/**
	 * Adding option
	 */
	std::uint8_t match[] = {1,2,3,4};
	if(!serial.add_option(CoAP::Message::Option::option{CoAP::Message::Option::code::if_match, match, 4}))
		exit_error("option");

	std::printf("adding option [%lu]:\n", serial.used());
	CoAP::Debug::print_message_str(serial.get_message());

	/**
	 * Adding option
	 */
	unsigned size = 0;
	if(!serial.add_option(CoAP::Message::Option::option{CoAP::Message::Option::code::size1, size}))
		exit_error("option");

	std::printf("adding option [%lu]:\n", serial.used());
	CoAP::Debug::print_message_str(serial.get_message());

	/**
	 * Remove option
	 */
	if(!serial.remove_option(CoAP::Message::Option::code::size1))
		exit_error("option");

	std::printf("removing option [%lu]:\n", serial.used());
	CoAP::Debug::print_message_str(serial.get_message());

	/**
	 * Remove option
	 */
	if(!serial.remove_option(CoAP::Message::Option::code::uri_query))
		exit_error("option");

	std::printf("removing option [%lu]:\n", serial.used());
	CoAP::Debug::print_message_str(serial.get_message());

	/**
	 * Remove option
	 */
	if(!serial.remove_option(CoAP::Message::Option::code::uri_path))
		exit_error("option");

	std::printf("removing option [%lu]:\n", serial.used());
	CoAP::Debug::print_message_str(serial.get_message());

	/**
	 * Remove option
	 */
	if(!serial.remove_option(CoAP::Message::Option::code::if_match))
		exit_error("option");

	std::printf("removing option [%lu]:\n", serial.used());
	CoAP::Debug::print_message_str(serial.get_message());

	/**
	 * Remove option
	 */
	if(!serial.remove_option(CoAP::Message::Option::code::proxy_uri))
		exit_error("option");

	std::printf("removing option [%lu]:\n", serial.used());
	CoAP::Debug::print_message_str(serial.get_message());

	/**
	 * Remove option
	 */
	if(!serial.remove_option(CoAP::Message::Option::code::size2))
		exit_error("option");

	std::printf("removing option [%lu]:\n", serial.used());
	CoAP::Debug::print_message_str(serial.get_message());

	/**
	 * Remove option
	 */
	if(!serial.remove_option(CoAP::Message::Option::code::if_none_match))
		exit_error("option");

	std::printf("removing option [%lu]:\n", serial.used());
	CoAP::Debug::print_message_str(serial.get_message());

	/**
	 * Remove option
	 */
	if(!serial.remove_option(CoAP::Message::Option::code::content_format))
		exit_error("option");

	std::printf("removing option [%lu]:\n", serial.used());
	CoAP::Debug::print_message_str(serial.get_message());

	/**
	 * Remove option
	 */
	if(!serial.remove_option(CoAP::Message::Option::code::uri_path))
		exit_error("option");

	std::printf("removing option [%lu]:\n", serial.used());
	CoAP::Debug::print_message_str(serial.get_message());

	/**
	 * Remove option
	 */
	if(!serial.remove_option(CoAP::Message::Option::code::uri_path))
		exit_error("option");

	std::printf("removing option [%lu]:\n", serial.used());
	CoAP::Debug::print_message_str(serial.get_message());

	/**
	 * Remove option
	 */
	if(!serial.remove_option(CoAP::Message::Option::code::proxy_scheme))
		exit_error("option");

	std::printf("removing option [%lu]:\n", serial.used());
	CoAP::Debug::print_message_str(serial.get_message());

	return EXIT_SUCCESS;
}
