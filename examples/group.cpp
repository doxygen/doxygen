/*! \defgroup group1 The First Group
 *  This is the first group
 */

/*! \defgroup group2 The Second Group
 *  This is the second group
 */

/*! \defgroup group3 The Third Group
 *  This is the third group
 */

/*! \ingroup group1 
 *  \brief class C1 in group 1
 */
class C1 {};

/*! \ingroup group1
 *  \brief class C2 in group 1
 */
class C2 {};

/*! \ingroup group2
 *  \brief class C3 in group 2
 */
class C3 {};

/*! \ingroup group2
 *  \brief class C4 in group 2
 */
class C4 {};

/*! \ingroup group3
 *  \brief class C5 in \link group3 the third group\endlink.
 */
class C5 {};

/*! \ingroup group1 group2 group3
 *  \brief class C6 in all groups
 *  \sa \link group1 The first group\endlink, group2, group3
 */
class C6 {};

/*! \file
 *  \ingroup group3
 *  \brief this file in group 3
 */
