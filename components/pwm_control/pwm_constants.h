#ifndef PWM_CONTROL_CONSTANTS_
#define PWM_CONTROL_CONSTANTS_

#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_OUTPUT_IO          (CONFIG_LEDC_OUTPUT_IO) // Pin GPIO de salida
#define LEDC_CHANNEL            LEDC_CHANNEL_0
#define LEDC_FREQUENCY          (5000) // Frecuencia en Hz
#define LEDC_DUTY_RES           LEDC_TIMER_13_BIT // Resolucion del ciclo de trabajo en 13 bits (0 - 8191)

#endif