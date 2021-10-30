#include <Arduino.h>

#include <PinDefinitions.h>

#include <CpuMicrocode.h>

#ifndef CPU_CONTROLLER_H
#define CPU_CONTROLLER_H

/* Class which controls all cpu functions. */
class CpuController
{
private:
    const uint8_t maxInstructionStep = 0b100;

    boolean executingCode = true;
    uint16_t controlWord;

    uint8_t flags;

    uint8_t instruction;
    uint8_t instructionStep;

    /* Gets set when a risign edge of the cpu clock was detected. */
    static volatile boolean clockDetected;

    /* Gets called whenever a rising edge of the CPU clock is detected. */
    static void IRAM_ATTR cpuClockCallback();

    /* This will initialize all needed IO pins for the shift registers. */
    void initShiftRegisters();

    /* This will initialize a pin interupt which will trigger on the raising edge of the cpu clock. */
    void initClockInterrupt();

    /* This reads the inputs of 74HC165 shift registers and stores them in the given buffer. */
    void shiftInInstructionBuffer(uint8_t buffer[], uint8_t size);

    /* This will shift out the control word and bus value to three 74HC595 shift registers. */
    void shiftOutControlWord(uint16_t controlWord, uint8_t busValue);

public:

    /* The microcode the cpu uses. */
    CpuMicrocode UCode;

    /* This will initialize the cpu controller and all its functions. */
    void init();

    /* This will try to execute the current instruction on a rising clock pulse. */
    void executeInstruction();

    /* Returns the last read flags. */
    uint8_t getFlags() { return instruction; }

    /* Returns the last read instruction. */
    uint8_t getInstruction() { return instruction; }

    /* Returns the current instruction step. */
    uint8_t getInstructionStep() { return instructionStep; }

    /* Returns the current control word. */
    uint16_t getControlWord() { return controlWord; }
};

#endif