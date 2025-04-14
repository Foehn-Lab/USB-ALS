/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2023/12/26
 * Description        : Main program body.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/* @Note
 * Compatibility HID Example:
 * This program provides examples of the pass-through of USB-HID data and serial port
 *  data based on compatibility HID device. And the data returned by Get_Report request is
 *  the data sent by the last Set_Report request.Speed of UART1/2 is 115200bps.
 *
 * Interrupt Transfers:
 *   UART2_RX   ---> Endpoint2
 *   Endpoint1  ---> UART2_TX
 *
 *   Note that the first byte is the valid data length and the remaining bytes are
 *   the transmission data for interrupt Transfers.
 *
 * Control Transfers:
 *   Set_Report ---> UART1_TX
 *   Get_Report <--- last Set_Report packet
 *
 *  */

#include <ch32x035_usbfs_device.h>
#include "debug.h"
#include "usbd_compatibility_hid.h"
#include "als.h"
#include "i2c.h"
#include "report.h"

uint8_t rx_data_i2c[2] = {0};
/*********************************************************************
 * @fn      Var_Init
 *
 * @brief   Software parameter initialisation
 *
 * @return  none
 */
void Var_Init(void)
{
    uint16_t i;
    RingBuffer_Comm.LoadPtr = 0;
    RingBuffer_Comm.StopFlag = 0;
    RingBuffer_Comm.DealPtr = 0;
    RingBuffer_Comm.RemainPack = 0;
    for (i = 0; i < DEF_Ring_Buffer_Max_Blks; i++)
    {
        RingBuffer_Comm.PackLen[i] = 0;
    }
}

void read_pd()
{

    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) != RESET)
        ;
    I2C_AcknowledgeConfig(I2C1, ENABLE);
    I2C_NACKPositionConfig(I2C1, I2C_NACKPosition_Next);

    I2C_GenerateSTART(I2C1, ENABLE);

    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
        ;
    I2C_Send7bitAddress(I2C1, (0b1000100 << 1), I2C_Direction_Receiver);

    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
        ;

    I2C_AcknowledgeConfig(I2C1, DISABLE);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED))
        ;
    // I2C_AcknowledgeConfig(I2C1, DISABLE);

    rx_data_i2c[0] = I2C_ReceiveData(I2C1);

    I2C_GenerateSTOP(I2C1, ENABLE);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED))
        ;

    rx_data_i2c[1] = I2C_ReceiveData(I2C1);

    Delay_Ms(1000);
}

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);
    I2C1_Init(); // I2C1 init
    // USART_SendData(USART1, 0x0D);
    printf("SystemClk:%d\r\n", SystemCoreClock);
    printf("ChipID:%08x\r\n", DBGMCU_GetCHIPID());

    /* Variables init */
    Var_Init();

    /* UART2 init */
    // UART2_Init();
    // UART2_DMA_Init();

    TIM3_Init();
    /* Usb Init */
    USBFS_RCC_Init();
    USBFS_Device_Init(ENABLE, PWR_VDD_SupplyVoltage());

    als_init();
    // float lux_value = 0.0f;
    while (1)
    {
        usb_lux_report(); // Read data from I2C device

        Delay_Ms(800);
        // if(USBFS_DevEnumStatus)
        // {
        //     UART2_Rx_Service();
        //     UART2_Tx_Service();
        //     HID_Set_Report_Deal();
        // }
    }
}
