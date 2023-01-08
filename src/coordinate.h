#ifndef COORDINATE_H
#define COORDINATE_H
typedef struct {
    char lsb, msb;
} twobytes;

typedef union {
    unsigned int i;
    twobytes b;
} coordinate;
#endif