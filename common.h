#pragma once
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>

typedef int DATA_MAX;

#ifndef DEBUG
#define DEBUG 0
#endif

typedef enum Status {
    ALLOCATED = 0x01,
    FREE = 0x00
} Status;

typedef enum OUT_MODE {
    STDOUT = 0,
    F_WRITE = 1,
    F_CONCAT = 2
} OUT_MODE;
