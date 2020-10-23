/*
 * user.h
 *
 *  Created on: 2018-02-16 18:00
 *      Author: Jack Chen <redchenjs@live.com>
 */

#ifndef INC_SYSTEM_USER_H_
#define INC_SYSTEM_USER_H_

extern void user_init(void);
extern void key_dect(void);
extern void mode_choose_interface(void);
extern void high_time();
extern void cal_distance();

extern int REdge,FEdge;
extern int overflow;
extern unsigned char show_flag;
extern float ON_Period;
#endif /* INC_SYSTEM_USER_H_ */
