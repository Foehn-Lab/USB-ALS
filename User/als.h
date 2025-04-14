

#include "stdint.h"
#ifdef __cplusplus
extern "C"
{
#endif

    void i2c_als_write_data(uint8_t *data, uint8_t len);

    void als_init();

    void get_als_value_lux(float *lux_value);

    // typedef struct als_reg
    // {
    //     uint8_t device_address;
    // } asl_reg_type_def;

#ifdef __cplusplus
}
#endif
