
#include <stdio.h>
//#include <stdbool.h>

#include "gd32f3x0.h"

#include "mbcrc.h"
#include "mcu_init.h"



uint8_t addr = 0;

int out_rx_wdg = 0;

bool proces_data(uint8_t * data, size_t len, uint8_t adr);

void main(){

    nvic_config();
    com_usart_init();
    gpio_init();

    addr = hw_get_addres();

    int main_cnt = 0;

    while(1){
        main_cnt++;
        if(main_cnt > (int) 16e5){
            main_cnt = 0;
            gpio_bit_toggle(GPIOC, GPIO_PIN_13);
        }
    }
}




