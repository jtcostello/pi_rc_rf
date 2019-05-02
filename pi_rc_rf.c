


// can try this, but to get more precise timing might need to use dma buffers like pi-fm





#include "radio.h"
#include "nexa.h"

int main(int argc, char **argv){

    uint frequency = 0x374F; // not sure if this should be uint or what

	// setup DMA and fm
	setup_io();
	setup_fm();
        // if (arguments.frequency == 0x374F)
        //     printf("Tune to 144.64 MHz\n");
        // if (arguments.frequency == 0x5000)
        //     printf("Tune to 100 MHz\n");
	ACCESS(CM_GP0DIV) = (0x5a << 24) + frequency; // Tune to 144.64 MHz to get the third harmonic at 433.92 MHz

	// setup u-sec timer
	initUScounter();


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




// setup interrupt to fire every 14.4 ms
int transmitInterrupt() {
	transmit(lrpulse, fbpulse);
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


