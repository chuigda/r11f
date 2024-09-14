    .global r11f_call_fn
    .type r11f_call_fn, @function
; uint64_t r11f_call_fn(void *f, char *descriptor, uint32_t *args)
r11f_call_fn:
    push %rbp
    mov %rsp, %rbp
    push %r12
    push %r13
    push %r14
    push %r15
    push %rbx
    mov %rdi, %r12
    lea (%rsi,1), %r13 ; descriptor = descriptor + 1
    mov %rdx, %r14
    mov $0, %r15 ; args_idx = 0
    mov $0, %rbx

.loop:
    ; %al = *descriptor
    mov (%rsi), %al
    ; if (%al == ')') break;
    cmp $')', %al
    je .loop_end
    ; if this argument takes up 8 bytes, we will compose two 32-bit arguments
    ; if (%al == 'J' or %al == 'D' or %al == 'L') {
    ;     push_twice
    ; }

.loop_end: