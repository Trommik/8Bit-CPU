#include <Arduino.h>

#include <PinDefinitions.h>

#include <CpuMicrocode.h>

#ifndef CPU_CONTROLLER_H
#define CPU_CONTROLLER_H

/* Class which controls all cpu functions. */
class CpuController
{
private:
    const uint8_t maxInstructionStep = 0b101;

    boolean executeMode = false;
    boolean loadCodeMode = false;

    uint8_t *code;
    uint8_t codeSize = 0;
    uint8_t codeLoaded = 0;
    uint8_t codeToLoad = 0;

    boolean addressSetup = false;

    uint16_t controlWord = 0x00;

    uint8_t flags = 0x00;

    uint8_t instruction = 0x00;
    uint8_t instructionStep = 0x00;

    /* Gets set when a risign edge of the cpu clock was detected. */
    static volatile boolean clockDetected;

    /* Gets called whenever a rising edge of the CPU clock is detected. */
    static void IRAM_ATTR cpuClockCallback();

    /* This will initialize all needed IO pins for the shift registers. */
    void initShiftRegisters();

    /* This will initialize a pin interupt which will trigger on the raising edge of the cpu clock. */
    void initClockInterrupt();

    /* This will try to execute the current instruction on a rising clock pulse. */
    void executeInstruction();

    /* This will try to load the given code into RAM everytime a rising clock pulse is detected. */
    void executeLoadCode();

    /* This reads the inputs of 74HC165 shift registers and stores them in the given buffer. */
    void shiftInInstructionBuffer(uint8_t buffer[], uint8_t size);

    /* This will shift out the control word and bus value to three 74HC595 shift registers. */
    void shiftOutControlWord(uint16_t controlWord, uint8_t busValue);

public:
    /* The microcode the cpu uses. */
    CpuMicrocode UCode;

    CpuController()
    {
        // Init the code array
        code = new uint8_t[0];
    }

    /* This will initialize the cpu controller and all its functions. */
    void init();

    /* This will reset the cpu controller. */
    void reset();

    /* This handles all instructions for the cpu controller. */
    void handleInstructions();

    /* Sets the code the cpu should load into RAM. */
    void loadCodeToRam(uint8_t buffer[], uint8_t size);

    /* Sets the cpu controller into load code mode to load external code into RAM. */
    void setLoadCodeMode(boolean loadCode);

    /* Returns if the cpu controller is in load code mode. */
    boolean getLoadCodeMode() { return loadCodeMode; }

    /* Sets the cpu controller into execute mode to execute instructions from the instruction register. */
    void setExecuteMode(boolean execute);

    /* Returns if the cpu controller is in execute mode. */
    boolean getExecuteMode() { return executeMode; }

    /* Returns the last read flags. */
    uint8_t getFlags() { return flags; }

    /* Returns the last read instruction. */
    uint8_t getInstruction() { return instruction; }

    /* Returns the current instruction step. */
    uint8_t getInstructionStep() { return instructionStep; }

    /* Returns the current control word. */
    uint16_t getControlWord() { return controlWord; }
};

#endif