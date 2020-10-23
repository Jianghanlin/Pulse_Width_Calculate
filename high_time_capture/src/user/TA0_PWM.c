/*
 * TA_PWM.c
 * 描述：PWM库函数，可利用TA生成三种PWM。
 *  Created on: 2015-11-20
 *  Author: yumin
 */

#include "msp430F5529.h"
#define Dead 1                                          //预设死区时间，以TA的clk为单位

#define TA01_SET    P1SEL |= BIT2; P1DIR |= BIT2        //P1.2
#define TA02_SET    P1SEL |= BIT3; P1DIR |= BIT3        //P1.3
#define TA03_SET    P1SEL |= BIT4; P1DIR |= BIT4        //P1.4
#define TA04_SET    P1SEL |= BIT5; P1DIR |= BIT5        //P1.5

#define TA01_OFF    P1SEL &= ~BIT2                      //P1.2
#define TA02_OFF    P1SEL &= ~BIT3                      //P1.3
#define TA03_OFF    P1SEL &= ~BIT4                      //P1.4
#define TA04_OFF    P1SEL &= ~BIT5                      //P1.5
/*****************************************************************************************
* 名       称：TA0_PWM_Init()
* 功       能：TA0定时器作为PWM发生器的初始化设置函数
* 入口参数：Clk:时钟源 'S'=SMCLK;   'A'=ACLK ;   'E'=TACLK(外部输入)； 'e'= TACLK(TACLK取反)
                    Div:时钟分频系数: 1/2/4/8
            Mode1:通道1的输出模式 'F'设为超前PWM（模式7），'B'滞后PWM（模式3） ，'D'带死区增PWM（模式6），0=禁用
            Mode2:通道2的输出模式 'F'设为超前PWM（模式7），'B'滞后PWM （模式3），'D'带死区减PWM（模式2），0=禁用
            设置输出带死区控制的PWM时，两通道均需使用，且均为死区模式。
* 出口参数：1表示设置成功，0表示参数错误，设置失败。
* 说       明 : 在调用PWM相关函数之前，需要调用该函数设置TA的模式和时钟源。
* 范       例 : TA0_PWM_Init('A',1,'F','P')TA时钟设为ACLK,通道1和通道2均为超前PWM输出
                   TA0_PWM_Init('S',4,'D','D')TA时钟设为SMCLK/4, 通道1为死区增PWM、通道2为死区减PWM
                   TA0_PWM_Init('A',1,'F',0)TA时钟设为ACLK,通道1超前PWM输出，通道2不作TA用。
 ****************************************************************************************/
char TA0_PWM_Init(char Clk,char Div,char Mode1,char Mode2)
{
  TA0CTL =0;                                                // 清除以前设置

  switch(Mode1)                                             //为定时器选择计数模式
  {
  case 'F': case 'f':                                       //普通PWM
          TA0CTL |=MC_1; break;                             //主定时器为增计数
  case 'B':case 'b':
          TA0CTL |=MC_1; break;                             //主定时器为增计数
  case 'D': case 'd':                                       //死区PWM
          TA0CTL |=MC_3; break;                             //主定时器为增减计数
  default : return(0);                                      //其他情况都是设置参数有误，返回0
  }

  switch(Clk)                                               //为定时器TA选择时钟源
  {
    case 'A': case 'a':     TA0CTL|=TASSEL_1; break;        //ACLK
    case 'S': case 's':     TA0CTL|=TASSEL_2; break;        //SMCLK
    case 'E':                   TA0CTL|=TASSEL_0; break;    //外部输入(TACLK)
    case 'e':                   TA0CTL|=TASSEL_3; break;    //外部输入(TACLK取反)
    default :  return(0);                                   //设置参数有误，返回0
  }
  switch(Div)                                               //为定时器TA选择分频系数
  {
    case 1:   TA0CTL|=ID_0; break;                          //1
    case 2:   TA0CTL|=ID_1; break;                          //2
    case 4:   TA0CTL|=ID_2; break;                          //4
    case 8:   TA0CTL|=ID_3; break;                          //8
    default :  return(0);                                   //设置参数有误，返回0
  }
  switch(Mode1)                                             //设置PWM通道1的输出模式。
  {
     case 'F':  case 'f':
              TA0CCTL1 = OUTMOD_7;
              TA01_SET;
              break;
     case 'B':  case 'b':
              TA0CCTL1 = OUTMOD_3;
              TA01_SET;
              break;
     case 'D': case'd':
         TA0CCTL1 = OUTMOD_6;
             TA01_SET;
             break;
      case '0':case 0:                                      //如果设置为禁用
             TA01_OFF;                                      //TA0.1恢复为普通IO口
              break;
     default :  return(0);                                  //设置参数有误，返回0
  }
  switch(Mode2)                                             //设置PWM通道2的输出模式。
  {
      case 'F':  case 'f':
              TA0CCTL2 = OUTMOD_7;
              TA02_SET;  break;
       case 'B':    case 'b':
              TA0CCTL2 = OUTMOD_3;
              TA02_SET;
                break;
       case 'D': case 'd':
               TA0CCTL2 = OUTMOD_2;
               TA02_SET;
               break;
       case '0':case 0:                                     //如果设置为禁用
                TA02_OFF;                                   //TA0.1恢复为普通IO口
                break;
       default :  return(0);                                //设置参数有误，返回0
    }
  return(1);
}
/****************************************************************************************
* 名       称：TA0_PWM_SetPeriod()
* 功       能：设置PWM发生器的周期
* 入口参数：Channel: TA0=0, TA1=1
*                     Period:周期(0~65535) 时钟个数
* 出口参数：1：设置成功     0：设置失败
* 说       明 : 普通PWM与带死区PWM周期相差一倍
* 范       例 : TA0_PWM_SetPeriod(500)设置PWM方波周期为500或1000个时钟周期
 ****************************************************************************************/
char TA0_PWM_SetPeriod(unsigned int Period)
{
    if (Period>65535)   return(0);
     TA0CCR0 = Period;
    return(1);
}

/*****************************************************************************************
//此函数有问题，Duty它不是占空比，实际上是CCRx的值
 ****************************************************************************************/
char TA0_PWM_SetPermill(char Channel,unsigned int Duty)
{
    unsigned char Mod = 0;

    switch (Channel)                                    //先判断出通道的工作模式
        {
    case 1:
        Mod = (TA0CCTL1& 0x00e0)>>5;        break;      //读取输出模式，OUTMOD0位于5-7位
    case 2:
        Mod = (TA0CCTL2 & 0x00e0)>>5;   break;          //读取输出模式，OUTMOD1位于5-7位
    default:    return(0);
        }

    switch(Mod)                                         //根据模式设定TACCRx
        {
    case 2: case 6:       /*死区模式2,6时，需要判断修正死区时间，且同时设定TA0CCR1/2 的值*/
        {
            TA0CCR1 = Duty;
            TA0CCR2 = TA0CCR1-Dead;                 break;
        }
    case 7: if(Channel==1) TA0CCR1 = Duty;
            if(Channel==2) TA0CCR2 = Duty;          break;
    case 3: if(Channel==1) TA0CCR1 = TA0CCR0-Duty;
            if(Channel==2) TA0CCR2 = TA0CCR0-Duty;  break;
        default: return(0);
        }
    return (1);
    }
