#include "type_defs.h"

/**
 * Checks if AVX2 (Advanced Vector Extensions 2) is supported by the CPU.
 * AVX2 is a set of SIMD (Single Instruction, Multiple Data) instructions that can
 * significantly improve performance for certain tasks.
 *
 * @return True if AVX2 is supported, false otherwise.
 */
bool isAVX2Supported()
{
    int info[4];

#ifdef _MSC_VER
    __cpuidex(info, 0, 0);  // Get maximum input value for extended info.
    if (info[0] < 7)
        return false;  // Check if CPU supports extended features.

    __cpuidex(info, 7, 0);  // Query extended features (leaf 7, subleaf 0).
    return (info[1] & (1 << 5)) != 0;  // Check if AVX2 (bit 5 of EBX) is supported.

#elif defined(__GNUC__) || defined(__clang__)
    if (__get_cpuid_max(0, 0) < 7) return false;

    unsigned int ebx = 0, ecx = 0, edx = 0;
    __cpuid_count(7, 0, info[0], ebx, ecx, edx);
    return (ebx & (1 << 5)) != 0;  // Check if AVX2 (bit 5 of EBX) is supported.
#endif

    return false;
}

// Global constant to check if AVX2 is supported.
const bool AVX2Support = isAVX2Supported();
