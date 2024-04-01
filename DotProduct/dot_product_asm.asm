section .text
global dot_product_asm
dot_product_asm:
    ; *A stored at rcx
    ; *B stored at rdx
    ; n stored at r8
    ; *sdot stored at r9

    mov r15, 0
    
    ; empty out xmm0
    pxor xmm0, xmm0
    
mul_and_add:
    cmp r8, r15    
    jz end         
    movsd xmm1, [rcx]
    movsd xmm2, [rdx]
    vmulsd xmm3, xmm1, xmm2
    addsd xmm0, xmm3
    
    add rcx, 8
    add rdx, 8
    inc r15
    jmp mul_and_add

end:
    movsd [r9], xmm0
    ret