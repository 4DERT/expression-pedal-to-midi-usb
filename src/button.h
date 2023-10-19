#ifndef BUTTON_H_
#define BUTTON_H_

#include <pico/time.h>
#include <stdbool.h>
#include <stdint.h>

#define BUTTON_DEBOUNCE_TIME_MS 50
#define BUTTON_SOFT_TIMER_DURATION_MS 10  // time base of timer
#define BUTTON_LONG_PRESS_TIME_MS 3000

// Pull configuration for buttons (0 - none, 1 - pull-up, 2 - pull-down)
#define BUTTON_PULL 1

#define BUTTON_GPIO_VALUE_WHEN_PRESS 0  // logic 0 if button pressed

enum button_state_enum {
  _IDLE,
  _PRESS,
  _LONG_PRESS,
  _DEBOUNCE
};

typedef enum {
  PRESS,
  RELEASE,
  LONG_PRESS
} button_interaction_type;

// calback function prototype
typedef void (*button_callback_t)(button_interaction_type);

typedef struct {
  uint32_t gpio;

  // private
  uint16_t _timer_1;
  uint16_t _timer_2;
  bool _long_press_flag;
  enum button_state_enum _state;

  struct repeating_timer _timer;

  button_callback_t _callback;

} button_t;

void button_init(button_t* button, button_callback_t callback);
void button_task(button_t* button);

#endif  // BUTTON_H_