#include <iostream>
#include <vector>
#include "k_mismatch_search.h"
#include <numeric>
#include <fstream>
#include <stdexcept>
#include <limits>
#include <optional>

/**
 * Safely converts a string to an integer and checks if the input is valid.
 *
 * @param str The string to convert to an integer.
 * @param paramName The name of the parameter for error messages.
 * @return The converted integer value.
 * @throws std::invalid_argument if the input string is not a valid integer.
 * @throws std::out_of_range if the integer is out of range.
 */
int safeStoi(const char* str, const std::string& paramName) {
    char* end;
    long value = std::strtol(str, &end, 10);

    // Check if the string contains a valid integer
    if (end == str || *end != '\0')
        throw std::invalid_argument("Invalid " + paramName + ": '" + str + "' is not a valid integer.");

    // Check if the integer is within a valid range
    if (value < 0 || value > std::numeric_limits<int>::max())
        throw std::out_of_range(paramName + " out of range: " + str);

    return static_cast<int>(value);
}

/**
 * Prints an error message displaying the correct usage of the program.
 *
 * @param programName The name of the program (argv[0]).
 */
void errMsg(std::string programName)
{
    std::cerr << "Usage: " << programName << " -t <text_file> -q <queries_file> -m <misMatches> "
        << "[-mc <mcs_file>] [-i <index_file>] [-sm <mcs_file_to_save>] "
        << "[-si <index_file_to_save>] [-sr <results_file_to_save>] [-h]";
}

/**
 * Prints a help message with the available options for the program.
 *
 * @param programName The name of the program (argv[0]).
 */
void helpMsg(std::string programName)
{
    std::cout << "Usage: " << programName << " [options]\n\n"
        << "Options:\n"
        << "  -t,  --text <text_file>            Path to the text file (required).\n"
        << "  -q,  --queries <queries_file>      Path to the queries file (required).\n"
        << "  -m,  --mismatches <number>         Maximum number of mismatches allowed (required).\n"
        << "  -mc, --mcs <mcs_file>              Path to the MCS file (optional).\n"
        << "  -i,  --index <index_file>          Path to the index file (optional).\n"
        << "  -sm, --save_mcs <mcs_file>         Path to save the MCS file (optional).\n"
        << "  -si, --save_index <index_file>     Path to save the index file (optional).\n"
        << "  -sr, --save_result <results_file>  Path to save the result file (optional).\n"
        << "  -h,  --help                        Display this help message.\n\n"
        << "Example usage:\n"
        << "  " << programName << " -t text.txt -q queries.txt -m 2 -mc mcsfile.txt -i indexfile.txt\n\n";
}

/**
 * The main function that handles command-line arguments, sets up the k-mismatch search,
 * and outputs the search results.
 *
 * @param argc The number of command-line arguments.
 * @param argv The array of command-line arguments.
 * @return Returns 0 on success, 1 on error.
 */
int main(int argc, char* argv[])
{
    KMismatchSearch kMismatchSearch;  // k-mismatch search object
    int misMatches = -1;              // Number of mismatches allowed
    std::string textFile;             // Path to the text file
    std::string queriesFile;          // Path to the queries file
    std::string mcsFile;              // Path to the MCS file (optional)
    std::string indexFile;            // Path to the index file (optional)
    std::string mcsFileToSave;        // Path to save the MCS file (optional)
    std::string indexFileToSave;      // Path to save the index file (optional)
    std::string resultsFileToSave;    // Path to save the result file (optional)

    // Parse command-line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        // Check for each option and retrieve its argument if necessary
        if ((arg == "-t" || arg == "--text") && i + 1 < argc)
            textFile = argv[++i];
        else if ((arg == "-q" || arg == "--queries") && i + 1 < argc)
            queriesFile = argv[++i];
        else if ((arg == "-m" || arg == "--mismatches") && i + 1 < argc)
        {
            misMatches = safeStoi(argv[++i], "misMatches");
            if (misMatches < 0)
            {
                std::cerr << "misMatches must be non-negative.\n";
                return 1;
            }
        }
        else if ((arg == "-mc" || arg == "--mcs") && i + 1 < argc)
            mcsFile = argv[++i];
        else if ((arg == "-i" || arg == "--index") && i + 1 < argc)
            indexFile = argv[++i];
        else if ((arg == "-sm" || arg == "--save_mcs") && i + 1 < argc)
            mcsFileToSave = argv[++i];
        else if ((arg == "-si" || arg == "--save_index") && i + 1 < argc)
            indexFileToSave = argv[++i];
        else if ((arg == "-sr" || arg == "--save_result") && i + 1 < argc)
            resultsFileToSave = argv[++i];
        else if (arg == "-h" || arg == "--help")
        {
            helpMsg(argv[0]);
            return 0;
        }
        else
        {
            std::cerr << "Unknown option or missing argument for: " << arg << "\n";
            errMsg(argv[0]);
            return 1;
        }
    }

    // Validate required arguments
    if (textFile.empty() || queriesFile.empty() || misMatches == -1)
    {
        std::cerr << "Error: text_file, queries_file and mismatches number are required.\n";
        errMsg(argv[0]);
        return 1;
    }

    // Initialize the KMismatchSearch object with the provided files and options
    try
    {
        if (mcsFile.empty())
            kMismatchSearch = KMismatchSearch(textFile, queriesFile, misMatches);
        else if (indexFile.empty())
            kMismatchSearch = KMismatchSearch(textFile, queriesFile, mcsFile);
        else
            kMismatchSearch = KMismatchSearch(textFile, queriesFile, mcsFile, indexFile);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    // Perform the k-mismatch search
    auto result = kMismatchSearch.mcsSearch(misMatches);

    // Save the MCS file if requested
    if (!mcsFileToSave.empty())
        kMismatchSearch.getMcs().saveToFile(mcsFileToSave);

    // Save the index file if requested
    if (!indexFileToSave.empty())
        kMismatchSearch.saveCacheToFile(indexFileToSave);

    // Save the results to a file if requested, otherwise print to stdout
    if (!resultsFileToSave.empty())
    {
        std::ofstream outFile(resultsFileToSave);
        for (auto& [query, positions] : result)
        {
            outFile << query << " ";
            for (auto position : positions)
                outFile << position << " ";
            outFile << std::endl;
        }
        outFile.close();
    }
    else
    {
        for (auto& [query, positions] : result)
        {
            std::cout << query << " ";
            for (auto position : positions)
                std::cout << position << " ";
            std::cout << std::endl;
        }
    }

    return 0;
}
