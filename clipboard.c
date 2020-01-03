/************************************************************
 * PROJECT: KLEIN
 * FILE:    CLIPBOARD.C
 * PURPOSE:
 * VERSION: 0.8.5
 *
 ************************************************************/

#include "library.h"

void
undo_init (void)
{
  g_undo_clipboard = *line_new ();
  g_undo_clipboard.no = -0x1;
}

void
undo_set_line (struct line_st *linep)
{
  RETURN_IF_TRUE (IS_NULL (linep));
  g_undo_clipboard.no = linep->no;
  g_undo_clipboard.text = linep->text;
  g_undo_clipboard.cursor_char = linep->cursor_char;
  g_undo_clipboard.first_visible_char = linep->first_visible_char;
  g_undo_clipboard.prev_line = g_undo_clipboard.next_line = NULL;
}

BOOL16
undo_restore_line (struct buffer_st *bfrp)
{
  RETURN_VAL_IF_TRUE (IS_NULL (bfrp), FALSE);
  if (IS_EQUAL (g_undo_clipboard.no, -0x1))
    {
      return FALSE;
    }
  INT32 no = (g_undo_clipboard.no > bfrp->line_count - 0x1) ? bfrp->line_count - 0x1 : g_undo_clipboard.no;
  struct line_st *linep = line_new ();
  *linep = g_undo_clipboard;
  buffer_insert_line (bfrp, no, linep);
  return TRUE;
}