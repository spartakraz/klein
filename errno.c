/************************************************************
 * PROJECT: KLEIN
 * FILE:    ERRNO.C
 * PURPOSE:
 * VERSION: 0.8.5
 *
 ************************************************************/

#include "library.h"

const CHAR8 *
errtype_to_str (const error_symbol_e errsym)
{
  error_st *p = g_error_list;
  while (!IS_NULL (p->description))
    {
      if (IS_EQUAL (errsym, p->symbol))
        {
          return (const CHAR8 *) p->description;
        }
      p++;
    }
  return "[UnknownErrorSymbol]";
}