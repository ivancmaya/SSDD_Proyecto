#include <stdio.h>
#include "driver/ledc.h"
#include "esp_err.h"
#include "esp_log.h"

#include "pwm_control.h"
#include "pwm_constants.h"

static const char *TAG = "PWM_CONTROL";

void pwm_init(void)
{
    // Configurar los valores para el timer del PWM
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .timer_num        = LEDC_TIMER,
        .duty_resolution  = LEDC_DUTY_RES,
        .freq_hz          = LEDC_FREQUENCY,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Configurar los valores para el canal a usar por la senal PWM
    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = LEDC_OUTPUT_IO,
        .duty           = 0,
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}

void pwm_control_fan(int estado)
{
    //Si la temperatura interior es superior a 25ºC, el ventilador trabaja al 100%
    if (estado == 2) {
        ESP_ERROR_CHECK( ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 8190) );  //Ciclo de trabajo: 100%
        ESP_ERROR_CHECK( ledc_update_duty(LEDC_MODE, LEDC_CHANNEL) );
        ESP_LOGI(TAG, "Ventilador al 100");
    } 
    //Si la temperatura interior es superior a 20ºC, el ventilador trabaja al 50%
    else if (estado == 1) {
        ESP_ERROR_CHECK( ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 1023) );  //Ciclo de trabajo: 50%
        ESP_ERROR_CHECK( ledc_update_duty(LEDC_MODE, LEDC_CHANNEL) );
        ESP_LOGI(TAG, "Ventilador al 50");
    }
    //Si la temperatura interior es inferior a 20ºC, el ventilador se apaga
    else {
        ESP_ERROR_CHECK( ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 1) );  //Ciclo de trabajo: 0%
        ESP_ERROR_CHECK( ledc_update_duty(LEDC_MODE, LEDC_CHANNEL) );
        ESP_LOGI(TAG, "Ventilador al 0");
    }
}