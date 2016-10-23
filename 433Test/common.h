/*
 * common setup
 */
#ifndef COMMON_H
#define COMMON_H

#define DEBUG

#define LED D7
#define NIRQPIN D6
#define NSELPIN A2
#define SDNPIN D5
#define DEFAULT_SPI_CLOCK_DIV SPI_CLOCK_DIV4

#ifdef DEBUG
#define DEBUG_PRINT(...) Serial.print("DEBUG:"); Serial.printf( __VA_ARGS__ ); Serial.println();
#else
#define DEBUG_PRINT(...)
#endif

int nIRQ;
int nSEL;
int SDN;
byte FreqSel;

#endif
