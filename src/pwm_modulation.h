#ifndef PWM_MODULATION_H_
#define PWM_MODULATION_H_

#include "led.h"

extern void led_pwm_pedal_modulation_init(led_t* l);
extern void led_pwm_pedal_modulation_task(uint8_t pedal_midi_value);
extern void led_pwm_pedal_modulation_enable();
extern void led_pwm_pedal_modulation_disable();

#endif //PWM_MODULATION_H_