#ifndef MQTT_SETUP_
#define MQTT_SETUP_

#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "mqtt_client.h"

void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);

esp_mqtt_client_handle_t mqtt_client_start(void);

void mqtt_publish_data(esp_mqtt_client_handle_t client, char * topic, char * data);

#endif