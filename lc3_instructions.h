#ifndef LC3_INSTRUCTIONS_H
#define LC3_INSTRUCTIONS_H

#include "types.h"

void op_add(u16 instruction);
void op_and(u16 instruction);
void op_branch(u16 instruction);
void op_jump(u16 instruction);
void op_jump_register(u16 instruction);
void op_load(u16 instruction);
void op_load_indirect(u16 instruction);
void op_load_register(u16 instruction);
void op_load_effective_address(u16 instruction);
void op_not(u16 instruction);
void op_store(u16 instruction);
void op_store_indirect(u16 instruction);
void op_store_register(u16 instruction);

void trap_getc();
void trap_halt();
void trap_in();
void trap_out();
void trap_puts();
void trap_putsp();

#endif
