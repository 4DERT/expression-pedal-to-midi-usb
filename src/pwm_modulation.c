#include "pwm_modulation.h"

#include <stdbool.h>
#include <stdint.h>

static bool is_enable = false;
static led_t* led;

// ------ gamma  RED   = 2.5
const uint16_t gamma_correction[] = {
    0, 1, 5, 11, 19, 30, 45, 63, 85, 110, 140, 174, 213, 257, 305, 359, 417, 482,
    551, 627, 708, 796, 889, 989, 1096, 1209, 1328, 1455, 1588, 1729, 1876, 2032, 2194, 2364, 2542, 2727,
    2921, 3122, 3332, 3549, 3775, 4010, 4253, 4504, 4765, 5034, 5312, 5599, 5895, 6201, 6515, 6840, 7173, 7516,
    7869, 8232, 8604, 8987, 9379, 9781, 10194, 10617, 11050, 11494, 11949, 12413, 12889, 13375, 13873, 14381, 14900, 15430,
    15971, 16524, 17088, 17663, 18250, 18848, 19458, 20080, 20713, 21359, 22016, 22685, 23366, 24059, 24765, 25483, 26213, 26955,
    27711, 28478, 29258, 30051, 30857, 31675, 32507, 33351, 34208, 35079, 35962, 36859, 37769, 38692, 39629, 40580, 41543, 42521,
    43512, 44517, 45536, 46568, 47615, 48675, 49750, 50838, 51941, 53058, 54189, 55335, 56495, 57669, 58858, 60062, 61280, 62513,
    63761, 65024};

void led_pwm_pedal_modulation_init(led_t* l) {
  led = l;
}

void led_pwm_pedal_modulation_task(uint8_t pedal_midi_value) {
  if (is_enable) {
    led_pwm(led, gamma_correction[pedal_midi_value]);
  }
}

void led_pwm_pedal_modulation_enable() {
  is_enable = true;
}

void led_pwm_pedal_modulation_disable() {
  is_enable = false;
}
