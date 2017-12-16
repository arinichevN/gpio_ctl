
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "lib/gpio.h"
#include "lib/app.h"
#define STR_LEN 8

static void print_usage(FILE *stream, char *app_name) {
    fprintf(stream, "Usage:\n\t %s PIN mode [in | out]\n", app_name);
    fprintf(stream, "\t %s PIN pull [up | down | off]\n", app_name);
    fprintf(stream, "\t %s PIN write [low | high]\n", app_name);
    fprintf(stream, "\t %s PIN read\n", app_name);
    fprintf(stream, "\t %s PIN rate [test1 | test2] COUNT\n", app_name);
    fputs("\t\twhere PIN - physical pin number\n", stream);
    fputs("\t\ttest1 - pinLow(), pinHigh() iteration\n", stream);
    fputs("\t\ttest2 - pinRead() iteration\n", stream);
    fputs("\t\tCOUNT - number of iterations\n", stream);
}
#define PRINT_TIME_SPENT(V) struct timespec td = getTimePassed_ts(V);printf("time spent: %ld s %ld ns\n", td.tv_sec, td.tv_nsec);
#define PRINT_INFO printf("waiting for %u iterations on physical pin %d...\n", count, pin);

static void test1(int pin, unsigned int count) {
    PRINT_INFO
    pinModeOut(pin);
    struct timespec t1 = getCurrentTime();
    for (unsigned int i = 0; i < count; i++) {
        pinHigh(pin);
        pinLow(pin);
    }
    PRINT_TIME_SPENT(t1)
}

static void test2(int pin, unsigned int count) {
    PRINT_INFO
    pinModeIn(pin);
    struct timespec t1 = getCurrentTime();
    for (unsigned int i = 0; i < count; i++) {
        int v = pinRead(pin);
    }
    PRINT_TIME_SPENT(t1)
}
#define ARGC_MIN 3
#define CHECK_ARGC(V) if (argc != V) {fprintf(stderr, "%d arguments expected, and got %d\n",V, argc);print_usage(stderr, argv[0]);return (EXIT_FAILURE);}

int main(int argc, char** argv) {
    if (mlockall(MCL_CURRENT | MCL_FUTURE) == -1) {
        perror("memory locking failed");
    }
   // setPriorityMax(SCHED_FIFO);
    if (argc < ARGC_MIN) {
        fprintf(stderr, "%d arguments expected, and got %d\n", ARGC_MIN, argc);
        print_usage(stderr, argv[0]);
        return (EXIT_FAILURE);
    }
    int pin;
    sscanf(argv[1], "%d", &pin);
    if (!checkPin(pin)) {
        fputs("Bad pin\n", stderr);
        return (EXIT_FAILURE);
    }
    if (!gpioSetup()) {
        fputs("gpioSetup failed\n", stderr);
        return (EXIT_FAILURE);
    }
    if (strcmp(argv[2], "mode") == 0) {
        CHECK_ARGC(4)
        if (strcmp(argv[3], "in") == 0) {
            pinModeIn(pin);
        } else if (strcmp(argv[3], "out") == 0) {
            pinModeOut(pin);
        } else {
            fputs("Bad parameter value. You can use: in, out\n", stderr);
            return (EXIT_FAILURE);
        }
    } else if (strcmp(argv[2], "pull") == 0) {
        CHECK_ARGC(4)
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
    } else if (strcmp(argv[2], "write") == 0) {
        CHECK_ARGC(4)
        if (strcmp(argv[3], "low") == 0) {
            pinLow(pin);
        } else if (strcmp(argv[3], "high") == 0) {
            pinHigh(pin);
        } else {
            fputs("Bad parameter value. You can use: low, high\n", stderr);
            return (EXIT_FAILURE);
        }
    } else if (strcmp(argv[2], "read") == 0) {
        CHECK_ARGC(3)
        while (1) {
            int value = pinRead(pin);
            printf("pin %d is %d\r", pin, value);
            fflush(stdout);
            struct timespec rqt = {0, 5000000};
            struct timespec rmt;
            nanosleep(&rqt, &rmt);
        }
    } else if (strcmp(argv[2], "rate") == 0) {
        CHECK_ARGC(5)
                unsigned int count;
        sscanf(argv[4], "%u", &count);
        if (count < 0) {
            fputs("Bad parameter value. You can use: unsigned integer\n", stderr);
            return (EXIT_FAILURE);
        }
        if (strcmp(argv[3], "test1") == 0) {
            test1(pin, count);
        } else if (strcmp(argv[3], "test2") == 0) {
            test2(pin, count);
        } else {
            fputs("Bad parameter value. You can use: test1, test2\n", stderr);
            return (EXIT_FAILURE);
        }

    } else {
        fputs("Bad parameter. You can use: mode, pull, write, read\n", stderr);
        return (EXIT_FAILURE);
    }

    return (EXIT_SUCCESS);
}

