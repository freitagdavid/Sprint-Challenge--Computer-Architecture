#ifndef _CPU_H_
#define _CPU_H_

// Holds all information about the CPU

struct cpu {
  unsigned char registers[8];
  unsigned char ram[256];
  unsigned char PC;
  unsigned char IR;
  unsigned char MAR;
  unsigned char MDR;
  unsigned char FL;
  unsigned char SP;
};

// ALU operations
enum alu_op {
  ALU_MUL,
  ALU_ADD,
  ALU_AND,
  ALU_CMP,
  ALU_DEC,
  ALU_INC
  // Add more here
};

// Instructions

// These use binary literals. If these aren't available with your compiler, hex
// literals should be used.
typedef void op(struct cpu *cpu, unsigned char operandA,
                unsigned char operandB);

#define ADD 0b10100000
#define AND 0b10101000
#define CALL 0b01010000
#define CMP 0b10100111
#define DEC 0b01100110
#define HLT 0b00000001
#define INC 0b01100101
#define JEQ 0b01010101
#define JMP 0b01010100
#define JNE 0b01010110
#define LDI 0b10000010
#define MUL 0b10100010
#define POP 0b01000110
#define PRN 0b01000111
#define PUSH 0b01000101
#define RET 0b00010001

// TODO: more instructions here. These can be used in cpu_run().

// Function declarations

extern void cpu_load(struct cpu *cpu, int num_args, char *args);
extern void cpu_init(struct cpu *cpu);
extern void cpu_run(struct cpu *cpu);

#endif
