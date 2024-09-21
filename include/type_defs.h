#pragma once
#include <cstdint>
#include <ostream>
#include <sstream>
#include <algorithm>


extern bool const AVX2Support;

// Platform-specific popcount function, which calculates the number of set bits in an integer.

#ifdef _MSC_VER  // For MSVC
#include <intrin.h>
#pragma intrinsic(__popcnt)

/**
 * Returns the number of set bits in a 32-bit unsigned integer.
 * @param x The 32-bit unsigned integer.
 * @return The number of set bits (population count).
 */
inline uint32_t popcount(uint32_t x) {
    return __popcnt(x);
}

/**
 * Returns the number of set bits in a 64-bit unsigned integer.
 * @param x The 64-bit unsigned integer.
 * @return The number of set bits (population count).
 */
inline uint64_t popcount(uint64_t x) {
    return __popcnt64(x);
}

#elif defined(__GNUC__) || defined(__clang__)
#include <cpuid.h>

/**
 * Returns the number of set bits in a 32-bit unsigned integer.
 * @param x The 32-bit unsigned integer.
 * @return The number of set bits (population count).
 */
inline uint32_t popcount(uint32_t x) {
    return __builtin_popcount(x);
}

/**
 * Returns the number of set bits in a 64-bit unsigned integer.
 * @param x The 64-bit unsigned integer.
 * @return The number of set bits (population count).
 */
inline uint64_t popcount(uint64_t x) {
    return __builtin_popcountll(x);
}
#endif

namespace kMismatchIntegerType {
    // Integer type used for sequences in the k-mismatch search algorithms.
    typedef uint64_t uint_type;
    const size_t UINT_TYPE_SIZE = 64;
}

/**
 * Template class representing a base sequence of binary integers.
 * This class provides basic functionality for handling binary integer sequences, including
 * comparison operators, size retrieval, and output formatting.
 */
template <class Derived>
class BinaryIntBaseSequence
{
public:
    /**
     * Constructor that initializes the binary integer sequence.
     * @param sequenceInt The binary integer representing the sequence.
     */
    BinaryIntBaseSequence(kMismatchIntegerType::uint_type sequenceInt)
    {
        this->sequenceInt = sequenceInt;
        kMismatchIntegerType::uint_type formIntSeq = sequenceInt;
        size_t result = 0;
        while (formIntSeq) {
            result++;
            formIntSeq >>= 1;
        }
        this->size = result;
    }

    /// Copy constructor.
    BinaryIntBaseSequence(const BinaryIntBaseSequence& other)
    {
        this->sequenceInt = other.sequenceInt;
        this->size = other.getSize();
    }

    /// Assignment operator.
    Derived& operator=(const Derived& other)
    {
        this->sequenceInt = other.sequenceInt;
        return static_cast<Derived&>(*this);
    }

    /**
     * Less-than operator for comparing binary sequences, used for strong total ordering in sets and maps.
     * @param other The other binary sequence to compare against.
     * @return True if this sequence is less than the other.
     */
    bool operator<(const Derived& other) const {
        return sequenceInt < other.sequenceInt;
    }

    /// Returns the size of the binary sequence.
    size_t getSize() const
    {
        return this->size;
    }

    /**
     * Overloads the << operator for BinaryIntBaseSequence objects.
     * Prints the binary representation of the sequence to an output stream.
     * @param os The output stream.
     * @param binaryIntBaseSequence The binary integer sequence to print.
     * @return The output stream with the printed sequence.
     */
    friend std::ostream& operator<<(std::ostream& os, const BinaryIntBaseSequence<Derived>& binaryIntBaseSequence)
    {
        kMismatchIntegerType::uint_type sequenceInt = binaryIntBaseSequence.sequenceInt;
        std::ostringstream oss;
        std::string resultToPrint;
        do {
            oss << (sequenceInt & 1); // last bit
            sequenceInt >>= 1;
        } while (sequenceInt);
        resultToPrint = oss.str();

        // Reverse the result so the MSB is on the left.
        std::reverse(resultToPrint.begin(), resultToPrint.end());
        os << resultToPrint;
        return os;
    }

protected:
    kMismatchIntegerType::uint_type sequenceInt;  ///< The binary integer representing the sequence.
    size_t size;  ///< The size of the sequence (number of bits).
};
