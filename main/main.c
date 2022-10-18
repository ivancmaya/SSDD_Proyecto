#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include "wifi_setup.h"
#include "mqtt_setup.h"
#include "dht11.h"

void app_main(void)
{
    char* temperatura[10];
    char* humedad[10];
    //Inicializacion NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
    
    start_wifi_ap_sta();
    
    esp_mqtt_client_handle_t client = mqtt_client_start();

    while(1)
    {
        struct dht11_reading data_dht = DHT11_read();

        itoa(data_dht.temperature, temperatura, 10);
        itoa(data_dht.humidity, humedad, 10);

        mqtt_publish_data(client, "sensors/temperatura", temperatura);
        mqtt_publish_data(client, "sensors/humedad", humedad);

        vTaskDelay(30000/portTICK_PERIOD_MS);
    }
}
