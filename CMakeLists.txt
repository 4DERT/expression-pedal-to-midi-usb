cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)
project(pico_usb_midi_exp_pedal)
pico_sdk_init()
add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src/main.c
                                   ${CMAKE_CURRENT_LIST_DIR}/src/usb_descriptors.c
                                   ${CMAKE_CURRENT_LIST_DIR}/src/exp_pedal.c
                                   ${CMAKE_CURRENT_LIST_DIR}/src/button.c
                                   ${CMAKE_CURRENT_LIST_DIR}/src/led.c
                                   ${CMAKE_CURRENT_LIST_DIR}/src/flash.cpp
                                   ${CMAKE_CURRENT_LIST_DIR}/src/pwm_modulation.c
                                   )

# Make sure TinyUSB can find tusb_config.h
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src)

# In addition to pico_stdlib required for common PicoSDK functionality, add
# dependency on tinyusb_device for TinyUSB device support and tinyusb_board for
# the additional board support library used by the example
target_link_libraries(${PROJECT_NAME} PUBLIC pico_stdlib hardware_adc hardware_flash hardware_pwm tinyusb_device)

# Uncomment this line to enable fix for Errata RP2040-E5 (the fix requires use
# of GPIO 15) target_compile_definitions(picomidirouter PUBLIC
# PICO_RP2040_USB_DEVICE_ENUMERATION_FIX=1)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

pico_add_extra_outputs(${PROJECT_NAME})
