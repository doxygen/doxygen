// objective: test INLINE_FUNCTION_MACROS (the layout difference is not observable in XML output)
// check: 124__inline__function__like__macros_8c.xml
// config: INLINE_FUNCTION_MACROS = YES

/** \file */

/** A function-like macro with a one-line body. */
#define ADD_ZERO(value) (value) + 0

/**
 * \name Named group for macros with arguments.
 *
 * \{ */
/** First grouped function-like macro. */
#define MACRO_GROUP_ARG_0(arg) (arg) + 0
/** Second grouped function-like macro. */
#define MACRO_GROUP_ARG_1(arg) (arg) + 0
/** \} */
