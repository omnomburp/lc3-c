#ifndef UTIL_H
#define UTIL_H

#include "types.h"
#include "core.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/termios.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/time.h>
#include <signal.h>
#include <stdlib.h>

struct termios original_tio;

static void disable_input_buffering() {
    tcgetattr(STDIN_FILENO, &original_tio);
    struct termios new_tio = original_tio;
    new_tio.c_lflag &= ~ICANON & ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);
}

static void restore_input_buffering() {
    tcsetattr(STDIN_FILENO, TCSANOW, &original_tio);
}

static inline u16 sign_extend(u16 input, u16 bit_count) {
    if ((input >> (bit_count - 1)) & 1 ) {
        input |= (0xFFFF << bit_count);
    }
    return input;
}

static inline u16 swap16(u16 input) {
    return (input << 8) | (input >> 8);
}

static inline u16 check_key() {
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    return select(1, &readfds, NULL, NULL, &timeout) != 0;
}

static inline u16 mem_read(u16 addr) {
    if (addr == MR_KBSR) {
        if (check_key()) {
            memory[MR_KBSR] = (1 << 15);
            memory[MR_KBDR] = getchar();
        }
        else {
            memory[MR_KBSR] = 0;
        }
    }
    return memory[addr];
}

static inline void mem_write(u16 addr, u16 val) {
    memory[addr] = val;
}

static void read_image_file(FILE *file) {
    u16 origin;
    fread(&origin, sizeof(origin), 1, file);
    origin = swap16(origin);

    u16 max_read = MAX_MEMORY - origin;
    u16 *ptr = memory + origin;
    size_t read = fread(ptr, sizeof(u16), max_read, file);

    while (read-- > 0) {
        *ptr = swap16(*ptr);
        ++ptr;
    }
}

static u16 read_image(const char* image_path) {
    FILE *file = fopen(image_path, "rb");

    if (!file) {
        return 0;
    }

    read_image_file(file);
    fclose(file);
    return 1;
}

static void handle_interrupt(int signal) {
    restore_input_buffering();
    printf("\n");
    exit(-2);
}

static inline void update_flags(uint16_t r) {
  if (registers[r] == 0)
  {
      registers[R_COND] = FL_ZRO;
  }
  else if (registers[r] >> 15)
  {
      registers[R_COND] = FL_NEG;
  }
  else
  {
      registers[R_COND] = FL_POS;
  }
}

#endif