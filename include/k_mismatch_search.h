#pragma once
#include <string>
#include <map>
#include <vector>
#include <set>
#include "mcs.h"
#include <fstream>
#include <random>
#include <numeric>
#include <execution>
#include <unordered_map>
#include <immintrin.h>
#include "type_defs.h"
#include <iostream>


//
// KMismatchSearch class performs k-mismatch search operations on text strings.
// This class can be constructed using text and query files, and it allows for searches with specified mismatch thresholds.
// The search functionality is supported through either MCS-based or naive approaches.

class KMismatchSearch
{
public:
    /// Default constructor that initializes empty text, queries, MCS, and cache.
    KMismatchSearch();

    /**
     * Constructor to initialize the search with text and queries from files, and a number of mismatches for building MCS.
     * @param textFile Path to the text file to search in.
     * @param queriesFile Path to the file containing query strings.
     * @param misMatches Number of allowed mismatches during the search.
     */
    KMismatchSearch(std::string textFile, std::string queriesFile, int misMatches);

    /**
     * Constructor to initialize the search with text and queries from files, and MCS data from a file.
     * @param textFile Path to the text file to search in.
     * @param queriesFile Path to the file containing query strings.
     * @param mcsFile Path to the file containing precomputed MCS data.
     */
    KMismatchSearch(std::string textFile, std::string queriesFile, std::string mcsFile);

    /**
     * Constructor to initialize the search with text, queries, MCS data, and cache from files.
     * @param textFile Path to the text file to search in.
     * @param queriesFile Path to the file containing query strings.
     * @param mcsFile Path to the file containing precomputed MCS data.
     * @param cacheFile Path to the file containing cached search results.
     */
    KMismatchSearch(std::string textFile, std::string queriesFile, std::string mcsFile, std::string cacheFile);

    /// Sets the text for the search.
    void setText(std::string& textToSet);

    /// Returns the current text used for the search.
    const std::string& getText() const;

    /// Sets the query strings for the search.
    void setQueries(std::vector<std::string>& queriesToSet);

    /// Returns the current query strings used for the search.
    const std::vector<std::string>& getQueries() const;

    /// Sets the MCS (multiple common subsequence) object for the search.
    void setMcs(MCS& mcsToSet);

    /// Returns the current MCS object used for the search.
    const MCS& getMcs() const;

    /// Sets the cache for the search.
    void setCache(std::map<std::string, std::set<size_t>>& cacheToSet);

    /// Returns the current cache used for the search.
    const std::map<std::string, std::set<size_t>>& getCache() const;

    /// Loads the text from a file.
    std::string loadTextFromFile(std::string& filename) const;

    /// Loads query strings from a file.
    std::vector<std::string> loadQueriesFromFile(std::string& filename) const;

    /// Loads cached search results from a file.
    std::map<std::string, std::set<size_t>> loadCacheFromFile(std::string& fileName) const;

    /// Saves the current cache to a file.
    void saveCacheToFile(std::string fileName);

    /// Performs an MCS-based search with a specified mismatch threshold.
    std::map<std::string, std::set<size_t>> mcsSearch(size_t misMatches);

    /// Performs a naive search with a specified mismatch threshold.
    std::map<std::string, std::set<size_t>> naiveSearch(size_t misMatches);

private:
    /// Checks if a query matches the text at a given position with the allowed number of mismatches.
    bool CheckQueryOnPosition(const std::string& query, int64_t position, size_t misMatches) const;

    std::string text;  ///< The text to search in.
    std::vector<std::string> queries;  ///< The query strings for the search.
    std::map<std::string, std::set<size_t>> cache;  ///< The cache storing previous search results.
    MCS mcs;  ///< The MCS object used in the search.
};
