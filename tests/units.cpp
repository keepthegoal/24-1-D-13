#include <cassert>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include <chrono>
#include "../include/gen_samples.h"
#include "../include/k_mismatch_search.h"
#include "../include/mcs.h"
#include "../include/type_defs.h"

// Function declarations
int safeStoi(const char* str, const std::string& paramName);

void testSafeStoi() {
    std::cout << "Starting testSafeStoi()" << std::endl;
    assert(safeStoi("123", "test") == 123);
    
    try {
        safeStoi("abc", "test");
        assert(false);
    } catch (const std::invalid_argument&) {
        // Expected
    }
    
    try {
        safeStoi("2147483648", "test");
        assert(false);
    } catch (const std::out_of_range&) {
        // Expected
    }
    std::cout << "Finished testSafeStoi()" << std::endl;
}

void testKMismatchSearchEdgeCases() {
    std::cout << "Starting testKMismatchSearchEdgeCases()" << std::endl;
    try {
        // Test with empty text
        {
            std::ofstream textFile("temp_empty_text.txt");
            std::ofstream queriesFile("temp_queries.txt");
            queriesFile << "ACGT";
        }
        KMismatchSearch emptyTextSearch("temp_empty_text.txt", "temp_queries.txt", 0);
        auto result = emptyTextSearch.mcsSearch(0);
        assert(result.empty());

        // Test with empty queries
        {
            std::ofstream textFile("temp_text.txt");
            textFile << "ACGTACGTACGT";
            std::ofstream queriesFile("temp_empty_queries.txt");
        }
        KMismatchSearch emptyQueriesSearch("temp_text.txt", "temp_empty_queries.txt", 0);
        result = emptyQueriesSearch.mcsSearch(0);
        assert(result.empty());

        // Test with mismatches > query length
        {
            std::ofstream textFile("temp_text.txt");
            textFile << "ACGTACGTACGT";
            std::ofstream queriesFile("temp_queries.txt");
            queriesFile << "ACGT";
        }
        KMismatchSearch largeMismatchSearch("temp_text.txt", "temp_queries.txt", 5);
        result = largeMismatchSearch.mcsSearch(5);
        assert(result["ACGT"].size() == 12);  // Should match everywhere

        // Clean up
        std::remove("temp_empty_text.txt");
        std::remove("temp_text.txt");
        std::remove("temp_empty_queries.txt");
        std::remove("temp_queries.txt");
    } catch (const std::exception& e) {
        std::cerr << "Exception in testKMismatchSearchEdgeCases: " << e.what() << std::endl;
        throw;
    }
    std::cout << "Finished testKMismatchSearchEdgeCases()" << std::endl;
}

void testLargeInputs() {
    std::cout << "Starting testLargeInputs()" << std::endl;
    try {
        const int textLen = 1000000;
        const int queryCount = 1000;
        const int queryLen = 10;
        const int misMatches = 2;

        std::string largeText = initRandomText(textLen, 4, 0);
        std::vector<std::string> largeQueries = initRandomQueries(largeText, queryCount, queryLen);

        // Write to files
        std::ofstream textFile("large_text.txt");
        textFile << largeText;
        textFile.close();

        std::ofstream queriesFile("large_queries.txt");
        for (const auto& query : largeQueries) {
            queriesFile << query << std::endl;
        }
        queriesFile.close();

        KMismatchSearch largeSearch("large_text.txt", "large_queries.txt", misMatches);

        auto start = std::chrono::high_resolution_clock::now();
        auto mcsResult = largeSearch.mcsSearch(misMatches);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        std::cout << "MCS search took " << duration.count() << " ms" << std::endl;

        start = std::chrono::high_resolution_clock::now();
        auto naiveResult = largeSearch.naiveSearch(misMatches);
        end = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        std::cout << "Naive search took " << duration.count() << " ms" << std::endl;

        assert(mcsResult == naiveResult);

        // Clean up
        std::remove("large_text.txt");
        std::remove("large_queries.txt");
    } catch (const std::exception& e) {
        std::cerr << "Exception in testLargeInputs: " << e.what() << std::endl;
        throw;
    }
    std::cout << "Finished testLargeInputs()" << std::endl;
}

void testKMismatchSearch() {
    std::cout << "Starting testKMismatchSearch()" << std::endl;
    try {
        // Create temporary files for testing
        {
            std::ofstream textFile("temp_text.txt");
            if (!textFile) {
                throw std::runtime_error("Failed to open temp_text.txt for writing");
            }
            textFile << "ACGTACGTACGT";
        }

        {
            std::ofstream queriesFile("temp_queries.txt");
            if (!queriesFile) {
                throw std::runtime_error("Failed to open temp_queries.txt for writing");
            }
            queriesFile << "ACGT\nCGTA\nTACG";
        }

        int misMatches = 1;
        KMismatchSearch kMismatchSearch("temp_text.txt", "temp_queries.txt", misMatches);
        
        auto result = kMismatchSearch.mcsSearch(misMatches);
        assert(result.size() == 3);
        assert(result["ACGT"].size() == 3);
        assert(result["CGTA"].size() == 3);
        assert(result["TACG"].size() == 3);
        
        auto naiveResult = kMismatchSearch.naiveSearch(misMatches);
        assert(result == naiveResult);

        // Clean up temporary files
        if (std::remove("temp_text.txt") != 0 || std::remove("temp_queries.txt") != 0) {
            std::cerr << "Warning: Failed to remove temporary files" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception in testKMismatchSearch: " << e.what() << std::endl;
        throw;
    }
    std::cout << "Finished testKMismatchSearch()" << std::endl;
}

void testRandomTextAndQueries() {
    std::cout << "Starting testRandomTextAndQueries()" << std::endl;
    try {
        static const int alphabetSize = 4;
        static const int numberOfQueries = 5;  
        static const int queryLen = 5;         
        static const size_t textLen = 100;     
        const int misMatches = 1;              

        std::string randomText = initRandomText(textLen, alphabetSize, 0);
        std::vector<std::string> randomQueries = initRandomQueries(randomText, numberOfQueries, queryLen);

        // Create temporary files for testing
        {
            std::ofstream textFile("temp_random_text.txt");
            if (!textFile) {
                throw std::runtime_error("Failed to open temp_random_text.txt for writing");
            }
            textFile << randomText;
        }

        {
            std::ofstream queriesFile("temp_random_queries.txt");
            if (!queriesFile) {
                throw std::runtime_error("Failed to open temp_random_queries.txt for writing");
            }
            for (const auto& query : randomQueries) {
                queriesFile << query << std::endl;
            }
        }

        KMismatchSearch kMismatchSearch("temp_random_text.txt", "temp_random_queries.txt", misMatches);

        auto mcsResult = kMismatchSearch.mcsSearch(misMatches);
        auto naiveResult = kMismatchSearch.naiveSearch(misMatches);

        assert(mcsResult.size() == naiveResult.size());
        for (const auto& [query, positions] : mcsResult) {
            assert(naiveResult.find(query) != naiveResult.end());
            assert(positions == naiveResult[query]);
        }

        // Clean up temporary files
        if (std::remove("temp_random_text.txt") != 0 || std::remove("temp_random_queries.txt") != 0) {
            std::cerr << "Warning: Failed to remove temporary files" << std::endl;
        }

        std::cout << "Random text and queries test passed successfully!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Exception in testRandomTextAndQueries: " << e.what() << std::endl;
        throw;
    }
    std::cout << "Finished testRandomTextAndQueries()" << std::endl;
}

void runAllTests() {
    std::cout << "Starting runAllTests()" << std::endl;
    try {
        // Start with the simplest test
        testSafeStoi();

        // Then move to basic functionality tests
        testKMismatchSearch();

        // Test with random inputs
        testRandomTextAndQueries();

        // Finally, run the most time-consuming test
        testLargeInputs();

        std::cout << "All tests passed successfully!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Exception in runAllTests: " << e.what() << std::endl;
    }
    std::cout << "Finished runAllTests()" << std::endl;
}
// This function should be in your test_main.cpp file
void runAllTests();
