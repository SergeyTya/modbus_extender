
#include "gd32f3x0.h"
#include "mcu_init.h"


#define GD32_CONGIG_PIN_AS_AF(PORT, AF, PIN)                                      \
    {                                                                             \
        gpio_mode_set((PORT), GPIO_MODE_AF, GPIO_PUPD_NONE, (PIN));               \
        gpio_output_options_set((PORT), GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, (PIN)); \
        gpio_af_set((PORT), AF, (PIN));                                           \
    }

#define GD32_CONGIG_PIN_AS_OUT(PORT, PIN)                                     \
    {                                                                         \
        gpio_mode_set(PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, PIN);           \
        gpio_output_options_set(PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, PIN); \
    }

#define GD32_CONGIG_PIN_AS_IN(PORT, PIN)                        \
  {                                                            \
        gpio_mode_set(PORT, GPIO_MODE_INPUT, GPIO_PUPD_NONE, PIN); \
  }


/*!
    \brief      configure the nested vectored interrupt controller
    \param[in]  none
    \param[out] none
    \retval     none
*/
void nvic_config(void)
{
    nvic_irq_enable(USART0_IRQn, 0, 0);
    nvic_irq_enable(USART1_IRQn, 0, 0);
}


/*!
    \brief      initialize the USART configuration of the com
    \param[in]  none
    \param[out] none
    \retval     none
*/
void com_usart_init(void)
{
    /* enable USART and GPIOA clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_USART0);
    rcu_periph_clock_enable(RCU_USART1);

    /* connect port to USART1 TX */
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_2);
    /* connect port to USART0 TX */
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_9);

    /* configure USART0 Tx as alternate function push-pull */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_9);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);

    /* configure USART1 Tx as alternate function push-pull */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_2);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);

    /* configure USART0 */
    usart_baudrate_set(USART0, 115200U);
    usart_receive_config(USART0, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);
    /* enable USART0 half duplex Mode */
    usart_halfduplex_enable(USART0);
    usart_enable(USART0);

    /* configure USART1 */
    usart_baudrate_set(USART1, 115200U);
    usart_receive_config(USART1, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART1, USART_TRANSMIT_ENABLE);
    /* enable USART1 half duplex Mode */
    usart_halfduplex_enable(USART1);
    usart_enable(USART1);
}

void gpio_init(){

    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);

    GD32_CONGIG_PIN_AS_OUT(GPIOA, GPIO_PIN_4); // DE USART1
    GD32_CONGIG_PIN_AS_OUT(GPIOA, GPIO_PIN_8); // DE USART0

    GD32_CONGIG_PIN_AS_IN(GPIOB, GPIO_PIN_4);
    GD32_CONGIG_PIN_AS_IN(GPIOB, GPIO_PIN_5);
    GD32_CONGIG_PIN_AS_IN(GPIOB, GPIO_PIN_6);
    GD32_CONGIG_PIN_AS_IN(GPIOB, GPIO_PIN_7);


    GD32_CONGIG_PIN_AS_OUT(GPIOC, GPIO_PIN_13);
}

uint8_t hw_get_addres(){
    uint8_t res = 0;
    
    res += gpio_input_bit_get(GPIOB, GPIO_PIN_4)<<0;
    res += gpio_input_bit_get(GPIOB, GPIO_PIN_5)<<1;
    res += gpio_input_bit_get(GPIOB, GPIO_PIN_6)<<2;
    res += gpio_input_bit_get(GPIOB, GPIO_PIN_7)<<3;

    return res;
}

void txDMA_set_state(bool state, int32_t cnt, USART_TYPE usart_type ){
    
    dma_channel_enum RS_TX_DMA;
    uint32_t RS_USART;

    if(usart_type == USART_TYPE_IN){
        
    }else{

    }
    
    if(state == true){
    dma_transfer_number_config(RS_TX_DMA, cnt); // setup dma max transfers
    usart_dma_transmit_config(RS_USART, USART_TRANSMIT_DMA_ENABLE); // enable uart receiver DMA
    dma_flag_clear          (RS_TX_DMA, DMA_FLAG_FTF); // clean dma transfer finishing flag
    dma_channel_enable      (RS_TX_DMA);  // enable DMA
    return;
  }
  usart_dma_transmit_config(RS_USART,USART_TRANSMIT_DMA_DISABLE); // enable uart receiver DMA
  dma_channel_disable(RS_TX_DMA);           // enable DMA
}

void rxDMA_set_state(bool state, USART_TYPE usart_type){

    dma_channel_enum RS_RX_DMA;
    uint32_t RS_USART;

    if(usart_type == USART_TYPE_IN){
        
    }else{

    }

  if(state == true){
    dma_transfer_number_config(RS_RX_DMA, 256); // setup dma max transfers
    usart_dma_receive_config(RS_USART, USART_RECEIVE_DMA_ENABLE); // enable uart receiver DMA
    dma_flag_clear          (RS_RX_DMA, DMA_FLAG_FTF); // clean dma transfer finishing flag
    dma_channel_enable      (RS_RX_DMA);  // enable DMA
    return;
  }
  usart_dma_receive_config(RS_USART, USART_RECEIVE_DMA_DISABLE); // enable uart receiver DMA
  dma_channel_disable(RS_RX_DMA);           // enable DMA
}


// static void usart_dma_config(USART_TYPE usart_type)
// {
//     dma_channel_enum RS_RX_DMA;
//     dma_channel_enum RS_TX_DMA;
//     uint32_t RS_USART;
//     uint32_t rtx_buffer;

//     uint32_t usart_adr;
   
//     if(usart_type == USART_TYPE_IN){
        
//     }else{

//     }
    
//   dma_parameter_struct dma_init_struct;
//   /* enable DMA1 */
//   rcu_periph_clock_enable(RCU_DMA);
//   /* deinitialize DMA channe7(RS_USART TX) */
//   dma_deinit(RS_TX_DMA);
//   dma_struct_para_init(&dma_init_struct);

//   dma_init_struct.direction = DMA_MEMORY_TO_PERIPHERAL;
//   dma_init_struct.memory_addr = (uint32_t)rtx_buffer;
//   dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
//   dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
//   dma_init_struct.number = 256;
//   dma_init_struct.periph_addr = usart_adr;
//   dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
//   dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
//   dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
//   dma_init(RS_TX_DMA, &dma_init_struct);


//   dma_channel_subperipheral_select(RS_TX_DMA, RS_DMA_SUBPERI);
//   /* configure DMA mode */
//   dma_circulation_disable(RS_TX_DMA);

//   /* deinitialize DMA channe2(USART0 RX) */
//   dma_deinit(RS_RX_DMA);
//   dma_init_struct.direction = DMA_PERIPH_TO_MEMORY;
//   dma_init_struct.memory0_addr = (uint32_t)rtx_buffer;
//   dma_single_data_mode_init(RS_RX_DMA, &dma_init_struct);
//   dma_channel_subperipheral_select(RS_RX_DMA, RS_DMA_SUBPERI);
//   /* configure DMA mode */
//   dma_circulation_disable(RS_RX_DMA);
// }

