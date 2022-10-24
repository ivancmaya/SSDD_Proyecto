#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "esp_system.h"
#include "esp_log.h"
#include "driver/gpio.h"


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "mqtt_client.h"

#include "mqtt_setup.h"
#include "pwm_control.h"

static const char *TAG = "MQTT_SETUP";

void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;

    switch ((esp_mqtt_event_id_t)event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            break;
        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED");
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED");
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED");
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            printf("DATA=%.*s\r\n", event->data_len, event->data);
            printf("%s", event->data);
            if(!strcmp(event->topic, "actuador/ventilador")) {
                if(!strncmp("2", event->data, 1)) {
                    pwm_control_fan(2);
                    vTaskDelay(100/portTICK_PERIOD_MS);
                }
                else if(!strncmp("1", event->data, 1)) {
                    pwm_control_fan(1);
                    vTaskDelay(100/portTICK_PERIOD_MS);
                }
                else if(!strncmp("0", event->data, 1)) {
                    pwm_control_fan(0);
                    vTaskDelay(100/portTICK_PERIOD_MS);
                }
            }
            else if(!strcmp(event->topic, "actuador/led1")) {
                if(!strncmp("true", event->data, 4)) {
                    gpio_set_level(CONFIG_LED_1, 1);
                    printf("True led1");
                    vTaskDelay(100/portTICK_PERIOD_MS);
                }
                else if(!strncmp("false", event->data, 5)) {
                    gpio_set_level(CONFIG_LED_1, 0);
                    printf("False led1");
                    vTaskDelay(100/portTICK_PERIOD_MS);
                }
            }
            else if(!strcmp(event->topic, "actuador/led2")) {
                if(!strncmp("true", event->data, 4)) {
                    gpio_set_level(CONFIG_LED_2, 1);
                    printf("True led2");
                    vTaskDelay(100/portTICK_PERIOD_MS);
                }
                else if(!strncmp("false", event->data, 5)) {
                    gpio_set_level(CONFIG_LED_2, 0);
                    printf("False led2");
                    vTaskDelay(100/portTICK_PERIOD_MS);
                }
            }
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            break;
        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
}

esp_mqtt_client_handle_t mqtt_client_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .uri = "mqtt://172.20.10.3",
        .port = 1883,
        .username = "user",
    };

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    esp_mqtt_client_start(client);

    return client;
}

void mqtt_publish_data(esp_mqtt_client_handle_t client, char * topic, char * data)
{
    esp_mqtt_client_publish(client, topic, data, 0, 1, 0);
}