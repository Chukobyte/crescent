#pragma once

#include <stdio.h>
#include <time.h>

#define SE_PROFILE_CODE(CodeBlock)                                   \
{                                                                    \
clock_t start_time, end_time;                                        \
double cpu_time_used;                                                \
start_time = clock();                                                \
CodeBlock                                                            \
end_time = clock();                                                  \
cpu_time_used = ((double) (end_time - start_time)) / CLOCKS_PER_SEC; \
printf("Time taken: %f seconds\n", cpu_time_used);                   \
}

/*
 * // Example
 * SE_PROFILE_CODE(
 *     for (int i = 0; i < 1000000; i++) {}
 * )
 */

#define SE_PROFILE_CODE_WITH_VAR(CpuTimeVar, CodeBlock)             \
{                                                                   \
clock_t start_time, end_time;                                       \
start_time = clock();                                               \
CodeBlock                                                           \
end_time = clock();                                                 \
(CpuTimeVar) = ((double) (end_time - start_time)) / CLOCKS_PER_SEC; \
}

/*
 * // Example:
 * double cpu_time_taken;
 * SE_PROFILE_CODE_WITH_VAR(
 *     cpu_time_taken,
 *     for (int i = 0; i < 1000000; i++) {}
 * )
 * printf("Time taken: %f seconds\n", cpu_time_taken);
 */
