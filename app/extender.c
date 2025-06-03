#include "extender.h"
#include "mbcrc.h"

bool extend(uint8_t * data, size_t len, uint8_t adr){

    if(len < 4) return false;

    uint16_t crc_calc = ChMbcrc16(data, len-2);
    uint16_t crc_fram = data[len-2] + (data[len-1]<<8);   

    if(crc_calc != crc_fram) return false;

    data[0] = adr;

    crc_calc = ChMbcrc16(data, len);

    data[len-2] = crc_calc & 0xFF;
    data[len-1] = (crc_calc & 0xFF00)>>8;

    return true;
}