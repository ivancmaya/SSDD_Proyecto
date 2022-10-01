#ifndef WIFI_AP_STA_CONSTANTS_
#define WIFI_AP_STA_CONSTANTS_

//Datos de la conexion a la red Wi-Fi
#define ESP_WIFI_STA_SSID                  CONFIG_ESP_WIFI_STA_SSID
#define ESP_WIFI_STA_PASS                  CONFIG_ESP_WIFI_STA_PASSWORD
#define ESP_STA_MAXIMUM_RETRY              CONFIG_ESP_STA_MAXIMUM_RETRY

//Datos del punto de acceso
#define ESP_WIFI_AP_SSID                   CONFIG_ESP_WIFI_AP_SSID
#define ESP_WIFI_AP_PASS                   CONFIG_ESP_WIFI_AP_PASSWORD
#define ESP_WIFI_AP_MAX_STA_CONN           CONFIG_AP_MAX_STA_CONN
#define ESP_WIFI_AP_CHANNEL                CONFIG_ESP_WIFI_AP_CHANNEL

//Flags para comprobar conexion y errores
#define WIFI_CONNECTED_BIT                 BIT0
#define WIFI_FAIL_BIT                      BIT1

#endif