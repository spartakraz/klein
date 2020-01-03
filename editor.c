/************************************************************
 * PROJECT: KLEIN
 * FILE:    EDITOR.C
 * PURPOSE:
 * VERSION: 0.8.5
 *
 ************************************************************/

#include "library.h"

/// use this macro to update the cursor on the program's screen
#define _redraw_cursor()                                                \
MACRO_START                                                             \
    ui_redraw_status_bar();                                             \
    ui_draw_cursor();                                                   \
    conio_refresh_window(g_text_area);                                  \
MACRO_END

/// use this macro to mark a buffer as dirty, i.e. having unsaved changes
#define _mark_as_dirty(bufp_)                                           \
MACRO_START                                                             \
    (bufp_)->is_dirty = TRUE;                                           \
MACRO_END

/// use this macro to insert the specified number of consecutive spaces
#define _text_insert_spaces(nspaces_)                                   \
MACRO_START                                                             \
    for (i = 0x0; i < nspaces_; i++)                                    \
        {                                                               \
            text_accept_printable_key(SPACE_CHAR);                      \
        }                                                               \
MACRO_END

/// use this macro to redraw a line
#define _text_redraw_line(linep_)                                       \
MACRO_START                                                             \
    ui_draw_line (linep_);                                              \
    _redraw_cursor ();                                                  \
MACRO_END

/// returns TRUE if the current line needs to be horizontally scrolled in the
/// forward direction
static inline BOOL16
_need_frwd_hscroll (void);
/// returns TRUE if the current line needs to be horizontally scrolled in the
/// backward direction
static inline BOOL16
_need_bkwd_hscroll (void);
/// inserts spaces to the given line to have indented at the same level
/// as the line prior to it
static void
_prepend_indent (struct line_st *, INT32 *);
/// indents the current line
static void
_indent_line (screen_draw_function);
/// breaks the current line after the user hits the RETURN key
static void
_break_line (void);
/// merges the current line with the next one after the user hits the DELETE key
/// at the end of the current line
static void
_merge_lines (screen_draw_function);
/// checks if a line with given number belongs to the current page
static BOOL16
_is_in_current_page (const INT32);
/// use this function to save parameters of the current page before switching
/// to the console mode
static inline void
_save_current_page_state (void);
/// Use this function to restore the current page after switching from
/// the console mode back to the frame mode. The page is restored using data saved
/// by _save_current_page_state().
static inline void
_restore_current_page_state (void);
/// redraws the contents of the text area
static void
_text_draw_text_area (void);

BOOL16
text_is_on_BOL (void)
{
  struct line_st *p = g_current_buffer_ptr->current_line;
  BOOL16 rc = (!p->cursor_char) ? TRUE : FALSE;
  return rc;
}

BOOL16
text_is_on_EOL (void)
{
  struct line_st *p = g_current_buffer_ptr->current_line;
  BOOL16 rc = (p->cursor_char == line_len (p)) ? TRUE : FALSE;
  return rc;
}

BOOL16
text_is_on_left_edge (void)
{
  struct line_st *p = g_current_buffer_ptr->current_line;
  BOOL16 rc = (text_char_idx_to_x (p->cursor_char) == FRAME_TOP_LEFT_X) ? TRUE : FALSE;
  return rc;
}

BOOL16
text_is_on_right_edge (void)
{
  struct line_st *p = g_current_buffer_ptr->current_line;
  BOOL16 rc = (text_char_idx_to_x (p->cursor_char) == FRAME_COLS_COUNT () - 0x1) ? TRUE : FALSE;
  return rc;
}

void
text_to_left_edge (void)
{
  struct line_st *p = g_current_buffer_ptr->current_line;
  RETURN_IF_TRUE (line_is_null (p));
  while (!IS_TRUE (text_is_on_BOL ()) && !IS_TRUE (text_is_on_left_edge ()))
    {
      text_move_char_bkwd ();
    }
}

void
text_to_right_edge (void)
{
  struct line_st *p = g_current_buffer_ptr->current_line;
  RETURN_IF_TRUE (line_is_null (p));
  while (!IS_TRUE (text_is_on_EOL ()) && !IS_TRUE (text_is_on_right_edge ()))
    {
      text_move_char_frwd ();
    }
}

void
text_scroll_frwd (void)
{
  struct line_st *p = g_current_buffer_ptr->current_line;
  text_to_right_edge ();
  RETURN_IF_TRUE (!IS_TRUE (_need_frwd_hscroll ()));
  p->first_visible_char += FRAME_COLS_COUNT ();
  p->cursor_char = p->first_visible_char;
}

void
text_scroll_bkwd (void)
{
  struct line_st *p = g_current_buffer_ptr->current_line;
  text_to_left_edge ();
  RETURN_IF_TRUE (!IS_TRUE (_need_bkwd_hscroll ()));
  p->first_visible_char = (p->first_visible_char > 0x0) ? p->first_visible_char - FRAME_COLS_COUNT () : 0x0;
  p->cursor_char--;
}

void
text_move_line_up (void)
{
  BOOL16 need_redraw = FALSE;
  screen_draw_function funcp = (screen_draw_function) ui_redraw_screen;
  RETURN_IF_TRUE (IS_NULL (g_current_buffer_ptr->current_line->prev_line));
  if (!IS_NEGATIVE (g_current_buffer_ptr->current_line->selstart_char))
    {
      line_disable_selection (g_current_buffer_ptr->current_line);
    }
  if (g_current_buffer_ptr->current_line == g_current_buffer_ptr->page_top_line)
    {
      g_current_buffer_ptr->page_top_line = g_current_buffer_ptr->page_top_line->prev_line;
      need_redraw = TRUE;
    }
  g_current_buffer_ptr->current_line = g_current_buffer_ptr->current_line->prev_line;
  if (IS_TRUE (need_redraw))
    {
      CALL_FUNCTION (funcp);
    }
  else
    {
      _redraw_cursor ();
    }
}

void
text_move_line_down (screen_draw_function funcp)
{
  if (!IS_NEGATIVE (g_current_buffer_ptr->current_line->selstart_char))
    {
      line_disable_selection (g_current_buffer_ptr->current_line);
    }
  BOOL16 need_redraw = FALSE;
  RETURN_IF_TRUE (IS_NULL (g_current_buffer_ptr->current_line->next_line));
  if (!IS_NEGATIVE (g_current_buffer_ptr->current_line->selstart_char))
    {
      line_disable_selection (g_current_buffer_ptr->current_line);
    }
  if (g_current_buffer_ptr->current_line == g_current_buffer_ptr->page_bottom_line)
    {
      g_current_buffer_ptr->page_top_line = g_current_buffer_ptr->page_top_line->next_line;
      need_redraw = TRUE;
    }
  g_current_buffer_ptr->current_line = g_current_buffer_ptr->current_line->next_line;
  if (IS_TRUE (need_redraw))
    {
      CALL_FUNCTION (funcp);
    }
  else
    {
      _redraw_cursor ();
    }
}

void
text_move_char_frwd ()
{
  struct line_st *p = g_current_buffer_ptr->current_line;
  RETURN_IF_TRUE (line_is_null (p));
  RETURN_IF_TRUE (IS_TRUE (text_is_on_EOL ()));
  if (IS_TRUE (_need_frwd_hscroll ()))
    {
      text_scroll_frwd ();
      ui_draw_line (p);
    }
  else
    {
      p->cursor_char++;
    }
  _redraw_cursor ();
}

void
text_move_char_bkwd ()
{
  struct line_st *p = g_current_buffer_ptr->current_line;
  RETURN_IF_TRUE (line_is_null (p));
  RETURN_IF_TRUE (IS_TRUE (text_is_on_BOL ()));
  if (IS_TRUE (_need_bkwd_hscroll ()))
    {
      text_scroll_bkwd ();
      ui_draw_line (p);
    }
  else
    {
      p->cursor_char--;
    }
  _redraw_cursor ();
}

void
text_move_to_BOL (void)
{
  struct line_st *p = g_current_buffer_ptr->current_line;
  RETURN_IF_TRUE (line_is_null (p));
  while (!IS_TRUE (text_is_on_BOL ()))
    {
      text_move_char_bkwd ();
    }
}

void
text_move_to_EOL (void)
{
  struct line_st *p = g_current_buffer_ptr->current_line;
  RETURN_IF_TRUE (line_is_null (p));
  while (!IS_TRUE (text_is_on_EOL ()))
    {
      text_move_char_frwd ();
    }
}

void
text_scroll_page_up (const INT32 delta)
{
  screen_draw_function funcp = (screen_draw_function) ui_redraw_screen;
  RETURN_IF_TRUE (IS_NULL (g_current_buffer_ptr->page_top_line->prev_line));
  if (!IS_NEGATIVE (g_current_buffer_ptr->current_line->selstart_char))
    {
      line_disable_selection (g_current_buffer_ptr->current_line);
    }
  INT32 no = g_current_buffer_ptr->page_top_line->no;
  while (TRUE)
    {
      if (IS_NULL (buffer_get_line (g_current_buffer_ptr, no)->prev_line))
        {
          break;
        }
      if (g_current_buffer_ptr->page_top_line->no - no == delta)
        {
          break;
        }
      no--;
    }
  g_current_buffer_ptr->page_top_line = buffer_get_line (g_current_buffer_ptr, no);
  g_current_buffer_ptr->current_line = g_current_buffer_ptr->page_top_line;
  CALL_FUNCTION (funcp);
}

void
text_scroll_page_down (screen_draw_function funcp)
{
  RETURN_IF_TRUE (IS_NULL (g_current_buffer_ptr->page_bottom_line->next_line));
  if (!IS_NEGATIVE (g_current_buffer_ptr->current_line->selstart_char))
    {
      line_disable_selection (g_current_buffer_ptr->current_line);
    }
  g_current_buffer_ptr->page_top_line = g_current_buffer_ptr->page_bottom_line->next_line;
  g_current_buffer_ptr->current_line = g_current_buffer_ptr->page_top_line;
  CALL_FUNCTION (funcp);
}

void
text_to_first_page (screen_draw_function funcp)
{
  g_current_buffer_ptr->page_top_line = buffer_get_line (g_current_buffer_ptr, 0);
  g_current_buffer_ptr->current_line = g_current_buffer_ptr->page_top_line;
  if (!IS_NEGATIVE (g_current_buffer_ptr->current_line->selstart_char))
    {
      line_disable_selection (g_current_buffer_ptr->current_line);
    }
  CALL_FUNCTION (funcp);
}

void
text_mark_text (void)
{
  struct line_st *linep = g_current_buffer_ptr->current_line;
  RETURN_IF_TRUE (IS_NULL (linep));
  RETURN_IF_TRUE (line_is_null (linep));
  RETURN_IF_TRUE (!line_len (linep));
  if (!IS_NEGATIVE (linep->selstart_char) &&
      !IS_NEGATIVE (linep->selend_char))
    {
      if ((linep->selstart_char != linep->cursor_char) &&
          (linep->selend_char != linep->cursor_char))
        {
          goto end;
        }
    }
  if (!IS_NEGATIVE (linep->cursor_char))
    {
      if ((linep->selstart_char == linep->cursor_char) ||
          (linep->selend_char == linep->cursor_char))
        {
          line_disable_selection (linep);
          return;
        }
    }
  if (IS_NEGATIVE (linep->selstart_char))
    {
      linep->selstart_char = linep->cursor_char;
      linep->selend_char = -0x1;
      _text_redraw_line (linep);
      return;
    }
  else
    {
      if (linep->cursor_char <= linep->selstart_char)
        {
          line_disable_selection (linep);
          return;
        }
      linep->selend_char = linep->cursor_char;
      _text_redraw_line (linep);
    }
end:
  linep = NULL;
}

extern inline INT32
text_char_idx_to_x (const char_index_t) ALWAYS_INLINE_ATTRIB;

void
text_accept_printable_key (INT32 key)
{
  key = GET_FIRST_TWO_BYTES (key);
  struct line_st *p = g_current_buffer_ptr->current_line;
  INT32 len = conio_get_key_length (key);
  if ((len != 0x1) || (key <= 0x0))
    {
      flash ();
    }
  RETURN_IF_TRUE (len != 0x1);
  _mark_as_dirty (g_current_buffer_ptr);
  if (line_is_null (p))
    {
      line_append_char (p, (const CHAR8) key);
      p->cursor_char = 0x1;
    }
  else
    {
      if (IS_TRUE (text_is_on_EOL ()))
        {
          line_append_char (p, (const CHAR8) key);
        }
      else
        {
          line_insert_char (p, p->cursor_char, (const CHAR8) key);
        }
      if (IS_TRUE (_need_frwd_hscroll ()))
        {
          text_scroll_frwd ();
        }
      else
        {
          p->cursor_char++;
        }
    }
  _text_redraw_line (p);
}

void
text_accept_tab_key (void)
{
  INT32 i; // used by _text_insert_spaces!!!
  INT32 colno = g_current_buffer_ptr->current_line->cursor_char + 0x1;
  INT32 tabstop = (INT32) g_tab_stop;
  INT32 nspaces = CALC_NSPACES (tabstop, colno);
  _text_insert_spaces (nspaces);
}

void
text_accept_return_key (void)
{
  screen_draw_function funcp = (screen_draw_function) ui_redraw_screen;
  struct line_st *linep = g_current_buffer_ptr->current_line;
  _save_current_page_state ();
  if (line_is_null (linep))
    {
      buffer_insert_line (g_current_buffer_ptr, linep->no, line_new ());
      goto end;
    }
  if (!linep->cursor_char)
    {
      buffer_insert_line (g_current_buffer_ptr, linep->no, line_new ());
      goto end;
    }
  if (IS_TRUE (text_is_on_EOL ()))
    {
      if (!linep->next_line)
        {
          buffer_append_line (g_current_buffer_ptr, line_new ());
        }
      else
        {
          buffer_insert_line (g_current_buffer_ptr, linep->next_line->no, line_new ());
        }
      goto end;
    }
  else
    {
      _break_line ();
    }
end:
  _restore_current_page_state ();
  CALL_FUNCTION (funcp);
  text_move_line_down (funcp);
  if (g_auto_indent)
    {
      _indent_line (funcp);
    }
  _mark_as_dirty (g_current_buffer_ptr);
}

void
text_accept_backspace_key (void)
{
  struct line_st *p = g_current_buffer_ptr->current_line;
  RETURN_IF_TRUE (line_is_null (p));
  RETURN_IF_TRUE (IS_TRUE (text_is_on_BOL ()));
  text_move_char_bkwd ();
  text_accept_delete_key ();
}

void
text_accept_delete_key (void)
{
  if (_need_bkwd_hscroll ())
    {
      text_scroll_bkwd ();
    }
  screen_draw_function funcp = (screen_draw_function) ui_redraw_screen;
  struct line_st *linep = g_current_buffer_ptr->current_line;
  if (line_is_null (linep))
    {
      if (linep->next_line)
        {
          text_drop_line (funcp);
        }
      else
        {
          flash ();
        }
      return;
    }
  if (IS_TRUE (text_is_on_EOL ()))
    {
      if (!IS_NULL (linep->next_line))
        {
          if (line_is_null (linep->next_line))
            {
              text_move_line_down (NULL);
              text_drop_line (funcp);
            }
          else
            {
              _merge_lines (funcp);
            }
        }
      return;
    }
  line_drop_char (linep, linep->cursor_char);
  if (line_is_null (linep))
    {
      linep->first_visible_char = 0x0;
      linep->cursor_char = 0x0;
    }
  else
    {
      if (linep->first_visible_char >= line_len (linep))
        {
          linep->first_visible_char = 0x0;
        }
    }
  _mark_as_dirty (g_current_buffer_ptr);
  line_disable_selection (linep);
}

void
text_drop_chars (void)
{
  struct line_st *linep = g_current_buffer_ptr->current_line;
  RETURN_IF_TRUE (line_is_null (linep));
  RETURN_IF_TRUE (IS_TRUE (text_is_on_EOL ()));
  INT32 len = linep->cursor_char;
  str_realloc (&linep->text, len);
  if (linep->first_visible_char < line_len (linep))
    {
      linep->first_visible_char = 0x0;
    }
  _mark_as_dirty (g_current_buffer_ptr);
  _text_redraw_line (linep);
}

void
text_drop_line (screen_draw_function funcp)
{
  RETURN_IF_TRUE (g_current_buffer_ptr->line_count <= 0x1);
  _save_current_page_state ();
  buffer_drop_line (g_current_buffer_ptr, g_current_line_no);
  if (g_current_line_no > g_current_buffer_ptr->line_count - 0x1)
    {
      g_current_line_no = g_current_buffer_ptr->line_count - 0x1;
    }
  if (g_top_line_no > g_current_buffer_ptr->line_count - 0x1)
    {
      g_top_line_no = g_current_buffer_ptr->line_count - 0x1;
    }
  _restore_current_page_state ();
  _mark_as_dirty (g_current_buffer_ptr);
  CALL_FUNCTION (funcp);
}

void
text_drop_lines (void)
{
  screen_draw_function funcp = (screen_draw_function) ui_redraw_screen;
  if (g_current_buffer_ptr->line_count > 0x1)
    {
      if (!IS_NULL (g_current_buffer_ptr->current_line->next_line))
        {
          text_move_line_down (NULL);
        }
      while (TRUE)
        {
          if (IS_EQUAL (g_current_buffer_ptr->line_count, 0x1))
            {
              break;
            }
          if (IS_NULL (g_current_buffer_ptr->current_line->next_line))
            {
              text_drop_line (NULL);
              break;
            }
          text_drop_line (NULL);
        }
    }
  CALL_FUNCTION (funcp);
}

void
text_copy_chars (void)
{
  struct line_st *linep = g_current_buffer_ptr->current_line;
  RETURN_IF_TRUE (IS_NULL (linep));
  RETURN_IF_TRUE (line_is_null (linep));
  RETURN_IF_TRUE (!line_len (linep));
  if (!IS_NEGATIVE (linep->selstart_char) &&
      !IS_NEGATIVE (linep->selend_char))
    {
      if (!IS_NULL (g_text_clipboard))
        {
          mem_free (g_text_clipboard);
          g_text_clipboard = NULL;
        }
      register CHAR8 *sp = linep->text + linep->selstart_char;
      CHAR8 c;
      INT32 len = 0x0;
      while (*sp)
        {
          c = *sp++;
          if (!c)
            {
              break;
            }
          if (len > linep->selend_char - linep->selstart_char)
            {
              break;
            }
          len++;
          if (IS_NULL (g_text_clipboard))
            {
              str_alloc (&g_text_clipboard, len);
            }
          else
            {
              str_realloc (&g_text_clipboard, len);
            }
          *(g_text_clipboard + len - 0x1) = c;
        }
    }
}

void
text_yank_chars (void)
{
  RETURN_IF_TRUE (IS_NULL (g_text_clipboard));
  INT32 pos = g_current_buffer_ptr->current_line->cursor_char;
  INT32 tmp = pos;
  CHAR8 c;
  CHAR8 *txt = g_text_clipboard;
  while (*txt)
    {
      c = *txt++;
      if (line_is_null (g_current_buffer_ptr->current_line))
        {
          line_append_char (g_current_buffer_ptr->current_line, c);
        }
      else
        {
          line_insert_char (g_current_buffer_ptr->current_line, pos, c);
        }
      pos++;
    }
  g_current_buffer_ptr->current_line->cursor_char = tmp;
  _mark_as_dirty (g_current_buffer_ptr);
  _text_redraw_line (g_current_buffer_ptr->current_line);
}

void
text_yank_lines (struct buffer_st *srcbfrp)
{
  screen_draw_function funcp = (screen_draw_function) ui_redraw_screen;
  struct buffer_st *destbfrp = g_current_buffer_ptr;
  RETURN_IF_TRUE (!srcbfrp->line_count);
  struct line_st *linep, *newlinep;
  INT32 pos = destbfrp->current_line->no;
  for (linep = srcbfrp->first_line; !IS_NULL (linep); linep = linep->next_line)
    {
      newlinep = line_new ();
      CHAR8 *strp = linep->text;
      line_append_str (newlinep, strp);
      buffer_insert_line (destbfrp, pos, newlinep);
      pos++;
      text_move_line_up ();
    }
  _mark_as_dirty (destbfrp);
  CALL_FUNCTION (funcp);
}

void
text_locate_by_str (void)
{
  INT32 no = -0x1;
  if (IS_NULL (g_search_str))
    {
      g_search_str = "srch";
    }
  if (dlg_input_dlg ("Search: ", g_search_str))
    {
      INT32 inputlen = STRING_LEN (g_user_input);
      str_alloc (&g_search_str, inputlen);
      strcpy (g_search_str, g_user_input);
      struct line_st *linep = NULL;
      char_index_t *posp = (char_index_t *) mem_alloc (sizeof (char_index_t));
      char_index_t *lenp = (char_index_t *) mem_alloc (sizeof (char_index_t));
      for (linep = g_current_buffer_ptr->current_line; !IS_NULL (linep); linep =
              linep->next_line)
        {
          if (line_strstr (linep, g_search_str, posp, lenp))
            {
              no = linep->no;
              linep->selstart_char = *posp;
              linep->selend_char = *lenp;
              _text_redraw_line (linep);
              break;
            }
        }
      if (!IS_EQUAL (no, -0x1))
        {
          text_locate_by_no (no);
        }
      else
        {
          dlg_msgbox (errtype_to_str (es_locate_line_failed));
        }
    }
}

void
text_locate_by_no (const INT32 line_no)
{
  screen_draw_function funcp = (screen_draw_function) ui_redraw_screen;
  if (IS_TRUE (_is_in_current_page (line_no)))
    {
      goto end;
    }
  else
    {
      text_to_first_page (funcp);
      while (TRUE)
        {
          if ((line_no >= g_current_buffer_ptr->page_top_line->no)
              && (line_no <= g_current_buffer_ptr->page_bottom_line->no))
            {
              break;
            }
          text_scroll_page_down (funcp);
        }
    }
end:
  g_current_buffer_ptr->current_line = buffer_get_line (g_current_buffer_ptr, line_no);
  CALL_FUNCTION (funcp);
}

void
text_read_config_file (void)
{
  (void) config_read_option ("autoindent", &g_auto_indent);
  (void) config_read_option ("tabstop", &g_tab_stop);
}

void
ui_create_text_area (void)
{
  g_text_area = conio_get_stdscr (TEXT_AREA_COLOR_PAIR);
}

void
ui_redraw_screen (void)
{
  _text_draw_text_area ();
  ui_redraw_title_bar ();
  ui_redraw_hint_bar ();
  ui_redraw_status_bar ();
  ui_draw_cursor ();
  conio_refresh_window (g_text_area);
}

static inline BOOL16
_need_frwd_hscroll (void)
{
  BOOL16 rc = (IS_TRUE (text_is_on_right_edge ()) && !IS_TRUE (text_is_on_EOL ())) ? TRUE : FALSE;
  return rc;
}

static inline BOOL16
_need_bkwd_hscroll (void)
{
  struct line_st *p = g_current_buffer_ptr->current_line;
  BOOL16 rc = ((IS_TRUE (text_is_on_left_edge ())) && (p->first_visible_char > 0x0)) ? TRUE : FALSE;
  return rc;
}

static void
_prepend_indent (struct line_st *linep, INT32 *i)
{
  CHAR8 c;
  for (*i = 0x0; *i < line_len (linep->prev_line); (*i)++)
    {
      c = *(linep->prev_line->text + (*i));
      if (!IS_EQUAL (c, SPACE_CHAR))
        {
          break;
        }
      line_prepend_char (linep, c);
    }
}

static void
_indent_line (screen_draw_function drawfunc)
{
  struct buffer_st *bfrp = g_current_buffer_ptr;
  struct line_st *linep = g_current_buffer_ptr->current_line;
  RETURN_IF_TRUE (IS_NULL (linep->prev_line));
  RETURN_IF_TRUE (line_is_null (linep->prev_line));
  INT32 i;
  _prepend_indent (linep, &i);
  linep->cursor_char = i;
  _mark_as_dirty (bfrp);
  CALL_FUNCTION (drawfunc);
}

static void
_break_line (void)
{
  struct buffer_st *bfrp = g_current_buffer_ptr;
  struct line_st *linep = line_new ();
  INT32 i;
  CHAR8 ch;
  for (i = bfrp->current_line->cursor_char; i < line_len (bfrp->current_line); i++)
    {
      ch = *(bfrp->current_line->text + i);
      line_append_char (linep, ch);
    }
  INT32 len = bfrp->current_line->cursor_char;
  str_realloc (&bfrp->current_line->text, len);
  if (bfrp->current_line->first_visible_char >= line_len (bfrp->current_line))
    {
      bfrp->current_line->first_visible_char = 0x0;
    }
  if (IS_NULL (bfrp->current_line->next_line))
    {
      buffer_append_line (bfrp, linep);
    }
  else
    {
      buffer_insert_line (bfrp, bfrp->current_line->next_line->no, linep);
    }
}

static void
_merge_lines (screen_draw_function drawfunc)
{
  register CHAR8 *s = g_current_buffer_ptr->current_line->next_line->text;
  line_append_str (g_current_buffer_ptr->current_line, s);
  text_move_line_down (NULL);
  text_drop_line (NULL);
  text_move_line_up ();
  s = NULL;
  _mark_as_dirty (g_current_buffer_ptr);
  CALL_FUNCTION (drawfunc);
}

static BOOL16
_is_in_current_page (const INT32 line_no)
{
  struct buffer_st *p = g_current_buffer_ptr;
  RETURN_VAL_IF_TRUE (IS_NULL (p), FALSE);
  BOOL16 rc = ((line_no >= p->page_top_line->no) && (line_no <= p->page_bottom_line->no)) ? TRUE : FALSE;
  return rc;
}

static inline void
_save_current_page_state (void)
{
  g_current_line_no = g_current_buffer_ptr->current_line->no;
  g_top_line_no = g_current_buffer_ptr->page_top_line->no;
}

static inline void
_restore_current_page_state (void)
{
  g_current_buffer_ptr->page_top_line = buffer_get_line (g_current_buffer_ptr, g_top_line_no);
  g_current_buffer_ptr->current_line = buffer_get_line (g_current_buffer_ptr, g_current_line_no);
}

static void
_text_draw_text_area (void)
{
  conio_empty_window (g_text_area, FRAME_TOP_LEFT_Y,
                      ui_get_hint_bar_y () - 1,
                      FRAME_TOP_LEFT_X, FRAME_COLS_COUNT ());
  struct line_st *p = g_current_buffer_ptr->page_top_line;
  g_current_buffer_ptr->page_bottom_line = p;
  INT32 y = ui_lineno_to_y (p->no);
  while (TRUE)
    {
      ui_draw_line (p);
      if (IS_NULL (p->next_line))
        {
          break;
        }
      p = p->next_line;
      y = ui_lineno_to_y (p->no);
      if (y == ui_get_hint_bar_y ())
        {
          break;
        }
      g_current_buffer_ptr->page_bottom_line = p;
    }
}