#include <stdlib.h>
#include "core.h"
#include "util.h"
#include "lc3_instructions.h"

static inline void trap(u16 instr) {
    u16 trap_code = instr & 0xFF;
    switch (trap_code) {
        case TRAP_GETC: trap_getc(); break;
        case TRAP_OUT: trap_out(); break;
        case TRAP_PUTS: trap_puts(); break;
        case TRAP_IN: trap_in(); break;
        case TRAP_PUTSP: trap_putsp(); break;
        case TRAP_HALT: trap_halt(); break;
    }
}

static inline void fetch_and_execute() {
  u16 instruction = mem_read(registers[R_PC]++);
  u16 opcode = instruction >> 12;

  switch (opcode) {
  case OP_ADD:
    op_add(instruction);      
    break;
  case OP_AND:
    op_and(instruction);
    break;
  case OP_NOT:
    op_not(instruction);
    break;
  case OP_BR:
    op_branch(instruction);
    break;
  case OP_JMP:
    op_jump(instruction);
    break;
  case OP_JSR:
    op_jump_register(instruction);
    break;
  case OP_LD:
    op_load(instruction);
    break;
  case OP_LDI:
    op_load_indirect(instruction);
    break;
  case OP_LDR:
    op_load_register(instruction);
    break;
  case OP_LEA:
    op_load_effective_address(instruction);
    break;
  case OP_ST:
    op_store(instruction);
    break;
  case OP_STI:
    op_store_indirect(instruction);
    break;
  case OP_STR:
    op_store_register(instruction);
    break;
  case OP_TRAP:
    trap(instruction);
    break;
  case OP_RES:
  case OP_RTI:
    abort();
    break;
  default:
    printf("BAD OPCODE\n");
    break;
  }
}

int main(int argc, const char* argv[]) {
    if (argc < 2) {
        printf("lc3 [image-file1] ...\n");
        exit(2);
    }

    for (int j = 1; j < argc; ++j) {
        if (!read_image(argv[j])) {
            printf("failed to load image: %s\n", argv[j]);
            exit(1);
        }
    }

    signal(SIGINT, handle_interrupt);
    disable_input_buffering();

    registers[R_COND] = FL_ZRO;

    enum { PC_START = 0x3000 };
    registers[R_PC] = PC_START;

    while (1) {
        fetch_and_execute();
    }

    restore_input_buffering();

    return 0;
}