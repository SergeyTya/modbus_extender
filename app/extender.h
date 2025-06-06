#ifndef EXTENDER_H_
#define EXTENDER_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum {

    RS485_IN_RX_ENABLE,
    RS485_IN_TX_ENABLE,
    RS485_OUT_RX_ENABLE,
    RS485_OUT_TX_ENABLE,
    RS485_ERROR

}RS485States;

bool extend(uint8_t * data, size_t len, uint8_t id_now, uint8_t id_new );

#endif