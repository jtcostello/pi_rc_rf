// from https://yosh.ke.mu/article/raspberry_pi_getting_time

#ifndef _USTIMER_H_
#define _USTIMER_H_

#define BCM2708_ST_BASE  0x7E003000 //JC: this one might be wrong - 0x20003000 /* BCM 2835 System Timer */

unsigned int getUSTime();

void initUScounter();

#endif