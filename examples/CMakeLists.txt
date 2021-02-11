# CMakeList.txt : CMake project for time_seriescpp, include source and define
# project specific logic here.
#
#cmake_minimum_required (VERSION 3.8)

#project(tspp_examples)

add_executable(tspp_examples "tspp_examples.cpp" "tspp_examples.hpp" "utilities.hpp" "parquet_utils.hpp" "parquet_utils.cpp" "parquet_tests.hpp" "parquet_tests.cpp" "bencher.hpp")

target_compile_features(tspp_examples PRIVATE cxx_std_17)

message(STATUS "Compiling using toolset: ${CMAKE_CXX_COMPILER_ID}")

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")    
    if(CMAKE_BUILD_TYPE MATCHES "Release")
        message(STATUS "Setting release flags")
        target_compile_options(tspp_examples PRIVATE -O3 -mavx2 -mpopcnt -Wall -Wextra -Wold-style-cast)
    else()
        message(STATUS "Setting debug flags")
        if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            target_compile_options(tspp_examples PRIVATE -Rpass=loop-vectorize -Rpass-missed=loop-vectorize -Rpass-analysis=loop-vectorize -Wall -Wextra -Wold-style-cast -DTSL_DEBUG -UNDEBUG)  
        else()
            target_compile_options(tspp_examples PRIVATE -Wall -Wextra -Wold-style-cast -DTSL_DEBUG -UNDEBUG)  
        endif()
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    if(CMAKE_BUILD_TYPE MATCHES "Release")
        target_compile_options(tspp_examples PRIVATE /O2 /bigobj /WX /W3 /DTSL_DEBUG /UNDEBUG)
    else()
        target_compile_options(tspp_examples PRIVATE /bigobj /WX /W3 /DTSL_DEBUG /UNDEBUG)
    endif()
endif()

# link tspp::time_series
# add_subdirectory(../ ${CMAKE_CURRENT_BINARY_DIR}/tspp)
target_link_libraries(tspp_examples PRIVATE tspp::time_series)  


find_package(Arrow REQUIRED PATHS ${ARROW_PARQUET_DIR})
find_package(Parquet REQUIRED PATHS ${ARROW_PARQUET_DIR})
target_link_libraries(tspp_examples PRIVATE arrow_shared parquet_shared pthread)  
