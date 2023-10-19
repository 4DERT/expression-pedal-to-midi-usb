#include "flash.hpp"

#include <hardware/flash.h>
#include <hardware/sync.h>

#include <cstring>

// Flash-based address of the last sector
constexpr uint32_t FLASH_TARGET_OFFSET(PICO_FLASH_SIZE_BYTES - FLASH_SECTOR_SIZE);

void flash_write(void* data, size_t size) {
  uint8_t* dataAsBytes = (uint8_t*)data;

  if (size > FLASH_PAGE_SIZE)
    return;

  uint32_t ints = save_and_disable_interrupts();

  flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);
  flash_range_program(FLASH_TARGET_OFFSET, dataAsBytes, size);

  restore_interrupts(ints);
}

void flash_read(void* data, size_t size) {
  // Read the area of memory that stores struct. remember to check CRC's
  // Assign what we read from flash to the struct

  const uint8_t* flash_target_contents = (const uint8_t*)(XIP_BASE + FLASH_TARGET_OFFSET);
  memcpy(data, flash_target_contents, size);
}
