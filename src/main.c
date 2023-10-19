#include <hardware/flash.h>
#include <hardware/gpio.h>
#include <pico/bootrom.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "button.h"
#include "exp_pedal.h"
#include "flash.hpp"
#include "led.h"
#include "midi.h"
#include "pwm_modulation.h"
#include "tusb.h"

#define LED_GPIO 10
#define BUTTON_GPIO 13
#define PEDAL_GPIO 26
#define PEDAL_ADC_INPUT 0

#define BLINK_TIME_MS_CALIBRATION_1 500
#define BLINK_TIME_MS_CALIBRATION_2 100
#define BLINK_TIME_MS_USB_NOT_CONNECTED 2000

button_t button;
led_t led;

void read_midi_task(void);
void on_button_action(button_interaction_type interaction);

/*------------- MAIN -------------*/
int main(void) {
  // rescue option to enter bootsel
  const uint32_t btn = BUTTON_GPIO;
  gpio_init(btn);
  gpio_set_dir(btn, GPIO_IN);
  gpio_pull_up(btn);
  sleep_ms(250);
  if (!gpio_get(btn))
    reset_usb_boot(0, 0);

  // led init
  led.gpio = LED_GPIO;
  led_init(&led);
  led_blink(&led, BLINK_TIME_MS_USB_NOT_CONNECTED);
  led_pwm_pedal_modulation_init(&led);

  // button init
  button.gpio = BUTTON_GPIO;
  button_init(&button, on_button_action);

  // adc pedal init
  setup_exp_pedal(PEDAL_GPIO, PEDAL_ADC_INPUT);

  // read pedal calibration from flash
  flash_read(&expression_pedal_calibration, sizeof(expression_pedal_calibration));

  // init device stack on configured roothub port
  tud_init(BOARD_TUD_RHPORT);

  turn_on_pedal();

  while (1) {
    tud_task();
    read_midi_task();
    expression_pedal_task();
    button_task(&button);
    led_task(&led);
    led_pwm_pedal_modulation_task(get_current_pos_midi());
  }

  return 0;
}

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// // Invoked when device is mounted
void tud_mount_cb(void) {
  led_off(&led);
  led_pwm_pedal_modulation_enable(&led);
}

// // Invoked when device is unmounted
void tud_umount_cb(void) { led_blink(&led, BLINK_TIME_MS_USB_NOT_CONNECTED); }

// // Invoked when usb bus is suspended
// // remote_wakeup_en : if host allow us  to perform remote wakeup
// // Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en) {
  (void)remote_wakeup_en;
  led_off(&led);
  led_pwm_pedal_modulation_disable(&led);
}

// // Invoked when usb bus is resumed
void tud_resume_cb(void) {
  led_off(&led);
  led_pwm_pedal_modulation_enable(&led);
}

//--------------------------------------------------------------------+
// MIDI Task
//--------------------------------------------------------------------+

void read_midi_task(void) {
  // The MIDI interface always creates input and output port/jack descriptors
  // regardless of these being used or not. Therefore incoming traffic should be
  // read (possibly just discarded) to avoid the sender blocking in IO
  uint8_t packet[4];
  while (tud_midi_available())
    tud_midi_packet_read(packet);
}

void on_button_action(button_interaction_type interaction) {
  switch (interaction) {
    case PRESS:
      if (expression_pedal_get_mode() == CALIBRATION_RANGE) {
        expression_pedal_set_mode(CALIBRATION_NOISE);
        led_blink(&led, BLINK_TIME_MS_CALIBRATION_2);

      } else if (expression_pedal_get_mode() == CALIBRATION_NOISE) {
        expression_pedal_set_mode(NORMAL);
        led_off(&led);

        // save calibration into flash
        flash_write(&expression_pedal_calibration, sizeof(expression_pedal_calibration));

        led_pwm_pedal_modulation_enable(&led);
      }

      break;

    case RELEASE:

      break;

    case LONG_PRESS:
      led_pwm_pedal_modulation_disable(&led);
      led_blink(&led, BLINK_TIME_MS_CALIBRATION_1);
      expression_pedal_set_mode(CALIBRATION_RANGE);

      break;

    default:
      break;
  }
}
