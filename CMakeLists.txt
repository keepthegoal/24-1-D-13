cmake_minimum_required(VERSION 3.21)
set(CMAKE_BUILD_TYPE Release)
project(k_mismatch_lib VERSION 1.0 DESCRIPTION "My C++ Library" LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(CheckIPOSupported)
check_ipo_supported(RESULT lto_supported OUTPUT error)
if(lto_supported)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()


if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-mavx2)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options(/arch:AVX)
endif()



include_directories(${PROJECT_SOURCE_DIR}/include)
link_directories(${PROJECT_SOURCE_DIR}/lib)

file(GLOB_RECURSE LIB_SOURCES "src/*.cpp")
list(REMOVE_ITEM LIB_SOURCES "${PROJECT_SOURCE_DIR}/src/main.cpp")

add_library(${PROJECT_NAME}_shared SHARED ${LIB_SOURCES})
add_library(${PROJECT_NAME}_static STATIC ${LIB_SOURCES})

set_target_properties(${PROJECT_NAME}_shared PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
set_target_properties(${PROJECT_NAME}_static PROPERTIES OUTPUT_NAME ${PROJECT_NAME})

install(TARGETS ${PROJECT_NAME}_shared ${PROJECT_NAME}_static
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ DESTINATION include)

# Main application
add_executable(k_mismatch_app src/main.cpp)
target_link_libraries(k_mismatch_app PRIVATE ${PROJECT_NAME}_static)

enable_testing()

# Test executable
add_executable(run_tests
    tests/units.cpp
    tests/utils.cpp
    tests/gen_samples.cpp
    tests/test_main.cpp
)

target_link_libraries(run_tests PRIVATE ${PROJECT_NAME}_static)

# Add the test
add_test(NAME UnitTests COMMAND run_tests)

# add_subdirectory(examples)  # Uncomment if you have an examples subdirectory
# add_subdirectory(tests)     # Uncomment if you have a tests subdirectory
