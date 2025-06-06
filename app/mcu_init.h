#ifndef MCU_INIT_H_
#define MCU_INIT_H_

#include "stdint.h"
#include "stdbool.h"


void nvic_config(void);
void com_usart_init(void);
void gpio_init(void);
uint8_t hw_get_addres();



typedef struct  {
    
    const uint32_t RS_USART;
    const uint32_t usart_adr_tx;
    const uint32_t usart_adr_rx;
    const dma_channel_enum TX_DMA;
    const dma_channel_enum RX_DMA;
    const uint32_t rde_port;
    const uint32_t rde_pin;
    uint8_t rtxbuff[256];
    
}TypeDef_usart;

void txDMA_set_state(bool state, int32_t cnt, TypeDef_usart *  usart);
void rxDMA_set_state(bool state, TypeDef_usart *  usart);
void setRDEstate(TypeDef_usart *  usart, bool state);

extern TypeDef_usart RS485_in_u0;
extern TypeDef_usart RS485_out_u1;

 void usart_dma_config(TypeDef_usart *);

#endif
