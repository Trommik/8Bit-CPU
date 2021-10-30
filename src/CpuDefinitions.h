/*
 * Here are all cpu specific constants defined. 
 * 
 * Like all different signals in the control word, the 
 * values for each differnt instruction and the ALU flags.
 * 
 */

#ifndef CPU_DEFINITIONS
#define CPU_DEFINITIONS

// ##############################################
// Here are all different signals in the control word defined.
// ##############################################
#define C_INV 0b0001101100000010  // Invert bitmask, all set bits will be inverted (Active LOW)
              
#define C_HLT 0b0000000000000001  // Halt clock
#define C_JMP 0b0000000000000010  // Jump (program counter in)
#define C_CE  0b0000000000000100  // Program counter enable
#define C_AI  0b0000000000001000  // A register in
#define C_BI  0b0000000000010000  // B register in
// SPARE      
// SPARE
#define C_RI  0b0000000010000000  // Random access memory in
#define C_MI  0b0000000100000000  // Memory address register in
#define C_FI  0b0000001000000000  // Flags register in
#define C_SU  0b0000010000000000  // ALU subtract
#define C_IRI 0b0000100000000000  // Instruction register in
#define C_IOI 0b0001000000000000  // Instruction-Op register in

#define C_CO  0b1000000000000000  // Program counter out
#define C_AO  0b0100000000000000  // A register out
#define C_BO  0b1100000000000000  // B register out
#define C_EO  0b0010000000000000  // ∑ register out
#define C_RO  0b1010000000000000  // Random access memory out
#define C_IOO 0b0110000000000000  // Instruction-Op register out
#define C_EPO 0b1110000000000000  // External programer out

// ##############################################
// Here are all different ALU flags defined.
// ##############################################
#define FLAGS_Z0C0 0b00
#define FLAGS_Z0C1 0b01
#define FLAGS_Z1C0 0b10
#define FLAGS_Z1C1 0b11

// ##############################################
// Here are all different instructions defined.
// ##############################################
#define NOP 0x00
#define HLT 0x01
#define JMP 0x04
#define JMC 0x05
#define JMZ 0x06
#define JNZ 0x07
#define LDA 0x10
#define LDB 0x11
#define STA 0x12
#define STB 0x13
#define STE 0x14
#define ADD 0x20
#define SUB 0x21
#define TAB 0x30
#define TBA 0x31

#endif