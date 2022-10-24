#ifndef PWM_CONTROL_
#define PWM_CONTROL_

/*
    Funcion para inicializar los valores para generar la senal PWM
*/
void pwm_init(void);

/*
    Funcion para controlar la velocidad del ventilador en funcion de la temperatura
*/
void pwm_control_fan(int estado);

#endif