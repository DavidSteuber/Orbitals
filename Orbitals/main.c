//
//  main.c
//  Orbitals
//
//  Created by David Steuber on 6/16/13.
//  Copyright (c) 2013 David Steuber.
//

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "targa.h"

// Constants to deterimine how big the map is and how accurate.
#define PIXELS 2880
#define MAX_ITERATIONS 4000

// Evil global variables to hold the computation state and
// avoid messing with memory management.
double gmarray[PIXELS];
double givec[MAX_ITERATIONS];
double grvec[MAX_ITERATIONS];
int gmap[PIXELS][PIXELS];
int gmaxhits;
uint8_t gimage[PIXELS][PIXELS];

// This is needed after each pixel has been iterated.
void clearvecs(void)
{
    memset(givec, 0, sizeof givec);
    memset(grvec, 0, sizeof grvec);
}

void initgmarray(void)
{
    int i;
    gmaxhits = 0;
    for (i = 0; i < PIXELS; i++) {
        gmarray[i] = (i * 4.0L) / PIXELS - 2.0L;
    }
}

// This should be suppurflous given that globals are zero initialized.
void initgmap(void)
{
    int i,j;
    for (i = 0; i < PIXELS; i++) {
        for (j = 0; j < PIXELS; j++) {
            gmap[i][j] = 0;
        }
    }
}

int indexFordouble(double x)
{
    return (int)(0.25 * (x + 2.0) * PIXELS);
}

// Top left is -2.0L,-2.0L
// Bottom right is 2.0L,2.0L
void incrementGmapPoints(void)
{
    int r,i,p;
    for (p = 0; p < MAX_ITERATIONS; p++) {
        r = indexFordouble(grvec[p]);
        i = indexFordouble(givec[p]);
        if ((r > -1) && (i > -1)) {
            gmap[i][r]++;
            if (gmaxhits < gmap[i][r]) {
                gmaxhits = gmap[i][r];
            }
        }
    }
}

int computePixel(int a, int b)
{
    int i;
    double zr, zr2, zi, zi2, ci, cr;
    zi = ci = gmarray[a];
    zr = cr = gmarray[b];
    clearvecs();
	for (i = 0; i < MAX_ITERATIONS; i++) {
		zr2 = zr * zr;
        zi2 = zi * zi;
		if (zr2 + zi2 > 4.0L) {
			return 0;
		}
		givec[i] = zi = 2 * zr * zi + ci;
		grvec[i] = zr = zr2 - zi2 + cr;
	}
    return 1;
}

double computeMultiplier(void)
{
    return 255.0L / log(gmaxhits);
}

void drawImage(void)
{
    int r,i,k;
    double multiplier = computeMultiplier();
    for (r = 0; r < PIXELS; r++) {
        for (i = PIXELS - 1, k = 0; i >= 0; i--, k++) {
            if(gmap[r][k]){
               gimage[r][i] = (int8_t)round((multiplier * logf(gmap[r][k])));
            }
        }
    }
}

int main(int argc, const char * argv[])
{
    int r,i;
    tga_result result;
    initgmarray();
    initgmap();
    for (i = 0; i < PIXELS; i++) {
        printf("Computing row %d of %d\r", i+1, PIXELS);
        fflush(NULL);
        for (r = 0; r < PIXELS; r++) {
            if (computePixel(r, i)) {
                incrementGmapPoints();
            }
        }
    }
    drawImage();
    result = tga_write_mono("/Users/david/Desktop/orbitals.tga", (uint8_t *)gimage, PIXELS, PIXELS);
    printf("\nDone! Return code = %d\a\n", result);
    return 0;
}

