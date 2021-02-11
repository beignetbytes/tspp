# CMakeList.txt : CMake project for time_seriescpp, include source and define
# project specific logic here.
#



set(BINARY ${CMAKE_PROJECT_NAME}_tests)
enable_testing()


list(APPEND TEST_SOURCES
"${CMAKE_CURRENT_SOURCE_DIR}/test_utils.hpp"
"${CMAKE_CURRENT_SOURCE_DIR}/print_utils.hpp"
"${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/point_tests.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/basic_ts_funcs.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/indexed_ts_funcs.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/ts_joins.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/chrono_extens.cpp"
) 


set(SOURCES ${TEST_SOURCES})

add_executable(${BINARY} ${TEST_SOURCES})


target_compile_features(${BINARY} PRIVATE cxx_std_17)

message(STATUS "Compiling using toolset: ${CMAKE_CXX_COMPILER_ID}")

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")    
    if(CMAKE_BUILD_TYPE MATCHES "Release")
        message(STATUS "Setting release flags")
        target_compile_options(${BINARY} PRIVATE -O3 -mavx2 -mpopcnt -Wall -Wextra -Wold-style-cast)
    else()
        message(STATUS "Setting debug flags")
        if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            target_compile_options(${BINARY} PRIVATE -Rpass=loop-vectorize -Rpass-missed=loop-vectorize -Rpass-analysis=loop-vectorize -Wall -Wextra -Wold-style-cast -DTSL_DEBUG -UNDEBUG)  
        else()
            target_compile_options(${BINARY} PRIVATE -Wall -Wextra -Wold-style-cast -DTSL_DEBUG -UNDEBUG)  
        endif()
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    if(CMAKE_BUILD_TYPE MATCHES "Release")
        target_compile_options(${BINARY} PRIVATE /O2 /bigobj /WX /W3 /DTSL_DEBUG /UNDEBUG)
    else()
        target_compile_options(${BINARY} PRIVATE /bigobj /WX /W3 /DTSL_DEBUG /UNDEBUG)
    endif()
endif()





find_package(GTest REQUIRED PATHS ${GTEST_DIR}) 

target_link_libraries(${BINARY} PRIVATE tspp::time_series)

target_link_libraries(${BINARY} PUBLIC GTest::gtest pthread)

add_test(NAME ${BINARY} COMMAND ${BINARY})