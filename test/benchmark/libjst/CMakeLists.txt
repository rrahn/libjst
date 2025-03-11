cmake_minimum_required (VERSION 3.20)

libjst_benchmark (SOURCE bit_vector_benchmark.cpp DEPENDS libjst::libjst)
libjst_benchmark (SOURCE journaled_sequence_benchmark.cpp DEPENDS libjst::libjst)
libjst_benchmark (SOURCE sorted_container_benchmark.cpp DEPENDS libjst::libjst)
