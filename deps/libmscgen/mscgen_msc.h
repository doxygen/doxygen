/***************************************************************************
 *
 * $Id: msc.h 175 2011-02-06 21:07:43Z Michael.McTernan $
 *
 * The message sequence parser API.
 * Copyright (C) 2009 Michael C McTernan, Michael.McTernan.2001@cs.bris.ac.uk
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 ***************************************************************************/

#ifndef MSCGEN_MSC_H
#define MSCGEN_MSC_H

#include "mscgen_bool.h"

/***************************************************************************
 * Types
 ***************************************************************************/

/** Msc Options.
 */
typedef enum MscOptTypeTag
{
    MSC_OPT_HSCALE,
    MSC_OPT_WIDTH,
    MSC_OPT_ARCGRADIENT,
    MSC_OPT_WORDWRAPARCS
}
MscOptType;


/** Arc attributes.
 * An arc may have one or more attributes listed in square brackets after
 * the declaration.  This gives an enumerated type for each permissible
 * attribute.
 */
typedef enum MscAttribTypeTag
{
    MSC_ATTR_LABEL,
    MSC_ATTR_ID,
    MSC_ATTR_URL,
    MSC_ATTR_IDURL,
    MSC_ATTR_LINE_COLOUR,
    MSC_ATTR_TEXT_COLOUR,
    MSC_ATTR_TEXT_BGCOLOUR,
    MSC_ATTR_ARC_LINE_COLOUR,
    MSC_ATTR_ARC_TEXT_COLOUR,
    MSC_ATTR_ARC_TEXT_BGCOLOUR,
    MSC_ATTR_NO_ARROWS,
    MSC_ATTR_BI_ARROWS,
    MSC_ATTR_ARC_SKIP
}
MscAttribType;


typedef enum
{
    MSC_ARC_METHOD,
    MSC_ARC_RETVAL,
    MSC_ARC_SIGNAL,
    MSC_ARC_CALLBACK,
    MSC_ARC_DOUBLE,
    MSC_ARC_DISCO,      /* ... Discontinuity in time line */
    MSC_ARC_DIVIDER,    /* --- Divider */
    MSC_ARC_SPACE,      /* ||| */
    MSC_ARC_PARALLEL,   /* Comma instead of semicolon */
    MSC_ARC_BOX,
    MSC_ARC_ABOX,
    MSC_ARC_RBOX,
    MSC_ARC_NOTE,
    MSC_ARC_LOSS,       /* -x or x- */

    MSC_INVALID_ARC_TYPE
}
MscArcType;


/***************************************************************************
 * Abstract types
 ***************************************************************************/

typedef struct MscTag           *Msc;

typedef struct MscOptTag        *MscOpt;

typedef struct MscEntityTag     *MscEntity;

typedef struct MscEntityListTag *MscEntityList;

typedef struct MscArcTag        *MscArc;

typedef struct MscArcListTag    *MscArcList;

typedef struct MscAttribTag     *MscAttrib;


/***************************************************************************
 * MSC Building Functions
 ***************************************************************************/

/** Parse some input to build a message sequence chart.
 * This will parse characters from \a in and build a message sequence chart
 * ADT.
 * \retval Msc  The message sequence chart, which may equal \a NULL is a
 *               parse error occurred.
 */
#ifdef __cplusplus
extern "C" {
#endif
Msc           MscParse(FILE *in);

MscEntity     MscAllocEntity(char *entityName);

MscEntityList MscLinkEntity(MscEntityList list, MscEntity elem);

void          MscPrintEntityList(MscEntityList list);

MscOpt        MscAllocOpt(MscOptType  type,
                          char       *value);

MscOpt        MscLinkOpt(MscOpt head,
                         MscOpt newHead);

MscArc        MscAllocArc(char        *srcEntity,
                          char        *dstEntity,
                          MscArcType   type,
                          unsigned int inputLine);

MscArcList    MscLinkArc (MscArcList list,
                          MscArc     elem);

void          MscPrintArcList(struct MscArcListTag *list);

MscAttrib     MscAllocAttrib(MscAttribType  type,
                             char          *value);

MscAttrib     MscLinkAttrib(MscAttrib head,
                            MscAttrib newHead);

void          MscArcLinkAttrib(MscArc    arc,
                               MscAttrib att);

void          MscEntityLinkAttrib(MscEntity ent,
                                  MscAttrib att);

void          MscPrintAttrib(const struct MscAttribTag *att);

const char   *MscPrettyAttribType(MscAttribType t);

Msc           MscAlloc(MscOpt        optList,
                       MscEntityList entityList,
                       MscArcList    arcList);

void          MscFree(struct MscTag *m);

/** Print the passed msc in textual form to stdout.
 * This prints a human readable format of the parsed msc to stdout.  This
 * is primarily of use in debugging the parser.
 */
void          MscPrint(Msc m);

unsigned int  MscGetNumEntities(Msc m);

unsigned int  MscGetNumArcs(Msc m);

unsigned int  MscGetNumParallelArcs(Msc m);

unsigned int  MscGetNumOpts(Msc m);

/** Get an MSC option, returning the value as a float.
 *
 * \param[in]     m      The MSC to analyse.
 * \param[in]     type   The option type to retrieve.
 * \param[in,out] f      Pointer to be filled with parsed value.
 * \retval TRUE  If the option was found and parsed successfully.
 */
Boolean       MscGetOptAsFloat(struct MscTag *m, MscOptType type, float *const f);

/** Get an MSC option, returning the value as a Boolean.
 *
 * \param[in]     m      The MSC to analyse.
 * \param[in]     type   The option type to retrieve.
 * \param[in,out] b      Pointer to be filled with parsed value.
 * \retval TRUE  If the option was found and parsed successfully,
 *                otherwise FALSE in which case *b is unmodified.
 *
 */
Boolean      MscGetOptAsBoolean(struct MscTag *m, MscOptType type, Boolean *const b);

/** Get the index of some entity.
 * This returns the column index for the entity identified by the passed
 * label.
 *
 * \param  m      The MSC to analyse.
 * \param  label  The label to find.
 * \retval -1     If the label was not found, otherwise the column index.
 */
int           MscGetEntityIndex(struct MscTag *m, const char *label);

/***************************************************************************
 * Entity processing functions
 ***************************************************************************/

/** \defgroup EntityFuncs  Entity handling functions
 * @{
 */

/** Reset the entity iterator.
 * This moves the pointer to the current entity to the head of the list.
 */
void          MscResetEntityIterator(Msc m);

/** Move to the next entity in the MSC.
 * \retval TRUE if there is another entity, otherwise FALSE if the end of the
 *          list has been reached.
 */
Boolean       MscNextEntity(struct MscTag *m);

/** Get the value of some attribute for the current entity.
 * \retval The attribute string, or NULL if unset.
 */
const char   *MscGetCurrentEntAttrib(Msc m, MscAttribType a);

/** Get an attribute associated with some entity.
 * \param[in] entIdx  The index of the entity.
 * \retval The attribute string, or NULL if unset.
 */
const char   *MscGetEntAttrib(Msc m, unsigned int entIdx, MscAttribType a);

/** @} */

/***************************************************************************
 * Arc processing functions
 ***************************************************************************/

/** \defgroup ArcFuncs  Arc handling functions
 * @{
 */

/** Reset the arc iterator.
 * This moves the pointer to the current arc to the head of the list.
 */
void          MscResetArcIterator   (Msc m);

/** Move to the next arc in the MSC.
 * \retval TRUE if there is another arc, otherwise FALSE if the end of the
 *          list has been reached.
 */
Boolean       MscNextArc(struct MscTag *m);


/** Get the name of the entity from which the current arc originates.
 * \retval The label for the entity from which the current arc starts.
 *          The returned string must not be modified.
 */
const char   *MscGetCurrentArcSource(Msc m);


/** Get the name of the entity at which the current arc terminates.
 * \retval The label for the entity at which the current arc stops.
 *          The returned string must not be modified.
 */
const char   *MscGetCurrentArcDest(Msc m);

/** Get the type for some arc.
 *
 */
MscArcType    MscGetCurrentArcType(struct MscTag *m);

/** Get the value of some attribute for the current arc.
 * \retval The attribute string, or NULL if unset.
 */
const char   *MscGetCurrentArcAttrib(Msc m, MscAttribType a);

/** Get the line of the input file at which the current arc was defined.
 * \retval The line number of the input file.
 */
unsigned int  MscGetCurrentArcInputLine(Msc m);

/** @} */
#ifdef __cplusplus
}
#endif

#endif /* MSCGEN_MSC_H */

/* END OF FILE */
