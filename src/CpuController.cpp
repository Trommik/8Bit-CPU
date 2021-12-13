#include "CpuController.h"

volatile boolean CpuController::clockFalling = false;
volatile boolean CpuController::clockRising = false;

void CpuController::init()
{
    UCode.init();

    initShiftRegisters();
    initClockInterrupt();

    reset();
}

void CpuController::cpuClockCallback() 
{
    if (!digitalRead(CPU_CLOCK_PIN))
        clockFalling = true;
    else
        clockRising = true;
}

void CpuController::initShiftRegisters()
{
    // Setup the pins connected to the shift registers
    pinMode(DATA_PIN, INPUT);
    pinMode(CLOCK_PIN, OUTPUT);
    pinMode(IN_LATCH_PIN, OUTPUT);
    pinMode(OUT_LATCH_PIN, OUTPUT);
}

void CpuController::initClockInterrupt()
{
    // Attach a pin interupt to the cpu clock rising edge
    pinMode(CPU_CLOCK_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(CPU_CLOCK_PIN), cpuClockCallback, CHANGE);
}

void CpuController::reset()
{
    // Fetch the current instruction
    uint8_t instructionBuffer[1];
    shiftInInstructionBuffer(instructionBuffer, sizeof(instructionBuffer));

    // Set the current instruction and reset the step
    instruction = instructionBuffer[0];
    instructionStep = 0;

    // Set the current control word depending on the instruction, flags and step
    controlWord = UCode.getControlWord(instruction, flags, instructionStep);

    // Clear the last control word
    shiftOutControlBuffer(0x00, 0x00);
}

void CpuController::setLoadCodeMode(boolean loadCode)
{
    if (executeMode)
        return;

    reset();
    loadCodeMode = loadCode;
}

void CpuController::setExecuteMode(boolean execute)
{
    if (loadCodeMode)
        return;
        
    reset();
    executeMode = execute;
}

void CpuController::handleInstructions()
{
    // If there was no clock cylce detected return
    if (!clockFalling && !clockRising)
        return;

    // Handle falling clock
    if (clockFalling)
    {
        // Reset the detected clock cylce
        clockFalling = false;

        // Check the mode and execute its current instruction
        if (executeMode) executeInstruction();
        else if (loadCodeMode) executeLoadCode();
    }

    // Handle rising clock
    if (clockRising)
    {
        // Reset the detected clock cylce
        clockRising = false;

        // Set the ready flag when the clock is rising
        if (executeMode) shiftOutControlBuffer(controlWord | C_RDY, 0x00);
    }
}

void CpuController::executeInstruction()
{
    // Reset the instruction step back to zero
    if (instructionStep > UCode.MaxInstructionStep)
        instructionStep = 0;

    // Fetch the current instruction
    uint8_t instructionBuffer[2];
    shiftInInstructionBuffer(instructionBuffer, sizeof(instructionBuffer));

    // Set the current instruction
    instruction = instructionBuffer[0];
    flags = instructionBuffer[1];

    // Set the current control word depending on the instruction, flags and step
    controlWord = UCode.getControlWord(instruction, flags, instructionStep);

    // Shift out the control word
    shiftOutControlBuffer(controlWord, 0x00);

    // Debug statement
    Serial.print("Executed instruction!\n\tinstruction: 0x");
    Serial.print(instruction, HEX);
    Serial.print("; flags: ");
    Serial.print(flags, BIN);
    Serial.print("; step: ");
    Serial.print(instructionStep, BIN);
    Serial.print("\n\tcontrolWord: ");
    Serial.print(controlWord, BIN);
    Serial.println();
    
    // Increase the cpu instruction step
    instructionStep++;
}

void CpuController::executeLoadCode()
{
    // Check if there is code left to load
    if (codeLoaded >= codeSize)
    {
        // No code left to load switch off registers
        controlWord = 0x00;

        // Shift out the control word
        shiftOutControlBuffer(controlWord, 0x00);

        return;
    }

    // Get the next code instruction to load
    codeToLoad = code[codeLoaded];

    // Check if the RAM address is already setup
    if (!addressSetup)
    {
        // Write the address into the MAR
        controlWord = C_EPO | C_MI;

        // Shift out the control word and set the ready flag
        shiftOutControlBuffer(controlWord | C_RDY, codeLoaded);
        
        // Set the address setup flag
        addressSetup = true;
    }
    else
    {
        // Write the code to load into RAM
        controlWord = C_EPO | C_RI;

        // Shift out the control word and set the ready flag
        shiftOutControlBuffer(controlWord | C_RDY, codeToLoad);

        // Debug statement
        Serial.print("Code loaded!\n\tcodeToLoad: 0x");
        Serial.print(codeToLoad, HEX);
        Serial.print(", 0b");
        Serial.print(codeToLoad, BIN);
        Serial.print("\n\tcodeLoaded: ");
        Serial.print(codeLoaded);
        Serial.print("\n\tcodeSize: ");
        Serial.print(codeSize);
        Serial.print("\n\tcontrolWord: ");
        Serial.print(controlWord, BIN);
        Serial.println();

        // Increase the code loaded count
        addressSetup = false;
        codeLoaded++;
    }
}

void CpuController::loadCodeToRam(uint8_t buffer[], uint8_t size)
{
    if (!loadCodeMode)
        return;
    
    // Copy the buffer into the code array
    if (size < 0xFF)
    {
        code = new uint8_t[size]{0};
        codeSize = size;

        memcpy(code, buffer, size);
    }
    else
    {
        code = new uint8_t[0xFF]{0};
        codeSize = 0xFF;
        
        memcpy(code, buffer, 0xFF);
    }

    // Reset the flags
    codeLoaded = 0; 
    codeToLoad = 0;
    addressSetup = false;
}

void CpuController::shiftInInstructionBuffer(uint8_t buffer[], uint8_t size)
{
    pinMode(DATA_PIN, INPUT);
    
    digitalWrite(IN_LATCH_PIN, LOW);
    delayMicroseconds(5);
    digitalWrite(IN_LATCH_PIN, HIGH);

    for (uint8_t j = 0; j < size; j++)
    {
        uint8_t bitVal;
        uint8_t bytesVal = 0;

        for (uint8_t i = 0; i < 8; i++)
        {
            bitVal = digitalRead(DATA_PIN);
            bytesVal |= (bitVal << ((8 - 1) - i));

            digitalWrite(CLOCK_PIN, HIGH);
            delayMicroseconds(5);
            digitalWrite(CLOCK_PIN, LOW);
        }

        buffer[j] = bytesVal;
    }
}

void CpuController::shiftOutControlBuffer(uint16_t controlWord, uint8_t busValue)
{
    pinMode(DATA_PIN, OUTPUT);
    digitalWrite(OUT_LATCH_PIN, LOW);

    // Shift out the data for the bus
    shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, busValue);

    // Shift out the upper byte of the uint16_t
    shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, (controlWord ^ C_INV) >> 8);

    // Shift out the lower byte of the uint16_t
    shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, (controlWord ^ C_INV) & 0xFF);

    // Latch the data
    digitalWrite(OUT_LATCH_PIN, HIGH);
    pinMode(DATA_PIN, INPUT);
}
