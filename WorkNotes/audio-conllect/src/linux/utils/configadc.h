#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <ctype.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/mman.h>

#define FATAL do { fprintf(stderr, "Error at line %d, file %s (%d) [%s]\n", \
  __LINE__, __FILE__, errno, strerror(errno)); exit(1); } while(0)

#define MAP_SIZE 4096UL
#define MAP_MASK (MAP_SIZE - 1)

#define XPAR_BRAM_0_BASEADDR 0x82000000U

void Xil_Out32(unsigned long Addr, unsigned long Value);

unsigned long Xil_In32(unsigned long Addr);

// AD4630
unsigned int ad4630_reg_write(unsigned int data);

unsigned int ad4630_reg_read(unsigned int addr);





