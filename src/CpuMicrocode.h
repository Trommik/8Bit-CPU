#include <Arduino.h>

#include <CpuDefinitions.h>

#ifndef CPU_MICRO_CODE_H
#define CPU_MICRO_CODE_H

/* Class which manages all microcode for the different instructions. */
class CpuMicrocode
{
private:

    uint16_t UCODE[0x04][0xFF][0x07]{0};

public:

    /* This initializes the UCODE structure with the microcodes for each instruction. */
    void init();

    /* This returns the control word for the given instructions step when the given flags are active. */
    uint16_t getControlWord(uint8_t instruction, uint8_t flags, uint8_t step);
};

#endif