


// can try this, but to get more precise timing might need to use dma buffers like pi-fm
// also if use DMA don't have to keep looping

// might be useful if use dma for rc commands: https://github.com/bskari/pi-rc/blob/master/pi_pcm.c

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
// probably don't need all these
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <math.h>
#include <fcntl.h>
#include <assert.h>
#include <malloc.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <signal.h>
#include <unistd.h>


#include "radio.h"
//#include "ustimer.h"
#include "delay.h"


#define LOWDELAY 300 // 0.2 ms
#define FREQUENCY 27 // MHz



int main(int argc, char *argv[]){

	// setup gpio mem and fm
	setup_io();
	setup_fm();

	// set initial driving settings
	int lrpulse = atoi(argv[1]); //2100; // 0.5 to 2.1ms
	int fbpulse = atoi(argv[2]); //1300; // 0.5 to 2.1ms

	// turn off transmit if selected
	if (lrpulse == 111) {
		printf("turning off transmitter\n");
		askLow();
		exit(0);
	}

	// set frequency
	/* more info on how raspi calculates/divides clocks (scroll down for equation):
	   https://www.tablix.org/~avian/blog/archives/2018/02/notes_on_the_general_purpose_clock_on_bcm2835/ 
	   -> Base clock is probably 500 MHz, then divide by centerFreq. Shift 12 bits to proper position, round up.
	   additional info on clock base frequencies: 
	   https://raspberrypi.stackexchange.com/questions/1153/what-are-the-different-clock-sources-for-the-general-purpose-clocks */
	float centerFreq = FREQUENCY;
	int centerFreqDivider = (int)((500.0 / centerFreq) * (float)(1<<12) + 0.5);
	ACCESS(CM_GP0DIV) = (0x5a << 24) + centerFreqDivider; // set the GPIO clock frequency divider (0x5a is password)

	

	// variables for timing
	struct timeval tNow, tLong, tEnd;
	unsigned int period = 14400; // usec

	// reset timer
	gettimeofday(&tNow, NULL);
	tLong.tv_sec  = period / 1000000;
	tLong.tv_usec = period % 1000000;
	timeradd(&tNow, &tLong, &tEnd) ;

	while(1) {
		// update drive speed/direction
		// lrpulse += 10;
		// if (lrpulse>2100) {
		// 	lrpulse = 500;
		// }
		// fbpulse += 10;
		// if (fbpulse>2100) {
		// 	fbpulse = 500;
		// }


		// check if time to send pulse
		gettimeofday(&tNow, NULL);
		if (!timercmp(&tNow, &tEnd, <)) {

			// transmit to car
			transmit(lrpulse, fbpulse);
			printf("sending\n");

			//delayMicroseconds(14400);// - (lrpulse+fbpulse + 3*LOWDELAY));


			// reset timer
			tLong.tv_sec  = period / 1000000;
			tLong.tv_usec = period % 1000000;
			timeradd(&tNow, &tLong, &tEnd) ;
		}
		




		// do other computations here (have about 13ms of time)

		// //https://community.hpe.com/t5/Languages-and-Scripting/Timeout-getchar-in-C/td-p/4305162#.XM30qJNKigQ
		// void got_alarm(int sig) {
		// 	fprintf(stderr, "Got signal %d\n", sig);
		// }
		// int main() {
		// 	alarm(5*60);
		// 	signal(SIGALRM, got_alarm);
		// 	int c = getchar();
		// 	printf("getchar returned %x\n", c);
		// 	return 0;
		// }



		
	}

	return 0;
}







void transmit(int lrpulse, int fbpulse){
	// go low to start pulses
	askLow();
	delayMicroseconds(LOWDELAY);

	// 1st pulse - left/right
	askHigh();
	delayMicroseconds(lrpulse);
	askLow();
	delayMicroseconds(LOWDELAY);

	// 2nd pulse - forward/back
	askHigh();
	delayMicroseconds(fbpulse);
	askLow();
	delayMicroseconds(LOWDELAY);

	// return to high
	askHigh();


}


