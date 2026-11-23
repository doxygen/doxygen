/** A C pair.
 *  @field first First value.
 *  @field second Second value.
 */
typedef struct CPair
{
  unsigned first, second;
} CPair;

/** A C union.
 *  @field number A number.
 */
typedef union CValue
{
  int number;
} CValue;
