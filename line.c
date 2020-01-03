/************************************************************
 * PROJECT: KLEIN
 * FILE:    LINE.C
 * PURPOSE:
 * VERSION: 0.8.5
 *
 ************************************************************/

#include "library.h"

struct line_st *
line_new (void)
{
  size_t memsiz = sizeof (struct line_st);
  struct line_st *linep = (struct line_st *) mem_alloc (memsiz);
  RETURN_VAL_IF_TRUE (IS_NULL (linep), NULL);
  FILL_PTR_WITH_ZERO(linep, struct line_st);
  linep->no = -0x1;
  linep->text = NULL;
  linep->cursor_char = 0x0;
  linep->first_visible_char = 0x0;
  linep->selstart_char = -0x1;
  linep->selend_char = -0x1;
  return linep;
}

void
line_free (struct line_st *linep)
{
  RETURN_IF_TRUE (IS_NULL (linep));
  if (!line_is_null (linep))
    {
      (void) mem_free (linep->text);
    }
  (void) mem_free (linep);
}

void
line_text_alloc (struct line_st *linep, const INT32 nbytes)
{
  if (nbytes <= 0x0)
    {
      if (!line_is_null (linep))
        {
          (void) mem_free (linep->text);
          linep->text = NULL;
        }
    }
  else
    {
      if (line_is_null (linep))
        {
          str_alloc (&linep->text, nbytes);
        }
      else
        {
          str_realloc (&linep->text, nbytes);
        }
      line_edit_char (linep, nbytes, '\0');
    }
}

BOOL16
line_is_null (struct line_st *linep)
{
  BOOL16 rc = IS_NULL (linep->text) ? TRUE : FALSE;
  return rc;
}

INT32
line_len (struct line_st *linep)
{
  INT32 retval = (line_is_null (linep)) ? 0x0 : STRING_LEN (linep->text);
  return retval;
}

void
line_prepend_char (struct line_st *linep, const CHAR8 c)
{
  if (line_is_null (linep))
    {
      line_append_char (linep, c);
    }
  else
    {
      line_insert_char (linep, 0x0, c);
    }
}

BOOL16
line_strstr (struct line_st *linep, CHAR8 *s, char_index_t *stridxp, char_index_t *lenp)
{
  RETURN_VAL_IF_TRUE (line_is_null (linep) || (IS_NULL (s)), FALSE);
  CHAR8 *t = strstr (linep->text, s);
  BOOL16 rc = !IS_NULL(t) ? TRUE : FALSE;
  *stridxp = !IS_TRUE(rc) ? -0x1 : t - linep->text;
  *lenp = !IS_TRUE(rc) ? -0x0 : *stridxp + STRING_LEN(s) - 1;
  return rc;
}