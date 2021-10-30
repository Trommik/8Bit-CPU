#include "CpuController.h"

volatile boolean CpuController::clockDetected = false;

void CpuController::init()
{
    UCode.init();

    initShiftRegisters();
    initClockInterrupt();

    // Fetch the current instruction
    uint8_t instructionBuffer[1];
    shiftInInstructionBuffer(instructionBuffer, sizeof(instructionBuffer));

    // Set the current instruction
    instruction = instructionBuffer[0];

    // Set the current control word depending on the instruction, flags and step
    controlWord = UCode.getControlWord(instruction, flags, instructionStep);

    // Clear the last control word
    shiftOutControlWord(C_OFF, 0x00);
}

void CpuController::cpuClockCallback() 
{ 
    clockDetected = true;
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
    attachInterrupt(digitalPinToInterrupt(CPU_CLOCK_PIN), cpuClockCallback, RISING);
}

void CpuController::executeInstruction()
{
    // If there was no clock cylce detected return
    if (!clockDetected)
        return;

    // Reset the detected clock cylce
    clockDetected = false;

    // If code execution is stopped return
    // if (!executingCode)
    //     return;

    // Reset the instruction step back to zero
    if (instructionStep > maxInstructionStep)
        instructionStep = 0;

    // Fetch the current instruction
    uint8_t instructionBuffer[1];
    shiftInInstructionBuffer(instructionBuffer, sizeof(instructionBuffer));

    // Set the current instruction
    instruction = instructionBuffer[0];

    // Set the current control word depending on the instruction, flags and step
    controlWord = UCode.getControlWord(instruction, flags, instructionStep);

    // Shift out the control word
    shiftOutControlWord(controlWord, 0x00);

    // Increase the cpu instruction step
    instructionStep++;

    // Debug statement
    Serial.print("Executed clock cycle!\n\tinstruction: ");
    Serial.print(instruction, BIN);
    Serial.print("; step: ");
    Serial.print(instructionStep, BIN);
    Serial.print("\n\tcontrolWord: ");
    Serial.print(controlWord, BIN);
    Serial.println();
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

void CpuController::shiftOutControlWord(uint16_t controlWord, uint8_t busValue)
{
    pinMode(DATA_PIN, OUTPUT);
    digitalWrite(OUT_LATCH_PIN, LOW);

    // Shift out the data for the bus
    shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, busValue);

    // Shift out the upper byte of the uint16_t
    shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, controlWord >> 8);

    // Shift out the lower byte of the uint16_t
    shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, controlWord & 0xFF);

    // Latch the data
    digitalWrite(OUT_LATCH_PIN, HIGH);
    pinMode(DATA_PIN, INPUT);
}
