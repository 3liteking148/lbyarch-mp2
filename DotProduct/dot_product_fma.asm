section .text
global dot_product_fma
dot_product_fma:
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
    
    ; since subtraction cannot be done in x86 addressing, convert to -num_elements_remaining
    neg r8
    jz end

add_loop_scalar: ; add vector prefix to movsd because https://stackoverflow.com/questions/41303780/why-is-this-sse-code-6-times-slower-without-vzeroupper-on-skylake
    vmovsd xmm1, [rcx + r8 * 8]
    vmovsd xmm2, [rdx + r8 * 8]
    vfmadd231sd xmm0, xmm1, xmm2

    ; next element
    inc r8
    jnz add_loop_scalar

end:
    ; move dot product to *sdot
    movsd [r9], xmm0
    ret