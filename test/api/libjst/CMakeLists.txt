cmake_minimum_required (VERSION 3.20)

macro (add_libjst2_test test_filename)
    add_api_test(${test_filename} "libjst::libjst")
endmacro()

macro (add_libjst_test test_filename)
    add_api_test(${test_filename} "libjst::libjst")
endmacro()

add_subdirectories ()
