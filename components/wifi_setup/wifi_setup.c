#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include "wifi_setup.h"
#include "wifi_constants.h"

static const char *TAG = "WIFI_SETUP";

static EventGroupHandle_t s_wifi_event_group;

int s_retry_num = 0;

void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < ESP_STA_MAXIMUM_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "Retry to connect to the AP");
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG,"Connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "Got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

void initialise_wifi(void)
{
	static bool initialized = false;

    esp_log_level_set("wifi", ESP_LOG_WARN);
	if (initialized) {
		return;
	}

    s_wifi_event_group = xEventGroupCreate();

	ESP_ERROR_CHECK(esp_netif_init());
	ESP_ERROR_CHECK(esp_event_loop_create_default());

	esp_netif_t *ap_netif = esp_netif_create_default_wifi_ap();
	assert(ap_netif);

	esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
	assert(sta_netif);

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK( esp_wifi_init(&cfg) );

	ESP_ERROR_CHECK( esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &wifi_event_handler, NULL) );
	ESP_ERROR_CHECK( esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL) );

	ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
	ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_NULL) );
	ESP_ERROR_CHECK( esp_wifi_start() );

	initialized = true;
}

bool wifi_ap_sta(int timeout_ms)
{
	//Configuracion del los datos del punto de acceso AP
	wifi_config_t ap_config = { 0 };
	strcpy((char *)ap_config.ap.ssid,ESP_WIFI_AP_SSID);
	strcpy((char *)ap_config.ap.password, ESP_WIFI_AP_PASS);
	ap_config.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;
	ap_config.ap.ssid_len = strlen(ESP_WIFI_AP_SSID);
	ap_config.ap.max_connection = ESP_WIFI_AP_MAX_STA_CONN;
	ap_config.ap.channel = ESP_WIFI_AP_CHANNEL;

	if (strlen(ESP_WIFI_AP_PASS) == 0) {
		ap_config.ap.authmode = WIFI_AUTH_OPEN;
	}

	//Configuracion de los datos de la conexion Wi-Fi a la STA
	wifi_config_t sta_config = { 0 };
	strcpy((char *)sta_config.sta.ssid, ESP_WIFI_STA_SSID);
	strcpy((char *)sta_config.sta.password, ESP_WIFI_STA_PASS);

	//Creacion del punto de acceso AP con la nueva configuracion
	ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_APSTA) );
	ESP_ERROR_CHECK( esp_wifi_set_config(ESP_IF_WIFI_AP, &ap_config) );
	ESP_ERROR_CHECK( esp_wifi_set_config(ESP_IF_WIFI_STA, &sta_config) );
	ESP_ERROR_CHECK( esp_wifi_start() );
	ESP_LOGI(TAG, "WIFI_MODE_AP started. SSID:%s password:%s channel:%d", ESP_WIFI_AP_SSID, ESP_WIFI_AP_PASS, ESP_WIFI_AP_CHANNEL);

	//Conexion como STA a la red Wi-Fi del punto de acceso indicado
	ESP_ERROR_CHECK( esp_wifi_connect() );
	int bits = xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT, pdFALSE, pdTRUE, timeout_ms / portTICK_PERIOD_MS);
	ESP_LOGI(TAG, "bits=%x", bits);
	if (bits) {
		ESP_LOGI(TAG, "WIFI_MODE_STA connected. SSID:%s password:%s", ESP_WIFI_STA_SSID, ESP_WIFI_STA_PASS);
	} else {
		ESP_LOGI(TAG, "WIFI_MODE_STA can't connected. SSID:%s password:%s", ESP_WIFI_STA_SSID, ESP_WIFI_STA_PASS);
	}
	
	return (bits & WIFI_CONNECTED_BIT) != 0;
}

void start_wifi_ap_sta(void)
{
    initialise_wifi();
    wifi_ap_sta(ESP_STA_MAXIMUM_RETRY*1000);
}