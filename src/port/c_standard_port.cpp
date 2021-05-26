#include "port.hpp"
//#include <ctime>
#include <chrono>
#include <cstdlib>
#if COAP_TE_ESP_IDF_PLATAFORM == 1
#include <esp_system.h>
#endif /* COAP_TE_ESP_IDF_PLATAFORM == 1 */

namespace CoAP{

time_t time() noexcept
{
//	return std::time(NULL);
	return std::chrono::duration_cast<std::chrono::milliseconds>
    		(std::chrono::system_clock::now().time_since_epoch()).count();
}

unsigned random_generator() noexcept
{
#if COAP_TE_ESP_IDF_PLATAFORM == 1
	return esp_random();
#else /* COAP_TE_ESP_IDF_PLATAFORM == 1 */
	return std::rand();
#endif /* COAP_TE_ESP_IDF_PLATAFORM == 1 */
}

void init() noexcept
{
#if COAP_TE_PORT_POSIX == 1
	std::srand(static_cast<unsigned>(time()));
	CoAP::Port::POSIX::init();
#endif
}

}//CoAP
