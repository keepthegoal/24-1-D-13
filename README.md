
# k-Mismatch Search Algorithm

## Overview
This project implements a **k-mismatch search algorithm** for searching text files with an allowance for mismatches. The system provides two search methods: one based on **Multiple Common Subsequence (MCS)** and a **naive approach**. The k-mismatch algorithm allows efficient searching by considering forms (binary sequences of 1s and 0s) to represent matching patterns.

## Features
- **k-Mismatch Search**: Allows searching for query strings in a text with a specified number of mismatches.
- **MCS-Based Search**: Utilizes precomputed forms for efficient search.
- **Naive Search**: A more straightforward but slower approach for smaller datasets.
- **Multithreaded Execution**: Uses parallel execution for faster processing.
- **Caching**: Previous search results are cached to improve performance on repeated searches.

## Key Files
- `main.cpp`: The main entry point of the program. It handles command-line arguments and executes the k-mismatch search based on user input.

## How to Run
The program accepts the following command-line options:

```
Usage: ./k_mismatch_search -t <text_file> -q <queries_file> -m <mismatches> 
                           [-mc <mcs_file>] [-i <index_file>] 
                           [-sm <mcs_file_to_save>] [-si <index_file_to_save>] 
                           [-sr <results_file_to_save>] [-h]
```

### Example Usage
```
./k_mismatch_search -t text.txt -q queries.txt -m 2 -mc mcsfile.txt -i indexfile.txt
```

## Command-Line Arguments
- `-t, --text <text_file>`: Path to the text file (required).
- `-q, --queries <queries_file>`: Path to the queries file (required).
- `-m, --mismatches <number>`: Maximum number of mismatches allowed (required).
- `-mc, --mcs <mcs_file>`: Path to the MCS file (optional).
- `-i, --index <index_file>`: Path to the index file (optional).
- `-sm, --save_mcs <mcs_file>`: Path to save the MCS file (optional).
- `-si, --save_index <index_file>`: Path to save the index file (optional).
- `-sr, --save_result <results_file>`: Path to save the result file (optional).
- `-h, --help`: Display this help message.

## Dependencies
This project requires a C++ compiler with support for AVX2 (Advanced Vector Extensions 2) for SIMD instructions. If the CPU does not support AVX2, the system gracefully falls back to a non-AVX2 implementation.

## Compilation
You can compile the project using a C++ compiler such as `g++` or `clang++`. For example:
```
g++ -std=c++17 main.cpp k_mismatch_search.cpp mcs.cpp -o k_mismatch_search
```

## License
This project is open-source and licensed under the MIT License.
