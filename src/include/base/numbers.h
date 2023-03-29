#ifndef __NUMBERS__H
#define __NUMBERS__H

#include <stdbool.h>

int _getHexString(char *buffer, unsigned long hexnumber, bool use_capital);
int _getDecString(char *buffer, long number);
int _getUnsignedDecString(char *buffer, unsigned long number);
int _getNumericString(char *buffer, unsigned long number, int base, bool use_capital);
#endif
