section .text
bits 64
default rel
global dot_product_asm
dot_product_asm:
    ; *A stored at rcx
    ; *B stored at rdx
    ; n stored at r8
    ; *sdot stored at r9

    mov r9, 0
    
    ; empty out xmm0
    pxor xmm0, xmm0
    
    lea rcx, [rcx]
    lea rdx, [rdx]

mul_and_add:
    movsd xmm1, [rcx]
    movsd xmm2, [rdx]
    vmulsd xmm3, xmm1, xmm2
    addsd xmm0, xmm3
    
    inc rcx
    inc rdx
    inc r10
    cmp r8, r10
    jnz mul_and_add

end:
    movsd [r9], xmm0
    ret