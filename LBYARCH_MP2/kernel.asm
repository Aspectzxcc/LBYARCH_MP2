; Fixed x86-64 assembly implementation for Euclidean distance calculation
; Targets Windows x64 calling convention with scalar SSE instructions
;
; Function signature:
; void calculate_distance_asm(long long n, const float* X1, const float* X2, const float* Y1, const float* Y2, float* Z);
;
; Windows x64 calling convention:
; RCX: n (vector length)
; RDX: X1 pointer
; R8:  X2 pointer  
; R9:  Y1 pointer
; Stack: Y2 pointer at [RSP + 8*5] (5th parameter)
; Stack: Z pointer at [RSP + 8*6] (6th parameter)

section .text
    global calculate_distance_asm

calculate_distance_asm:
    ; Save callee-saved registers
    push    rbx         ; Save RBX
    push    rdi         ; Save RDI (will hold X1)
    push    rsi         ; Save RSI (will hold X2)
    push    r12         ; Save R12 (will hold Y1)
    push    r13         ; Save R13 (will hold Y2)
    push    r14         ; Save R14 (will hold Z)
    
    ; Move parameters to callee-saved registers
    mov     rbx, rcx    ; n (vector length)
    mov     rdi, rdx    ; X1 pointer
    mov     rsi, r8     ; X2 pointer
    mov     r12, r9     ; Y1 pointer
    
    ; Load stack parameters (adjusted for 48 bytes of pushes + 8 bytes return address = 56 bytes)
    ; Original stack positions were [RSP + 40] and [RSP + 48] (before any pushes)
    ; After 6 pushes (48 bytes), they become [RSP + 40 + 48] and [RSP + 48 + 48]
    mov     r13, qword [rsp + 88] ; Y2 pointer (40 + 48 = 88)
    mov     r14, qword [rsp + 96] ; Z pointer (48 + 48 = 96)
    
    ; Initialize loop index
    xor     rax, rax    ; i = 0

.loop_start:
    ; Check loop condition
    cmp     rax, rbx    ; compare i with n
    jge     .loop_end   ; exit if i >= n
    
    ; Calculate dx = X2[i] - X1[i]
    movss   xmm0, dword [rsi + rax*4] ; load X2[i]
    movss   xmm1, dword [rdi + rax*4] ; load X1[i]
    subss   xmm0, xmm1                ; dx = X2[i] - X1[i]
    
    ; Square dx
    mulss   xmm0, xmm0                ; dx^2
    
    ; Calculate dy = Y2[i] - Y1[i]
    movss   xmm1, dword [r13 + rax*4] ; load Y2[i]
    movss   xmm2, dword [r12 + rax*4] ; load Y1[i]
    subss   xmm1, xmm2                ; dy = Y2[i] - Y1[i]
    
    ; Square dy
    mulss   xmm1, xmm1                ; dy^2
    
    ; Sum squared differences
    addss   xmm0, xmm1                ; dx^2 + dy^2
    
    ; Calculate square root
    sqrtss  xmm0, xmm0                ; sqrt(dx^2 + dy^2)
    
    ; Store result
    movss   dword [r14 + rax*4], xmm0 ; Z[i] = distance
    
    ; Increment loop counter
    inc     rax                       ; i++
    jmp     .loop_start               ; repeat loop

.loop_end:
    ; Restore callee-saved registers in reverse order
    pop     r14
    pop     r13
    pop     r12
    pop     rsi
    pop     rdi
    pop     rbx
    ret