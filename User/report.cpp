#include "als.h"
#include "stdint.h"
#include <ch32x035_usbfs_device.h>
#include "string.h"
#include "report.h"

float lux_value = 0.f;

uint8_t buf[64] = {0};

void usb_lux_report()
{
    // i2c_als_read_2bytes(rx_data_i2c); // Read data from I2C device
    get_als_value_lux(&lux_value); // Read data from I2C device
    memcpy(buf, &lux_value, sizeof(lux_value)); // Copy data to buffer
    printf("Lux value: %f\r\n", lux_value);
    USBFS_Endp_DataUp(DEF_UEP2, buf, sizeof(buf), DEF_UEP_CPY_LOAD); // Send data to USB HID device
}
