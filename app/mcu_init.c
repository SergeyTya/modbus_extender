
#include "gd32f3x0.h"
#include "mcu_init.h"

TypeDef_usart RS485_in_u0 = {
    .RS_USART  = USART0,
    .usart_adr_tx = ((uint32_t) &USART_TDATA(USART0)),
    .usart_adr_rx = ((uint32_t) &USART_RDATA(USART0)),
    .rde_port  = GPIOA,
    .rde_pin   = GPIO_PIN_8,
    .RX_DMA    = DMA_CH2,
    .TX_DMA    = DMA_CH1
};

TypeDef_usart RS485_out_u1 = {
    .RS_USART  = USART1,
    .usart_adr_tx = ((uint32_t) &USART_TDATA(USART1)),
    .usart_adr_rx = ((uint32_t) &USART_RDATA(USART1)),
    .rde_port  = GPIOA,
    .rde_pin   = GPIO_PIN_4,
    .RX_DMA    = DMA_CH4,
    .TX_DMA    = DMA_CH3
};

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

static void usart_dma_config(TypeDef_usart *  usart);

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

    nvic_irq_enable(DMA_Channel1_2_IRQn, 0, 0);
    nvic_irq_enable(DMA_Channel3_4_IRQn, 0, 0);
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


     /* connect port to USART0 tx */
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_9);
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_10);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_9);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_9);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_10);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_10);

    /* connect port to USART1 TX */
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_2);
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_3);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_3);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_2);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);


    /* configure USART0 */
    usart_deinit(USART0);
    usart_word_length_set(USART0, USART_WL_8BIT);
    usart_stop_bit_set(USART0, USART_STB_1BIT);
    usart_parity_config(USART0, USART_PM_NONE);
    usart_baudrate_set(USART0, 115200U);
    usart_receive_config(USART0, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);
    usart_halfduplex_disable(USART0);
    usart_enable(USART0);

    /* configure USART1 */
    usart_baudrate_set(USART1, 115200U);
    usart_receive_config(USART1, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART1, USART_TRANSMIT_ENABLE);
    usart_halfduplex_disable(USART1);
    usart_enable(USART1);

    usart_receiver_timeout_enable(USART0);
    usart_receiver_timeout_threshold_config(USART0,  115);
    usart_interrupt_enable(USART0, USART_INT_RT);
 
    usart_dma_config(&RS485_in_u0 );
    usart_dma_config(&RS485_out_u1);
    usart_dma_receive_config(USART0, USART_RECEIVE_DMA_ENABLE);
    usart_dma_receive_config(USART1, USART_RECEIVE_DMA_ENABLE);
    

    rxDMA_set_state(false , &RS485_in_u0);
    rxDMA_set_state(true , &RS485_in_u0);

    setRDEstate(&RS485_out_u1, false);
    setRDEstate(&RS485_in_u0, false);

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

void txDMA_set_state(bool state, int32_t cnt, TypeDef_usart *  usart){
    
    usart_dma_transmit_config(usart->RS_USART,USART_TRANSMIT_DMA_DISABLE); // enable uart receiver DMA
    dma_channel_disable(usart->TX_DMA);           // enable DMA
    if(state == true){
        dma_transfer_number_config(usart->TX_DMA, cnt); // setup dma max transfers
        usart_dma_transmit_config(usart->RS_USART, USART_TRANSMIT_DMA_ENABLE); // enable uart receiver DMA
        dma_flag_clear          (usart->TX_DMA, DMA_FLAG_FTF); // clean dma transfer finishing flag
        dma_channel_enable      (usart->TX_DMA);  // enable DMA
        return;
    }
}

void rxDMA_set_state(bool state, TypeDef_usart *  usart){

    dma_channel_disable(usart->RX_DMA);
    if(state == true){
        dma_transfer_number_config(usart->RX_DMA, 256); // setup dma max transfers
        dma_flag_clear          (usart->RX_DMA, DMA_FLAG_FTF); // clean dma transfer finishing flag
        dma_channel_enable      (usart->RX_DMA);  // enable DMA
    }
}


static void usart_dma_config(TypeDef_usart *  usart)
{  
  rcu_periph_clock_enable(RCU_DMA);
  dma_parameter_struct dma_init_struct;
  /* deinitialize DMA channe7(RS_USART TX) */
  dma_deinit(usart->TX_DMA);
  dma_struct_para_init(&dma_init_struct);
  dma_init_struct.direction = DMA_MEMORY_TO_PERIPHERAL;
  dma_init_struct.memory_addr = (uint32_t)usart->rtxbuff;
  dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
  dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
  dma_init_struct.number = 256U;
  dma_init_struct.periph_addr = usart->usart_adr_tx;
  dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
  dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
  dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
  dma_init(usart->TX_DMA, &dma_init_struct);

  /* configure DMA mode */
  dma_circulation_disable(usart->TX_DMA);
  dma_memory_to_memory_disable(usart->TX_DMA);
  /* enable DMA channel1 transfer complete interrupt */
  dma_interrupt_enable(usart->TX_DMA, DMA_INT_FTF);

  /* deinitialize DMA channe2(USART0 RX) */
  dma_deinit(usart->RX_DMA);
  dma_init_struct.direction = DMA_PERIPHERAL_TO_MEMORY;
  dma_init_struct.periph_addr = usart->usart_adr_rx;
  /* configure DMA mode */
  dma_init(usart->RX_DMA, &dma_init_struct);
  dma_circulation_disable(usart->RX_DMA);
  dma_memory_to_memory_disable(usart->RX_DMA);

}

void setRDEstate(TypeDef_usart *  usart, bool state){
  if(state==true){
    gpio_bit_set(usart->rde_port, usart->rde_pin);
  }else{
    gpio_bit_reset(usart->rde_port, usart->rde_pin);
  }
}

