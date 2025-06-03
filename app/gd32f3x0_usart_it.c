 #include <string.h>
 
 #include "gd32f3x0.h"
 #include "gd32f3x0_libopt.h"

 #include "mcu_init.h"

 #include "extender.h"


 extern int out_rx_wdg;


/*!
    \brief      this function handles USART0 interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void USART0_IRQHandler(void)
{
    if(RESET != usart_interrupt_flag_get(USART0, USART_INT_FLAG_RT)) {
        /* clear idle flag */
        usart_interrupt_flag_clear(USART0, USART_INT_FLAG_RT);
        /* toggle the LED2 */
        
        setRDEstate(&RS485_in_u0,false);

        int rx_count = 256 - (dma_transfer_number_get(RS485_in_u0.RX_DMA));
              
        if( extend(RS485_in_u0.rtxbuff, rx_count, 1) == true ){

            memcpy(RS485_out_u1.rtxbuff, RS485_in_u0.rtxbuff, rx_count);
            setRDEstate(&RS485_out_u1, true);
            txDMA_set_state(true,  rx_count, &RS485_out_u1);
            txDMA_set_state(false,        0, &RS485_in_u0);
            rxDMA_set_state(false, &RS485_in_u0);
            rxDMA_set_state(false, &RS485_out_u1);

        }else{
            rxDMA_set_state(true, &RS485_in_u0);
        }
    }
}

void DMA_Channel3_4_IRQHandler(void){
    // TODO systick wdg
    if(RESET != dma_interrupt_flag_get(DMA_CH3, DMA_INT_FLAG_FTF)) {
        dma_interrupt_flag_clear(DMA_CH3, DMA_INT_FLAG_G); // tx 
        setRDEstate(&RS485_out_u1, false);
        txDMA_set_state(false,  0, &RS485_out_u1);
        txDMA_set_state(false,  0, &RS485_in_u0);
        rxDMA_set_state(false, &RS485_in_u0);
        rxDMA_set_state(true , &RS485_out_u1);
    }
}

void USART1_IRQHandler(void)
{

       out_rx_wdg = 0;
       if(RESET != usart_interrupt_flag_get(USART1, USART_INT_FLAG_IDLE)) {
        /* clear idle flag */
        usart_interrupt_flag_clear(USART1, USART_INT_FLAG_IDLE);
        /* toggle the LED2 */
        
        int rx_count = 256 - (dma_transfer_number_get(RS485_out_u1.RX_DMA));
   
        if( extend(RS485_out_u1.rtxbuff, rx_count, 1) == true ){

            memcpy(RS485_in_u0.rtxbuff, RS485_out_u1.rtxbuff, rx_count);
            setRDEstate(&RS485_in_u0, true);
            txDMA_set_state(false,         0, &RS485_out_u1);
            txDMA_set_state( true,  rx_count, &RS485_in_u0);
            rxDMA_set_state(false, &RS485_in_u0);
            rxDMA_set_state(false, &RS485_out_u1);

        }else{
            rxDMA_set_state(true, &RS485_in_u0);
        }
    }
}


void DMA_Channel1_2_IRQHandler(void)
{
    if(RESET != dma_interrupt_flag_get(DMA_CH1, DMA_INT_FLAG_FTF)) {
        dma_interrupt_flag_clear(DMA_CH1, DMA_INT_FLAG_G); // tx 
        setRDEstate(&RS485_in_u0, false);
        txDMA_set_state(false,  0, &RS485_out_u1);
        txDMA_set_state(false,  0, &RS485_in_u0);
        rxDMA_set_state(true,   &RS485_in_u0);
        rxDMA_set_state(false , &RS485_out_u1);
    }
}


