
#include <stdio.h>
//#include <stdbool.h>

#include "gd32f3x0.h"

#include "mbcrc.h"
#include "mcu_init.h"



uint8_t addr = 0;

bool proces_data(uint8_t * data, size_t len, uint8_t adr);

void main(){

    // nvic_config();
    // com_usart_init();
     gpio_init();

    // addr = hw_get_addres();

    int main_cnt = 0;

    while(1){
        main_cnt++;
        if(main_cnt > (int) 16e5){
            main_cnt = 0;
            gpio_bit_toggle(GPIOC, GPIO_PIN_13);
        }
    }
}


bool proces_data(uint8_t * data, size_t len, uint8_t adr){

    if(len < 4) return false;

    uint16_t crc_calc = usMBCRC16(data, len);
    uint16_t crc_fram = data[len-1] + data[len-2]*0xFFU;   

    if(crc_calc != crc_fram) return false;

    data[0] = adr;

    crc_calc = usMBCRC16(data, len);

    data[len-1] = crc_calc & 0xFF;
    data[len-2] = (crc_calc & 0xFF00)>>8;

    return true;
}

