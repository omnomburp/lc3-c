#include "util.h"
#include "lc3_instructions.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "core.h"

void op_add(u16 instruction) {
  // Get the destination register
  u16 destination_register = (instruction >> 9) & 0x7;

  // Get the source 1 register
  u16 source_register_1 = (instruction >> 6) & 0x7;

  // Get the immediate mode flag
  u16 immediate = (instruction >> 5) & 0x1;

  if (immediate) {
    // Sign extend the immediate value
    u16 imm5 = sign_extend(instruction & 0x1F, 5);
    registers[destination_register] = registers[source_register_1] + imm5;
  }
  else {
    u16 source_register_2 = instruction & 0x7;
    registers[destination_register] = registers[source_register_1] + registers[source_register_2];
  }
  // Update the flags
  update_flags(destination_register);
}

void op_and(u16 instruction) {
  u16 destination_register = (instruction >> 9) & 0x7;

  u16 source_register_1 = (instruction >> 6) & 0x7;

  u16 immediate = (instruction >> 5) & 0x1;

  if (immediate) {
    u16 imm5 = sign_extend(instruction & 0x1F, 5);
    registers[destination_register] = registers[source_register_1] & imm5;
  }
  else {
    u16 source_register_2 = instruction & 0x7;
    registers[destination_register] = registers[source_register_1] & registers[source_register_2];
  }
  update_flags(destination_register);
}

void op_branch(u16 instruction) {
  u16 pc_offset = sign_extend(instruction & 0x1FF, 9);
  u16 conditional_flags = (instruction >> 9) & 0x7;

  if (conditional_flags & registers[R_COND]) {
    registers[R_PC] += pc_offset;
  }
}

void op_jump(u16 instruction) {
  u16 base_register = (instruction >> 6) & 0x7;
  registers[R_PC] = registers[base_register];
}

void op_jump_register(u16 instruction) {
  u16 pc_offset = sign_extend(instruction & 0x7FF, 11);
  u16 long_flag = (instruction >> 11) & 0x1;
  registers[R_R7] = registers[R_PC];

  if (long_flag) {
    registers[R_PC] += pc_offset;
  }
  else {
    u16 base_register = (instruction >> 6) & 0x7;
    registers[R_PC] = registers[base_register];
  }
}

void op_load(u16 instruction) {
  u16 destination_register = (instruction >> 9) & 0x7;
  u16 pc_offset = sign_extend(instruction & 0x1FF, 9);
  u16 value = mem_read(registers[R_PC] + pc_offset);

  registers[destination_register] = value;

  update_flags(destination_register);
}

void op_load_indirect(u16 instruction) {
  u16 destination_register = (instruction >> 9) & 0x7;
  u16 pc_offset = sign_extend(instruction & 0x1FF, 9);
  u16 value = mem_read(mem_read(registers[R_PC] + pc_offset));

  registers[destination_register] = value;

  update_flags(destination_register);
}

void op_load_register(u16 instruction) {
  u16 destination_register = (instruction >> 9) & 0x7;
  u16 baseRegister = (instruction >> 6) & 0x7;
  u16 pc_offset = sign_extend(instruction & 0x3F, 6);

  registers[destination_register] = mem_read(registers[baseRegister] + pc_offset);

  update_flags(destination_register);
}

void op_load_effective_address(u16 instruction) {
  u16 destination_register = (instruction >> 9) & 0x7;
  u16 pc_offset = sign_extend(instruction & 0x1FF, 9);

  registers[destination_register] = registers[R_PC] + pc_offset;

  update_flags(destination_register);
}

void op_not(u16 instruction) {
  u16 destination_register = (instruction >> 9) & 0x7;
  u16 source_register_1 = (instruction >> 6) & 0x7;

  registers[destination_register] = ~registers[source_register_1];

  update_flags(destination_register);
}

void op_store(u16 instruction) {
  u16 source = (instruction >> 9) & 0x7;
  u16 pc_offset = sign_extend(instruction & 0x1FF, 9);

  mem_write(registers[R_PC] + pc_offset, registers[source]);
}

void op_store_indirect(u16 instruction) {
  u16 source = (instruction >> 9) & 0x7;
  u16 pc_offset = sign_extend(instruction & 0x1FF, 9);
  u16 address = mem_read(registers[R_PC] + pc_offset);

  mem_write(address, registers[source]);
}

void op_store_register(u16 instruction) {
  u16 source = (instruction >> 9) & 0x7;
  u16 base_register = (instruction >> 6) & 0x7;
  u16 offset = sign_extend(instruction & 0x3F, 6);
  u16 address = registers[base_register] + offset;

  mem_write(address, registers[source]);
}

void trap_getc() {
  registers[R_R0] = (u16)getchar();
}

void trap_halt() {
  puts("HALT");
  fflush(stdout);
  exit(0);
}

void trap_in() {
  printf("Enter a character: ");
  registers[R_R0] = (u16)getchar();
}

void trap_out() {
  putc((char)registers[R_R0], stdout);
  fflush(stdout);
}

void trap_puts() {
  u16 *c = memory + registers[R_R0];

  while (*c) {
    putc((char)*c, stdout);
    ++c;
  }

  fflush(stdout);
}

void trap_putsp() {
  u16 *c = memory + registers[R_R0];
  while (*c) {
      char c1 = (*c) & 0xFF;
      putc(c1, stdout);

      char c2 = (*c) >> 8;
      if (c2) putc(c2, stdout);
      ++c;
  }
  fflush(stdout);
}