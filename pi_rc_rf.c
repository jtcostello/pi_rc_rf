


// can try this, but to get more precise timing might need to use dma buffers like pi-fm



#include "radio.h"
#include "nexa.h"

int main(int argc, char **argv){

	// setup DMA and fm
	setup_io();
	setup_fm();

	// setup u-sec timer
	initUScounter();

	// set frequency
	/* more info on how raspi calculates/divides clocks (scroll down for equation):
	   https://www.tablix.org/~avian/blog/archives/2018/02/notes_on_the_general_purpose_clock_on_bcm2835/ 
	   -> Base clock is probably 500 MHz, then divide by centerFreq. Shift 12 bits to proper position, round up.*/
	float centerFreq = 100.3;
	int centerFreqDivider = (int)((500.0 / centerFreq) * (float)(1<<12) + 0.5);
	ACCESS(CM_GP0DIV) = (0x5a << 24) + centerFreqDivider; // set the GPIO clock frequency divider (0x5a is password)


	int currentTime = getUSTime()
	while(1) {
		if ((getUSTime-currentTime) > 14400) {		// send pulse sequence every 14.4 ms
			// update drive speed/direction
			uint8_t lrpulse = 1000 // 0.5 to 2.1ms
			uint8_t fbpulse = 1000 // 0.5 to 2.1ms
			// transmit to car
			transmit(lrpulse, fbpulse);
		}
		
	}

	return 0;
}





#define LOWDELAY 200 // 0.2 ms

void transmit(uint8_t lrpulse, uint8_t fbpulse){
	// go low to start pulses
	askLow();
	usleep(LOWDELAY);

	// 1st pulse - left/right
    askHigh();
    usleep(lrpulse);
    askLow();
	usleep(LOWDELAY);

	// 2nd pulse - forward/back
    askHigh();
    usleep(fbpulse);
    askLow();
	usleep(LOWDELAY);

	// return to high
	askHigh();
    

}


