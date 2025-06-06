#include "extender.h"
#include "mbcrc.h"

extern uint8_t addr_from_hw;

/**
 * @brief Extends the data by adding an address and recalculating the CRC
 *
 * @param data Pointer to the data array
 * @param len Length of the data array
 * @param id_now Address of the current device
 * @param id_new New address to be added
 * @return true if the data was successfully extended, false otherwise
 */
bool extend(uint8_t* data, size_t len, uint8_t id_now, uint8_t id_new) {

    // Check if there is enough space to add the address and CRC
    if (len < 4)  {
        return false;
    }

    // Calculate the CRC for the original data
    uint16_t crc_calc = ChMbcrc16(data, len - 2);
    // Get the CRC from the original data
    uint16_t crc_fram = data[len - 2] + (data[len - 1] << 8);

    // Check if the calculated CRC matches the CRC from the original data
    if (crc_calc != crc_fram) {
        return false;
    }

    // Check if frame address is valid
    if (data[0] != id_now)  {
        return false;
    }

    // Add the new address to the data
    data[0] = id_new;

    // Recalculate the CRC for the extended data
    crc_calc = ChMbcrc16(data, len-2);

    // Write the new CRC to the data
    data[len - 2] = (uint8_t)(crc_calc & 0xFF);
    data[len - 1] = (uint8_t)((crc_calc & 0xFF00) >> 8);

    // Return a successful result
    return true;
}