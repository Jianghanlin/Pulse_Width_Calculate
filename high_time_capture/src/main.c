/*
 * main.c
 *
 *  Created on: 2018-03-11 15:57
 *      Author: Jack Chen <redchenjs@live.com>
 *  Fixed on :  2018-05-25 11:14
 *      Author: art-j <jjw903427521@gmail.com>
 *
 ** --------key------------
 *  P2.0 P2.2 P2.4 P2.5
* -----------------------
*/
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* ------------------------------------------------
    ---------SOFT_I2C-----   *  ---------SOFT_SPI-----   * --------SSD1351--------    * ------clock out-----
 * PORT     TYPE    PIN      * PORT     TYPE    PIN      * PORT     TYPE    PIN       *      ACLK:  P1.0
 * SDA      INOUT   P3.0     * MOSI     OUT     P3.3     * RES      OUT     P3.6      *      SMCLK: P2.2
 * SCL      OUT     P3.1     * MISO     IN      P3.4     * DC       OUT     P3.7      *      MCLK:  P7.7
 * -----------------------   * SCK      OUT     P2.7     * CS       OUT     P1.4
                                                         * SCK      OUT     P1.5(SPI)
                                                         * MOSI     OUT     P1.7(SPI)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

 * -----USCI_A0_SPI-------  * -----USCI_A1_SPI-------    * -----USCI_B0_SPI---------  * -----USCI_B1_SPI---------
 * PORT     TYPE    PIN     * PORT     TYPE    PIN       * PORT     TYPE    PIN       * PORT     TYPE    PIN
 * MOSI     OUT     P3.3    * MOSI     OUT     P4.4      * MOSI     OUT     P3.0      * MOSI     OUT     P4.1
 * MISO     IN      P3.4    * MISO     IN      P4.5      * MISO     IN      P3.1      * MISO     IN      P4.2
 * SCK      OUT     P2.7    * SCK      OUT     P4.0      * SCK      OUT     P3.2      * SCK      OUT     P4.3
 * ----------------------   * ----------------------     * ---------------------      * ---------------------

 * -----USCI_B1_I2C---------
 * PORT     TYPE    PIN
 * SDA      INOUT   P4.1
 * SCL      OUT     P4.2
 * -------------------------
 */
#include <stdio.h>
#include <msp430.h>
#include <inc/system/init.h>
#include <inc/system/user.h>
#include <inc/driver/ssd1351.h>
#include <inc/system/fonts.h>
#include <src/user/TA0_PWM.h>
#include <src/user/TIMER_INIT.h>


int main(void)
{
    char high_time[12]={0};
    WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer
    device_init(); //开发板内设资源初始化
    driver_init(); //外部设备驱动初始化
//    P1_2_PWM_generate();
    TA1_Init();
    //io_init();
    _EINT();//打开总中断
    //user_init();//OLED显示固定内容
    while(1)
    {
      if(show_flag==1)
      {
          //TA1_CCR1_stop();
          sprintf(high_time, "%.2f ms  ", ON_Period); //A1通道的电压
          ssd1351_display_string(0,0,high_time,1,White,Black);
          show_flag=0;
          TA1_CCR1_start();
      }
/*        ssd1351_show_rainbow();*/
    }
}





