section .text
global dot_product_fma_avx2
dot_product_fma_avx2:
    ; *A stored at rcx
    ; *B stored at rdx
    ; n stored at r8
    ; *sdot stored at r9
    
    ; this stores the dot product before moving to *sdot
    pxor xmm0, xmm0
    
    ; instead of doing A[i], we do A[n - num_elements_remaining] so we dont have to do "i < n"
    ; convert A and B to pointers pointing to the end of the arrays
    lea rcx, [rcx + r8 * 8]
    lea rdx, [rdx + r8 * 8]
    
    ; since we calculate 4 at a time, there are cases where there won't be enough elements, we calculate them scalarly instead
    mov rax, r8
    and rax, 0b11 ; calculate n remainder 4
    xor r8, rax ; ensure that n is divisible by 4 now
    
    ; since subtraction cannot be done in x86 addressing, convert to -num_elements_remaining
    neg r8
    jz end

    vpxor ymm3, ymm3
add_loop:
    vmovapd ymm1, [rcx + r8 * 8]
    vmovapd ymm2, [rdx + r8 * 8]

    ; ymm3 = ymm1 * ymm2 + ymm3
    vfmadd231pd ymm3, ymm1, ymm2

    ; next 4 elements
    add r8, 4
    jnz add_loop

    ; obtain the sum of the 4 dot product streams
    sub rsp, 32
    vmovupd [rsp], ymm3
    addsd xmm0, [rsp + 0]
    addsd xmm0, [rsp + 8]
    addsd xmm0, [rsp + 16]
    addsd xmm0, [rsp + 24]
    add rsp, 32
    
    ; include the leftover elements, if there's any
    sub r8, rax
    jz end

add_loop_scalar:
    movsd xmm1, [rcx + r8 * 8]
    movsd xmm2, [rdx + r8 * 8]
    vfmadd231sd xmm0, xmm1, xmm2

    ; next element
    inc r8
    jnz add_loop_scalar

end:
    ; move dot product to *sdot
    movsd [r9], xmm0
    ret