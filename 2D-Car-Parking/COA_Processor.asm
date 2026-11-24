; ==============================================================================
; FILE: COA_Processor.asm
; SUBJECT: Computer Organization & Architecture (Course Code: 2310221L)
; COA CO1: 80386 Register Simulation (R00-R03) & ALU Condition Flags (ZF, SF, TOL_OK, ROT_OK)
; COA CO2: Assembly Arithmetic Scoring & Tolerance Comparison Sequences
; COA CO3: Algorithmic (Successive Addition Dist^2) & String Processing ("PARK" Validation)
; COA CO4: Integrated Processor-Level Processing Flow & Control Flow Branching
;
; Assembled with NASM:
;   nasm -f win64 COA_Processor.asm -o COA_Processor_asm.o
; Linked with C++ Game Engine via:
;   COA_Processor.h (extern "C" function declarations)
; ==============================================================================

default rel

section .data
    align 8
    expected_park_cmd db "PARK", 0

section .text

; ------------------------------------------------------------------------------
; SEQUENCE 0: PROCESSOR RESET (coa_asm_reset)
; Clears registers R00-R02 to 0, sets R03 = 30 (default tolerance), clears flags.
; RCX = pointer to CPU_Registers struct
; RDX = pointer to ALU_Flags struct
; ------------------------------------------------------------------------------
global coa_asm_reset
coa_asm_reset:
    test rcx, rcx
    jz .check_flags
    mov dword [rcx],      0     ; R00 = 0 (Score Accumulator)
    mov dword [rcx + 4],  0     ; R01 = 0 (Collision Penalty)
    mov dword [rcx + 8],  0     ; R02 = 0 (Time Penalty)
    mov dword [rcx + 12], 30    ; R03 = 30 (Tolerance threshold in pixels)
.check_flags:
    test rdx, rdx
    jz .done_reset
    mov dword [rdx], 0          ; Clear ZF, SF, TOL_OK, ROT_OK
.done_reset:
    ret

; ------------------------------------------------------------------------------
; COA CO1 & CO2: ARITHMETIC SCORE FORMULA + SUBSEQUENT MULTIPLY LOOP
; (coa_asm_calculate_score)
; Formula: Final Score = Base(100) - ColPenalty(R01) - TimePenalty(R02) + Bonus(R03)
; Multiplied by 10 using repeated addition loop (COA CO3 Arithmetic Algorithm)
;
; Arguments (Microsoft x64 Calling Convention):
;   RCX = collisions (int)
;   RDX = timePenalty (int)
;   R8  = pointer to CPU_Registers struct
; Returns: EAX = finalPoints (int)
; ------------------------------------------------------------------------------
global coa_asm_calculate_score
coa_asm_calculate_score:
    push rbx

    ; Calculate collision penalty = min(60, collisions * 20)
    mov eax, ecx
    imul eax, 20
    cmp eax, 60
    jle .col_ok
    mov eax, 60
.col_ok:
    mov ebx, eax        ; EBX = R01 (Collision Penalty)

    ; Calculate time penalty = min(40, timePenalty)
    mov ecx, edx
    cmp ecx, 40
    jle .time_ok
    mov ecx, 40
.time_ok:               ; ECX = R02 (Time Penalty)

    ; Load Base Score and Precision Bonus
    mov eax, 100        ; EAX = R00 (Base Score = 100)
    mov edx, 15         ; EDX = R03 (Precision Bonus = 15)

    ; CO1 Arithmetic Sequence:
    sub eax, ebx        ; R00 = R00 - R01
    sub eax, ecx        ; R00 = R00 - R02
    add eax, edx        ; R00 = R00 + R03
    cmp eax, 0
    jge .score_non_negative
    xor eax, eax
.score_non_negative:

    ; Store register state into CPU_Registers struct
    test r8, r8
    jz .skip_save
    mov dword [r8],      eax ; R00
    mov dword [r8 + 4],  ebx ; R01
    mov dword [r8 + 8],  ecx ; R02
    mov dword [r8 + 12], edx ; R03
.skip_save:

    ; CO2 & CO3 Algorithm: Subsequent Multiply Loop (Multiply R00 * 10 via repeated addition)
    mov r10d, eax       ; Multiplicand
    mov r11d, 10        ; Multiplier / Loop counter
    xor eax, eax        ; Accumulator = 0
.mul_loop:
    add eax, r10d       ; Accumulate product
    dec r11d            ; Decrement counter
    jnz .mul_loop       ; Loop until counter is zero

    pop rbx
    ret

; ------------------------------------------------------------------------------
; COA CO3: STRING PROCESSING — Character-by-Character Command Validation
; (coa_asm_validate_park_command)
; Compares input string against "PARK" character by character
;
; Arguments:
;   RCX = pointer to null-terminated command string (const char*)
; Returns: EAX = 1 (Valid "PARK"), 0 (Invalid)
; ------------------------------------------------------------------------------
global coa_asm_validate_park_command
coa_asm_validate_park_command:
    push rbx
    push rsi
    push rdi

    test rcx, rcx
    jz .invalid_cmd

    lea rsi, [expected_park_cmd]
    mov rdi, rcx
    mov edx, 4                  ; 4 characters in "PARK"

.compare_loop:
    mov al, byte [rsi]
    mov bl, byte [rdi]
    cmp al, bl
    jne .invalid_cmd
    inc rsi
    inc rdi
    dec edx
    jnz .compare_loop

    ; Ensure null terminator at end
    cmp byte [rdi], 0
    jne .invalid_cmd

    mov eax, 1                  ; 1 = Valid command
    pop rdi
    pop rsi
    pop rbx
    ret

.invalid_cmd:
    xor eax, eax                ; 0 = Invalid command
    pop rdi
    pop rsi
    pop rbx
    ret

; ------------------------------------------------------------------------------
; COA CO3: COMPUTATIONALLY INTENSIVE ARITHMETIC ALGORITHM
; (coa_asm_distance_square)
; Calculates Distance Square = (cx - sx)^2 + (cy - sy)^2
; Uses SUCCESSIVE ADDITION loops instead of direct multiplication instructions!
;
; Arguments (Microsoft x64 Calling Convention):
;   RCX = cx (int)
;   RDX = cy (int)
;   R8  = sx (int)
;   R9  = sy (int)
; Returns: RAX = distance square (int64)
; ------------------------------------------------------------------------------
global coa_asm_distance_square
coa_asm_distance_square:
    push rbx
    push r12
    push r13
    push r14
    push r15

    ; dx = |cx - sx|
    mov r10, rcx
    sub r10, r8
    jns .abs_dx
    neg r10
.abs_dx:

    ; dy = |cy - sy|
    mov r11, rdx
    sub r11, r9
    jns .abs_dy
    neg r11
.abs_dy:

    ; --- Successive Addition Loop for dx^2 = dx * dx ---
    xor r12, r12        ; r12 = accumulator for dx^2
    mov r13, r10        ; multiplicand = dx
    mov r14, r10        ; loop counter = dx
.x_square_loop:
    test r14, r14
    jz .x_square_done
    add r12, r13        ; repeated addition
    dec r14
    jmp .x_square_loop
.x_square_done:

    ; --- Successive Addition Loop for dy^2 = dy * dy ---
    xor r15, r15        ; r15 = accumulator for dy^2
    mov rbx, r11        ; multiplicand = dy
    mov rcx, r11        ; loop counter = dy
.y_square_loop:
    test rcx, rcx
    jz .y_square_done
    add r15, rbx        ; repeated addition
    dec rcx
    jmp .y_square_loop
.y_square_done:

    ; Distance^2 = dx^2 + dy^2
    add r12, r15
    mov rax, r12

    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    ret

; ------------------------------------------------------------------------------
; COA CO4: INTEGRATED PROCESSOR CONTROL FLOW & PARKING CHECK
; (coa_asm_parking_check)
; Integrates:
;   1) String processing: Validates "PARK" command character by character.
;   2) Successive addition: Calculates dx^2 + dy^2.
;   3) ALU flags evaluation: ZF, SF, TOL_OK, ROT_OK.
;   4) Control Flow: Conditional branching (jle/jg) deciding parking success.
;
; Arguments (Microsoft x64 Calling Convention):
;   RCX = cx (car X)
;   RDX = cy (car Y)
;   R8  = sx (slot X)
;   R9  = sy (slot Y)
;   Stack [rbp + 48] = tolerance threshold (int)
;   Stack [rbp + 56] = angleDiffDeg (int)
;   Stack [rbp + 64] = pReg (CPU_Registers*)
;   Stack [rbp + 72] = pFlags (ALU_Flags*)
;   Stack [rbp + 80] = cmd_str (const char*)
; Returns: EAX = 2 (PARK_SUCCESS_CONFIRMED), 1 (ROTATION_MISALIGNED), 0 (OUTSIDE_SLOT)
; ------------------------------------------------------------------------------
global coa_asm_parking_check
coa_asm_parking_check:
    push rbp
    mov rbp, rsp

    push rbx
    push rsi
    push rdi
    push r12
    push r13
    push r14
    push r15

    ; Save coordinate inputs in preserved registers
    mov r12d, ecx                   ; r12d = cx
    mov r13d, edx                   ; r13d = cy
    mov r14d, r8d                   ; r14d = sx
    mov r15d, r9d                   ; r15d = sy

    ; Load stack arguments using reliable frame pointer offsets
    mov ebx, dword [rbp + 48]       ; ebx = tolerance
    mov esi, dword [rbp + 56]       ; esi = angleDiffDeg
    mov r10, qword [rbp + 64]       ; r10 = pReg
    mov r11, qword [rbp + 72]       ; r11 = pFlags
    mov rdi, qword [rbp + 80]       ; rdi = cmd_str

    ; 1. String Processing Verification (CO3: Character-by-character "PARK" match)
    test rdi, rdi
    jz .cmd_fail
    lea r8, [expected_park_cmd]
    mov ecx, 4
.cmd_loop:
    mov al, byte [r8]
    mov dl, byte [rdi]
    cmp al, dl
    jne .cmd_fail
    inc r8
    inc rdi
    dec ecx
    jnz .cmd_loop
    jmp .cmd_ok

.cmd_fail:
    xor eax, eax                    ; 0 = OUTSIDE_SLOT
    jmp .epilogue

.cmd_ok:
    ; 2. Distance Computation (CO2 & CO3 Successive Addition)
    ; Compute Delta X = |cx - sx|
    xor r8d, r8d                    ; r8d = SF (Sign Flag: cx < sx)
    mov eax, r12d
    sub eax, r14d
    jns .abs_dx
    neg eax
    mov r8d, 1                      ; cx < sx => SF = 1
.abs_dx:
    mov ecx, eax                    ; ecx = |Delta X|

    ; Compute Delta Y = |cy - sy|
    mov eax, r13d
    sub eax, r15d
    jns .abs_dy
    neg eax
.abs_dy:
    mov edx, eax                    ; edx = |Delta Y|

    ; CO3 Successive addition loop for dx^2
    xor eax, eax                    ; eax = accumulator for dx^2
    mov r9d, ecx                    ; counter = dx
.dx_sq:
    test r9d, r9d
    jz .dx_sq_done
    add eax, ecx
    dec r9d
    jmp .dx_sq
.dx_sq_done:

    ; CO3 Successive addition loop for dy^2
    xor r9d, r9d                    ; r9d = accumulator for dy^2
    mov r14d, edx                   ; counter = dy
.dy_sq:
    test r14d, r14d
    jz .dy_sq_done
    add r9d, edx
    dec r14d
    jmp .dy_sq
.dy_sq_done:

    ; Total Dist^2 = dx^2 + dy^2 (in eax)
    add eax, r9d

    ; 3. Evaluate TOL_OK: (|Delta X| <= tol && |Delta Y| <= tol)
    ; Enforce minimum 30px tolerance threshold for responsive parking gameplay
    cmp ebx, 30
    jge .tol_threshold_ok
    mov ebx, 30
.tol_threshold_ok:

    xor r14d, r14d                  ; r14d = TOL_OK flag (0)
    cmp ecx, ebx
    jg .eval_flags
    cmp edx, ebx
    jg .eval_flags
    mov r14d, 1                     ; TOL_OK = 1

.eval_flags:
    ; Evaluate ZF (Zero Flag: exact center deltaX == 0 && deltaY == 0)
    xor r15d, r15d                  ; ZF = 0
    test ecx, ecx
    jnz .check_rot
    test edx, edx
    jnz .check_rot
    mov r15d, 1                     ; ZF = 1

.check_rot:
    ; Evaluate ROT_OK: angleDiffDeg <= 25 degrees (allows natural steering alignment)
    xor r9d, r9d                    ; ROT_OK = 0
    cmp esi, 25
    jg .store_flags
    mov r9d, 1                      ; ROT_OK = 1

.store_flags:
    ; Write ALU condition flags into ALU_Flags struct if pointer is non-null
    test r11, r11
    jz .make_decision
    mov byte [r11],     r15b        ; flags->ZF
    mov byte [r11 + 1], r8b         ; flags->SF
    mov byte [r11 + 2], r14b        ; flags->TOL_OK
    mov byte [r11 + 3], r9b         ; flags->ROT_OK

.make_decision:
    ; 4. CO4 Control Flow Decision
    test r14b, r14b                 ; Check TOL_OK
    jz .outside_slot
    test r9b, r9b                   ; Check ROT_OK
    jz .rot_misaligned

.park_success:
    mov eax, 2                      ; 2 = PARK_SUCCESS_CONFIRMED
    jmp .epilogue

.rot_misaligned:
    mov eax, 1                      ; 1 = ROTATION_MISALIGNED
    jmp .epilogue

.outside_slot:
    xor eax, eax                    ; 0 = OUTSIDE_SLOT

.epilogue:
    pop r15
    pop r14
    pop r13
    pop r12
    pop rdi
    pop rsi
    pop rbx
    pop rbp
    ret
