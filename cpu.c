#include "cpu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * Load the binary bytes from a .ls8 source file into a RAM array
 */
void cpu_load(struct cpu *cpu, int num_args, char *file_name) {
  if (num_args != 2) {
    printf("usage: ls8 <rom_file>.ls8");
    exit(1);
  }
  FILE *file = fopen(file_name, "r");
  char *c;

  while (1) {
    fgets(c, sizeof(char) * 30, file);
    if (feof(file)) {
      break;
    }
    char *endptr;
    cpu->ram[cpu->PC] = (unsigned char)strtol(c, endptr, 2);
    cpu->PC++;
  }
  cpu->PC = 0;
}

// void handle_PUSH(struct cpu *cpu, unsigned char operandA, unsigned char
// operandB) {

// }

/**
 * ALU
 */
void alu(struct cpu *cpu, enum alu_op op, unsigned char regA,
         unsigned char regB) {
  switch (op) {
  case ALU_MUL:
    cpu->registers[regA] = cpu->registers[regA] * cpu->registers[regB];
    break;
  case ALU_ADD:
    regA += regB;
    break;
  case ALU_CMP:
    if (cpu->registers[regA] == cpu->registers[regB]) {
      cpu->FL = cpu->FL | 0b00000001;
    }
  case ALU_DEC:
    cpu->registers[regA]--;
  case ALU_INC:
    cpu->registers[regA]++;
  }
}

/**
 * Run the CPU
 */
int cpu_ram_read(struct cpu *cpu, int source) { return cpu->ram[source]; }

void cpu_ram_write(struct cpu *cpu, int destination, int source) {
  cpu->ram[destination] = source;
}

void ldi(struct cpu *cpu, unsigned char operandA, unsigned char operandB) {
  cpu->registers[operandA] = operandB;
}

void prn(struct cpu *cpu, unsigned char operandA) {
  printf("%d", cpu->registers[operandA]);
}

void push(struct cpu *cpu, unsigned char operandA, unsigned char operandB) {
  cpu->SP -= 1;
  cpu->ram[cpu->SP] = cpu->registers[operandA];
}

void mul(struct cpu *cpu, unsigned char operandA, unsigned char operandB) {
  alu(cpu, ALU_MUL, operandA, operandB);
}

void pop(struct cpu *cpu, unsigned char operandA, unsigned char operandB) {
  cpu->registers[operandA] = cpu_ram_read(cpu, cpu->SP);
  cpu->SP += 1;
}

void call(struct cpu *cpu, unsigned char operandA, unsigned char operandB) {
  cpu->SP -= 1;
  cpu->ram[cpu->SP] = cpu->PC;
  cpu->PC = cpu->registers[operandA];
}

void ret(struct cpu *cpu, unsigned char operandA, unsigned char operandB) {
  cpu->PC = cpu->ram[cpu->SP];
  cpu->SP += 1;
}

void add(struct cpu *cpu, unsigned char operandA, unsigned char operandB) {
  alu(cpu, ALU_ADD, operandA, operandB);
}

void and (struct cpu * cpu, unsigned char operandA, unsigned char operandB) {
  cpu->registers[operandA] =
      cpu->registers[operandA] & cpu->registers[operandB];
}

void cmp(struct cpu *cpu, unsigned char operandA, unsigned char operandB) {
  alu(cpu, ALU_CMP, operandA, operandB);
}

void dec(struct cpu *cpu, unsigned char operandA, unsigned char operandB) {
  alu(cpu, ALU_CMP, operandA, operandB);
}

void inc(struct cpu *cpu, unsigned char operandA, unsigned char operandB) {
  alu(cpu, ALU_INC, operandA, operandB);
}

void jmp(struct cpu *cpu, unsigned char operandA, unsigned char operandB) {
  cpu->PC = cpu->registers[operandA];
}

void jeq(struct cpu *cpu, unsigned char operandA, unsigned char operandB) {
  if (cpu->FL >> 7 == 1) {
    cpu->PC = cpu->registers[operandA];
  }
}

void jne(struct cpu *cpu, unsigned char operandA, unsigned char operandB) {}

void cpu_run(struct cpu *cpu) {
  // op *handle_ops[36] = {
  //   ldi,
  //   prn,
  //   push,
  //   mul
  // };
  void (*op[35])(struct cpu * cpu, unsigned char operandA,
                 unsigned char operandB) = {ldi, prn, mul, push, pop, call, ret,
                                            add, and, dec, jmp,  jeq, jne};
  int running = 1;
  unsigned char operandA;
  unsigned char operandB;

  while (running) {
    cpu->IR = cpu->ram[cpu->PC];
    int new_pc = ((cpu->IR >> 6) & 0b11) + 1;
    if (cpu->IR >= 64) {
      operandA = cpu_ram_read(cpu, cpu->PC + 1);
    }
    if (cpu->IR >= 128) {
      operandB = cpu_ram_read(cpu, cpu->PC + 2);
    }
    cpu->PC += new_pc;
    switch (cpu->IR) {
    case LDI:
      op[0](cpu, operandA, operandB);
      break;
    case PRN:
      op[1](cpu, operandA, operandB);
      break;
    case MUL:
      op[2](cpu, operandA, operandB);
      break;
    case PUSH:
      op[3](cpu, operandA, operandB);
      break;
    case POP:
      op[4](cpu, operandA, operandB);
      break;
    case CALL:
      op[5](cpu, operandA, operandB);
      break;
    case RET:
      op[6](cpu, operandA, operandB);
      break;
    case ADD:
      op[7](cpu, operandA, operandB);
      break;
    case AND:
      op[8](cpu, operandA, operandB);
      break;
    case DEC:
      op[9](cpu, operandA, operandB);
      break;
    case JMP:
      op[10](cpu, operandA, operandB);
      break;
    case JEQ:
      op[11](cpu, operandA, operandB);
      break;
    case JNE:
      op[12](cpu, operandA, operandB);
      break;
    case HLT:
      running = 0;
      break;
    }
  }
}

/**
 * Initialize a CPU struct
 */
void cpu_init(struct cpu *cpu) {
  memset(cpu->ram, 0, 256 * sizeof(unsigned char));
  memset(cpu->registers, 0, 8 * sizeof(unsigned char));
  cpu->registers[7] = 0xF4;
  cpu->PC = 0;
  cpu->IR = 0;
  cpu->MAR = 0;
  cpu->MDR = 0;
  cpu->FL = 0;
  cpu->SP = 0xF4;
}
