// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2021, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2021, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE.md
// -----------------------------------------------------------------------------------------------------

/*!\file
 * \brief Provides .
 * \author Rene Rahn <rene.rahn AT fu-berlin.de>
 */

#pragma once

#include <filesystem>

namespace jstmap
{

struct index_options
{
    std::filesystem::path sequence_file{}; //!< The file path contianing the sequences to index.
    std::filesystem::path vcf_file{}; //!< The file path contianing the vcf file to build the jst for.
    std::filesystem::path output_file{}; //!< The file path to write the constructed index to.
    bool is_quite{false}; //!< Wether the index app should run in quite mode.
    bool is_verbose{false}; //!< Wether the index app should run in verbose mode.
};

}  // namespace jstmap
