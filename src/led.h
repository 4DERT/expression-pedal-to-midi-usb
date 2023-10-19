#ifndef LED_H_
#define LED_H_

#include <pico/time.h>
#include <stdbool.h>
#include <stdint.h>

#define _LED_ON 1
#define _LED_OFF 0
#define _LED_SOFT_TIMER_TIME_BASE_MS 10
#define _LED_PWM_SLICE 16384

typedef enum {
  LED_ON,
  LED_OFF,
  LED_BLINK,
  LED_PWM
} led_state_enum;

typedef struct {
  uint16_t gpio;
  led_state_enum state;

  // private
  uint16_t _soft_timer;
  struct repeating_timer _timer;
  uint16_t _blink_duration_ms;
  uint16_t _pwm_slice;
  uint16_t _pwm_channel;
  
} led_t;

extern void led_init(led_t* led);
extern void led_task(led_t* led);
extern void led_on(led_t* led);
extern void led_off(led_t* led);
extern void led_toggle(led_t* led);
extern void led_blink(led_t* led, uint16_t time_ms);
extern void led_pwm(led_t* led, uint16_t duty);

#endif  // LED_H_