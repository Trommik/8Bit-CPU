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
#define C_HLT 0b1100000011011000  // Halt clock
#define C_JMP 0b0000000011011000  // Jump (program counter in)
#define C_CE  0b0110000011011000  // Program counter enable
#define C_AI  0b0101000011011000  // A register in
#define C_BI  0b0100100011011000  // B register in
// SPARE
// SPARE
#define C_RI  0b0100000111011000  // Random access memory in
#define C_MI  0b0100000001011000  // Memory address register in
#define C_FI  0b0100000010011000  // Flags register in
#define C_SU  0b0100000011111000  // ALU subtract
#define C_IRI 0b0100000011001000  // Instruction register in
#define C_IOI 0b0100000011010000  // Instruction-Op register in
#define C_CO  0b0100000011011001  // Program counter out
#define C_AO  0b0100000011011010  // A register out
#define C_BO  0b0100000011011011  // B register out
#define C_EO  0b0100000011011100  // ∑ register out
#define C_RO  0b0100000011011101  // Random access memory out
#define C_IOO 0b0100000011011110  // Instruction-Op register out
#define C_EPO 0b0100000011011111  // External programer out

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