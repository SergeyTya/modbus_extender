#ifndef MCU_INIT_H_
#define MCU_INIT_H_

#include "stdint.h"
#include "stdbool.h"


void nvic_config(void);
void com_usart_init(void);
void gpio_init(void);
uint8_t hw_get_addres();

typedef enum {
    
    USART_TYPE_IN = 0,
    USART_TYPE_OUT = 1

}USART_TYPE;

#endif
