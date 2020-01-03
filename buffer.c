/************************************************************
 * PROJECT: KLEIN
 * FILE:    BUFFER.C
 * PURPOSE:
 * VERSION: 0.8.5
 *
 ************************************************************/

#include <unistd.h>
#include <fcntl.h>
#include "library.h"

#define NEW_FILE_PERMISSIONS                (O_WRONLY | O_CREAT | O_TRUNC), \
                                            (S_IRWXU | S_IRGRP| S_IWGRP | S_IROTH)

/// use this macro after a line is added to a buffer
#define _after_line_added(bfrp_)            \
MACRO_START                                 \
    (bfrp_)->line_count++;                  \
    buffer_renum_lines((bfrp_));            \
MACRO_END

/// use this macro to remove all lines from a buffer starting from the given line
#define _remove_all_lines(linep_)           \
MACRO_START                                 \
    struct line_st *tp;                     \
    while (!IS_NULL(linep_))                \
        {                                   \
            tp = linep_;                    \
            linep_ = linep_->next_line;     \
            line_free(tp);                  \
        }                                   \
MACRO_END

/// handles the tab char met when loading a file
static inline void
_process_read_tab (struct line_st *);
static inline void
_process_read_newline (struct buffer_st *, struct line_st **);

struct buffer_st
buffer_new (void)
{
  struct buffer_st bfr;
  FILL_VAR_WITH_ZERO(bfr, struct buffer_st);
  return bfr;
}

void
buffer_reset (struct buffer_st *bfrp)
{
  RETURN_IF_TRUE (IS_NULL (bfrp));
  FILL_PTR_WITH_ZERO(bfrp, struct buffer_st);
  bfrp->file_name = NULL;
  bfrp->first_line = NULL;
  bfrp->line_count = 0x0;
  bfrp->current_line = NULL;
  bfrp->page_top_line = NULL;
  bfrp->page_bottom_line = NULL;
  bfrp->is_dirty = FALSE;
}

void
buffer_renum_lines (struct buffer_st *bfrp)
{
  if (bfrp->line_count > 0x0)
    {
      INT32 i = 0x0;
      struct line_st *p = NULL;
      for (p = bfrp->first_line; !IS_NULL (p); p = p->next_line)
        {
          p->no = i++;
        }
    }
}

struct line_st *
buffer_get_line (struct buffer_st *bfrp, const INT32 pos)
{
  RETURN_VAL_IF_TRUE (IS_NULL (bfrp), NULL);
  struct line_st *linep = bfrp->first_line;
  INT32 n = 0x0;
  while (TRUE)
    {
      if (pos == n)
        {
          break;
        }
      n++;
      if (IS_NULL (linep->next_line))
        {
          break;
        }
      linep = linep->next_line;
    }
  return linep;
}

void
buffer_prepend_line (struct buffer_st *bfrp, struct line_st *newlinep)
{
  RETURN_IF_TRUE (IS_NULL (bfrp));
  struct line_st *nextlinep = NULL;
  if (bfrp->line_count > 0x0)
    {
      nextlinep = bfrp->first_line;
      nextlinep->prev_line = newlinep;
    }
  newlinep->prev_line = NULL;
  newlinep->next_line = nextlinep;
  bfrp->first_line = newlinep;
  _after_line_added (bfrp);
}

void
buffer_append_line (struct buffer_st *bfrp, struct line_st *newlinep)
{
  if (!bfrp->line_count)
    {
      buffer_prepend_line (bfrp, newlinep);
    }
  else
    {
      struct line_st *prevlinep = buffer_get_line (bfrp, bfrp->line_count - 0x1);
      prevlinep->next_line = newlinep;
      newlinep->next_line = NULL;
      newlinep->prev_line = prevlinep;
      _after_line_added (bfrp);
    }
}

void
buffer_insert_line (struct buffer_st *bfrp, 
                    const INT32 pos,
                    struct line_st *newlinep)
{
  if (!pos)
    {
      buffer_prepend_line (bfrp, newlinep);
    }
  else
    {
      struct line_st *nxtp = buffer_get_line (bfrp, pos);
      struct line_st *prvp = buffer_get_line (bfrp, pos - 0x1);
      nxtp->prev_line = newlinep;
      prvp->next_line = newlinep;
      newlinep->next_line = nxtp;
      newlinep->prev_line = prvp;
      _after_line_added (bfrp);
    }
}

void
buffer_drop_line (struct buffer_st *bfrp, const INT32 pos)
{
  RETURN_IF_TRUE (IS_EQUAL(bfrp->line_count, 0x0));
  struct line_st *linep = buffer_get_line (bfrp, pos);
  if (!pos)
    {
      if (!IS_NULL (linep->next_line))
        {
          linep->next_line->prev_line = NULL;
          bfrp->first_line = linep->next_line;
        }
      else
        {
          bfrp->first_line = NULL;
        }
      goto end;
    }
  if (pos == bfrp->line_count - 0x1)
    {
      linep->prev_line->next_line = NULL;
      goto end;
    }
  linep->prev_line->next_line = linep->next_line;
  linep->next_line->prev_line = linep->prev_line;
end:
  line_free (linep);
  linep = NULL;
  bfrp->line_count--;
  buffer_renum_lines (bfrp);
}

void
buffer_clear (struct buffer_st *bfrp)
{
  RETURN_IF_TRUE (IS_NULL (bfrp) || (!bfrp->line_count));
  struct line_st *p = bfrp->first_line;
  _remove_all_lines (p);
  buffer_reset (bfrp);
}

BOOL16
buffer_load_file (struct buffer_st *bfrp)
{
  BOOL16 rc = TRUE;
  INT32 fd = open (bfrp->file_name, O_RDONLY, 0x0);
  if (IS_NEGATIVE (fd))
    {
      rc = FALSE;
    }
  else
    {
      struct line_st *linep = NULL;
      CHAR8 c;
      while (file_read_byte (fd, &c))
        {
          if (IS_NULL (linep))
            {
              buffer_append_line (bfrp, line_new ());
              linep = bfrp->first_line;
            }
          switch (c)
            {
            case TAB_CHAR:
              _process_read_tab (linep);
              break;
            case LF_CHAR:
              _process_read_newline (bfrp, &linep);
              break;
            default:
              line_append_char (linep, c);
              break;
            }
        }
      close (fd);
      if (!IS_NULL (linep))
        {
          if (line_is_null (linep))
            {
              buffer_drop_line (bfrp, bfrp->line_count - 0x1);
            }
        }
      if (!bfrp->line_count)
        {
          buffer_append_line (bfrp, line_new ());
        }
      buffer_renum_lines (bfrp);
    }
  return rc;
}

BOOL16
buffer_save_file (struct buffer_st *bfrp)
{
  BOOL16 rc = TRUE;
  INT32 fd = open (bfrp->file_name, NEW_FILE_PERMISSIONS);
  if (IS_NEGATIVE (fd))
    {
      rc = FALSE;
    }
  else
    {
      INT32 i;
      CHAR8 *s = NULL;
      struct line_st *linep = NULL;
      INT32 len;
      for (i = 0x0; i < bfrp->line_count; i++)
        {
          linep = buffer_get_line (bfrp, i);
          if (line_is_null (linep))
            {
              s = "";
              len = 0x0;
            }
          else
            {
              len = line_len (linep);
              str_alloc (&s, len);
              strcpy (s, linep->text);
            }
          file_write_str (fd, s, TRUE);
        }
      close (fd);
    }
  bfrp->is_dirty = FALSE;
  return rc;
}

static inline void
_process_read_tab (struct line_st *linep)
{
  line_append_char (linep, SPACE_CHAR);
}

static inline void
_process_read_newline (struct buffer_st *bfrp, struct line_st **linepp)
{
  buffer_append_line (bfrp, line_new ());
  *linepp = buffer_get_line (bfrp, bfrp->line_count - 0x1);
}