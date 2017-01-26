#ifndef ASSIGN1_H
#define ASSIGN1_H
//#define _GNU_SOURCE
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <unistd.h>

enum format_t{ OCT = 66, BIN, HEX};
unsigned long byte_sort(unsigned long arg);
unsigned long nibble_sort(unsigned long arg);
void convert(enum format_t mode, unsigned long value);
 struct elt {
    char val;
    struct elt *link;
};
struct elt *name_list(void);

void draw_me(void);


#endif
