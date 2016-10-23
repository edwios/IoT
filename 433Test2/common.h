/*
 * common setup
 */
#ifndef COMMON_H
#define COMMON_H

#include "application.h"
#define DEBUG

#define LED D7
#define nIRQ D6
#define nSEL A2
#define SDN D5
#define TxNRx D4            // 1 -- TX, 0 -- Rx

#define DEFAULT_SPI_CLOCK_DIV SPI_CLOCK_DIV16

#ifdef DEBUG
#define DEBUG_PRINT(...) Serial.print("DEBUG:"); Serial.printf( __VA_ARGS__ ); Serial.println();
#define ERROR_PRINT(...) Serial.print("ERROR:"); Serial.printf( __VA_ARGS__ ); Serial.println();
#else
#define DEBUG_PRINT(...)
#define ERROR_PRINT(...)
#endif


#endif
