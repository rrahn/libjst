cmake_minimum_required (VERSION 3.20)

set (LIBJST_TARGET_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/../libjst)
set (LIBJST_TARGET_COMPILE_FEATURES cxx_std_20)

### Add an interface library for libjst.
add_library (libjst_libjst INTERFACE)
target_include_directories (libjst_libjst INTERFACE ${LIBJST_TARGET_INCLUDE_DIRECTORIES})
target_compile_features (libjst_libjst INTERFACE ${LIBJST_TARGET_COMPILE_FEATURES})
target_link_libraries (libjst_libjst INTERFACE ${LIBJST_TARGET_LINK_LIBRARIES})
add_library (libjst::libjst ALIAS libjst_libjst)
