; targets Windows x64 calling convention with scalar SSE instructions
;
; function signature:
; void calculate_distance_asm(long long n, const float* X1, const float* X2, const float* Y1, const float* Y2, float* Z);
;
; windows x64 calling convention:
; RCX: n (vector length)
; RDX: X1 pointer
; R8:  X2 pointer  
; R9:  Y1 pointer
; [RSP + 0x48]: Y2 pointer (after pushes)
; [RSP + 0x50]: Z pointer (after pushes)

section .text
    global calculate_distance_asm

calculate_distance_asm:
    ; save callee-saved registers
    push    rdi         ; save RDI (will hold X1)
    push    rsi         ; save RSI (will hold X2)
    push    r12         ; save R12 (will hold Y1)
    push    r13         ; save R13 (will hold Y2)
    push    r14         ; save R14 (will hold Z)

    ; move pointers to callee-saved registers
    mov     rdi, rdx    ; X1 pointer
    mov     rsi, r8     ; X2 pointer
    mov     r12, r9     ; Y1 pointer

    ; load stack arguments (adjusted for 40 bytes of pushes)
    mov     r13, qword [rsp + 0x48] ; Y2 pointer
    mov     r14, qword [rsp + 0x50] ; Z pointer

    ; initialize loop index
    xor     rax, rax    ; i = 0

.loop_start:
    ; check loop condition
    cmp     rax, rcx    ; compare i with n
    jge     .loop_end   ; exit if i >= n

    ; calculate dx = X2[i] - X1[i]
    movss   xmm0, [rsi + rax*4] ; load X2[i]
    movss   xmm1, [rdi + rax*4] ; load X1[i]
    subss   xmm0, xmm1          ; dx = X2[i] - X1[i]

    ; square dx
    mulss   xmm0, xmm0          ; dx^2

    ; calculate dy = Y2[i] - Y1[i]
    movss   xmm1, [r13 + rax*4] ; load Y2[i]
    movss   xmm2, [r12 + rax*4] ; load Y1[i]
    subss   xmm1, xmm2          ; dy = Y2[i] - Y1[i]

    ; square dy
    mulss   xmm1, xmm1          ; dy^2

    ; sum squared differences
    addss   xmm0, xmm1          ; dx^2 + dy^2

    ; calculate square root
    sqrtss  xmm0, xmm0          ; sqrt(dx^2 + dy^2)

    ; store result
    movss   [r14 + rax*4], xmm0 ; Z[i] = distance

    ; increment loop counter
    inc     rax                 ; i++

    jmp     .loop_start         ; repeat loop

.loop_end:
    ; restore callee-saved registers
    pop     r14
    pop     r13
    pop     r12
    pop     rsi
    pop     rdi

    ret                         