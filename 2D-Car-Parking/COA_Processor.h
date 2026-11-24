#ifndef COA_PROCESSOR_H
#define COA_PROCESSOR_H

// ==============================================================================
// Subject: Computer Organization & Architecture (Course Code: 2310221L)
// COA CO1: 80386 CPU Register Simulation (R00-R03) & ALU Condition Flags (ZF, SF, TOL_OK, ROT_OK)
// COA CO2: Native Assembly Scoring Formula & Comparison Sequences
// COA CO3: Algorithmic Successive Addition Distance^2 & Character String "PARK" Validation
// COA CO4: Integrated Processor-Level Processing Flow & Control Flow Branching
//
// Assembly Source File Path: COA_Processor.asm
// Assembled with NASM: nasm -f win64 COA_Processor.asm -o COA_Processor_asm.o
// Linked into: 2D_Car_Parking.exe
// ==============================================================================

#include "PL_DataStructures.h"
#include <string>
#include <cmath>
#include <algorithm>
#include <iostream>

struct CPU_Registers {
    int R00;
    int R01;
    int R02;
    int R03;
    CPU_Registers() : R00(0), R01(0), R02(0), R03(30) {}
};

struct ALU_Flags {
    bool ZF;
    bool SF;
    bool TOL_OK;
    bool ROT_OK;
};

// External native NASM assembly routines implemented in COA_Processor.asm
extern "C" {
    void coa_asm_reset(CPU_Registers* pReg, ALU_Flags* pFlags);
    int coa_asm_calculate_score(int collisions, int timePenalty, CPU_Registers* pReg);
    int coa_asm_validate_park_command(const char* cmd_str);
    long long coa_asm_distance_square(int cx, int cy, int sx, int sy);
    int coa_asm_parking_check(int cx, int cy, int sx, int sy, int tolerance, int angleDiffDeg, CPU_Registers* pReg, ALU_Flags* pFlags, const char* cmd_str);
}

class COAProcessor {
private:
    CPU_Registers reg;
    ALU_Flags flags;
    std::string currentDecision;

public:
    COAProcessor() {
        reset();
    }

    void reset() {
        coa_asm_reset(&reg, &flags);
        currentDecision = "READY";
    }

    int calculateScore(int collisions, float timeElapsedSec) {
        int timePenalty = std::min(40, (int)(timeElapsedSec * 1.5f));
        // COA CO1 & CO2 Native NASM assembly calculation
        int finalPoints = coa_asm_calculate_score(collisions, timePenalty, &reg);
        std::cout << "[COA ASM] Score Calculation: 100 - " << reg.R01 << " - "
                  << reg.R02 << " + " << reg.R03 << " = " << reg.R00
                  << " (Scaled x10: " << finalPoints << " pts)\n";
        return finalPoints;
    }

    bool validateCommand(const std::string& cmd) {
        // COA CO3 Native NASM string processing
        return coa_asm_validate_park_command(cmd.c_str()) == 1;
    }

    long long computeDistanceSquare(int cx, int cy, int sx, int sy) {
        // COA CO3 Native NASM successive addition loop
        return coa_asm_distance_square(cx, cy, sx, sy);
    }

    void executeParkingCheck(float carX, float carY, float carAngleDeg, const ParkingSlot* targetSlot) {
        if (!targetSlot) return;

        float cDeg = std::fmod((carAngleDeg + 360.0f), 360.0f);
        float tDeg = std::fmod((targetSlot->targetAngle + 360.0f), 360.0f);
        float diff = std::abs(cDeg - tDeg);
        if (diff > 180.0f) diff = 360.0f - diff;
        float revDiff = std::abs(diff - 180.0f);
        int finalAngleDiff = (int)std::min(diff, revDiff);

        // COA CO4 Native NASM integrated processing & control flow
        const char* parkCmd = "PARK";
        int dec = coa_asm_parking_check(
            (int)carX, (int)carY,
            (int)targetSlot->x, (int)targetSlot->y,
            reg.R03, finalAngleDiff,
            &reg, &flags, parkCmd
        );

        if (dec == 2) {
            currentDecision = "PARK_SUCCESS_CONFIRMED";
        } else if (dec == 1) {
            currentDecision = "ROTATION_MISALIGNED";
        } else {
            currentDecision = "OUTSIDE_SLOT";
        }
    }

    const CPU_Registers& getRegisters() const { return reg; }
    const ALU_Flags& getFlags() const { return flags; }
    std::string getDecisionState() const { return currentDecision; }
};

#endif
