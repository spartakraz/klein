/************************************************************
 * PROJECT: KLEIN
 * FILE:    MATH.C
 * PURPOSE:
 * VERSION: 0.8.5
 *
 ************************************************************/

#include "library.h"

extern inline INT32
math_qmult (INT32 n1, INT32 n2) ALWAYS_INLINE_ATTRIB;
extern inline INT32
math_qdiv (INT32 n1, INT32 n2) ALWAYS_INLINE_ATTRIB;
extern inline INT32
math_qmod (INT32 n1, INT32 n2) ALWAYS_INLINE_ATTRIB;
extern inline INT32
math_asm_div(int n1, int n2);