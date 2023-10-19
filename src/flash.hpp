#ifndef FLASH_HPP_
#define FLASH_HPP_

#include <stddef.h> // size_t

#ifdef __cplusplus
extern "C" {  
#endif  

void flash_write(void* data, size_t size);
void flash_read(void* data, size_t size);

#ifdef __cplusplus  
} // extern "C"  
#endif

#endif //FLASH_HPP_

