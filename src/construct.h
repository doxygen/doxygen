/******************************************************************************
 *
 * Copyright (C) 1997-2024 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby
 * granted. No representations are made about the suitability of this software
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 * Documents produced by Doxygen are derivative works derived from the
 * input used in their production; they are not affected by this license.
 *
 */

#ifndef CONSTRUCT_H
#define CONSTRUCT_H

//! Macro to implement rule of 5 for an abstract base class
#define ABSTRACT_BASE_CLASS(cls)                  \
    cls() = default;                              \
    cls(const cls &) = delete;                    \
    cls &operator=(const cls &) = delete;         \
    cls(cls &&) = delete;                         \
    cls &operator=(cls &&) = delete;              \
    virtual ~cls() = default;                     \

//! Macro to help implementing the rule of 5 for a default copyable & movable class
#define DEFAULT_COPYABLE(cls)                     \
    cls(const cls &) = default;                   \
    cls &operator=(const cls &) = default;        \
    cls(cls &&) = default;                        \
    cls &operator=(cls &&) = default;             \
    virtual ~cls() = default;

//! Macro to help implementing the rule of 5 for a non-copyable & movable class
#define NON_COPYABLE(cls)                         \
    cls(const cls &) = delete;                    \
    cls &operator=(const cls &) = delete;         \
    cls(cls &&) = delete;                         \
    cls &operator=(cls &&) = delete;              \

//! Macro to help implementing the rule of 5 for a class that can be moved but not copied
#define ONLY_DEFAULT_MOVABLE(cls)                 \
    cls(const cls &) = delete;                    \
    cls &operator=(const cls &) = delete;         \
    cls(cls &&) = default;                        \
    cls &operator=(cls &&) = default;             \

#define ONLY_MOVABLE_DECL(cls)                    \
    cls(const cls &) = delete;                    \
    cls &operator=(const cls &) = delete;         \
    cls(cls &&);                                  \
    cls &operator=(cls &&);                       \

#define DEFAULT_MOVABLE_IMPL(cls)                 \
    cls::cls(cls &&) = default;                   \
    cls &cls::operator=(cls &&) = default;        \

#endif
