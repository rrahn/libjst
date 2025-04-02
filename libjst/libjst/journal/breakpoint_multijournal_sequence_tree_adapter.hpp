// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2021, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2021, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE.md
// -----------------------------------------------------------------------------------------------------

/**
 * @file
 * @brief Provides journaled sequence tree adapter for multisequence journals.
 * @author Rene Rahn <rene.rahn AT fu-berlin.de>
 */

#pragma once

#include <libjst/sequence_tree/breakpoint_sequence_tree_node.hpp>
#include <libjst/sequence_tree/breakpoint_sequence_tree_sentinel.hpp>

namespace libjst
{

    /**
     * @cond
     * This class is experimental and is removed from the public API.
     */
    template <typename multisequence_journal_t> class breakpoint_multijournal_sequence_tree_adapter
    {
        /// @name Member variables
        /// @{
      private:
        using node_type = breakpoint_sequence_tree_node<multisequence_journal_t>;
        /// @}

        /// @name Member variables
        /// @{
      private:
        multisequence_journal_t const &_journal;

        /// @}

        /// @name Member functions
        /// @{
      public:
        constexpr breakpoint_multijournal_sequence_tree_adapter() = delete;
        constexpr explicit breakpoint_multijournal_sequence_tree_adapter(multisequence_journal_t const &journal)
            : _journal{journal}
        {
        }
        /// @}

        /// @name Tree interface
        /// @{
      public:
        constexpr node_type root() const noexcept
        {
            return node_type{_journal};
        }

        constexpr breakpoint_sequence_tree_sentinel sink() const noexcept
        {
            return breakpoint_sequence_tree_sentinel{};
        }
        /// @}
    };
    /// @endcond
} // namespace libjst
