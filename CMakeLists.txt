# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.15)

set(CMAKE_CXX_STANDARD 17)


project(tspp-time-series VERSION 0.1.0 LANGUAGES CXX)
include(GNUInstallDirs)


# set up extern folder if it doesnt exist
if(NOT IS_DIRECTORY "${CMAKE_SOURCE_DIR}/extern/depsrc/" )    
    message(STATUS "setting up extern directory")    
    file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/extern/depsrc)
endif()

#pull and build arrow if needed
if(NOT IS_DIRECTORY "${CMAKE_SOURCE_DIR}/extern/deps/arrow/")
    message(STATUS "starting arrow generate and build")
    execute_process(COMMAND git clone https://github.com/apache/arrow.git
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/extern/depsrc
    )
    file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/extern/depsrc/arrow/cpp/release)

    execute_process(COMMAND cmake -DCMAKE_BUILD_TYPE=Release -DARROW_PARQUET=ON -DARROW_WITH_SNAPPY=ON -DARROW_WITH_BROTLI=ON -DARROW_WITH_ZLIB=ON ..
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/extern/depsrc/arrow/cpp/release
    )

    execute_process(COMMAND make
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/extern/depsrc/arrow/cpp/release
    )

    execute_process(COMMAND cmake --install . --prefix ${CMAKE_SOURCE_DIR}/extern/deps/arrow
       WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/extern/depsrc/arrow/cpp/release
    )
  
endif()

set(ARROW_PARQUET_DIR "${CMAKE_SOURCE_DIR}/extern/deps/arrow/${CMAKE_INSTALL_LIBDIR}/cmake/arrow/")

#pull and build googletest if needed
if(NOT IS_DIRECTORY "${CMAKE_SOURCE_DIR}/extern/deps/googletest/")
    message(STATUS "starting googletest generate and build")
    execute_process(COMMAND git clone https://github.com/google/googletest.git
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/extern/depsrc
    )
    file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/extern/depsrc/googletest/release)

    execute_process(COMMAND cmake ..
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/extern/depsrc/googletest/release
    )

    execute_process(COMMAND make
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/extern/depsrc/googletest/release
    )

    execute_process(COMMAND cmake --install . --prefix ${CMAKE_SOURCE_DIR}/extern/deps/googletest
       WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/extern/depsrc/googletest/release
    )



endif()

set(GTEST_DIR "${CMAKE_SOURCE_DIR}/extern/deps/googletest/${CMAKE_INSTALL_LIBDIR}/cmake/GTest/")

add_library(time_series INTERFACE)

add_library(tspp::time_series ALIAS time_series)

target_include_directories(time_series INTERFACE
                           "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                           "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

list(APPEND tspp_headers 
"${CMAKE_CURRENT_SOURCE_DIR}/include/tspp/time_series.hpp"
"${CMAKE_CURRENT_SOURCE_DIR}/include/tspp/common/iterator_extensions.hpp"
"${CMAKE_CURRENT_SOURCE_DIR}/include/tspp/common/time_series_errors.hpp"
"${CMAKE_CURRENT_SOURCE_DIR}/include/tspp/common/chrono_extensions.hpp"
"${CMAKE_CURRENT_SOURCE_DIR}/include/tspp/common/tspp_enums.hpp"
"${CMAKE_CURRENT_SOURCE_DIR}/include/tspp/common/compare_extensions.hpp"
)

target_sources(time_series INTERFACE "$<BUILD_INTERFACE:${tspp_headers}>")


# Installation (only compatible with CMake version >= 3.3)
if(${CMAKE_VERSION} VERSION_GREATER "3.2")
    include(CMakePackageConfigHelpers)

    ## Install include directory and potential natvis file
    install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/tspp"
            DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

    ## Create and install tspp-time-seriesConfig.cmake
    configure_package_config_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/tspp-time-seriesConfig.cmake.in"
                                  "${CMAKE_CURRENT_BINARY_DIR}/tspp-time-seriesConfig.cmake"
                                  INSTALL_DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/cmake/tspp-time-series")

    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/tspp-time-seriesConfig.cmake"
            DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/cmake/tspp-time-series")



    ## Create and install tspp-time-seriesTargets.cmake
    install(TARGETS time_series
            EXPORT tspp-time_seriesTargets)

    install(EXPORT tspp-time_seriesTargets
            NAMESPACE tspp::
            DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/cmake/tspp-time-series")



    ## Create and install tspp-time-seriesConfigVersion.cmake
    # tspp-time-series is header-only and does not depend on the architecture.
    # Remove CMAKE_SIZEOF_VOID_P from tspp-time-seriesConfigVersion.cmake so that a
    # tspp-time-seriesConfig.cmake generated for a 64 bit target can be used for 32 bit
    # targets and vice versa.
    set(CMAKE_SIZEOF_VOID_P_BACKUP ${CMAKE_SIZEOF_VOID_P})
    unset(CMAKE_SIZEOF_VOID_P)
    write_basic_package_version_file("${CMAKE_CURRENT_BINARY_DIR}/tspp-time-seriesConfigVersion.cmake"
                                     COMPATIBILITY SameMajorVersion)
    set(CMAKE_SIZEOF_VOID_P ${CMAKE_SIZEOF_VOID_P_BACKUP})

    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/tspp-time-seriesConfigVersion.cmake"
            DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/cmake/tspp-time-series")
endif()


add_subdirectory(examples)
add_subdirectory(tests)


