
#include <stdio.h>
//#include <stdbool.h>

#include "gd32f3x0.h"

#include "mbcrc.h"
#include "mcu_init.h"
#include "extender.h"



uint8_t addr_from_hw = 1;

RS485States glob_rs485_state = RS485_OUT_RX_ENABLE;

void main(){

    nvic_config();
    com_usart_init();
    gpio_init();
    

    addr_from_hw = hw_get_addres();

    int main_cnt = 0;

    while(1){
        main_cnt++;
        if(main_cnt > (int) 16e5){
            main_cnt = 0;
            gpio_bit_toggle(GPIOC, GPIO_PIN_13);
        }
    }
}




