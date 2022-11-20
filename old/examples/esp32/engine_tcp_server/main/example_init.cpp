#include "example_init.hpp"

#include "nvs_flash.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_err.h"
#include "esp_wifi.h"
#include "esp_netif_types.h"

static int s_retry_num = 0;

#define EXAMPLE_WIFI_SSID 		CONFIG_ROUTER_SSID		//"xuxu_beleza_2_4G"
#define EXAMPLE_WIFI_PASS		CONFIG_ROUTER_PASSWD		//"cunha321"
#define MAXIMUM_RETRY  			4

#define TAG		"HELPER_INIT"

static bool is_connected = false;

static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        if (s_retry_num < MAXIMUM_RETRY)
        {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        }
        else
        {
            ESP_LOGE(TAG, "Fail to connect!");
        }
        ESP_LOGI(TAG,"connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        is_connected = true;
    }
}

void wifi_stack_init()
{
	/**
	 * Basic wifi initializations
	 */
	nvs_flash_init();

	esp_netif_init();

	esp_event_loop_create_default();
	esp_netif_create_default_wifi_sta();

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	esp_wifi_init(&cfg);

	esp_event_handler_instance_t instance_any_id;
	esp_event_handler_instance_t instance_got_ip;
	esp_event_handler_instance_register(WIFI_EVENT,
										ESP_EVENT_ANY_ID,
										&event_handler,
										NULL,
										&instance_any_id);
	esp_event_handler_instance_register(IP_EVENT,
										IP_EVENT_STA_GOT_IP,
										&event_handler,
										NULL,
										&instance_got_ip);

	wifi_config_t wifi_config = {
		.sta = {
			EXAMPLE_WIFI_SSID,		//ssid
			EXAMPLE_WIFI_PASS,		//password
			WIFI_FAST_SCAN,			//scan_method
			0,						//bssid_set
			{},						//bssid
			0,						//channel
			0,						//listen_interval
			WIFI_CONNECT_AP_BY_SIGNAL,//sort_method
			{						//threshold
				0,					//rssi
				static_cast<wifi_auth_mode_t>(CONFIG_AP_AUTHMODE),//WIFI_AUTH_WPA2_PSK,	//authmode
			},
			{						//pmf_cfg
				true,				//capable
				false				//required
			},
			0,						//rm_enabled
			0, 						//btm_enabled
			0,						//reserved
		},
	};
	esp_wifi_set_mode(WIFI_MODE_STA);
	esp_wifi_set_config(WIFI_IF_STA, &wifi_config);

	esp_wifi_start();

	while(!is_connected)
	{
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}
