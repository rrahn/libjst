// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2021, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2021, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE.md
// -----------------------------------------------------------------------------------------------------

/*!\file
 * \brief Provides a journal implementation using contiguous memory to store the elements.
 * \author Rene Rahn <rene.rahn AT fu-berlin.de>
 */

#pragma once

#include <algorithm>
#include <iterator>
#include <vector>
#include <ranges>
#include <type_traits>

#include <libjst/sequence/journal_position.hpp>
#include <libjst/reference_sequence/reference_sequence_concept.hpp>

namespace libjst
{

    /**
     * @class inline_sequence_journal
     * @brief Represents a dictionary over non-overlapping segments that, when concatenated, form a new sequence.
     * @tparam source_t The type of the source sequence. Must model libjst::preserving_reference_sequence.
     *
     * The `inline_sequence_journal` class is designed to manage and manipulate sequences by dividing them into
     * non-overlapping segments whose type is determined by the type trait libjst::breakpoint_slice_t.
     * These slices are typically views over external sources, allowing for efficient sequence manipulation without
     * duplicating data.
     *
     * The underlying journal is implemented as a sorted vector of these segments, ensuring that the segments
     * remain in order and do not overlap. This structure allows for efficient operations such as insertion,
     * deletion, and modification of segments while maintaining the integrity of the overall sequence.
     *
     * Updates to the journal are performed \a inline by modifying the segments directly, rather than tracking
     * the changes. Thus, it only represents the current state of the modified sequence.
     *
     * Typical Use Case:
     * This class is useful in scenarios where sequences need to be dynamically modified or constructed from
     * multiple sources without duplicating the underlying data. Examples include text editors, genomic sequence
     * analysis, or any application requiring efficient sequence manipulation.
     * The libjst::journaled_sequence is a wrapper around the libjst::inline_sequence_journal that provides a natural
     * sequence interface.
     */
    template <libjst::preserving_reference_sequence source_t>
    class inline_sequence_journal
    {
    private:
        class record_impl;

        class breakend_impl;

        ///@name Member types
        ///@{
    private:

        /// @brief The type of the underlying journal to hold the records.
        using journal_type = std::vector<record_impl>;

    public:
        using source_type = source_t;  ///< The type of the source sequence.
        using sequence_type = typename record_impl::sequence_type; ///< The type of the sequence obtained from the journal.
        using breakend_type = breakend_impl; ///< The type of a breakend inside the journal.
        using breakpoint_type = std::pair<breakend_type, breakend_type>; ///< The type of a breakpoint inside the journal.
        using size_type = std::ranges::range_size_t<source_type>; ///< The type of the size of the journal.
        using key_type = size_type; ///< The type of the key used to access the journal.
        using iterator = std::ranges::iterator_t<journal_type>; ///< The type of the iterator to the journal.
        using const_iterator = std::ranges::iterator_t<journal_type const>; ///< The type of the iterator to an inmutable journal.
        ///@}

        /// @name Member Variables
        /// @{
    private:
        source_type _source;  ///< The source sequence to which the journal is applied.
        journal_type _journal{}; ///< The journal that holds the records.
        /// @}

        /// @name Member functions
        /// @{
    public:

        /**
         * @brief Initializes the journal with an empty source sequence.
         *
         * After construction the journal contains one entry that represents an empty sequence.
         *
         * @note The default constructor is only available if the source type is default constructible.
         */
        constexpr inline_sequence_journal()
            noexcept(noexcept(initialize_journal()))
            requires std::default_initializable<source_type>
            : _journal{}
        {
            initialize_journal();
        }

        /**
         * @brief Initializes the journal with an empty source sequence.
         *
         * After construction the journal contains one entry that covers the entire source sequence.
         */
        constexpr explicit inline_sequence_journal(source_type source)
            noexcept(noexcept(initialize_journal()))
            : _source{std::move(source)},
              _journal{}
        {
            initialize_journal();
        }

        /// @brief Returns the source sequence of the journal.
        constexpr source_type const & source() const & noexcept
        {
            return _source;
        }

        /// @brief Returns the source sequence of the journal.
        constexpr source_type source() && noexcept
        {
            return std::move(_source);
        }
    protected:

        /// @brief Returns the underlying journal structure.
        constexpr journal_type & journal() noexcept
        {
            return _journal;
        }

        /// @brief Returns the underlying journal structure.
        constexpr journal_type const & journal() const noexcept
        {
            return _journal;
        }
        /// @}

        /// @name Iterators
        /// @{
    public:
        /// @brief Returns an iterator to the beginning of the journal.
        iterator begin() noexcept
        {
            return _journal.begin();
        }

        /// @brief Returns a const iterator to the beginning of the journal.
        const_iterator begin() const noexcept
        {
            return _journal.begin();
        }

        /// @brief Returns an iterator to the end of the journal.
        iterator end() noexcept
        {
            return _journal.end() - 1; // see initialize_journal() for why subtracting one.
        }

        /// @brief Returns a const iterator to the end of the journal.
        const_iterator end() const noexcept
        {
            return _journal.end() - 1; // see initialize_journal() for why subtracting one.
        }
        /// @}

        /// @name Capacity
        /// @{
    public:
        /// @brief Returns the number of records in the journal.
        size_type size() const noexcept
        {
            return _journal.size() - 1; // see initialize_journal() for why subtracting one.
        }

        /// @brief Returns the maximal number of records the journal can hold.
        size_type max_size() const noexcept
        {
            return _journal.max_size() - 1; // see initialize_journal() for why subtracting one.
        }

        /// @brief Returns whether the journal is empty.
        constexpr bool empty() const noexcept
        {
            return size() == 0;
        }
        /// @}

        /// @name Modifiers
        /// @{
    public:
        /// @brief Clears the journal.
        void clear() noexcept(noexcept(initialize_journal()))
        {
            _journal.clear();
            initialize_journal();
        }

        /**
         * @brief Records a new sequence in the journal inline at the given breakpoint.
         *
         * @param breakpoint The breakpoint in the journal at which to record the new sequence.
         * @param sequence The sequence to record in the journal.
         * @return iterator
         *
         * This function will overwrite the existing sequence at the given breakpoint with the new sequence.
         */
        constexpr iterator record(breakpoint_type breakpoint, sequence_type sequence)
        {
            return record_inline(std::move(breakpoint), std::move(sequence));
        }
        /// @}

        /// @name Lookup
        /// @{
    public:
        /// @brief Returns an iterator to the first element in the journal that is not less than the given key.
        /// @param key The key to search for.
        iterator lower_bound(key_type const key) noexcept
        {
            return std::ranges::lower_bound(begin(), end(), key, std::ranges::less{}, &record_impl::position);
        }

        /// @brief Returns an iterator to the first element in the journal that is not less than the given key.
        /// @param key The key to search for.
        const_iterator lower_bound(key_type const key) const noexcept
        {
            return std::ranges::lower_bound(begin(), end(), key, std::ranges::less{}, &record_impl::position);
        }

        /// @brief Returns an iterator to the first element in the journal that is greater than the given key.
        /// @param key The key to search for.
        iterator upper_bound(key_type const key) noexcept
        {
            return std::ranges::upper_bound(begin(), end(), key, std::ranges::less{}, &record_impl::position);
        }

        /// @brief Returns an iterator to the first element in the journal that is greater than the given key.
        /// @param key The key to search for.
        const_iterator upper_bound(key_type const key) const noexcept
        {
            return std::ranges::upper_bound(begin(), end(), key, std::ranges::less{}, &record_impl::position);
        }

        /// @brief Returns an iterator to the first element in the journal that contains the given key.
        /// @param key The key to search for.
        iterator find(key_type const key) noexcept
        {
            return std::ranges::find(begin(), end(), key, std::ranges::less{}, &record_impl::position);
        }

        /// @brief Returns an iterator to the first element in the journal that contains the given key.
        /// @param key The key to search for.
        const_iterator find(key_type const key) const noexcept
        {
            return std::ranges::find(begin(), end(), key, std::ranges::less{}, &record_impl::position);
        }
        /// @}

        /// @name Utilities
        /// @{
    private:

        /**
         * @brief Get the breakpoint slice for a given segment and two positions.
         *
         * @tparam sequence_t The type of the sequence.
         * @param segment The sequence to get a breakpoint slice for.
         * @param from The start position of the slice.
         * @param to  The end position of the slice.
         * @return The breakpoint slice of the segment.
         */
        template <typename sequence_t>
        constexpr static auto get_breakpoint_slice(sequence_t && segment,
                                                   std::ranges::iterator_t<sequence_t> from,
                                                   std::ranges::iterator_t<sequence_t> to)
        {
            auto breakpoint = libjst::to_breakpoint(segment, std::move(from), std::move(to));
            return libjst::breakpoint_slice(segment, std::move(breakpoint));
        }

        /**
         * @brief Splits the journal entry at the given position.
         *
         * @param breakend The breakend at which to split the entry.
         * @retval std::pair<record_impl, record_impl> A pair of journal entries, where the first entry covers the
         *         interval `[segment().begin(), breakend)` and the second entry covers the interval
         *         `[breakend, segment().end())`.
         * @rst
         * .. note:: If ``breakend`` is out of bounds, either the first entry or the second entry will have an
         *           empty segment.
         * @endrst
         *
         * @rst
         * .. danger:: If ``breakend`` is not a valid iterator of the sequence segment, the behaviour is undefined.
         * @endrst
         */
        constexpr std::pair<record_impl, record_impl> split_at(breakend_type breakend) const noexcept
        {
            auto [journal_it, split_it] = std::move(breakend).base();
            auto && segment = journal_it->sequence();

            auto record_prefix = record_impl{journal_it->position(),
                                             get_breakpoint_slice(segment, std::ranges::begin(segment), split_it)};

            auto split_offset = std::ranges::distance(std::ranges::begin(segment), std::move(split_it));
            auto record_suffix = record_impl{journal_it->position() + split_offset,
                                             get_breakpoint_slice(segment, split_it, std::ranges::end(segment))};

            return {std::move(record_prefix), std::move(record_suffix)};
        }

        /**
         * @brief Overwrites the journal entry at the given iterator with the new record.
         *
         * @param it The iterator to the journal entry to overwrite.
         * @param record The new record to insert into the journal.
         */
        constexpr void force_overwrite_through(std::ranges::iterator_t<journal_type const> it, record_impl record)
            noexcept(std::is_nothrow_move_constructible_v<record_impl>)
        {
            auto out_it = std::ranges::begin(_journal) + std::ranges::distance(_journal.cbegin(), it);
            *out_it = std::move(record);
        }

        /**
         * @brief Implementation of the record function that handles the inline recording of a sequence.
         *
         * @param breakpoint The breakpoint at which to record the new sequence.
         * @param new_sequence The new sequence to record in the journal.
         * @retval iterator The iterator to the newly inserted record.
         */
        iterator record_inline(breakpoint_type breakpoint, sequence_type new_sequence)
        {
            std::ptrdiff_t deletion_size = libjst::breakend_span(breakpoint);
            std::ptrdiff_t insertion_size = std::ranges::ssize(new_sequence);

            std::array<record_impl, 2> entries_marked_for_insertion{};
            size_t marked_insert_entries{};

            auto [low_prefix, low_suffix] = split_at(libjst::low_breakend(breakpoint));
            auto [high_prefix, high_suffix] = split_at(libjst::high_breakend(breakpoint));

            if (!std::ranges::empty(low_prefix.sequence()))
                entries_marked_for_insertion[marked_insert_entries++] = std::move(low_prefix);

            if (insertion_size > 0)
                entries_marked_for_insertion[marked_insert_entries++] =
                        record_impl{low_suffix.position(), std::move(new_sequence)};

            auto from_journal_it = std::get<0>(libjst::low_breakend(breakpoint).base());
            auto to_journal_it = std::get<0>(libjst::high_breakend(breakpoint).base());
            force_overwrite_through(to_journal_it, std::move(high_suffix));
            auto insert_position = _journal.erase(from_journal_it, to_journal_it);
            auto insert_entries_it = std::make_move_iterator(entries_marked_for_insertion.begin());
            auto first_inserted = _journal.insert(insert_position,
                                                  insert_entries_it,
                                                  insert_entries_it + marked_insert_entries);

            update_positions_of_remaining_entries(first_inserted + marked_insert_entries, insertion_size - deletion_size);

            assert(check_journal_invariants());

            return std::ranges::next(first_inserted, marked_insert_entries - (insertion_size > 0));
        }

        /**
         * @brief Updates the positions of the remaining entries in the journal after recording a new entry.
         *
         * @param journal_it The iterator to the first entry that needs to be updated.
         * @param offset The offset to add to the positions of the remaining entries.
         */
        void update_positions_of_remaining_entries(journal_type::iterator journal_it, std::ptrdiff_t offset) noexcept
        {
            if (offset == 0)
                return;

            for (; journal_it != std::ranges::end(_journal); ++journal_it)
                *journal_it = record_impl{static_cast<size_type>(journal_it->position() + offset),
                                          journal_it->sequence()};
        }

        /**
         * @brief Sanity check for the journal.
         *
         * @returns `true` if the journal invariants are valid, `false` otherwise.
         */
        bool check_journal_invariants() const noexcept {
            // 1. check if first record starts at position 0
            // 2. check if all adjacent entries are non-overlapping such that end position of smaller record is equal to begin position of successor
            if (_journal.empty() || _journal.begin()->position() != 0)
                return false;

            for (auto it = begin(); it != end(); ++it)
            {
                auto const &record = *it;
                auto const &next_record = *std::ranges::next(it);
                if (record.position() + std::ranges::size(record.sequence()) != next_record.position())
                    return false;
            }
            return true;
        }

        /**
         * @brief Initializes the underlying journal structure.
         *
         * If the source is not empty, the first entry in the journal will be a breakpoint slice of the source sequence.
         * Additionally, a sentinel entry is added to the journal representing an empty sequence with position equal to the size of the source.
         */
        constexpr void initialize_journal()
            noexcept(std::is_nothrow_constructible_v<record_impl, key_type, sequence_type>)
        {
            if (!std::ranges::empty(source())) {
                auto src_bpt = libjst::to_breakpoint(source(), std::ranges::begin(source()), std::ranges::end(source()));
                _journal.emplace_back(0, libjst::breakpoint_slice(source(), std::move(src_bpt)));
            }

            _journal.emplace_back(std::ranges::size(source()), sequence_type{});
        }
        /// @}
    };

    template <libjst::preserving_reference_sequence source_t>
    class inline_sequence_journal<source_t>::record_impl
    {
        ///@name Public member types
        ///@{
    public:
        ///@brief The breakpoint type to specify the inline positions of the segments.
        using size_type = std::ranges::range_size_t<source_t>;
        ///@brief The sequence type is implemented as std::ranges::subrange.*/
        using sequence_type = libjst::breakpoint_slice_t<source_t const>;

        ///@}

        /** @name Member variables */
        ///@{
    private:
        /** @brief The begin position of the referenced segment in the journaled sequence. */
        size_type _position{};
        /** @brief The referenced segment in the journaled sequence. */
        sequence_type _sequence{};
        ///@}

        ///@name Constructors, assignment and destructor */
        ///@{
    public:
        /**
         * @brief Construct a new journal entry object.
         *
         * The position will be zero initialized and the subrange will be default constructed.
         * This constructor is only available if the sequence type is std::default_initializable.
         */
        constexpr record_impl()
            requires std::default_initializable<sequence_type>
        = default;

        /**
         * @brief Constructs a journal entry from a position and a sequence.
         *
         * @param position The start position of the referenced sequence segment in the journaled sequence, which is also used as the key to sort the records.
         * @param sequence The sequence referenced by this journal entry.
         */
        constexpr record_impl(size_type position, sequence_type sequence) :
            _position{position},
            _sequence{std::move(sequence)}
        {
        }
        ///@}

        /** @name Accessors */
        ///@{
    public:
        /**
         * @brief Returns the begin position of the referenced segment in the journaled sequence.
         *
         * @returns size_type
         */
        constexpr size_type position() const noexcept
        {
            return _position;
        }

        /**
         * @brief Returns the represented sequence slice.
         *
         * @returns sequence_type
         * @exception implementation_defined If the sequence type is nothrow copy constructible this function never throws.
         */
        constexpr sequence_type sequence() const noexcept(std::is_nothrow_copy_constructible_v<sequence_type>)
        {
            return _sequence;
        }
        ///@}

        /// @name Non-member functions
        /// @{
    public:
        /**
         * @brief Equality comparison.
         *
         * @param lhs The left hand side of the comparison.
         * @param rhs The right hand side of the comparison.
         *
         * @result `true` if both entries refer to the same segment, `false` otherwise.
         * @rst
         * .. note::
         *    This operator does not compare the segments lexicographically, but only if they point to the same
         *    memory range in the original sequence.
         * @endrst
         */
        constexpr friend bool operator==(record_impl const &lhs, record_impl const &rhs) noexcept
        {
            return lhs.position() == rhs.position() &&
                   std::ranges::begin(lhs.sequence()) == std::ranges::begin(rhs.sequence()) &&
                   std::ranges::size(lhs.sequence()) == std::ranges::size(rhs.sequence());
        }

        /**
         * @brief Three-way comparison.
         *
         * @param lhs The left hand side of the comparison.
         * @param rhs The right hand side of the comparison.
         *
         * This operator only compares the begin positions of the journal entries to determine their order.
         *
         * @retval std::weak_ordering The result of the comparison:
         * @rst
         * |   ``std::weak_ordering::less`` -- if the begin position of ``lhs`` is less than the begin position of ``rhs``.
         * |   ``std::weak_ordering::greater`` -- if the begin position of ``lhs`` is greater than the begin position of ``rhs``.
         * |   ``std::weak_ordering::equivalent`` -- if the begin position of ``lhs`` is equal to the begin position of ``rhs``.
         * @endrst
         */
        constexpr friend std::weak_ordering operator<=>(record_impl const &lhs, record_impl const &rhs) noexcept
        {
            return lhs.position() <=> rhs.position();
        }
        ///@}
    };

    template <libjst::preserving_reference_sequence source_t>
    class inline_sequence_journal<source_t>::breakend_impl
    {
        /// @name Member types
        /// @{
    private:
        using journal_iterator = std::ranges::iterator_t<journal_type const>; ///< The iterator type of the journal.
        using sequence_iterator = std::ranges::iterator_t<sequence_type const>; ///< The iterator type of the sequence.
        /// @}

        /// @name Member variables
        /// @{
    private:
        journal_iterator _journal_it{}; ///< The iterator to the journal entry.
        sequence_iterator _sequence_it{}; ///< The iterator to the sequence entry.
        /// @}

        /// @name Member functions
        /// @{
    public:

        /// @brief Default constructor.
        constexpr breakend_impl()
            noexcept(std::is_nothrow_default_constructible_v<journal_iterator> &&
                     std::is_nothrow_default_constructible_v<sequence_iterator>) = default;
        /**
         * @brief Constructs a breakend from the given journal and sequence iterators.
         *
         * @param journal_it The iterator to the journal entry.
         * @param sequence_it The iterator to the sequence entry.
         */
        constexpr explicit breakend_impl(journal_iterator journal_it, sequence_iterator sequence_it)
            noexcept(std::is_nothrow_move_constructible_v<journal_iterator> &&
                     std::is_nothrow_move_constructible_v<sequence_iterator>) :
            _journal_it{std::move(journal_it)},
            _sequence_it{std::move(sequence_it)}
        {
        }

        /// @brief Returns the underlying journal and sequence iterators as a pair.
        constexpr std::pair<journal_iterator, sequence_iterator> base() const &
            noexcept(std::is_nothrow_copy_constructible_v<journal_iterator> &&
                     std::is_nothrow_copy_constructible_v<sequence_iterator>)
        {
            return {_journal_it, _sequence_it};
        }

        /// @brief Returns the underlying journal and sequence iterators as a pair.
        constexpr std::pair<journal_iterator, sequence_iterator> base() && noexcept
        {
            return {std::move(_journal_it), std::move(_sequence_it)};
        }
        /// @}

        /// @name Conversion
        /// @{
    public:
        /// @brief Converts the breakend to a global position inside of the concatenated view of all records.
        template <std::integral integral_t>
        constexpr operator integral_t() const noexcept
        {
            return _journal_it->position() +
                   std::ranges::distance(std::ranges::begin(_journal_it->sequence()), _sequence_it);
        }
        /// @}

        /// @name Non-member functions
        /// @{
    public:
        /// @brief Returns the distance between two breakends.
        constexpr friend std::ptrdiff_t operator-(breakend_impl const & lhs, breakend_impl const & rhs) noexcept
        {
            return static_cast<std::ptrdiff_t>(lhs) - static_cast<std::ptrdiff_t>(rhs);
        }

        /// @brief Defaulted equality comparison.
        constexpr friend bool operator==(breakend_impl const &, breakend_impl const &) = default;

        /// @brief Defaulted three-way comparison.
        constexpr friend std::strong_ordering operator<=>(breakend_impl const &, breakend_impl const &) = default;
        /// @}
    };

}  // namespace libjst
