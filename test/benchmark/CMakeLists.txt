cmake_minimum_required (VERSION 3.20)

set (JSTMAP_BENCHMARK_MIN_TIME "1" CACHE STRING "Set --benchmark_min_time= for each benchmark.")
set (JSTMAP_BENCHMARK_TIME_UNIT "ns" CACHE STRING "Set --benchmark_time_unit= for each benchmark.")

# Add seqan3 as dependency for some cmake functions.
CPMGetPackage (seqan3)

macro (libjst_benchmark)
    set(options "")
    set(oneValueArgs SOURCE REPETITIONS)
    set(multiValueArgs DEPENDS)
    cmake_parse_arguments(MACRO_BENCHMARK "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (NOT MACRO_BENCHMARK_REPETITIONS)
        set (MACRO_BENCHMARK_REPETITIONS "3")
    endif ()

    file (RELATIVE_PATH benchmark "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_LIST_DIR}/${MACRO_BENCHMARK_SOURCE}")

    include (get_test_component)
    get_test_component (target "${benchmark}" TARGET_NAME)
    get_test_component (test_name "${benchmark}" TEST_NAME)

    add_executable (${target} ${MACRO_BENCHMARK_SOURCE})
    target_include_directories (${target} PUBLIC "${seqan3_SOURCE_DIR}/test/include")
    target_link_libraries (${target} libjst::test::performance seqan3::seqan3 ${MACRO_BENCHMARK_DEPENDS})
    add_test (NAME "${test_name}"
              COMMAND   ${target}
                        --benchmark_repetitions=${MACRO_BENCHMARK_REPETITIONS}
                        --benchmark_min_time=0
                        --benchmark_time_unit=${JSTMAP_BENCHMARK_TIME_UNIT}
                        --benchmark_out=$<TARGET_FILE_NAME:${target}>.json
                        --benchmark_out_format=json)

    add_dependencies (benchmark_test ${target})

    unset (benchmark)
    unset (target)
    unset (test_name)
endmacro ()

# Adds a macro benchmark:
function (libjst_macro_benchmark macro_benchmark_cpp)
    cmake_parse_arguments(
        "MACRO_BENCHMARK"
        ""
        "REPETITIONS"
        "META_DATA"
        ${ARGN}
    )
    libjst_benchmark(SOURCE ${macro_benchmark_cpp}
                     REPETITIONS ${MACRO_BENCHMARK_REPETITIONS})
endfunction ()

add_subdirectories ()
