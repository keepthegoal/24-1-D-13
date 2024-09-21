#pragma once
#include <string>
#include <vector>
#include <random>

/**
 * Generates a random text string of a given size using characters from an alphabet of the specified size.
 *
 * @param size The length of the text string to generate.
 * @param alphabetSize The number of unique characters in the alphabet (starting from 'A').
 * @param seed An optional seed for the random number generator (default is 0).
 * @return A randomly generated text string.
 */
std::string initRandomText(size_t size, size_t alphabetSize, size_t seed = 0);

/**
 * Generates a set of random query strings from a given text.
 * Each query is of a specified length, and random characters in the queries can be replaced with a '-' character.
 *
 * @param text The source text from which to generate the queries.
 * @param numberOfQueries The number of queries to generate.
 * @param queryLen The length of each query string.
 * @return A vector of randomly generated query strings.
 */
std::vector<std::string> initRandomQueries(std::string text, int numberOfQueries, int queryLen);
