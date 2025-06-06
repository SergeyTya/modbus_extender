#include <string.h>

#include "gd32f3x0.h"
#include "gd32f3x0_libopt.h"

#include "mcu_init.h"

#include "extender.h"


extern uint8_t addr_from_hw;

void systick_disable();




static void setRS485State(RS485States state, int tx_len);

/**
 *  \brief      This function handles USART0_IRQn interrupt request.
 *              Data received from global master. If CRC is correct, extended and send
 *              frame via OUTPUT RS485 TX.
 */
void USART0_IRQHandler(void)
{
     NVIC_ClearPendingIRQ(USART0_IRQn);
    if (RESET != usart_interrupt_flag_get(USART0, USART_INT_FLAG_RT)) {
        usart_interrupt_flag_clear(USART0, USART_INT_FLAG_RT);

        // get rx count from dma
        int rx_count = 255U - (dma_transfer_number_get(RS485_in_u0.RX_DMA));
 

        // check frame
        if (extend(RS485_in_u0.rtxbuff, rx_count, addr_from_hw, 1) == true) {

            memcpy(RS485_out_u1.rtxbuff, RS485_in_u0.rtxbuff, rx_count);
            
            // send frame to slave
            setRS485State(RS485_OUT_TX_ENABLE, rx_count);

        }
        else {
            // frame error
            setRS485State(RS485_ERROR, 0);
            systick_clksource_set(SYSTICK_CLKSOURCE_HCLK_DIV8);
            SysTick_Config((uint32_t)5e6);
            
        }
        memset( RS485_in_u0.rtxbuff, 0, 256U);
    }
}

/**
 *  \brief      This function handles DMA_Channel3_4n interrupt request.
 *              Transitions of extended frame from global master to slave performed
 *              via OUTPUT RS485 TX, switching OUTPUT RS485 to RX.
 */
void DMA_Channel3_4_IRQHandler(void) {
    // TODO systick wdg
    
    if (RESET != dma_interrupt_flag_get(RS485_out_u1.TX_DMA, DMA_INT_FLAG_FTF)) { 
        // Frame sent, wait for response from slave
        setRS485State(RS485_OUT_RX_ENABLE, 0);
        // Start SysTick WDG
        systick_clksource_set(SYSTICK_CLKSOURCE_HCLK_DIV8);
        SysTick_Config((uint32_t)1e6);
    }

    dma_interrupt_flag_clear(DMA_CH3, DMA_INT_FLAG_G);
    NVIC_ClearPendingIRQ(DMA_Channel3_4_IRQn);
}

/**
 *  \brief      This function handles USART1_IRQn interrupt request.
 *              Response form slave via OUTPUT RS485 RX obtained.
 *              If frame CRC is ok enable Input RS485 TX and sendt frame to global master
 */
void USART1_RX(void)
{
    if (true) {
        // Stop SysTick WDG
        while(usart_flag_get(RS485_out_u1.RS_USART, USART_FLAG_BSY) != RESET);

        int rx_count = 255U - (dma_transfer_number_get(RS485_out_u1.RX_DMA));

        if (extend(RS485_out_u1.rtxbuff, rx_count, 1, addr_from_hw) == true) {

            memcpy(RS485_in_u0.rtxbuff, RS485_out_u1.rtxbuff, rx_count);
            // send frame to master
            setRS485State(RS485_IN_TX_ENABLE, rx_count);
            systick_disable();
        }
    }
}

/**
 *  \brief      This function handles DMA_Channel1_2n interrupt request.
 *              Finishing address change sequence, Response frame transmitted to global master.
 *              Input RS485 switched to RX.
 */
void DMA_Channel1_2_IRQHandler(void)
{
    if (RESET != dma_interrupt_flag_get(DMA_CH1, DMA_INT_FLAG_FTF)) {
        dma_interrupt_flag_clear(DMA_CH1, DMA_INT_FLAG_G); // tx 
        // Frame sent, wait for new frame from master
        setRS485State(RS485_IN_RX_ENABLE, 0);
    }
}
volatile RS485States rs485_state;

static void setRS485State(RS485States state, int tx_len) {

    while(usart_flag_get(RS485_out_u1.RS_USART, USART_FLAG_TC ) == RESET);
    while(usart_flag_get(RS485_out_u1.RS_USART, USART_FLAG_BSY) != RESET);
    while(usart_flag_get(RS485_in_u0.RS_USART, USART_FLAG_TC )  == RESET);
    while(usart_flag_get(RS485_in_u0.RS_USART, USART_FLAG_BSY)  != RESET);

    rs485_state = state;
    switch (state)
    {
    case RS485_IN_RX_ENABLE:  default:
        setRDEstate(&RS485_in_u0, false);
        setRDEstate(&RS485_out_u1, false);
        txDMA_set_state(false, tx_len, &RS485_in_u0);
        txDMA_set_state(false, tx_len, &RS485_out_u1);
        rxDMA_set_state(true, &RS485_in_u0);
        rxDMA_set_state(false, &RS485_out_u1);
        break;

    case RS485_IN_TX_ENABLE:
        setRDEstate(&RS485_in_u0, true);
        setRDEstate(&RS485_out_u1, false);
        txDMA_set_state(true, tx_len, &RS485_in_u0);
        txDMA_set_state(false, tx_len, &RS485_out_u1);
        rxDMA_set_state(false, &RS485_in_u0);
        rxDMA_set_state(false, &RS485_out_u1);
        break;

    case RS485_OUT_RX_ENABLE:
        setRDEstate(&RS485_in_u0, false);
        setRDEstate(&RS485_out_u1, false);
        txDMA_set_state(false, tx_len, &RS485_in_u0);
        txDMA_set_state(false, tx_len, &RS485_out_u1);
        rxDMA_set_state(false, &RS485_in_u0);
        rxDMA_set_state(true, &RS485_out_u1);
        break;

    case RS485_OUT_TX_ENABLE:
        setRDEstate(&RS485_in_u0, false);
        setRDEstate(&RS485_out_u1, true);
        txDMA_set_state(false, tx_len, &RS485_in_u0);
        txDMA_set_state(true, tx_len, &RS485_out_u1);
        rxDMA_set_state(false, &RS485_in_u0);
        rxDMA_set_state(false, &RS485_out_u1);
        break;
    }

}


int sys_cnt = 0;
void SysTick_Handler() {

    sys_cnt++;
    USART1_RX();
    if(sys_cnt>25){
    // reset IN RX DMA state
    setRS485State(RS485_IN_RX_ENABLE, 0);
    /* disable the systick timer */
    systick_disable();
    }
}

void systick_disable(){
        SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
        sys_cnt = 0;
}


