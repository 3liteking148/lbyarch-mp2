# Introduction
This study investigated the performance of the dot product operation of double-precision floating point numbers.

# Implementations
## C Implementation
The C implementation uses a for loop to multiply each pair of elements in A and B.

## Initial (Naive) Assembly Implementation


## Fused-Multiply Add Implementation
To further increase the performance, we can use dedicated instructions included in the processor. The fused multiply-add operation combines the previously separate multiplication and addition operations into one operation. The implementation uses the `VFMADD231SD` variant, wherein the double-precision floats 2nd and 3rd operands are multiplied together, and then added to the 1st double-precision operand. Note that rounding is only performed after the addition, which might slightly alter the results compared to the C implementation.

## Vectorized Fused-Multiply Add Implementation
As the scalar `VFMADD231SD` instruction has a vector counterpart `VFMADD231PD`, we can use that to perform 4 operations instead of one at a time. We can calculate four partial dot products, and add them together to get the resulting dot product. Note that as floating point operations are generally not associative, and that the order of additions in the kernel is altered due to the partial dot products, the results might be slightly different compared to the C, assembly, and FMA kernels.

The implementation uses `MOVAPD` to move four double-precision floating point numbers stored in memory to the SIMD registers. The use of `MOVAPD` (aligned memory) instead of `MOVUPD` (unaligned memory) assumes that the caller stored the numbers in an array with a starting index aligned to a 32-byte boundary, which can be done with `_aligned_malloc`.

# Results and Discussion
## Correctness
It works with minor discrepancies with FMA and vectorized due 
## Performance
The test was performed on a laptop with Intel(R) Core(TM) i7-8550U CPU @ 1.80GHz 1.99 GHz, with 36 GB of RAM.

| Kernel | Average Time (n=30) |
| ------ | ------------------- |
| C | 0.00 ms |
| Naive Assembly | 0.00 ms |
| FMA | 0.00 ms |
| Vectorized FMA | 0.00 ms |

## Discussion
