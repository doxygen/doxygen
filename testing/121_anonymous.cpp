// objective: test anonymous unions and structs
// check: struct_t.xml
// check: struct_t_1_1_0fstruct_0e_8s.xml
// check: union_0funion_0e_8g__uvar.xml
// check: union_t_1_1_0funion_0e_8s_8u.xml
// check: unionns_1_1_0funion_0e_8ns__u.xml

/** @file */

/** a struct */
typedef struct
{
  int x; /**< member x */
  struct
  {
    enum
    {
      EVal1, /**< enum value 1 */
      EVal2, /**< enum value 2 */
    } e_var; /**< enum variable */
    union
    {
      int ui; /**< union member ui */
      char uc; /**< union member uc */
    } u; /**< union u */
    int y; /**< member y */
  } s; /**< struct s */
} T;

/** a namespace */
namespace ns
{
  union
  {
    int ns_ui; /**< namespace union member ui */
    char ns_uc; /**< namespace union member uc */
  } ns_u; /**< namespace union u */
}

/** a global union type */
union
{
  int ga; /**< global union member ga */
  float gb; /**< global union member gb */
} g_uvar; /**< global union variable */

