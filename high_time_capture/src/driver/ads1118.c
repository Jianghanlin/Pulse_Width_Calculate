/*

 * ads1118.c
 *
 *  Created on: 2018-05-17 11:23
 *      Author: Jack Chen <redchenjs@live.com>
 *  Fixed on :  2018-05-25 11:14
 *      Author: art-j <jjw903427521@gmail.com>
*/

/*

#include <stdint.h>
#include <string.h>

#include <msp430.h>

#include <inc/driver/ads1118.h>
#include <inc/device/usci_b1_spi.h>

#define SS_NONE     0b0
#define SS_START    0b1

#define MUX_D0_1    0b000
#define MUX_D0_3    0b001
#define MUX_D1_3    0b010
#define MUX_D2_3    0b011
#define MUX_S0      0b100
#define MUX_S1      0b101
#define MUX_S2      0b110
#define MUX_S3      0b111

#define PGA_6_144   0b000
#define PGA_4_096   0b001
#define PGA_2_048   0b010
#define PGA_1_024   0b011
#define PGA_0_512   0b100
#define PGA_0_256   0b101

#define MODE_CONTINOUS  0b0
#define MODE_SINGLE     0b1

#define DR_8_SPS    0b000
#define DR_16_SPS   0b001
#define DR_32_SPS   0b010
#define DR_64_SPS   0b011
#define DR_128_SPS  0b100
#define DR_250_SPS  0b101
#define DR_475_SPS  0b110
#define DR_860_SPS  0b111

#define TS_MODE_ADC     0b0
#define TS_MODE_TEMP    0b1

#define PULL_UP_EN_OFF  0b0
#define PULL_UP_EN_ON   0b1

#define NOP_DATA_INVALID    0b00
#define NOP_DATA_VALID      0b01

#define RESERVED_BIT    0b1

static ads1118_t ads1118_dev;
ads1118_t *ads1118 = &ads1118_dev;

static int16_t ads1118_transmit(uint8_t *data)
{
    unsigned char tx_buff[4] = {0};
    unsigned char rx_buff[4] = {0};

    tx_buff[0] = data[1];
    tx_buff[1] = data[0];
    tx_buff[2] = 0;
    tx_buff[3] = 0;

    P4OUT &=~BIT0;
    usci_b1_spi_transmit_frame(tx_buff, rx_buff, 4);
    P4OUT |= BIT0;

    return rx_buff[0] << 8 | rx_buff[1];
}

void ads1118_set_channel(uint8_t channel)
{
    switch (channel) {
    case 0:
        ads1118->bits.mux = MUX_S0;
        break;
    case 1:
        ads1118->bits.mux = MUX_S1;
        break;
    case 2:
        ads1118->bits.mux = MUX_S2;
        break;
    case 3:
        ads1118->bits.mux = MUX_S3;
        break;
    default:
        return;
    }

    ads1118_transmit((uint8_t *)ads1118);
}

int16_t ads1118_convert(void)
{
    return ads1118_transmit((uint8_t *)"\x00\x00");
}

void ads1118_init(void)
{
    P4DIR |= BIT0;
    P4OUT |= BIT0;

    memset(&ads1118_dev, 0, sizeof(ads1118_dev));

    ads1118->bits.ss = SS_NONE;
    ads1118->bits.mux = MUX_S0;
    ads1118->bits.pga = PGA_2_048;
    ads1118->bits.mode = MODE_CONTINOUS;
    ads1118->bits.dr = DR_860_SPS;
    ads1118->bits.ts_mode = TS_MODE_ADC;
    ads1118->bits.pull_up_en = PULL_UP_EN_ON;
    ads1118->bits.nop = NOP_DATA_VALID;
    ads1118->bits.reserved = RESERVED_BIT;

    ads1118_transmit((uint8_t *)ads1118);
}
*/

//////////////////////////////////////////////////////
//AIN输入端口（正-负）                              //
//  0:AIN0-AIN1 1:AIN0-AIN3 2:AIN1-AIN3 3:AIN2-AIN3 //
//  4:AIN0-GND  5:AIN1-GND  6:AIN2-GND  7:AIN3-GND  //
//量程                                                //
//  0:±6.144V 1:±4.096V 2:±2.048V 3:±1.024V     //
//  4:±0.512V 5:±0.256V 6:±0.256V 7:±0.256V     //
//////////////////////////////////////////////////////
#include "msp430f5529.h"
#include <inc/driver/ssd1351.h>
#include <inc/system/fonts.h>
#include <stdio.h>
#include <math.h>
#include <inc/system/user.h>
#define SCLK_H P4OUT |= BIT3
#define SCLK_L P4OUT &= ~BIT3
#define CS_H P4OUT |= BIT0
#define CS_L P4OUT &= ~BIT0
#define DataIN_H P4OUT |= BIT1
#define DataIN_L P4OUT &= ~BIT1

#define uint unsigned int
#define uchar unsigned char

void ADS1118_Init(void)
{
    P4DIR |= BIT0;  //CS
    P4DIR |= BIT3;  //SCK
    P4DIR |= BIT1;  //输出，单片机写数据DIN
    P4DIR &= ~BIT2; //输入，单片机读数据OUT
}

//****************************************************************
//* 函数名称：WriteSPI
//* 功能描述：开始SPI写数据
//* 入口参数：config
//* 返回参数：void
//****************************************************************
void WriteSPI(uint config)
{
    uint i = 0;
    CS_L;
    SCLK_L;
    //delay_us(200);
    for (i = 0; i < 16; i++)
    {
        SCLK_H;
        //delay_us(200);
        if (config & 0x8000)
        {
            DataIN_H;
        }
        else
        {
            DataIN_L;
        }
        config <<= 1;
        SCLK_L;
        //delay_us(200);
    }
    CS_H;
}

//****************************************************************
//* 函数名称：ADS_ReadSPI读
//* 功能描述：开始SPI读数据
//* 入口参数：void
//* 返回参数：unsigned int
//****************************************************************
uint ADS_Read()
{
    uint i = 0;
    uint adc;
    CS_L;
    adc = 0x0000;

    while (P4IN & BIT2)
        ; //进行一次转换起始判断
    DataIN_H;
    SCLK_L;
    for (i = 0; i < 16; i++) //读取数据，单个位读取，共16位、高位开始
    {
        adc = adc << 1;
        SCLK_H;
        //delay_us(200);
        if (P4IN & BIT2)
        {
            adc |= 0x0001;
        }
        else
        {
            adc &= ~0x0001;
        }
        SCLK_L;

        //delay_us(200);
    }
    CS_H;
    return adc;
}

//****************************************************************
//* 函数名称：ADS1118_Config
//* 功能描述：配置寄存器
//* 入口参数：add---ADS1118的寄存器配置
//* 返回参数：void
//****************************************************************
void ADS1118_Config(uchar port, uchar scale)
{
    WriteSPI(0x81e3 + port * 4096 + scale * 512); //C1E3  1100 0001 1110 0011
}
//****************************************************************
//* 函数名称：ADS1118
//* 功能描述：进行一次AD转换
//* 入口参数：port---AIN输入端口(0~7)  scale---满量程(0~7)
//* 返回参数：void
//****************************************************************
float ADS1118(uchar port, uchar scale)
{
    float value;
    ADS1118_Config(port, scale);
    value = ADS_Read();
    if (value > 0x7FFF)
        value -= 0x10000;
    switch (scale)
    {
    case 0:
        value *= 6.144;
        break;
    case 1:
        value *= 4.096;
        break;
    case 2:
        value *= 2.048;
        break;
    case 3:
        value *= 1.024;
        break;
    case 4:
        value *= 0.512;
        break;
    case 5:
        value *= 0.256;
        break;
    case 6:
        value *= 0.256;
        break;
    case 7:
        value *= 0.256;
        break;
    default:
        break;
    }
    value /= 32768.0;
    return value;
}

void display_res(float adc2)
{
    char a[12]={};
    float res;
    res=33.524*pow(adc2,2) + 1005.6*adc2 + 3.1752;
    sprintf(a, "%.3fohm ",res);
    //ssd1351_clear_rect(0,60,127,50);
    ssd1351_display_string(0, 60, "R=", 1, White, Black);
    ssd1351_display_string(16, 60, a, 1, White, Black);
    ssd1351_display_string(0, 80, "               ", 1, White, Black);
}

void display_cap(float adc1)
{
    char a[12]={};
    float cap;
    cap=280.31*pow(adc1,-1.01);
    sprintf(a, "%.3fnF ",cap);
    //ssd1351_clear_rect(0,60,127,50);
    ssd1351_display_string(0, 60, "C=", 1, White, Black);
    ssd1351_display_string(16, 60, a, 1, White, Black);
    ssd1351_display_string(0, 80, "               ", 1, White, Black);
}

void display_l(float adc1)
{
    char a[12]={};
    float l;
    l=71.227*pow(adc1,2) + 510.68*adc1 + 20.549;
    sprintf(a, "%.3fuH ",l);
    //ssd1351_clear_rect(0,60,127,50);
    ssd1351_display_string(0, 60, "L=", 1, White, Black);
    ssd1351_display_string(16, 60, a, 1, White, Black);
    ssd1351_display_string(0, 80, "               ", 1, White, Black);
}


float average_voltage_caculate(unsigned char channel, unsigned char times) //传入参数为通道号和采样次数
{
    char i = 0;
    float sum = 0, AD = 0, max = 0, min = 10;
    for (i = times; i > 0; i--)
    {
        AD = ADS1118(channel, 0);
        if (max < AD)
        {
            max = AD;
        }
        if (min > AD)
        {
            min = AD;
        }
        sum = sum + AD;
    }
    sum = (sum - max - min) / (times - 2); //减2是因为max和min
    return sum;
}



void get_ads1118_sample_voltage(void)
{

}
