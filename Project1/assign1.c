#define _GNU_SOURCE
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <fcntl.h>
#include "assign1.h"



/*********************************************************************
 *
 * These C functions use patterns and functionality often found in
 * operating system code. Your job is to implement them. Of course you
 * should write test cases. However, do not hand in your test cases
 * and (especially) do not hand in a main() function since it will
 * interfere with our tester.
 *
 * Additional requirements on all functions you write:
 *
 * - you may not refer to any global variables
 *
 * - you may not call any functions except those specifically
 *   permitted in the specification
 *
 * - your code must compile successfully on CADE lab Linux
 *   machines when using:
 *
 * /usr/bin/gcc -O2 -fmessage-length=0 -pedantic-errors -std=c99 -Werror -Wall
 *-Wextra -Wwrite-strings -Winit-self -Wcast-align -Wcast-qual -Wpointer-arith
 *-Wstrict-aliasing -Wformat=2 -Wmissing-include-dirs -Wno-unused-parameter
 *-Wshadow -Wuninitialized -Wold-style-definition -c assign1.c
 *
 * NOTE 1: Some of the specifications below are specific to 64-bit
 * machines, such as those found in the CADE lab.  If you choose to
 * develop on 32-bit machines, some quantities (the size of an
 * unsigned long and the size of a pointer) will change. Since we will
 * be grading on 64-bit machines, you must make sure your code works
 * there.
 *
 * NOTE 2: You should not need to include any additional header files,
 * but you may do so if you feel that it helps.
 *
 * HANDIN: submit your finished file, still called assign.c, in Canvas
 *
 *
 *********************************************************************/

/*********************************************************************
 *
 * byte_sort()
 *
 * specification: byte_sort() treats its argument as a sequence of
 * 8 bytes, and returns a new unsigned long integer containing the
 * same bytes, sorted numerically, with the smaller-valued bytes in
 * the lower-order byte positions of the return value
 *
 * EXAMPLE: byte_sort (0x0403deadbeef0201) returns 0xefdebead04030201
 *
 *********************************************************************/

unsigned long byte_sort(unsigned long arg) {
  unsigned char *p = (unsigned char*)&arg;
  int i, j;
  unsigned char temp;
  for (i = 0; i < 8; i++){
    for (j = 0; j < 8; j++){
      if(p[j] < p[i]){
        temp = p[i];
        p[i] = p[j];
        p[j] = temp;
      }
    }
  }

  unsigned long l = ((((unsigned long) p[0]) << 56) | (((unsigned long)p[1]) << 48) + ((unsigned long)p[2] << 40) + ((unsigned long)p[3] << 32) + ((unsigned long)p[4] << 24) + ((unsigned long)p[5] << 16) + ((unsigned long)p[6] << 8) + (unsigned long)p[7]);

  return l;
}

/*********************************************************************
 *
 * nibble_sort()
 *
 * specification: nibble_sort() treats its argument as a sequence of
 * 16 4-bit numbers, and returns an unsigned long integer containing
 * the same nibbles, sorted numerically, with smaller-valued nibbles
 * towards the "small end" of the unsigned long value that you return
 *
 * the fact that nibbles and hex digits correspond should make it easy to
 * verify that your code is working correctly
 *
 * EXAMPLE: nibble_sort (0x0403deadbeef0201) returns 0xfeeeddba43210000
 *
 *********************************************************************/

unsigned long nibble_sort(unsigned long arg) {
  unsigned char *p = (unsigned char*)&arg;
  int i, j;
  unsigned char left_i_temp, right_i_temp, left_j_temp, right_j_temp, swap_temp ;
  //Loop sorts between each pair of 4-bit values of a long
  for (i = 0; i < 8; i++){
    left_i_temp = p[i] >> 4;
    right_i_temp = p[i] & 0x0F;

    if(left_i_temp < right_i_temp){
      swap_temp = left_i_temp;
      left_i_temp = right_i_temp;
      right_i_temp = swap_temp;
      p[i] = ((left_i_temp << 4) + right_i_temp);
    }
  }
  //Loop sorts every pair of 4-bit values
  for(i = 0; i < 8; i++){
    left_i_temp = p[i] >> 4;
    right_i_temp = p[i] & 0x0F;
    for (j = i+1; j < 8; j++){
      left_j_temp = p[j] >> 4;
      right_j_temp = p[j] & 0x0F;
      //Sorts left 4 bits for i,j
      if(left_j_temp < left_i_temp){
        swap_temp = left_j_temp;
        left_j_temp = left_i_temp;
        left_i_temp = swap_temp;
        p[i] = ((left_i_temp << 4) + right_i_temp);
        p[j] = ((left_j_temp << 4) + right_j_temp);
      }
      //Sorts left 4 bits of j with right 4 bits of i
      if(left_j_temp < right_i_temp){
        swap_temp = left_j_temp;
        left_j_temp = right_i_temp;
        right_i_temp = swap_temp;
        p[i] = ((left_i_temp << 4) + right_i_temp);
        p[j] = ((left_j_temp << 4) + right_j_temp);
      }
      //Sorts right 4 bits of j with left 4 bits of i
      if(right_j_temp < left_i_temp){
        swap_temp = right_j_temp;
        right_j_temp = left_i_temp;
        left_i_temp = swap_temp;
        p[i] = ((left_i_temp << 4) + right_i_temp);
        p[j] = ((left_j_temp << 4) + right_j_temp);
      }
      //Sorts right 4 bits of j with right 4 bits of i
      if(right_j_temp < right_i_temp){
        swap_temp = right_j_temp;
        right_j_temp = right_i_temp;
        right_i_temp = swap_temp;
        p[i] = ((left_i_temp << 4) + right_i_temp);
        p[j] = ((left_j_temp << 4) + right_j_temp);
      }
      //Compare both 4 bit values of i, since this position of i will never be visited again
      if(left_i_temp < right_i_temp){
        swap_temp = left_i_temp;
        left_i_temp = right_i_temp;
        right_i_temp = swap_temp;
        p[i] = ((left_i_temp << 4) + right_i_temp);
      }
    }
  }
  unsigned long l = ((((unsigned long) p[7]) << 56) | (((unsigned long)p[6]) << 48) + ((unsigned long)p[5] << 40) + ((unsigned long)p[4] << 32) + ((unsigned long)p[3] << 24) + ((unsigned long)p[2] << 16) + ((unsigned long)p[1] << 8) + (unsigned long)p[0]);
  return l;
}

/*********************************************************************
 *
 * name_list()
 *
 * specification: allocate and return a pointer to a linked list of
 * struct elts
 *
 * - the first element in the list should contain in its "val" field the first
 *   letter of your first name; the second element the second letter, etc.;
 *
 * - the last element of the linked list should contain in its "val" field
 *   the last letter of your first name and its "link" field should be a null
 *   pointer
 *
 * - each element must be dynamically allocated using a malloc() call
 *
 * - if any call to malloc() fails, your function must return NULL and must also
 *   free any heap memory that has been allocated so far; that is, it must not
 *   leak memory when allocation fails
 *
 *********************************************************************/

/* struct elt { */
/*   char val; */
/*   struct elt *link; */
/* }; */

struct elt *name_list(void) {
  struct elt *value = NULL;
  value = malloc(sizeof(struct elt));
  if(value == NULL){
    return NULL;
  }
  value->val = 'R';
  value->link = malloc(sizeof(struct elt));
  if(value->link == NULL){
    free(value);
    return NULL;
  }
  value->link->val = 'y';
  value->link->link = malloc(sizeof(struct elt));
  if(value->link->link == NULL){
    free(value->link);
    free(value);
    return NULL;
  }
  value->link->link->val = 'a';
  value->link->link->link = malloc(sizeof(struct elt));
  if(value->link->link->link == NULL){
    free(value->link->link);
    free(value->link);
    free(value);
    return NULL;
  }
  value->link->link->link->val = 'n';
  value->link->link->link->link = NULL;
  return value;
}

/*********************************************************************
 *
 * convert()
 *
 * specification: depending on the value of "mode", print "value" as
 * octal, binary, or hexidecimal to stdout, followed by a newline
 * character
 *
 * extra requirement 1: output must be via putc()
 *
 * extra requirement 2: print nothing if "mode" is not one of OCT,
 * BIN, or HEX
 *
 * extra requirement 3: all leading/trailing zeros should be printed
 *
 * EXAMPLE: convert (HEX, 0xdeadbeef) should print
 * "00000000deadbeef\n" (including the newline character but not
 * including the quotes)
 *
 *********************************************************************/

//enum format_t { OCT = 66, BIN, HEX };

void convert(enum format_t mode, unsigned long value) {
  if(mode != 66 && mode != 67 && mode != 68){
    printf("No value given\n");
    return;
  }
  unsigned long newval;
  int i;
  if(mode == OCT){
    for(i = 21; i >= 0; i--){
      newval = 0x0000000000000007 & (value >> i*3);
      char c = newval + 48;
      putc(c, stdout);
    }
    putc('\n', stdout);
  }
  if(mode == BIN){
    for(i = 63; i >= 0; i--){
      newval = 0x0000000000000001 & (value >> i);
      char c = newval + 48;
      putc(c, stdout);
    }
    putc('\n', stdout);

  }
  if(mode == HEX){
    for(i = 15; i >=0; i--){
      newval = 0x000000000000000F & (value >> i*4);
      if(newval < 10){
        char c = newval + 48;
        putc(c, stdout);
      }
      else{
        char c = newval + 87;
        putc(c, stdout);
      }

    }
    putc('\n', stdout);
  }


}

/*********************************************************************
 *
 * draw_me()
 *
 * this function creates a file called me.txt which contains an ASCII-art
 * picture of you (it does not need to be very big). the file must (pointlessly,
 * since it does not contain executable content) be marked as executable.
 *
 * extra requirement 1: you may only call the function syscall() (type "man
 * syscall" to see what this does)
 *
 * extra requirement 2: you must ensure that every system call succeeds; if any
 * fails, you must clean up the system state (closing any open files, deleting
 * any files created in the file system, etc.) such that no trash is left
 * sitting around
 *
 * you might be wondering how to learn what system calls to use and what
 * arguments they expect. one way is to look at a web page like this one:
 * http://blog.rchapman.org/post/36801038863/linux-system-call-table-for-x86-64
 * another thing you can do is write some C code that uses standard I/O
 * functions to draw the picture and mark it as executable, compile the program
 * statically (e.g. "gcc foo.c -O -static -o foo"), and then disassemble it
 * ("objdump -d foo") and look at how the system calls are invoked, then do
 * the same thing manually using syscall()
 *
 *********************************************************************/

void draw_me(void){
    char art[] = "\n ( )  \n\\ | /\n \\|/  \n  |  \n / \\ \n/   \\\nI am not an artist. \n";
    int asciifile = syscall(SYS_open, "./me.txt", O_CREAT|O_WRONLY, 0777);
    if(asciifile < 0){
        return;
    }
    int writeascii = syscall(SYS_write, asciifile, art, sizeof(art)-1);
    if(writeascii < 0){
        syscall(SYS_close, asciifile);
        syscall(SYS_unlink, "./me.txt");
        return;
    }
    //Close succeeds even when it fails
    int closeascii = syscall(SYS_close, asciifile);
    if(closeascii < 0){
        return;
    }

}
