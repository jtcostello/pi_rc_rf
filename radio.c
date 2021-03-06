// https://github.com/s7mx1/pihat



// more info on how raspi calculates/divides clocks (scroll down for equation):
// https://www.tablix.org/~avian/blog/archives/2018/02/notes_on_the_general_purpose_clock_on_bcm2835/


// slightly easier way to do this with python: 
// http://asliceofraspberrypi.blogspot.com/2014/10/generating-radio-frequencies-using.html



/******************************************************************************/
/*                    Fractional-N synthesizer setup                          */
/*                                                                            */
/* This code is nearly a direct copy of the PiFm-project                      */
/* by Oliver Mattos and Oskar Weigl.                                          */
/*                                                                            */
/*                          Jon Petter Skagmo, 2012                           */
/******************************************************************************/

#include "radio.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <math.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

void setup_fm(){

  // this part is in the original pifm, probably not needed unless actually doing DMA
  /* open /dev/mem */
    // if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) {
    //     printf("can't open /dev/mem \n");
    //     exit (-1);
    // }


    // JC: all of 7e refers to memories with hex base 7e... (clk, dma, pwm, etc)
    // see pifm intro for more details
    allof7e = (unsigned *)mmap(
                  NULL,
                  0x01000000,  //len
                  PROT_READ|PROT_WRITE,
                  MAP_SHARED,
                  mem_fd,
                  0x3F000000 //rpi2 base        0x20000000  //base
              );

    if ((int)allof7e==-1) exit(-1);

    SETBIT(GPFSEL0 , 14);
    CLRBIT(GPFSEL0 , 13);
    CLRBIT(GPFSEL0 , 12);

    // 
    struct GPCTL setupword = {6/*SRC*/, 1, 0, 0, 0, 1,0x5a};
    ACCESS(CM_GP0CTL) = *((int*)&setupword);
}



//
// Set up a memory regions to access GPIO
//
void setup_io(){
    // give high priority for more precise timing (added by JC)
    piHiPri(99);

    /* open /dev/mem */
    if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) {
        printf("can't open /dev/mem \n");
        exit (-1);
    }

    /* mmap GPIO */

    // Allocate MAP block
    if ((gpio_mem = malloc(BLOCK_SIZE + (PAGE_SIZE-1))) == NULL) {
        printf("allocation error \n");
        exit (-1);
    }

    // Make sure pointer is on 4K boundary
    if ((unsigned long)gpio_mem % PAGE_SIZE)
        gpio_mem += PAGE_SIZE - ((unsigned long)gpio_mem % PAGE_SIZE);

    // Now map it
    gpio_map = (unsigned char *)mmap(
                   gpio_mem,
                   BLOCK_SIZE,
                   PROT_READ|PROT_WRITE,
                   MAP_SHARED|MAP_FIXED,
                   mem_fd,
                   GPIO_BASE
               );

    if ((long)gpio_map < 0) {
        printf("mmap error %d\n", (int)gpio_map);
        exit (-1);
    }

    // Always use volatile pointer!
    gpio = (volatile unsigned *)gpio_map;
} // setup_io







/* Added functions to enable and disable carrier */
/* JC: system clocks can be divided and outputted to gpio pins. This enables/disables that function.
        (Pifm says you can't disable it (but we are?) so suggests changing the frequency) */
void askHigh() {
	struct GPCTL setupword = {6/*SRC*/, 1, 0, 0, 0, 1,0x5a};	// Set CM_GP0CTL.ENABLE to 1
  ACCESS(CM_GP0CTL) = *((int*)&setupword);
}

void askLow() {
	struct GPCTL setupword = {6/*SRC*/, 0, 0, 0, 0, 1,0x5a};	// Set CM_GP0CTL.ENABLE to 0
    ACCESS(CM_GP0CTL) = *((int*)&setupword);
}


// for a function that actually modulates the frequency see the "modulate" function in pifm









// from wiringPI library: https://github.com/WiringPi/WiringPi/blob/master/wiringPi/piHiPri.c
#include <sched.h>
#include <string.h>


/*
 * piHiPri:
 *  Attempt to set a high priority schedulling for the running program // (99 is highest priority)
 *********************************************************************************
 */

int piHiPri (const int pri) {
  struct sched_param sched ;

  memset (&sched, 0, sizeof(sched)) ;

  if (pri > sched_get_priority_max (SCHED_RR))
    sched.sched_priority = sched_get_priority_max (SCHED_RR) ;
  else
    sched.sched_priority = pri ;

  return sched_setscheduler (0, SCHED_RR, &sched) ;
}