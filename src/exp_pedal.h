#ifndef _EXP_PEDAL_H_
#define _EXP_PEDAL_H_

#include <stdbool.h>
#include <stdint.h>

typedef enum {
  NORMAL,
  CALIBRATION_RANGE,
  CALIBRATION_NOISE
} exp_pedal_mode_t;

typedef struct {
  uint16_t adc_lowest_value;
  uint16_t adc_highest_value;
  uint16_t adc_window;
  uint16_t adc_speed;
} expression_pedal_calibration_t;

extern expression_pedal_calibration_t expression_pedal_calibration;

extern void setup_exp_pedal(uint8_t gpio_pin, uint8_t adc_input);
extern bool read_pedal_value(uint8_t *midi_value_ptr);
extern void expression_pedal_task();
extern void turn_off_pedal();
extern void turn_on_pedal();
extern bool is_pedal_on();
extern void expression_pedal_set_mode(exp_pedal_mode_t m);
extern exp_pedal_mode_t expression_pedal_get_mode();
extern uint8_t get_current_pos_midi();

#endif