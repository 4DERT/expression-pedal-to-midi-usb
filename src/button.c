#include "button.h"

#include <hardware/gpio.h>
#include <hardware/timer.h>
#include <pico/stdlib.h>

static bool repeating_timer_callback(struct repeating_timer* t) {
  button_t* button = (button_t*)(t->user_data);

  if (button->_timer_1)
    button->_timer_1--;

  if (button->_timer_2)
    button->_timer_2--;

  return true;
}

void button_init(button_t* button, button_callback_t callback) {
  gpio_init(button->gpio);
  gpio_set_dir(button->gpio, GPIO_IN);

#if BUTTON_PULL == 1
  gpio_pull_up(button->gpio);
#elif BUTTON_PULL == 2
  gpio_pull_down(button->gpio)
#endif

  button->_callback = callback;

  add_repeating_timer_ms(BUTTON_SOFT_TIMER_DURATION_MS, repeating_timer_callback, button, &button->_timer);
}

void button_task(button_t* button) {
  register bool is_pressed = (gpio_get(button->gpio) == BUTTON_GPIO_VALUE_WHEN_PRESS);

  if (is_pressed && !button->_timer_1 && button->_state == _IDLE) {
    button->_state = _DEBOUNCE;
    button->_timer_1 = BUTTON_DEBOUNCE_TIME_MS / BUTTON_SOFT_TIMER_DURATION_MS;
    button->_timer_2 = BUTTON_LONG_PRESS_TIME_MS / BUTTON_SOFT_TIMER_DURATION_MS;
    button->_long_press_flag = true;

    // On press action
    if (button->_callback)
      button->_callback(PRESS);

  } else if (button->_timer_1 && button->_state == _DEBOUNCE) {
    button->_state = _PRESS;

    // On debounce action

  } else if (!is_pressed && !button->_timer_1 && button->_state == _PRESS) {
    button->_state = _IDLE;
    button->_timer_1 = BUTTON_DEBOUNCE_TIME_MS / BUTTON_SOFT_TIMER_DURATION_MS;
    button->_long_press_flag = false;

    // On release action
    if (button->_callback)
      button->_callback(RELEASE);

  } else if (!button->_timer_2 && button->_long_press_flag) {
    button->_long_press_flag = false;

    // On long press action
    if (button->_callback)
      button->_callback(LONG_PRESS);
  }
}