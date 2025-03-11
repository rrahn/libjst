cmake_minimum_required (VERSION 3.20)

# Add packages for testing and benchmarking.
CPMGetPackage (catch2)
CPMGetPackage (googletest)
CPMGetPackage (googlebenchmark)

# Set directories for test output files, input data and binaries.
if (NOT DATA_ROOT_DIR)
    set(DATA_ROOT_DIR "${CMAKE_CURRENT_BINARY_DIR}")
endif()

set (DATA_DIR "${DATA_ROOT_DIR}/data/")
message(STATUS "Using DATA_ROOT_DIR: ${DATA_ROOT_DIR}")

file (MAKE_DIRECTORY ${DATA_DIR})
file (MAKE_DIRECTORY ${DATA_ROOT_DIR}/output)

add_definitions (-DOUTPUTDIR=\"${DATA_ROOT_DIR}/output/\")
add_definitions (-DDATADIR=\"${DATA_DIR}\")
add_definitions (-DBINDIR=\"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/\")

# Add more cmake tooling from this project.
find_path (LIBJST_TEST_CMAKE_MODULE_DIR NAMES app_datasources.cmake HINTS "${CMAKE_CURRENT_LIST_DIR}/cmake/")
list(APPEND CMAKE_MODULE_PATH "${LIBJST_TEST_CMAKE_MODULE_DIR}")

# Build tests just before their execution, because they have not been built with "all" target.
# The trick is here to provide a cmake file as a directory property that executes the build command.
file (WRITE "${CMAKE_CURRENT_BINARY_DIR}/build_test_targets.cmake"
            "execute_process(COMMAND ${CMAKE_COMMAND} --build . --target api_test)\n"
            "execute_process(COMMAND ${CMAKE_COMMAND} --build . --target benchmark_test)")
set_directory_properties (PROPERTIES TEST_INCLUDE_FILE "${CMAKE_CURRENT_BINARY_DIR}/build_test_targets.cmake")

# Define the test targets. All depending targets are built just before the test execution.
add_custom_target (api_test)
add_custom_target (benchmark_test)

# Test executables and libraries should not mix with the application files.
unset (CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
unset (CMAKE_LIBRARY_OUTPUT_DIRECTORY)
unset (CMAKE_RUNTIME_OUTPUT_DIRECTORY)

#--------------------------------------------------------------------
# Cmake interface targets
#--------------------------------------------------------------------

add_library (libjst_test INTERFACE)
target_compile_options (libjst_test INTERFACE "-pedantic"  "-Wall" "-Wextra" "-Werror")
target_compile_features (libjst_test INTERFACE cxx_std_20)
target_link_libraries (libjst_test INTERFACE "pthread" "cereal::cereal")
add_library (libjst::test ALIAS libjst_test)

add_library (libjst_test_unit INTERFACE)
target_include_directories (libjst_test_unit INTERFACE "GTest::gtest" "GTest::gtest_main" "libjst::test")
target_link_libraries (libjst_test_unit INTERFACE "GTest::gtest" "GTest::gtest_main" "libjst::test")
add_library (libjst::test::unit ALIAS libjst_test_unit)

# Add dedicated unit test interface for libjst using Catch2 as test framework.
add_library (libjst_test_catch2 INTERFACE)
target_link_libraries (libjst_test_catch2 INTERFACE "libjst::test" "libjst::libjst" "Catch2::Catch2WithMain")
add_library (libjst::test::catch2 ALIAS libjst_test_catch2)

add_library (libjst_test_performance INTERFACE)
target_include_directories (libjst_test_performance INTERFACE "benchmark::benchmark" "libjst::test")
target_link_libraries (libjst_test_performance INTERFACE "benchmark::benchmark" "libjst::test" )
add_library (libjst::test::performance ALIAS libjst_test_performance)

add_library (libjst_test_asan INTERFACE)
target_compile_options(libjst_test_asan INTERFACE "${JST_SANITIZER_FLAGS}")
target_link_options(libjst_test_asan INTERFACE "${JST_SANITIZER_FLAGS}")
target_link_libraries (libjst_test_asan INTERFACE "libjst::test::unit")
add_library (libjst::test::asan ALIAS libjst_test_asan)

add_library (libjst_test_tsan INTERFACE)
target_compile_options(libjst_test_tsan INTERFACE "${JST_SANITIZER_FLAGS}")
target_link_options(libjst_test_tsan INTERFACE "${JST_SANITIZER_FLAGS}")
target_link_libraries (libjst_test_tsan INTERFACE "libjst::test::unit")
add_library (libjst::test::tsan ALIAS libjst_test_tsan)

# ----------------------------------------------------------------------------
# Commonly used macros for the different test modules.
# ----------------------------------------------------------------------------

include (app_datasources)
include (${CMAKE_CURRENT_LIST_DIR}/data/datasources.cmake)

include (add_subdirectories)
include (get_test_component)
enable_testing ()

message (STATUS "${FontBold}You can run `make test` to build and run tests.${FontReset}")
