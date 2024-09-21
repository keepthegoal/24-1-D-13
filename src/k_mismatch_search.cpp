#include "k_mismatch_search.h"

KMismatchSearch::KMismatchSearch()
{
    this->text = std::string();
    this->queries = std::vector<std::string>();
    this->mcs = MCS();
    this->cache = std::map<std::string, std::set<size_t>>();
}

KMismatchSearch::KMismatchSearch(std::string textFile, std::string queriesFile, int misMatches)
{
    this->text = loadTextFromFile(textFile);
    this->queries = loadQueriesFromFile(queriesFile);
    this->mcs = MCS::buildMCSNaiveMultithreaded(queries, misMatches);
    this->cache = std::map<std::string, std::set<size_t>>();
}

KMismatchSearch::KMismatchSearch(std::string textFile, std::string queriesFile, std::string mcsFile)
{
    this->text = loadTextFromFile(textFile);
    this->queries = loadQueriesFromFile(queriesFile);
    this->mcs = MCS::loadFromFile(mcsFile);
    this->cache = std::map<std::string, std::set<size_t>>();
}

KMismatchSearch::KMismatchSearch(std::string textFile, std::string queriesFile, std::string mcsFile, std::string cacheFile)
{
    this->text = loadTextFromFile(textFile);
    this->queries = loadQueriesFromFile(queriesFile);
    this->mcs = MCS::loadFromFile(mcsFile);
    this->cache = loadCacheFromFile(cacheFile);
}



void KMismatchSearch::setText(std::string& textToSet)
{
    this->text = textToSet;
}

const std::string& KMismatchSearch::getText() const
{
    return text;
}

void KMismatchSearch::setQueries(std::vector<std::string>& queriesToSet)
{
    this->queries = queriesToSet;
}

const std::vector<std::string>& KMismatchSearch::getQueries() const
{
    return queries;
}

void KMismatchSearch::setMcs(MCS& mcsToSet)
{
    this->mcs = mcsToSet;
}

 const MCS& KMismatchSearch::getMcs() const
{
    return mcs;
}

void KMismatchSearch::setCache(std::map<std::string, std::set<size_t>>& cacheToSet)
{
    this->cache = cacheToSet;
}

const std::map<std::string, std::set<size_t>>& KMismatchSearch::getCache() const
{
    return cache;
}

std::map<std::string, std::set<size_t>> KMismatchSearch::loadCacheFromFile(std::string& fileName) const
{
    std::map<std::string, std::set<size_t>> cache;
    std::ifstream file(fileName);
    if (!file) {
        throw std::runtime_error("Unable to open file: " + fileName);
    }
    std::string line;
    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string key;
        std::string value;
        std::getline(ss, key, ';');
        while (std::getline(ss, value, ';'))
        {
            cache[key].insert(std::stoi(value));
        }
    }
    return cache;
}

void KMismatchSearch::saveCacheToFile(std::string fileName)
{
    std::ofstream file(fileName);
    if (!file) {
        throw std::runtime_error("Unable to open file: " + fileName);
    }
    for (auto& [key, values] : this->cache)
    {
        file << key << ';';
        for (size_t pos : values)
            file << pos << ';';
        file << std::endl;
    }
    file.close();
}

std::string KMismatchSearch::loadTextFromFile(std::string& filename) const
{
    std::ifstream file(filename);
    if (!file) {
        throw std::runtime_error("Unable to open text file: " + filename);
    }
    return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

std::vector<std::string> KMismatchSearch::loadQueriesFromFile(std::string& filename) const
{
    std::ifstream file(filename);
    if (!file) {
        throw std::runtime_error("Unable to open queries file: " + filename);
    }
    std::vector<std::string> queries;
    std::string line;
    while (std::getline(file, line)) {
        queries.push_back(line);
    }
    return queries;
}

std::map<std::string, std::set<size_t>> KMismatchSearch::mcsSearch(size_t misMatches)
{
    std::mutex mtx;
    std::map<std::string, std::set<size_t>> resultMap;

    if (this->cache.empty())
    {
        std::vector<size_t> indices(text.size());
        std::iota(indices.begin(), indices.end(), 0);
        std::for_each(std::execution::par, indices.begin(), indices.end(),
            [&](size_t pos) 
            {
                std::map<std::string, std::vector<size_t>> localFormMap;
                for (auto& form : mcs.getMcsForms())
                    if (pos + form.getSize() <= text.size())
                    {
                        std::string cur_str = form.getStringFromPosition(text, pos);
                        localFormMap[cur_str].push_back(pos);
                    }
                std::lock_guard<std::mutex> lock(mtx);
                for (auto& [str, value] : localFormMap)
                {
                    this->cache[str].insert(value.begin(), value.end());
                }  
            });
    }

    std::for_each(std::execution::par, queries.begin(), queries.end(),
        [&](std::string& query)
        {
            size_t querySize = query.size();
            std::map<std::string, std::vector<size_t>> localResultMap;
            for (auto& form : mcs.getMcsForms())
            {
                size_t formSize = form.getSize();
                for (size_t qPos = 0; qPos + formSize <= querySize; qPos++)
                    for (size_t pos : this->cache[form.getStringFromPosition(query, qPos)])
                        if (CheckQueryOnPosition(query, pos - qPos, misMatches))
                            localResultMap[query].push_back(pos - qPos);
            }
            std::lock_guard<std::mutex> lock(mtx);
            for (auto& [str, value] : localResultMap)
                resultMap[str].insert(value.begin(), value.end());
        });

    return resultMap;
}


bool KMismatchSearch::CheckQueryOnPosition(const std::string& query, int64_t position, size_t misMatches) const
{
    size_t queryLen = query.size();

    if (misMatches > queryLen)
    {
        throw std::runtime_error("Mismatch number can not be greater than query length!");
        exit(1);
    }
    // Ensure position is valid and within bounds
    if (position < 0 || position + queryLen > text.size())
        return false;


    const char* textPtr = text.data() + position;
    const char* queryPtr = query.data();

    size_t i = 0;

     //Use AVX2 to compare 32 bytes (characters) at a time
     while (AVX2Support && i + 32 <= queryLen)
     {
         __m256i textChunk = _mm256_loadu_si256((__m256i*)(textPtr + i));
         __m256i queryChunk = _mm256_loadu_si256((__m256i*)(queryPtr + i));
         __m256i diff = _mm256_xor_si256(textChunk, queryChunk);

         if (!_mm256_testz_si256(diff, diff))
         {
             size_t mask = _mm256_movemask_epi8(diff);
             size_t numMismatches = popcount(mask);

             if (numMismatches > misMatches)
                 return false;

             misMatches -= numMismatches;
         }

         i += 32;
     }

    // Handle any remaining characters
    for (; i < queryLen; ++i)
        if (queryPtr[i] != textPtr[i])
            if (misMatches-- == 0)
                return false;

    return true;
}

std::map<std::string, std::set<size_t>> KMismatchSearch::naiveSearch(size_t misMatches)
{
    std::map<std::string, std::set<size_t>> resultMap;
    std::mutex mtx;
    std::vector<size_t> indices(text.size());
    std::iota(indices.begin(), indices.end(), 0);

    std::for_each(std::execution::par, indices.begin(), indices.end(),
        [&](size_t i) {
            for (const auto& query : this->queries)
                if (CheckQueryOnPosition(query, i, misMatches)) 
                {
                    // Lock before modifying shared data
                    std::lock_guard<std::mutex> lock(mtx);
                    resultMap[query].insert(i);
                }
        }
    );
    
    return resultMap;
}

