
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "lib/gpio.h"
#define STR_LEN 8

int main(int argc, char** argv) {
    int pin;
    if (argc < 3) {
        fputs("Usage: gpio_ctl pin [mode | pull | write | read] [in | out | up | down | off | low | high]\n", stderr);
        fputs(" where pin is physical pin number\n", stderr);
        return (EXIT_FAILURE);
    }
    if (sscanf(argv[1], "%d", &pin) != 1) {
        fputs("Bad pin\n", stderr);
        return (EXIT_FAILURE);
    }
    if (!checkPin(pin)) {
        fputs("Bad pin\n", stderr);
        return (EXIT_FAILURE);
    }
    if(!gpioSetup()){
        fputs("gpioSetup failed\n", stderr);
        return (EXIT_FAILURE);
    }
    if (strcmp(argv[2], "mode") == 0 && argc==4) {
        if (strcmp(argv[3], "in") == 0) {
            pinModeIn(pin);
        } else if (strcmp(argv[3], "out") == 0) {
            pinModeOut(pin);
        } else {
            fputs("Bad parameter value. You can use: in, out\n", stderr);
            return (EXIT_FAILURE);
        }
    } else if (strcmp(argv[2], "pull") == 0 && argc==4) {
        if (strcmp(argv[3], "up") == 0) {
            pinPUD(pin, PUD_UP);
        } else if (strcmp(argv[3], "down") == 0) {
            pinPUD(pin, PUD_DOWN);
        } else if (strcmp(argv[3], "off") == 0) {
            pinPUD(pin, PUD_OFF);
        } else {
            fputs("Bad parameter value. You can use: up, down, off\n", stderr);
            return (EXIT_FAILURE);
        }
    } else if (strcmp(argv[2], "write" ) == 0 && argc==4) {
        if (strcmp(argv[3], "low") == 0) {
            pinLow(pin);
        } else if (strcmp(argv[3], "high") == 0) {
            pinHigh(pin);
        } else {
            fputs("Bad parameter value. You can use: low, high\n", stderr);
            return (EXIT_FAILURE);
        }
    } else if (strcmp(argv[2], "read") == 0) {
        while (1) {
            int value = pinRead(pin);
            printf("pin %d is %d\r", pin, value);
            fflush(stdout);
            struct timespec rqt = {0, 5000000};
            struct timespec rmt;
            nanosleep(&rqt, &rmt);
        }
    } else {
        fputs("Bad parameter. You can use: mode, pull, write, read\n", stderr);
        return (EXIT_FAILURE);
    }

    return (EXIT_SUCCESS);
}

