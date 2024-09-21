#pragma once
#include <vector>
#include <tuple>
#include "type_defs.h"
#include <ostream>
#include "set"
#include <algorithm>
#include <map>
#include <execution>
#include <iostream>
#include <functional>
#include <fstream>

//
// The Form class represents a sequence of binary values (ones and zeros).
// It provides functionality to generate forms with a specified length and mismatch threshold, 
// as well as extracting a string representation from a specific position in a text.
//
class Form : public BinaryIntBaseSequence<Form>
{
public:
    // Inherits constructor from BinaryIntBaseSequence
    using BinaryIntBaseSequence::BinaryIntBaseSequence;

    // Inherits the getSize method from BinaryIntBaseSequence
    using BinaryIntBaseSequence::getSize;

    /**
      * Generates all forms of ones and zeros in a binary sequence with a maximal length.
      * Every combination begins and ends with 1.
     *
     * @param length Length of the forms to generate.
     * @param mismatchK Maximum number of mismatches allowed.
     * @return A vector of Form objects representing all generated forms.
     */
    static std::vector<Form> generateAllForms(uint64_t length, uint64_t mismatchK);

    /**
     * Extracts a substring from a given position in the original string, based on the form's binary sequence.
     *
     * @param str The original string.
     * @param pos The starting position in the string.
     * @return A string extracted from the original text based on the form's pattern.
     */
    std::string getStringFromPosition(const std::string& str, size_t pos) const;

    friend class Combination;
};

//
// The Combination class represents a binary sequence that can contain multiple forms.
// It allows checking if a form is part of a combination and can generate all possible forms.
//
class Combination : public BinaryIntBaseSequence<Combination>
{
public:
    // Inherits constructor from BinaryIntBaseSequence
    using BinaryIntBaseSequence::BinaryIntBaseSequence;

    // Inherits the getSize method from BinaryIntBaseSequence
    using BinaryIntBaseSequence::getSize;

    /**
     * Checks if the combination contains the specified form.
     *
     * @param form The form to check for.
     * @return True if the form is contained in the combination, otherwise false.
     */
    bool contains(const Form& form) const;

    /**
     * Generates all forms within the combination that have a specified number of matches.
     *
     * @param matches Number of matching ones in the form.
     * @return A set of Form objects representing all matching forms.
     */
    std::set<Form> getAllForms(uint64_t matches) const;

    /**
     * Generates all combinations with a given length and mismatch threshold.
     *
     * @param length Length of the combination.
     * @param mismatchK Maximum number of mismatches allowed.
     * @return A vector of Combination objects representing all generated combinations.
     */
    static std::vector<Combination> generateAllCombinations(uint64_t length, uint64_t mismatchK);
};

//
// The MCS (Multiple Common Subsequence) class represents a set of forms used for k-mismatch search.
// It provides methods to build an MCS using a multithreaded approach, as well as saving and loading the MCS from files.
//
class MCS
{
public:
    /// Default constructor initializes an empty MCS.
    MCS();

    /**
     * Returns the forms contained in the MCS.
     *
     * @return A constant reference to the vector of Form objects in the MCS.
     */
    const std::vector<Form>& getMcsForms() const;

    /**
     * Builds an MCS using a naive multithreaded approach based on the given queries and mismatch threshold.
     *
     * @param queries A vector of query strings.
     * @param mismatchK Maximum number of mismatches allowed.
     * @return An MCS object built from the queries.
     */
    static MCS buildMCSNaiveMultithreaded(std::vector<std::string>& queries, uint64_t mismatchK);

    /**
     * Loads an MCS from a file.
     *
     * @param fileName The name of the file containing the MCS data.
     * @return An MCS object loaded from the file.
     */
    static MCS loadFromFile(std::string fileName);

    /**
     * Saves the current MCS to a file.
     *
     * @param fileName The name of the file to save the MCS to.
     */
    const void saveToFile(std::string fileName) const;

private:
    std::vector<Form> mcsForms;  ///< The forms contained in the MCS.
};
