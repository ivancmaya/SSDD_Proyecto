#ifndef WIFI_SETUP_
#define WIFI_SETUP_

#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"

/*
    Funcion para manejar los eventos derivados de la conexion Wi-Fi
*/
void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);

/*
    Funcion para crear la conexión Wi-Fi predeterminada
*/
void initialise_wifi(void);

/*
    Funcion para crear el punto de acceso AP y conectarse a la red Wi-Fi de un punto de
    acceso con una configuracion determinada
*/
bool wifi_ap_sta(int timeout_ms);

/*
    Funcion para iniciar la conexion Wi-Fi y el punto de acceso indicados en la configuracion
*/
void start_wifi_ap_sta();

#endif