cmake_minimum_required (VERSION 3.20)

## CUSTOMISE

# Define the application name and version.
project (libjst VERSION 0.0.1 LANGUAGES CXX)
set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

## BUILD

# Make Release default build type
if (NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE Release CACHE STRING
         "Choose the type of build, options are: Debug Release RelWithDebInfo"
         FORCE)
endif ()

# Specify the directories where to store the built archives, libraries and executables
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# Messages
string (ASCII 27 Esc)
set (FontBold "")
set (FontReset "")

# This allows including `*.cmake` files from the `cmake` directory without specifying the full path.
list (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

# Add packages.
# We use CPM for package management: https://github.com/cpm-cmake/CPM.cmake
# The added packages (e.g., hibf, sharg, seqan3) are defined in the `cmake/package-lock.cmake` file.
message (STATUS "CMAKE_CURRENT_LIST_DIR: ${CMAKE_CURRENT_LIST_DIR}")
include (CPM)
CPMUsePackageLock (${CMAKE_CURRENT_LIST_DIR}/cmake/package-lock.cmake)

# Use ccache if available. This speeds up the build process by caching files that have been compiled before.
CPMGetPackage (use_ccache)
CPMGetPackage (cereal)

# Dependency: OpenMP
find_package(OpenMP REQUIRED)

# Dependency: PThreads
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

## APPLICATION

include (libjst-config.cmake)

option (LIBJST_DEVELOPER_MODE "Enable developer mode" ON)

if (LIBJST_DEVELOPER_MODE)
    message(STATUS "==== Developer mode enabled ====")
    ## DOCUMENTATION
    add_subdirectory (doc EXCLUDE_FROM_ALL)

    ## TEST
    include (test/libjst_test.cmake)

    add_subdirectory (test/api EXCLUDE_FROM_ALL)
    add_subdirectory (test/benchmark EXCLUDE_FROM_ALL)
endif ()
