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
Here is a screenshot of the result found from testing with 2^30 elements:
![image](https://github.com/3liteking148/lbyarch-mp2/assets/113081450/102a3691-23bd-4459-ba5d-05393b8e2a73)

## Performance
The test was performed on a laptop with Intel(R) Core(TM) i7-8550U CPU @ 1.80GHz 1.99 GHz, with 36 GB of RAM on the Windows OS. Four different versions all in one program: the ASM and C kernel, plus the additional two using different functions and optimizations. The time tracked was taken using QueryPerformanceCounter, which provides a more accurate time in Windows. 

| N (vector lengths) | Initial Assembly | C            | FMA          | Vectorized FMA |
| ------------------ | ---------------- | ------------ | ------------ | -------------- |
| 2^20 (debug)       | 1.995113ms       | 3.199453ms   | 1.888377ms   | 1.543543ms     |
| 2^20 (release)     | 1.991820ms       | 1.818743ms   | 2.094620ms   | 1.344800ms     |
| 2^24 (debug)       | 23.016667ms      | 41.188473ms  | 36.509147ms  | 20.148133ms    |
| 2^24 (release)     | 23.882043ms      | 22.106873ms  | 37.323137ms  | 20.286847ms    |
| 2^28 (debug)       | 527.413747ms     | 1111.007353ms| 517.955873ms | 315.934943ms   |
| 2^28 (release)     | 523.778360ms     | 497.623133ms | 516.181433ms | 313.955273ms   |
| 2^29 (debug)       | 1100.516717ms    | 2343.046567ms| 1117.419173ms| 641.342950ms   |
| 2^29 (release)     | 1090.384547ms    | 1040.778863ms| 1104.056477ms| 626.228163ms   |
| 2^30 (debug)       | 2378.512080ms    | 4711.526247ms| 2574.053550ms| 1410.134207ms  |
| 2^30 (release)     | 2310.322263ms    | 2290.322107ms| 2363.760447ms| 1290.921523ms  |

## Discussion

Based on the table, in debug mode, the Assembly kernel runs faster than its C counterpart by more than double. However, in its release form, the C kernel performs twice as fast, halving its time. This is likely due to the debug version having to keep track of the values generated within the function for debugging purposes as compared to the Assembly versions that can't track its own variables thus not requiring more runtime. Additionally, the time it took to run the values seems to take around twice as long proportional to the exponent of the number of values. This is evident in the last three since they only have gaps on 1, but 2^24 took around ~14 times as long as 2^20 and 2^28 took around ~15 times as long which is close to being 2^4 or 16.

As for the FMA versions, the basic one ran similarly to the Assembly version, though it performed slightly better with more variables. Meanwhile, Vectorized FMA took nearly half as much time as both the C release kernel and the Assembly kernel. This is due to the fact that it calculates values 4 at a time as compared to individually incrementing the loop in order to find each value.

Overall, based on the runtimes observed. the number of variables increases proportionally with the amount variables by an order of 2. So having 2^n+1 as compared to 2^n instructions will net you double the amount of time taken. Additionally, debug and release versions differ in time as debug requires additional time and memory to store variables in between lines while the release version simply forgoes them. 
