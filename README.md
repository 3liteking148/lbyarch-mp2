# Introduction
This study investigated the performance of the dot product operation of double-precision floating point numbers.

# Implementations
## C Implementation
The C implementation uses a for loop to multiply each pair of elements in A and B.

## Initial Assembly Implementation
The Assembly implemention simply uses ADDSD and VMULSD to multiply the given pair of floats, and readd it to the main XMM0. The loop increments through each value one at a time. 

## Fused Multiply-Add Implementation
To further increase the performance, we can use dedicated instructions included in recent processors. The fused multiply-add operation combines the previously separate multiplication and addition operations into one operation. The implementation uses the `VFMADD231SD` variant, wherein the double-precision floats 2nd and 3rd operands are multiplied together, and then added to the 1st double-precision operand. Note that rounding is only performed after the addition, rather than both after multiplying and adding, which might slightly alter the results compared to the C and initial assembly implementations.

## Vectorized Fused-Multiply Add Implementation
As the scalar `VFMADD231SD` instruction has a vector counterpart `VFMADD231PD`, we can use that to perform four fused multiply-add operations at a time instead of one. We can calculate four partial dot products, repeatedly loading four inputs each from A and B, multiplying the i-th loaded input of A with the i-th loaded input of B, and adding it (the product) to the i-th partial product. We then combine the four afterward to get the resulting dot product. Since floating point operations are generally not associative, and the order of additions in the kernel is altered due to the need for partial dot products, the results might be slightly different compared to the C, initial assembly, and FMA kernels.

The implementation uses `MOVAPD` to move four double-precision floating point numbers stored in memory to the SIMD registers. The use of `MOVAPD` (aligned memory) instead of `MOVUPD` (unaligned memory) assumes that the caller stored the numbers in an array with a starting index aligned to a 32-byte boundary, which can be created with `_aligned_malloc`.

# Results and Discussion
## Correctness
It works 100% as the numbers have exact representations
## Performance
The test was performed on a laptop with Intel(R) Core(TM) i7-8550U CPU @ 1.80GHz 1.99 GHz, with 36 GB of RAM on the Windows OS. Four different versions all in one program: the ASM and C kernel, plus the additional two using different functions and optimizations. The time tracked was taken using QueryPerformanceCounter, which provides a more accurate time in Windows. 

| N (vector lengths) | Initial Assembly | C            | FMA          | Vectorized FMA |
| ------------------ | ---------------- | ------------ | ------------ | -------------- |
| 2^20 (debug)       | 3.120340ms       | 6.728570ms   | 3.663050ms   | 2.208210ms     |
| 2^20 (release)     | 3.289010ms       | 3.240570ms   | 4.364267ms   | 2.270147ms     |
| 2^24 (debug)       | 42.161473ms      | 88.260553ms  | 42.349203ms  | 23.812050ms    |
| 2^24 (release)     | 52.466890ms      | 49.693583ms  | 44.498450ms  | 24.706360ms    |
| 2^28 (debug)       | 638.574887ms     | 1363.086077ms| 636.300113ms | 320.098723ms   |
| 2^28 (release)     | 644.189560ms     | 618.850377ms | 643.617250ms | 330.266897ms   |
| 2^28 (debug)       | 638.574887ms     | 1363.086077ms| 636.300113ms | 320.098723ms   |
| 2^28 (release)     | 644.189560ms     | 618.850377ms | 643.617250ms | 330.266897ms   |
| 2^29 (debug)       | 1272.739950ms    | 2727.105560ms| 1270.509047ms| 634.302463ms   |
| 2^29 (release)     | 1271.209250ms    | 1223.394793ms| 1271.269400ms| 633.611443ms   |
| 2^30 (debug)       | 2636.973870ms    | 5587.149803ms| 2594.649643ms| 1335.510820ms  |
| 2^30 (release)     | 2552.125013ms    | 2451.890587ms| 2543.640863ms| 1261.811573ms  |


## Discussion

Based on the table, in debug mode, the Assembly kernel runs faster than its C counterpart by more than double. However, in its release form, the C kernel performs twice as fast, halving its time. This is likely due to the debug version having to keep track of the values generated within the function for debugging purposes as compared to the Assembly versions that can't track its own variables thus not requiring more runtime. Additionally, the time it took to run the values seems to take around twice as long proportional to the exponent of the number of values. This is evident in the last three since they only have gaps on 1, but 2^24 took around ~14 times as long as 2^20 and 2^28 took around ~15 times as long which is close to being 2^4 or 16.

As for the FMA versions, the basic one ran similarly to the Assembly version, though it performed slightly better with more variables. Meanwhile, Vectorized FMA took nearly half as much time as both the C release kernel and the Assembly kernel. This is due to the fact that it calculates values 4 at a time as compared to individually incrementing the loop in order to find each value.

Overall, based on the runtimes observed. the number of variables increases proportionally with the amount variables by an order of 2. So having 2^n+1 as compared to 2^n instructions will net you double the amount of time taken. Additionally, debug and release versions differ in time as debug requires additional time and memory to store variables in between lines while the release version simply forgoes them. 
