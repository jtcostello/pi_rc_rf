CFLAGS=-Wall -fno-strict-aliasing -fwrapv -Wstrict-prototypes -std=c99 -lm
all: 
	gcc pi_rc_rf.c radio.c radio.h ustimer.h ustimer.c delay.h delay.c -o rcrf -w

clean:
	rm -f rcrf *.o