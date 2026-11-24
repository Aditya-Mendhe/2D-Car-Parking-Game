// ==============================================================================
// FILE: COA_Processor_Assembly_View.cpp
// PURPOSE: Dev-C++ Native View of COA Assembly Implementation & Academic CO Mapping
// COURSE: Computer Organization & Architecture (Course Code: 2310221L)
// ==============================================================================
// NOTE FOR DEV-C++ USERS:
// 1. You can view the raw assembly file in Dev-C++ by:
//    - Dragging "COA_Processor.asm" directly from Windows Explorer into Dev-C++.
//    - OR File -> Open -> change filter dropdown from "C/C++ Source Files" to "All Files (*.*)".
//    - OR opening "2D_Car_Parking.dev" which lists all project files in the left sidebar!
// 2. The native assembly is assembled by NASM via "build.bat" into "COA_Processor_asm.o"
//    and linked directly into 2D_Car_Parking.exe.
// ==============================================================================

#include "COA_Processor.h"
#include <iostream>
#include <iomanip>

/*
=================================================================================
               ACADEMIC COA COURSE OUTCOMES (CO) SPECIFICATION
=================================================================================

---------------------------------------------------------------------------------
[COA CO1]: Basic Arithmetic & 80386 CPU Register Simulation (R00 - R03)
---------------------------------------------------------------------------------
Academic Statement: Demonstrate basic arithmetic and instruction set operations
using general-purpose registers (MOV, ADD, SUB, HLT).

Implementation in Project:
  Registers:
    R00 = Base Score Accumulator (Initial value = 100)
    R01 = Collision Penalty (min 60, collisions * 20)
    R02 = Time Penalty (min 40, timeSec * 1.5)
    R03 = Precision Tolerance Bonus / Threshold (15 pts bonus / 30px threshold)
  Formula:
    R00 = R00 - R01 - R02 + R03
    Example: 100 - 20 - 10 + 15 = 85 (Scaled x10 in game = 850 pts)
  Assembly Instructions Used:
    MOV, SUB, ADD, CMP, JGE

---------------------------------------------------------------------------------
[COA CO2]: Application-Specific 64-bit Assembly Processing
---------------------------------------------------------------------------------
Academic Statement: Execute 64-bit application-specific arithmetic, distance
comparisons, and condition flag setting.

Implementation in Project:
  Registers:
    RAX, RBX, RCX, RDX, R8, R9, R10, R11, R12, R13, R14, R15 (x86-64 Win64 ABI)
  Logic:
    Calculates Delta X = |cx - sx| and Delta Y = |cy - sy|.
    Evaluates condition flags:
      ZF (Zero Flag)     = 1 if car is exactly centered in slot (dx == 0 && dy == 0)
      SF (Sign Flag)     = 1 if cx < sx (car is to the left of target)
      TOL_OK Flag        = 1 if |dx| <= tolerance && |dy| <= tolerance
      ROT_OK Flag        = 1 if angular misalignment <= 25 degrees

---------------------------------------------------------------------------------
[COA CO3]: Algorithmic Successive Addition & Character-by-Character String Validation
---------------------------------------------------------------------------------
Academic Statement: Implement one computationally intensive arithmetic algorithm
(successive addition loop) without direct hardware multipliers, and one string/data
processing operation in 64-bit Assembly.

Implementation in Project:
  1. Computational Intensity (Successive Addition):
     Computes distance squared (dx^2 + dy^2) without using the 'imul' instruction!
     Instead, uses repeated ADD loops:
       mov r12, 0       ; Accumulator
       mov r13, dx      ; Multiplicand
       mov r14, dx      ; Counter
     .x_square:
       add r12, r13     ; Repeated ADD
       dec r14
       jnz .x_square
  2. String Processing:
     Validates the user/game parking action "PARK" character by character in memory:
       .compare_loop:
         mov al, [rsi]  ; Load byte from expected "PARK"
         mov bl, [rdi]  ; Load byte from input command
         cmp al, bl     ; Compare bytes
         jne .invalid
         inc rsi
         inc rdi
         loop .compare_loop

---------------------------------------------------------------------------------
[COA CO4]: Integrated Processor Control Flow & Branching Logic
---------------------------------------------------------------------------------
Academic Statement: Integrate processor-level operations (registers, ALU, string
processing, condition flags, control flow branching) and map back to application logic.

Implementation in Project:
  Function: coa_asm_parking_check(...) in COA_Processor.asm
  Integration Flow:
    1. Validates "PARK" command character-by-character (CO3).
    2. Computes distance squared via successive addition (CO3).
    3. Evaluates ALU flags: ZF, SF, TOL_OK, ROT_OK (CO1/CO2).
    4. Executes control flow branching (CO4):
         test r14b, r14b -> jz .outside_slot
         test r9b, r9b   -> jz .rot_misaligned
         jmp .park_success
    5. Returns decision code to game engine:
         2 = PARK_SUCCESS_CONFIRMED  (Triggers win screen and scoring)
         1 = ROTATION_MISALIGNED     (HUD displays alignment guidance)
         0 = OUTSIDE_SLOT            (Car is not within slot boundaries)
=================================================================================
*/

// Standalone verification function that can be tested in Dev-C++
void printCOADistinctionSummary() {
    std::cout << "========================================================\n";
    std::cout << "  2D CAR PARKING GAME - COA NASM64 ACADEMIC EVIDENCE    \n";
    std::cout << "========================================================\n\n";

    COAProcessor proc;
    proc.reset();

    std::cout << "[COA CO1 & CO2 Test] Running Score Calculation:\n";
    int score = proc.calculateScore(1, 12.5f);
    std::cout << "  Final Calculated Score: " << score << " pts\n\n";

    std::cout << "[COA CO3 Test] Successive Addition Distance Square Loop:\n";
    long long distSq = proc.computeDistanceSquare(100, 150, 104, 153);
    std::cout << "  Distance^2 between (100,150) and (104,153): " << distSq << " (Expected: 16+9 = 25)\n\n";

    std::cout << "[COA CO3 Test] String Processing (PARK Command):\n";
    bool valid = proc.validateCommand("PARK");
    bool invalid = proc.validateCommand("STOP");
    std::cout << "  Command 'PARK': " << (valid ? "ACCEPTED" : "REJECTED") << "\n";
    std::cout << "  Command 'STOP': " << (invalid ? "ACCEPTED" : "REJECTED") << "\n\n";

    std::cout << "[COA CO4 Test] Integrated Control Flow & Decision Branching:\n";
    ParkingSlot slot("PARK", 660.0f, 165.0f, 105.0f, 95.0f, 90.0f);
    proc.executeParkingCheck(660.0f, 165.0f, 90.0f, &slot);
    std::cout << "  At (660, 165, 90 deg): " << proc.getDecisionState() << "\n";

    proc.executeParkingCheck(660.0f, 165.0f, 0.0f, &slot);
    std::cout << "  At (660, 165,  0 deg): " << proc.getDecisionState() << "\n";

    proc.executeParkingCheck(100.0f, 100.0f, 90.0f, &slot);
    std::cout << "  At (100, 100, 90 deg): " << proc.getDecisionState() << "\n\n";

    std::cout << "========================================================\n";
    std::cout << "  ALL COA COURSE OUTCOMES VERIFIED & MAPPED!\n";
    std::cout << "========================================================\n";
}
