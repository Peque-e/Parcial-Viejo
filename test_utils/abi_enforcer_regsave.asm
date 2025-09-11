global abienf_regsave
abienf_regsave:
    mov [rdi+ 0], rbx
    mov [rdi+ 8], rbp
    mov [rdi+16], r12
    mov [rdi+24], r13
    mov [rdi+32], r14
    mov [rdi+40], r15
    ret

