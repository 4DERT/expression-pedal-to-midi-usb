#ifndef _MIDI_MIDI_H_
#define _MIDI_MIDI_H_

#include <stdint.h>

// Status Byte -> Message type
#define MIDI_MSG_TYPE_NOTE_OFF 0b000
#define MIDI_MSG_TYPE_NOTE_ON 0b001
#define MIDI_MSG_TYPE_POLYPHONIC_AFTERTOUCH 0b010
#define MIDI_MSG_TYPE_CONTROL_CHANGE 0b011
#define MIDI_MSG_TYPE_PROGRAM_CHANGE 0b100
#define MIDI_MSG_TYPE_CHANNEL_AFTERTOUCH 0b101
#define MIDI_MSG_TYPE_PITCH_BEND_CHANGE 0b110
#define MIDI_MSG_TYPE_SYSTEM_MESSAGE 0b111

extern const char* midi_msg_type_name[];
extern const char* midi_cin_type_name[];

typedef void (*mackie_text_cb_t)(char* text);

typedef struct
{
  union {
    struct
    {
      uint8_t code_index_number : 4;
      uint8_t cable_number : 4;
    } usb_byte_ctrl;
    uint8_t usb_byte;
  };

  union {
    struct
    {
      uint8_t channel : 4;
      uint8_t message_type : 3;
      uint8_t is_status_byte : 1;
    } status_byte_ctrl;
    uint8_t status_byte;
  };

  uint8_t data_byte_1;
  uint8_t data_byte_2;

} midi_usb_t;

#endif