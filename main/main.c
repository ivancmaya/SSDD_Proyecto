#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "nvs_flash.h"
#include "driver/gpio.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include "wifi_setup.h"
#include "mqtt_setup.h"
#include "pwm_control.h"
#include "dht11.h"

void app_main(void)
{
    //Buffers para los datos del sensor DHT11
    char* temperatura[10];
    char* humedad[10];

    //Inicializacion de los GPIO empleados
    gpio_reset_pin(CONFIG_LED_1);
    gpio_set_direction(CONFIG_LED_1, GPIO_MODE_OUTPUT);

    gpio_reset_pin(CONFIG_LED_2);
    gpio_set_direction(CONFIG_LED_2, GPIO_MODE_OUTPUT);

    gpio_reset_pin(CONFIG_DHT_11_GPIO);
    gpio_set_direction(CONFIG_DHT_11_GPIO, GPIO_MODE_INPUT);

    //Inicializacion NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    //Inicializacion sensor DHT11
    DHT11_init(CONFIG_DHT_11_GPIO);

    //Inicializacion señal PWM
    pwm_init();

    //Inicializacion conexion Wi-Fi
    start_wifi_ap_sta();
    
    //Inicializacion cliente MQTT
    esp_mqtt_client_handle_t client = mqtt_client_start();

    while(1)
    {
        //Lectura de los datos del sensor DHT11
        struct dht11_reading data_dht = DHT11_read();

        itoa(data_dht.temperature, temperatura, 10);
        itoa(data_dht.humidity, humedad, 10);

        //Publicacion de los datos de temperatura y humedad
        mqtt_publish_data(client, "sensors/temperatura", temperatura);
        mqtt_publish_data(client, "sensors/humedad", humedad);

        //Subscripcion a los topic de los actuadores
        int msg_id_1 = esp_mqtt_client_subscribe(client, "actuador/led1", 1);
        int msg_id_2 = esp_mqtt_client_subscribe(client, "actuador/led2", 1);
        int msg_id_3 = esp_mqtt_client_subscribe(client, "actuador/ventilador", 1);

        vTaskDelay(30000/portTICK_PERIOD_MS);
    }
}
