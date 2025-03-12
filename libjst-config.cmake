# SPDX-FileCopyrightText: 2006-2025 Knut Reinert & Freie Universität Berlin
# SPDX-FileCopyrightText: 2016-2025 Knut Reinert & MPI für molekulare Genetik
# SPDX-License-Identifier: CC0-1.0

cmake_minimum_required (VERSION 3.20)

if (TARGET libjst::libjst)
    return()
endif()

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/libjst ${CMAKE_CURRENT_BINARY_DIR}/libjst)
