#include "exp_pedal.h"

#include <hardware/adc.h>
#include <tusb.h>

static uint8_t midi_channel = 0;
static const uint8_t midi_cable_num = 0;

static bool is_on = true;
exp_pedal_mode_t mode = NORMAL;

expression_pedal_calibration_t expression_pedal_calibration;

// variables nedded in calibration procedure
static uint16_t range_adc_min = UINT16_MAX;
static uint16_t range_adc_max = 0;
static uint16_t noise_adc_min = UINT16_MAX;
static uint16_t noise_adc_max = 0;

static uint8_t current_pos_midi;

uint8_t get_current_pos_midi() {
  return current_pos_midi;
}

void turn_off_pedal() {
  is_on = false;
}

void turn_on_pedal() {
  is_on = true;
}

bool is_pedal_on() {
  return is_on;
}

void setup_exp_pedal(uint8_t gpio_pin, uint8_t adc_input) {
  adc_init();
  adc_gpio_init(gpio_pin);
  adc_select_input(adc_input);
}

static uint16_t map(uint16_t x, uint16_t in_min, uint16_t in_max, uint16_t out_min,
                    uint16_t out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

bool read_pedal_value(uint8_t *midi_value_ptr) {
  static uint16_t vadc = 0;
  static int16_t prev_midi_value = 0;

  uint16_t adc = adc_read();

  if (vadc < adc - expression_pedal_calibration.adc_window)
    vadc += 1;
  else if (vadc > adc + expression_pedal_calibration.adc_window)
    vadc -= 1;

  if (vadc < adc - expression_pedal_calibration.adc_speed || vadc > adc + expression_pedal_calibration.adc_speed) {
    if (vadc < adc - expression_pedal_calibration.adc_speed)
      vadc += expression_pedal_calibration.adc_speed - 1;
    else if (vadc > adc + expression_pedal_calibration.adc_speed)
      vadc -= expression_pedal_calibration.adc_speed - 1;
  }

  if (adc <= expression_pedal_calibration.adc_window)
    vadc = 0;
  if (adc == 4096)
    vadc = 4096;

  int16_t midi_value = map(vadc, expression_pedal_calibration.adc_lowest_value, expression_pedal_calibration.adc_highest_value, 0, 127);

  if (midi_value > 127)
    midi_value = 127;

  if (midi_value < 0)
    midi_value = 0;

  if (midi_value != prev_midi_value) {
    prev_midi_value = midi_value;

    *midi_value_ptr = (uint8_t)midi_value;

    return true;
  }

  return false;
}

void expression_pedal_normal_task() {
  // static uint8_t pedal_pos = 0;

  if (read_pedal_value(&current_pos_midi)) {
    uint8_t cc[3] = {0xB9 | midi_channel, 11, current_pos_midi};
    tud_midi_stream_write(midi_cable_num, cc, 3);
  }
}

void expression_pedal_calibration_range_task() {
  if (!is_on)
    return;

  uint16_t pedal_pos = adc_read();

  if (pedal_pos < range_adc_min) {
    range_adc_min = pedal_pos;
  }

  if (pedal_pos > range_adc_max) {
    range_adc_max = pedal_pos;
  }
}

void expression_pedal_calibration_noise_task() {
  if (!is_on)
    return;

  uint16_t pedal_pos = adc_read();

  if (pedal_pos < noise_adc_min) {
    noise_adc_min = pedal_pos;
  }

  if (pedal_pos > noise_adc_max) {
    noise_adc_max = pedal_pos;
  }
}

void expresson_pedal_on_calibration_end() {
  uint16_t noise_value = noise_adc_max - noise_adc_min;

  // save calibration
  expression_pedal_calibration.adc_lowest_value = range_adc_min + noise_value;
  expression_pedal_calibration.adc_highest_value = range_adc_max - noise_value;
  expression_pedal_calibration.adc_window = noise_value;
  expression_pedal_calibration.adc_speed = noise_value;
}

void expression_pedal_on_calibration_start() {
  range_adc_min = UINT16_MAX;
  range_adc_max = 0;
  noise_adc_min = UINT16_MAX;
  noise_adc_max = 0;
}

void expression_pedal_task() {
  if (!is_on)
    return;

  switch (mode) {
    case NORMAL:
      expression_pedal_normal_task();
      break;

    case CALIBRATION_RANGE:
      expression_pedal_calibration_range_task();
      break;

    case CALIBRATION_NOISE:
      expression_pedal_calibration_noise_task();
      break;

    default:
      break;
  }
}

void expression_pedal_set_mode(exp_pedal_mode_t m) {
  if (mode == NORMAL && (m == CALIBRATION_NOISE || m == CALIBRATION_RANGE)) {
    expression_pedal_on_calibration_start();
  }

  if ((mode == CALIBRATION_NOISE || mode == CALIBRATION_RANGE) && m == NORMAL) {
    expresson_pedal_on_calibration_end();
  }

  mode = m;
}

extern exp_pedal_mode_t expression_pedal_get_mode() {
  return mode;
}