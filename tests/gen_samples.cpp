#include "gen_samples.h"

std::string initRandomText(size_t size, size_t alphabetSize, size_t seed)
{
    std::string text;
    text.clear();
    // Reserve the necessary space
    text.reserve(size);

    // Create a mersenne twister seeded with seed
    std::mt19937 gen(seed);

    // Create a distribution in range from 'A' to 'Z'
    std::uniform_int_distribution<size_t> dist('A', 'A' + alphabetSize - 1);

    // Generate the random text
    for (size_t i = 0; i < size; ++i)
        text.push_back(static_cast<char>(dist(gen)));

    return text;
}

std::vector<std::string> initRandomQueries(std::string text, int numberOfQueries, int queryLen)
{
    std::vector<std::string> queries(numberOfQueries);
    size_t k = 0;
    for (int i = 0; i < numberOfQueries; i++, k++)
        for (int j = 0; j < queryLen; j++)
        {
            if ((queryLen * k + j) >= text.length())
                k = 0;
            if (rand() % 10)
                queries[i].push_back(text[queryLen * k + j]);
            else
                queries[i].push_back('-');
        }
    //queries.push_back(text.substr(((i + 1) * queryLen <= text.length()) ? i * queryLen : queryLen, queryLen));
    return queries;
}