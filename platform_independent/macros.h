#ifndef MACROS_H
#define MACROS_H

// NOTE: Cannot be used with expressions
#define SWAP(a, b, temp, type) type temp = a; a = b; b = temp;

#define MIN(a, b) ((x) < (y)) ? (x) : (y)

#define MAX(a, b) ((x) > (y)) ? (x) : (y)

#define ABS(a) ((a) < 0) ? -(a) : (a)

#endif
