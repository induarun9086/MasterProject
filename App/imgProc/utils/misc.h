/*
 * misc.h
 *
 *  Created on: 31-Oct-2015
 *      Author: Indumathi Duraipandian
 */

#ifndef UTILS_MISC_H_
#define UTILS_MISC_H_

#include <sys/stat.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
#include <math.h>

typedef unsigned char uint8;
typedef unsigned int uint32;
typedef unsigned int uint16;

typedef signed char sint8;
typedef signed int sint32;
typedef signed int sint16;

uint32 mod (sint32 a, sint32 b);

struct pixel_pos {
    int x;
    int y;
};

typedef struct pixel_pos POS; 

#endif /* UTILS_MISC_H_ */
