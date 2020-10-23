/*
 * user.c
 *
 *  Created on: 2018-02-16 18:00
 *      Author: Jack Chen <redchenjs@live.com>
 *  Fixed on :  2018-05-25 11:14
 *      Author: art-j <jjw903427521@gmail.com>
 */
#include <stdio.h>
#include <math.h>
#include "msp430f5529.h"
#include <inc/user/input.h>
#include <inc/user/measure.h>
#include <inc/user/control.h>
#include <inc/user/display.h>
#include <inc/driver/ssd1351.h>
#include <inc/driver/ads1118.h>
#include <inc/driver/dac8571.h>
#include <inc/system/fonts.h> //使用颜色的名称必须包含fonts.h的头文件，比如要使用White，Red....等颜色名称
#include <inc/system/user.h>


#include <src/device/DS18B20.h>
#include <src/user/TIMER_INIT.h>

int REdge=0,FEdge=0,zhengshu=0,xiaoshu=0;
int overflow=0;
unsigned char show_flag=0;
float ON_Period=0;


void mode_choose_interface()
{
    ssd1351_display_string(0, 30, "=> Self Learn", 1, White, Black);
    ssd1351_display_string(0, 50, "   Net Detect", 1, White, Black);
    ssd1351_display_string(0, 70, "   Load Detect", 1, White, Black);
}

void user_init(void)
{
    //dislay_topic();
    //mode_choose_interface();
}

void cal_distance()
{
    ON_Period = (65536.0*overflow+FEdge-REdge)*1000/32768; //扩大1000倍，单位为ms
    show_flag=1;
    overflow=0;
}

float average_temperature_caculate_2(unsigned char times) //剃掉最大最小值算法
{
    char i = 0;
    float sum = 0, temp = 0, max = 0, min = 10000;
    for (i = times; i > 0; i--)
    {
        temp =DS18B20_ReadTemp();
        if (max < temp)
        {
            max = temp;
        }
        if (min > temp)
        {
            min = temp;
        }
        sum = sum + temp;
    }
    sum = (sum - max - min) *0.01/ (times - 2); //减2是因为max和min
    return sum;
}


float average_temperature_caculate_1(unsigned char times) //单纯平均值算法
{
    char i = 0;
    float sum = 0, temp = 0;
    for (i = times; i > 0; i--)
    {
        temp =DS18B20_ReadTemp();
        sum = sum + temp;
    }
    sum = sum*0.01/times;
    return sum;
}




void high_time()
{
    ON_Period = (65536.0*overflow+FEdge-REdge)*1000/8000000; //扩大1000倍，单位为ms
    show_flag=1;
    overflow=0;
}


void key_dect()
{
    static unsigned char now = 0,i=0; //position用来保存选择指针的位置,jiemian用来保存跳转的界面级数,confirm用来记录KEY3的按键次数,hold用来检测是否启用长按函数
    unsigned char past = 0;       //key来区别那个按键按下,i用来记录长按的时间
    past = now;
    if ((P2IN & BIT1) == 0)
    {
        now = 0;
    }
    else
        now = 1;
    if ((past == 1) && (now == 0))
    {
        if(i==0)
        {
            TA0CCTL0 &=~ CCIE;
            i=1;
        }
        else
        {
            TA0CCTL0 |= CCIE;
            i=0;
        }
    }

}

/*void key_dect() //检测事件确实发生了
{
    static unsigned char now = 0, net = 1; //position用来保存选择指针的位置,jiemian用来保存跳转的界面级数,confirm用来记录KEY3的按键次数,hold用来检测是否启用长按函数
    unsigned char past = 0, key = 0;       //key来区别那个按键按下,i用来记录长按的时间
    past = now;
    if ((P2IN & BIT0) == 0)
    {
        key = 1;
        now = 0;
    }
    else if ((P2IN & BIT2) == 0)
    {
        key = 2;
        now = 0;
    }
    else if ((P2IN & BIT4) == 0)
    {
        key = 3;
        now = 0;
    }
    else if ((P2IN & BIT5) == 0)
    {
        key = 4;
        now = 0;
    }
    else
        now = 1;
    if ((past == 1) && (now == 0))
    {
        switch (key)
        {
        case 1:
            if (jiemian == 1) //处于模式选择界面时，KEY1的功能
            {
                if (jiantou == 1) //箭头1按下KEY1
                {
                    jiantou = 3; //跳转到箭头3
                    ssd1351_display_string(0, 30, "  ", 1, White, Black);
                    ssd1351_display_string(0, 70, "=>", 1, White, Black);
                }
                else if (jiantou == 2) //箭头2按下KEY1
                {
                    jiantou = 1; //跳转到箭头1
                    ssd1351_display_string(0, 50, "  ", 1, White, Black);
                    ssd1351_display_string(0, 30, "=>", 1, White, Black);
                }
                else //箭头3按下KEY1
                {
                    jiantou = 2; //箭头3按下KEY2
                    ssd1351_display_string(0, 70, "  ", 1, White, Black);
                    ssd1351_display_string(0, 50, "=>", 1, White, Black);
                }
            }
            else //进入了界面2,即jiemian=2
            {
                switch (jiantou)
                {
                case 1: //处于自学习界面时，KEY1的功能
                    if (open_dc_1ma_flag == 0)
                    {
                        open_dc_1ma();
                        open_dc_1ma_flag = 1;
                        open_ac_1ma_flag = 0;
                        ssd1351_display_string(0, 20, "DC 1mA", 1, White, Black);
                        ssd1351_display_string(0, 40, "ADC2=", 1, White, Black); //显示反馈电压ADC2
                    }
                    else if (open_dc_1ma_flag == 1)
                    {
                        open_ac_1ma();
                        open_ac_1ma_flag = 1;
                        open_dc_1ma_flag = 0;
                        ssd1351_display_string(0, 20, "AC 1mA", 1, White, Black);
                        ssd1351_display_string(0, 40, "ADC1=", 1, White, Black); //显示反馈电压ADC2
                    }
                    else
                        ;
                    break;
                default:
                    break;
                }
            }
            break;
        case 2:
            if (jiemian == 1) //界面1下按键2的功能
            {
                if (jiantou == 1) //箭头1按下KEY1
                {
                    jiantou = 2; //跳转到箭头2
                    ssd1351_display_string(0, 30, "  ", 1, White, Black);
                    ssd1351_display_string(0, 50, "=>", 1, White, Black);
                }
                else if (jiantou == 2) //箭头2按下KEY1
                {
                    jiantou = 3; //跳转到箭头3
                    ssd1351_display_string(0, 50, "  ", 1, White, Black);
                    ssd1351_display_string(0, 70, "=>", 1, White, Black);
                }
                else //箭头3按下KEY1
                {
                    jiantou = 1; //跳转到箭头1
                    ssd1351_display_string(0, 70, "  ", 1, White, Black);
                    ssd1351_display_string(0, 30, "=>", 1, White, Black);
                }
            }
            else //界面2下按键2的功能，即jiemian=2
            {
                if (jiantou == 1) //进入界面2，并且是自学习界面key3的功能
                {
                    net++;
                    if (net == 9)
                        net = 1;
                    switch (net)
                    {
                    case 1:
                        ssd1351_display_string(30, 70, "C + R      ", 0, White, Black);
                        break;
                    case 2:
                        ssd1351_display_string(30, 70, "C // R     ", 0, White, Black);
                        break;
                    case 3:
                        ssd1351_display_string(30, 70, "L + R      ", 0, White, Black);
                        break;
                    case 4:
                        ssd1351_display_string(30, 70, "L // R     ", 0, White, Black);
                        break;
                    case 5:
                        ssd1351_display_string(30, 70, "C + L      ", 0, White, Black);
                        break;
                    case 6:
                        ssd1351_display_string(30, 70, "C // L     ", 0, White, Black);
                        break;
                    case 7:
                        ssd1351_display_string(30, 70, "C + R + L  ", 0, White, Black);
                        break;
                    case 8:
                        ssd1351_display_string(30, 70, "C // R // L  ", 0, White, Black);
                        break;
                    default:
                        break;
                    }
                }
                else
                    ;
            }
            break;
        case 3:
            jiemian = 2;
            if (confirm == 0)
            {
                ssd1351_clear_gram();
                confirm = 1;
                switch (jiantou)
                {
                case 1:
                    ssd1351_display_string(0, 0, "Self-Study Interface", 0, Yellow, Black);
                    ssd1351_draw_h_line(0, 65, 127, White); //第一条分割线
                    ssd1351_display_string(0, 70, "Net:", 0, White, Black);
                    ssd1351_draw_h_line(0, 85, 127, White); //第二条分割线
                    net = 1;
                    ssd1351_display_string(30, 70, "C + R", 0, White, Black);
                    break;
                case 2:
                    ssd1351_display_string(0, 0, "Net Detect", 1, White, Black);
                    break;
                case 3:
                    open_dc_1ma();
                    __delay_cycles(100);                                     //在看门狗里开启直流1mA
                    ssd1351_display_string(0, 20, "ADC1=", 1, White, Black); //交流电压峰值检测ADC1
                    ssd1351_display_string(0, 40, "ADC2=", 1, White, Black); //直流电压检测ADC2
                    break;
                default:
                    break;
                }
            }
            else //进入界面2以后key3按键的功能
            {
                if (jiantou == 1) //KEY2在界面2,并且是自学模式界面按下
                {
                    if (open_dc_1ma_flag == 1)
                    {
                        switch (net)
                        {
                        case 1:
                            adc[0][0] = a1; //在自学模式，并且是在直流模式下按下key2，将此时的ADC2保存进adc[0][0~7]
                            ssd1351_display_string(0, 90, "Save to adc[0][0]!! ", 0, White, Black);
                            ssd1351_display_string(0, 105, "--> C+R DC_1mA     ", 0, White, Black);
                            break;
                        case 2:
                            adc[0][1] = a1; //在自学模式，并且是在直流模式下按下key2，将此时的ADC2保存进adc[0][0~7]
                            ssd1351_display_string(0, 90, "Save to adc[0][1]!!", 0, White, Black);
                            ssd1351_display_string(0, 105, "--> C//R DC_1mA     ", 0, White, Black);
                            break;
                        case 3:
                            adc[0][2] = a1; //在自学模式，并且是在直流模式下按下key2，将此时的ADC2保存进adc[0][0~7]
                            ssd1351_display_string(0, 90, "Save to adc[0][2]!!", 0, White, Black);
                            ssd1351_display_string(0, 105, "--> L+R DC_1mA     ", 0, White, Black);
                            break;
                        case 4:
                            adc[0][3] = a1; //在自学模式，并且是在直流模式下按下key2，将此时的ADC2保存进adc[0][0~7]
                            ssd1351_display_string(0, 90, "Save to adc[0][3]!!", 0, White, Black);
                            ssd1351_display_string(0, 105, "--> L//R DC_1mA     ", 0, White, Black);
                            break;
                        case 5:
                            adc[0][4] = a1; //在自学模式，并且是在直流模式下按下key2，将此时的ADC2保存进adc[0][0~7]
                            ssd1351_display_string(0, 90, "Save to adc[0][4]!!", 0, White, Black);
                            ssd1351_display_string(0, 105, "--> C+L DC_1mA     ", 0, White, Black);
                            break;
                        case 6:
                            adc[0][5] = a1; //在自学模式，并且是在直流模式下按下key2，将此时的ADC2保存进adc[0][0~7]
                            ssd1351_display_string(0, 90, "Save to adc[0][5]!!", 0, White, Black);
                            ssd1351_display_string(0, 105, "--> C//L DC_1mA     ", 0, White, Black);
                            break;
                        case 7:
                            adc[0][6] = a1; //在自学模式，并且是在直流模式下按下key2，将此时的ADC2保存进adc[0][0~7]
                            ssd1351_display_string(0, 90, "Save to adc[0][6]!!", 0, White, Black);
                            ssd1351_display_string(0, 105, "--> C+R+L DC_1mA     ", 0, White, Black);
                            break;
                        case 8:
                            adc[0][7] = a1; //在自学模式，并且是在直流模式下按下key2，将此时的ADC2保存进adc[0][0~7]
                            ssd1351_display_string(0, 90, "Save to adc[0][7]!!", 0, White, Black);
                            ssd1351_display_string(0, 105, "--> C//R//L DC_1mA", 0, White, Black);
                            break;
                        default:
                            break;
                        }
                    }
                    else if (open_ac_1ma_flag == 1)
                    {
                        switch (net)
                        {
                        case 1:
                            adc[1][0] = a0; //在自学模式，并且是在直流模式下按下key2，将此时的ADC1保存进adc[1][0~7]
                            ssd1351_display_string(0, 90, "Save to adc[1][0]!! ", 0, White, Black);
                            ssd1351_display_string(0, 105, "--> C+R AC_1mA     ", 0, White, Black);
                            break;
                        case 2:
                            adc[1][1] = a0; //在自学模式，并且是在直流模式下按下key2，将此时的ADC1保存进adc[1][0~7]
                            ssd1351_display_string(0, 90, "Save to adc[1][1]!!", 0, White, Black);
                            ssd1351_display_string(0, 105, "--> C//R AC_1mA     ", 0, White, Black);
                            break;
                        case 3:
                            adc[1][2] = a0; //在自学模式，并且是在直流模式下按下key2，将此时的ADC1保存进adc[1][0~7]
                            ssd1351_display_string(0, 90, "Save to adc[1][2]!!", 0, White, Black);
                            ssd1351_display_string(0, 105, "--> L+R AC_1mA     ", 0, White, Black);
                            break;
                        case 4:
                            adc[1][3] = a0; //在自学模式，并且是在直流模式下按下key2，将此时的ADC1保存进adc[1][0~7]
                            ssd1351_display_string(0, 90, "Save to adc[1][3]!!", 0, White, Black);
                            ssd1351_display_string(0, 105, "--> L//R AC_1mA     ", 0, White, Black);
                            break;
                        case 5:
                            adc[1][4] = a0; //在自学模式，并且是在直流模式下按下key2，将此时的ADC1保存进adc[1][0~7]
                            ssd1351_display_string(0, 90, "Save to adc[1][4]!!", 0, White, Black);
                            ssd1351_display_string(0, 105, "--> C+L AC_1mA     ", 0, White, Black);
                            break;
                        case 6:
                            adc[1][5] = a0; //在自学模式，并且是在直流模式下按下key2，将此时的ADC1保存进adc[1][0~7]
                            ssd1351_display_string(0, 90, "Save to adc[1][5]!!", 0, White, Black);
                            ssd1351_display_string(0, 105, "--> C//L AC_1mA     ", 0, White, Black);
                            break;
                        case 7:
                            adc[1][6] = a0; //在自学模式，并且是在直流模式下按下key2，将此时的ADC1保存进adc[1][0~7]
                            ssd1351_display_string(0, 90, "Save to adc[1][6]!!", 0, White, Black);
                            ssd1351_display_string(0, 105, "--> C+R+L AC_1mA     ", 0, White, Black);
                            break;
                        case 8:
                            adc[1][7] = a0; //在自学模式，并且是在直流模式下按下key2，将此时的ADC1保存进adc[1][0~7]
                            ssd1351_display_string(0, 90, "Save to adc[1][7]!!", 0, White, Black);
                            ssd1351_display_string(0, 105, "--> C//R//L AC_1mA", 0, White, Black);
                            break;
                        default:
                            break;
                        }
                    }
                    else
                        ;
                }
                else
                    ;
            }
            break;
        case 4:
            back = 0;
            jiemian = 1;
            confirm = 0;
            jiantou = 1;
            open_dc_1ma_flag = 0;
            open_ac_1ma_flag = 0; //按下返回键，将所有标志变量置低
            ssd1351_clear_gram();
            break;
        default:
            break;
        }
    }
}*/
