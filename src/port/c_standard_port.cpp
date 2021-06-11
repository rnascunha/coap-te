#include "port.hpp"
//#include <ctime>
#include <cstdlib>
#if COAP_TE_ESP_IDF_PLATAFORM == 1
#include <esp_system.h>
#else
#include <chrono>
#endif /* COAP_TE_ESP_IDF_PLATAFORM == 1 */

namespace CoAP{

time_t time() noexcept
{
#if COAP_TE_ESP_IDF_PLATAFORM == 1
	return esp_timer_get_time() / 1000;	//esp_timer_get_time returns us.
#else /* COAP_TE_ESP_IDF_PLATAFORM == 1 */
	return static_cast<time_t>(std::chrono::duration_cast<std::chrono::milliseconds>
    		(std::chrono::system_clock::now().time_since_epoch()).count());
#endif /* COAP_TE_ESP_IDF_PLATAFORM == 1 */
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
