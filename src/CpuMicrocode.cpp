#include "CpuMicrocode.h"

void CpuMicrocode::init()
{
    /* Initialize all different instructions and their microcodes */
    auto nop = std::initializer_list<uint16_t>({C_MI | C_CO, C_RO | C_IRI | C_CE, 0, 0, 0, 0, 0});     // NOP
    auto hlt = std::initializer_list<uint16_t>({C_MI | C_CO, C_RO | C_IRI | C_CE, C_HLT, 0, 0, 0, 0}); // HLT

    auto jmp = std::initializer_list<uint16_t>({C_MI | C_CO, C_RO | C_IRI | C_CE, C_MI | C_CO, C_RO | C_IOI | C_CE, C_IOO | C_JMP, 0, 0}); // JMP
    auto jmc = std::initializer_list<uint16_t>({C_MI | C_CO, C_RO | C_IRI | C_CE, C_MI | C_CO, C_RO | C_IOI | C_CE, 0, 0, 0});             // JMC
    auto jmz = std::initializer_list<uint16_t>({C_MI | C_CO, C_RO | C_IRI | C_CE, C_MI | C_CO, C_RO | C_IOI | C_CE, 0, 0, 0});             // JMZ
    auto jnz = std::initializer_list<uint16_t>({C_MI | C_CO, C_RO | C_IRI | C_CE, C_MI | C_CO, C_RO | C_IOI | C_CE, C_IOO | C_JMP, 0, 0}); // JNZ

    auto lda = std::initializer_list<uint16_t>({C_MI | C_CO, C_RO | C_IRI | C_CE, C_MI | C_CO, C_RO | C_IOI | C_CE, C_MI | C_IOO, C_AI | C_RO, 0}); // LDA
    auto ldb = std::initializer_list<uint16_t>({C_MI | C_CO, C_RO | C_IRI | C_CE, C_MI | C_CO, C_RO | C_IOI | C_CE, C_MI | C_IOO, C_BI | C_RO, 0}); // LDB
    auto sta = std::initializer_list<uint16_t>({C_MI | C_CO, C_RO | C_IRI | C_CE, C_MI | C_CO, C_RO | C_IOI | C_CE, C_MI | C_IOO, C_RI | C_AO, 0}); // STA
    auto stb = std::initializer_list<uint16_t>({C_MI | C_CO, C_RO | C_IRI | C_CE, C_MI | C_CO, C_RO | C_IOI | C_CE, C_MI | C_IOO, C_RI | C_BO, 0}); // STB
    auto ste = std::initializer_list<uint16_t>({C_MI | C_CO, C_RO | C_IRI | C_CE, C_MI | C_CO, C_RO | C_IOI | C_CE, C_MI | C_IOO, C_RI | C_EO, 0}); // STE

    auto add = std::initializer_list<uint16_t>({C_MI | C_CO, C_RO | C_IRI | C_CE, C_AI | C_EO | C_FI, 0, 0, 0, 0});        // ADD
    auto sub = std::initializer_list<uint16_t>({C_MI | C_CO, C_RO | C_IRI | C_CE, C_AI | C_SU | C_EO | C_FI, 0, 0, 0, 0}); // SUB

    auto tab = std::initializer_list<uint16_t>({C_MI | C_CO, C_RO | C_IRI | C_CE, C_BI | C_AO, 0, 0, 0, 0}); // TAB
    auto tba = std::initializer_list<uint16_t>({C_MI | C_CO, C_RO | C_IRI | C_CE, C_AI | C_BO, 0, 0, 0, 0}); // TBA

    // ZF = 0, CF = 0
    std::copy(nop.begin(), nop.end(), UCODE[FLAGS_Z0C0][NOP]);
    std::copy(hlt.begin(), hlt.end(), UCODE[FLAGS_Z0C0][HLT]);

    std::copy(jmp.begin(), jmp.end(), UCODE[FLAGS_Z0C0][JMP]);
    std::copy(jmc.begin(), jmc.end(), UCODE[FLAGS_Z0C0][JMC]);
    std::copy(jmz.begin(), jmz.end(), UCODE[FLAGS_Z0C0][JMZ]);
    std::copy(jnz.begin(), jnz.end(), UCODE[FLAGS_Z0C0][JNZ]);

    std::copy(lda.begin(), lda.end(), UCODE[FLAGS_Z0C0][LDA]);
    std::copy(ldb.begin(), ldb.end(), UCODE[FLAGS_Z0C0][LDB]);
    std::copy(sta.begin(), sta.end(), UCODE[FLAGS_Z0C0][STA]);
    std::copy(stb.begin(), stb.end(), UCODE[FLAGS_Z0C0][STB]);
    std::copy(ste.begin(), ste.end(), UCODE[FLAGS_Z0C0][STE]);

    std::copy(add.begin(), add.end(), UCODE[FLAGS_Z0C0][ADD]);
    std::copy(sub.begin(), sub.end(), UCODE[FLAGS_Z0C0][SUB]);

    std::copy(tab.begin(), tab.end(), UCODE[FLAGS_Z0C0][TAB]);
    std::copy(tba.begin(), tba.end(), UCODE[FLAGS_Z0C0][TBA]);

    /* Copy the default microcode table to the specific tables when flags are active */
    memcpy(UCODE[FLAGS_Z0C1], UCODE[FLAGS_Z0C0], sizeof(UCODE[FLAGS_Z0C0])); // ZF = 0, CF = 1

    // Setup command for jump when carry
    UCODE[FLAGS_Z0C1][JMC][4] = C_IOO | C_JMP;

    memcpy(UCODE[FLAGS_Z1C0], UCODE[FLAGS_Z0C0], sizeof(UCODE[FLAGS_Z0C0])); // ZF = 1, CF = 0

    // Setup commands for jump when zero and jump when not zero
    UCODE[FLAGS_Z1C0][JMZ][4] = C_IOO | C_JMP;
    UCODE[FLAGS_Z1C0][JNZ][4] = 0;

    memcpy(UCODE[FLAGS_Z1C1], UCODE[FLAGS_Z0C0], sizeof(UCODE[FLAGS_Z0C0])); // ZF = 1, CF = 1

    // Setup command for jump when carry
    UCODE[FLAGS_Z1C1][JMC][4] = C_IOO | C_JMP;

    // Setup commands for jump when zero and jump when not zero
    UCODE[FLAGS_Z1C1][JMZ][4] = C_IOO | C_JMP;
    UCODE[FLAGS_Z1C0][JNZ][4] = 0;
}

uint16_t CpuMicrocode::getControlWord(uint8_t instruction, uint8_t flags, uint8_t step)
{
    return UCODE[flags][instruction][step];
}