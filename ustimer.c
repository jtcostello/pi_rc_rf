

// from https://yosh.ke.mu/article/raspberry_pi_getting_time

#include "ustimer.h"

// probably don't need all of these
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

volatile unsigned *TIMER_registers;





unsigned int getUSTime() {
    return TIMER_registers[1];
}




void initUScounter() {
    /* open /dev/mem */
    int TIMER_memFd;
    if ((TIMER_memFd = open("/dev/mem", O_RDWR/*|O_SYNC*/) ) < 0)
    {
        printf("can't open /dev/mem - need root ?\n");
        exit(-1);
    }

    /* mmap BCM System Timer */
    void *TIMER_map = mmap(
        NULL,
        4096, /* BLOCK_SIZE */
        PROT_READ /*|PROT_WRITE*/,
        MAP_SHARED,
        TIMER_memFd,
        BCM2708_ST_BASE
    );

    close(TIMER_memFd);

    if (TIMER_map == MAP_FAILED)
    {
        printf("mmap error %d\n", (int)TIMER_map);
        exit(-1);
    }
    TIMER_registers = (volatile unsigned *)TIMER_map;
}



// // another option: gettimeofday can do microsecond precision
// #include <sys/time.h>
// #include <stdio.h>
// #include <stdlib.h>

// int main(int argc, char **argv)
// {
// int delay = 500;
// struct timeval start, stop;
// if (argc > 1) delay = atoi(argv[1]);
// gettimeofday(&start, NULL);
// usleep(delay);
// gettimeofday(&stop, NULL);
// printf("Elapsed %d\n", (int)((stop.tv_sec-start.tv_sec)*1000000ULL+(stop.tv_usec-start.tv_usec)));
// return 0;
// }