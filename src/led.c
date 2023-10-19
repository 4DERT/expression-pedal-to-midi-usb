#include "led.h"

#include <hardware/gpio.h>
#include <hardware/pwm.h>
#include <hardware/timer.h>

static enum gpio_function def_func;

static bool repeating_timer_callback(struct repeating_timer* t) {
  led_t* led = (led_t*)(t->user_data);

  if (led->_soft_timer)
    led->_soft_timer--;

  return true;
}

static void led_init_pwm(led_t* led) {
  gpio_set_function(led->gpio, GPIO_FUNC_PWM);

  pwm_set_wrap(led->_pwm_slice, _LED_PWM_SLICE);
}

static void led_disable_pwm(led_t* led) {
  if (led->state == LED_PWM){
    pwm_set_enabled(led->_pwm_slice, false);
    gpio_set_function(led->gpio, def_func);

  }
}

void led_init(led_t* led) {
  gpio_init(led->gpio);
  gpio_set_dir(led->gpio, GPIO_OUT);

  // find slice and channel for pwm
  led->_pwm_slice = pwm_gpio_to_slice_num(led->gpio);
  led->_pwm_channel = pwm_gpio_to_channel(led->gpio);

  def_func = gpio_get_function(led->gpio);

  pwm_set_enabled(led->_pwm_slice, false);

  led_off(led);

  add_repeating_timer_ms(_LED_SOFT_TIMER_TIME_BASE_MS, repeating_timer_callback, led, &led->_timer);
}

void led_on(led_t* led) {
  led_disable_pwm(led);
  led->state = LED_ON;
  gpio_put(led->gpio, _LED_ON);
}

void led_off(led_t* led) {
  led_disable_pwm(led);
  led->state = LED_OFF;
  gpio_put(led->gpio, _LED_OFF);
}

void led_toggle(led_t* led) {
  switch (led->state) {
    case LED_ON:
      led_off(led);
      break;

    case LED_OFF:
      led_on(led);
      break;

    default:
      break;
  }
}

void led_blink(led_t* led, uint16_t time_ms) {
  led_disable_pwm(led);
  led->state = LED_BLINK;
  led->_blink_duration_ms = time_ms;
}

void led_task(led_t* led) {
  if (led->state == LED_BLINK && !led->_soft_timer) {
    led->_soft_timer = led->_blink_duration_ms / _LED_SOFT_TIMER_TIME_BASE_MS;

    gpio_put(led->gpio, !gpio_get(led->gpio));
  }
}

extern void led_pwm(led_t* led, uint16_t duty) {
  if (led->state != LED_PWM) {
    led_init_pwm(led);
    pwm_set_enabled(led->_pwm_slice, true);
  }

  pwm_set_chan_level(led->_pwm_slice, led->_pwm_channel, duty);

  led->state = LED_PWM;
}