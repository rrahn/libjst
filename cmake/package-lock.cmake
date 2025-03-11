# SPDX-FileCopyrightText: 2006-2025, Knut Reinert & Freie Universität Berlin
# SPDX-FileCopyrightText: 2016-2025, Knut Reinert & MPI für molekulare Genetik
# SPDX-License-Identifier: CC0-1.0

# CPM Package Lock
# This file should be committed to version control

# googletest
set (GOOGLETEST_VERSION 1.16.0)
CPMDeclarePackage (googletest
                   NAME GTest
                   VERSION ${GOOGLETEST_VERSION}
                   GITHUB_REPOSITORY google/googletest
                   SYSTEM TRUE
                   EXCLUDE_FROM_ALL TRUE
                   OPTIONS "BUILD_GMOCK OFF" "INSTALL_GTEST OFF" "CMAKE_CXX_STANDARD 20"
                           "CMAKE_MESSAGE_LOG_LEVEL WARNING"
)

#googlebenchmark
set (GOOGLEBENCHMARK_VERSION 1.9.1)
CPMDeclarePackage (googlebenchmark
                   NAME benchmark
                   VERSION ${GOOGLEBENCHMARK_VERSION}
                   GITHUB_REPOSITORY google/benchmark
                   SYSTEM TRUE
                   EXCLUDE_FROM_ALL TRUE
                   OPTIONS "BENCHMARK_ENABLE_TESTING OFF" "BENCHMARK_ENABLE_WERROR OFF" "BENCHMARK_ENABLE_INSTALL OFF"
                           "CMAKE_MESSAGE_LOG_LEVEL WARNING" "CMAKE_CXX_STANDARD 20"
)

# catch2
set (CATCH2_VERSION 3.8.0)
CPMDeclarePackage (catch2
                   NAME Catch2
                   VERSION ${CATCH2_VERSION}
                   GITHUB_REPOSITORY catchorg/Catch2
                   SYSTEM TRUE
                   EXCLUDE_FROM_ALL TRUE
                   OPTIONS "CMAKE_MESSAGE_LOG_LEVEL WARNING"
)

# use_ccache
set (USE_CCACHE_VERSION d2a54ef555b6fc2d496a4c9506dbeb7cf899ce37)
CPMDeclarePackage (use_ccache
                   NAME use_ccache
                   GIT_TAG ${USE_CCACHE_VERSION} # main
                   GITHUB_REPOSITORY seqan/cmake-scripts
                   SOURCE_SUBDIR ccache
                   SYSTEM TRUE
                   EXCLUDE_FROM_ALL TRUE
)

# cereal
set (LIBJST_CEREAL_VERSION 1.3.2 CACHE STRING "" FORCE)
CPMDeclarePackage (cereal
                   NAME cereal
                   VERSION ${LIBJST_CEREAL_VERSION}
                   GITHUB_REPOSITORY USCiLab/cereal
                   SYSTEM TRUE
                   OPTIONS "JUST_INSTALL_CEREAL ON" "CMAKE_MESSAGE_LOG_LEVEL WARNING")

# seqan3
# seqan3
set (LIBJST_SEQAN3_VERSION 2863cbbe336a51c21932c69635e814b6e3a8a4ce CACHE STRING "" FORCE)
CPMDeclarePackage (seqan3
                   NAME seqan3
                   GIT_TAG ${LIBJST_SEQAN3_VERSION} # main
                   GITHUB_REPOSITORY seqan/seqan3
                   SYSTEM TRUE
                   EXCLUDE_FROM_ALL TRUE
                   OPTIONS "INSTALL_SEQAN3 OFF" "CMAKE_MESSAGE_LOG_LEVEL WARNING"
)
