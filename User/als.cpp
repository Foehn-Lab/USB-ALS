#include "ch32x035.h"
#include "als.h"

namespace
{
    constexpr uint8_t als_device_0_address = 0b1000100; // need to be shifted in I2C_Send7bitAddress
    constexpr uint8_t als_result_reg = 0x00;
    constexpr uint8_t als_config_reg = 0x01;
    constexpr uint8_t als_low_limit_reg = 0x02;
    constexpr uint8_t als_high_limit_reg = 0x03;
    constexpr uint8_t als_manufacture_id_reg = 0x7E;
    constexpr uint8_t als_device_id_reg = 0x7F;

    constexpr uint16_t als_config_default = 0xc810;
    constexpr uint16_t als_config_continuous_converstion_set = (1 << 10);

    struct als
    {
        uint8_t device_address;
        uint16_t device_id;
        float als_value_lux;

        explicit als(uint8_t address = als_device_0_address, uint16_t id = 0x00)
            : device_address(address), device_id(id), als_value_lux(0.f) {}

        void refresh_als_value()
        {
            i2c_als_read_2bytes();
            uint16_t r = (static_cast<uint16_t>(als_data[0] & 0x0F) << 8) | als_data[1];
            als_value_lux = 0.01 * static_cast<float>(1 << (als_data[0] >> 4)) * r;
        }

    private:
        uint8_t als_data[2] = {0};

        void i2c_als_read_2bytes()
        {

            while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) != RESET)
                ;
            // I2C_AcknowledgeConfig(I2C1, ENABLE);
            // I2C_NACKPositionConfig(I2C1, I2C_NACKPosition_Next);

            I2C_GenerateSTART(I2C1, ENABLE);

            while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
                ;
            I2C_Send7bitAddress(I2C1, (als_device_0_address << 1), I2C_Direction_Receiver);

            while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
                ;

            // I2C_AcknowledgeConfig(I2C1, DISABLE);
            while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED))
                ;
            // I2C_AcknowledgeConfig(I2C1, DISABLE);

            als_data[0] = I2C_ReceiveData(I2C1);

            I2C_GenerateSTOP(I2C1, ENABLE);
            while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED))
                ;

            als_data[1] = I2C_ReceiveData(I2C1);
        }
    };

    auto als_instance = als{}; // Create an instance of the als struct
    // uint8_t als_data[2] = {0};
    // uint8_t als_reg[2] = {0};
    // uint8_t als_reg_addr = 0x00;
    // uint8_t als_reg_data = 0x00;
}

void get_als_value_lux(float *lux_value)
{
    als_instance.refresh_als_value();
    *lux_value = als_instance.als_value_lux;
}

void als_init()
{
    uint8_t als_reg_data[3] = {0};
    als_reg_data[0] = als_config_reg;
    als_reg_data[1] = ((als_config_default | als_config_continuous_converstion_set) >> 8) & 0xFF; // high byte
    als_reg_data[2] = als_config_default & 0xFF;                                                  // low byte
    i2c_als_write_data(als_reg_data, sizeof(als_reg_data));
    Delay_Ms(800);
    uint8_t res_reg[] = {0x00};
    i2c_als_write_data(res_reg, sizeof(res_reg)); // Write data to I2C device

    Delay_Ms(1000);
}

void i2c_als_write_data(uint8_t *data, uint8_t len)
{
    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) != RESET)
        ;
    I2C_NACKPositionConfig(I2C1, I2C_NACKPosition_Current);
    I2C_AcknowledgeConfig(I2C1, ENABLE);

    I2C_GenerateSTART(I2C1, ENABLE);

    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
        ;
    I2C_Send7bitAddress(I2C1, (als_device_0_address << 1), I2C_Direction_Transmitter);

    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
        ;

    for (int i = 0; i < len; i++)
    {
        I2C_SendData(I2C1, data[i]);
        while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
            ;
    }

    I2C_GenerateSTOP(I2C1, ENABLE);
}
