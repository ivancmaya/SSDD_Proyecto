#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

#include "wifi_setup.h"
#include "mqtt_setup.h"
#include "dht11.h"

void app_main(void)
{
    start_wifi_ap_sta();
    
    esp_mqtt_client_handle_t client = mqtt_client_start();

    while(1)
    {
        struct dht11_reading data_dht = DHT11_read();

        mqtt_publish_data(client, "sensors/temperatura", data_dht.temperature);
        mqtt_publish_data(client, "sensors/humedad", data_dht.humidity);

        vTaskDelay(30000/portTICK_PERIOD_MS);
    }
}
