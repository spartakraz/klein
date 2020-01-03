/************************************************************
 * PROJECT: KLEIN
 * FILE:    UI.C
 * PURPOSE:
 * VERSION: 0.8.5
 *
 ************************************************************/

#include "library.h"

#define dlg_redraw_prompt_bar(prompt_)                  \
MACRO_START                                             \
    conio_print_str_on_window(g_prompt_bar, (prompt_),  \
                                0x0,                    \
                                0x0,                    \
                                STRING_LEN((prompt_)),  \
                                -0x1, -0x1);            \
    conio_refresh_window(g_prompt_bar);                 \
MACRO_END

#define dlg_redraw_input_bar()                          \
MACRO_START                                             \
    conio_print_str_on_window(g_input_bar,              \
    g_user_input,                                       \
    0x0,                                                \
    0x0,                                                \
    BUFSIZ,                                             \
    -0x1, -0x1);                                        \
    conio_refresh_window(g_input_bar);                  \
MACRO_END

#define dlg_remove_prompt_bar()                         \
MACRO_START                                             \
    if (!IS_NULL(g_prompt_bar))                         \
        {                                               \
            conio_delete_window(g_prompt_bar);          \
            g_prompt_bar = NULL;                        \
        }                                               \
MACRO_END

#define dlg_remove_input_bar()                          \
MACRO_START                                             \
    if (!IS_NULL(g_input_bar))                          \
        {                                               \
            conio_delete_window(g_input_bar);           \
            g_input_bar = NULL;                         \
        }                                               \
MACRO_END

#define _dispose_bar(bar)                               \
MACRO_START                                             \
    if (!IS_NULL(bar))                                  \
        {                                               \
            conio_delete_window(bar);                   \
            bar = NULL;                                 \
        }                                               \
MACRO_END

extern inline INT32
ui_get_text_area_y (void) ALWAYS_INLINE_ATTRIB;
extern inline INT32
ui_get_title_bar_y (void) ALWAYS_INLINE_ATTRIB;
extern inline INT32
ui_get_input_bar_y (void) ALWAYS_INLINE_ATTRIB;
extern inline INT32
ui_get_status_bar_y (void) ALWAYS_INLINE_ATTRIB;
extern inline INT32
ui_get_hint_bar_y (void) ALWAYS_INLINE_ATTRIB;
extern inline INT32
ui_lineno_to_y (const INT32) ALWAYS_INLINE_ATTRIB;
extern inline void
ui_draw_line (struct line_st *) ALWAYS_INLINE_ATTRIB;
extern inline void
ui_draw_cursor (void) ALWAYS_INLINE_ATTRIB;

void
ui_create_title_bar (void)
{
  _dispose_bar (g_title_bar);
  g_title_bar = conio_create_subwindow (g_text_area,
                                        0x1,
                                        FRAME_COLS_COUNT (),
                                        ui_get_title_bar_y (),
                                        FRAME_TOP_LEFT_X);
  SET_WINDOW_BKGD (g_title_bar, COLOR_PAIR (TITLE_BAR_COLOR_PAIR));
}

void
ui_create_status_bar (void)
{
  _dispose_bar (g_status_bar);
  g_status_bar = conio_create_subwindow (g_text_area, 
                                         0x1, 
                                         FRAME_COLS_COUNT (), 
                                         ui_get_status_bar_y (), 
                                         FRAME_TOP_LEFT_X);
  SET_WINDOW_BKGD (g_status_bar, COLOR_PAIR (STATUS_BAR_COLOR_PAIR));
}

void
ui_create_hint_bar (void)
{
  _dispose_bar (g_hint_bar);
  g_hint_bar = conio_create_subwindow (g_text_area, 
                                       0x1, 
                                       FRAME_COLS_COUNT (), 
                                       ui_get_hint_bar_y (), 
                                       FRAME_TOP_LEFT_X);
  SET_WINDOW_BKGD (g_hint_bar, COLOR_PAIR (HINT_BAR_COLOR_PAIR));
}

void
ui_redraw_title_bar (void)
{
  WINDOW * winp = g_title_bar;
  INT32 len = BUFSIZ;
  CHAR8 *s = NULL;
  str_alloc (&s, len);
  sprintf (s, "%s - [%s]", VERSION, g_current_buffer_ptr->file_name);
  conio_print_str_on_window (winp, s, 0x0, 0x0, len, -0x1, -0x1);
  conio_refresh_window (winp);
}

void
ui_redraw_status_bar (void)
{
  INT32 line_no = g_current_buffer_ptr->current_line->no + 0x1;
  CHAR8 *dirty = (g_current_buffer_ptr->is_dirty) ? "*" : "";
  CHAR8 *scrollsym = "";
  if ((!line_is_null (g_current_buffer_ptr->current_line))
      && (g_current_buffer_ptr->current_line->first_visible_char > 0x0))
    {
      scrollsym = "$";
    }
  INT32 len = BUFSIZ;
  CHAR8 *str = NULL;
  str_alloc (&str, len);
  sprintf (str,
           "%s | #%d %s%s | Line:%d-%d%s | Col:%d | Autoindent:%s | Tabstop:%d",
           VERSION,
           g_current_buffer_idx + 0x1,
           g_current_buffer_ptr->file_name,
           dirty,
           line_no,
           g_current_buffer_ptr->line_count,
           scrollsym,
           g_current_buffer_ptr->current_line->cursor_char + 0x1,
           IS_EQUAL (g_auto_indent, 0x1) ? "On" : "Off",
           g_tab_stop);
  conio_print_str_on_window (g_status_bar, str, 0x0, 0x0, len, -0x1, -0x1);
  conio_refresh_window (g_status_bar);
}

void
ui_redraw_hint_bar (void)
{
  WINDOW * winp = g_hint_bar;
  const CHAR8 *help = "F1-Help F2-Open F3-Save F4-SaveAll F5-List F6-Drop F7-Info F8-ScriptDir F9-Run F10-Quit";
  INT32 len = STRING_LEN (help);
  conio_print_str_on_window (winp, help, 0x0, 0x0, len, -0x1, -0x1);
  conio_refresh_window (winp);
}

BOOL16
dlg_input_dlg (const CHAR8 *prompt, const CHAR8 *definp)
{
  INT32 y = ui_get_input_bar_y ();
  screen_draw_function funcp = (screen_draw_function) ui_redraw_screen;
  INT32 dlen = STRING_LEN (definp);
  strcpy (g_user_input, definp);
  g_user_input[dlen] = 0x0;
  INT32 plen = STRING_LEN (prompt);
  g_prompt_bar = conio_create_window (0x1, plen + FRAME_TOP_LEFT_X, y, FRAME_TOP_LEFT_X);
  g_input_bar = conio_create_window (0x1, BUFSIZ, y, FRAME_TOP_LEFT_X + plen + 0x1);
  if (conio_has_colors ())
    {
      wbkgd (g_input_bar, COLOR_PAIR (INPUT_BAR_COLOR_PAIR));
      wbkgd (g_prompt_bar, COLOR_PAIR (PROMPT_BAR_COLOR_PAIR));
    }
  dlg_redraw_prompt_bar (prompt);
  dlg_redraw_input_bar ();
  INT32 key = (INT32) wgetch (g_input_bar);
  while (!IS_EQUAL (key, CMD_CANCEL))
    {
      if (IS_EQUAL (key, '\r'))
        { // User hits ENTER.
          if (!dlen)
            { // if user's input is empty
              flash (); // we just flash the screen and do nothing,
            }
          else
            {
              break; // otherwise user has entered sth and we quit the loop.
            }
        }
      else if (IS_EQUAL (key, CMD_DROP_PREV_CHAR))
        { // Well, user hits BACKSPACE.
          if (!dlen)
            { // Again, if his input is empty then there's nothing to delete so we just
              flash (); // flash the screen
            }
          else
            {
              dlen--; // Otherwise we just remove the last CHAR8 of the input
              g_user_input[dlen] = '\0'; // by pushing it out of the input array. Then we
              goto end;
              //  jump to where the input area gets redrawn and the
              // dialog starts waiting for another user's key.
            }
        }
      else if (conio_get_key_length (key) == 1)
        { // We are here when user presses sth printable.
          if (IS_EQUAL (dlen, BUFSIZ))
            { // First we check if his input data has already reached the available limit.
              flash (); // If yes we accept nothing and just flash the screen.
            }
          else
            { // Otherwise
              dlen++; // we increase the size of the input array
              g_user_input[dlen] = '\0'; // by shifting zero CHAR8 to the right
              g_user_input[dlen - 1] = (const CHAR8) key; // to create a space for storing the just pressed CHAR8.
              goto end;
              // Then again we jump to where the input area gets redrawn and the
              // dialog starts waiting for another user's key.
            }
        }
end:
      dlg_redraw_input_bar (); // Here we redraw the input area
      key = (INT32) wgetch (g_input_bar); // and wait for another key hit by user. If he presses ESC
      // we quit the loop.
    }
  dlg_remove_input_bar (); // it's time to physically remove the input area from the screen
  dlg_remove_prompt_bar (); // we do the same to the prompt area
  CALL_FUNCTION (funcp); // it's wise to redraw the whole screen after removing the dialogs
  BOOL16 rc = (IS_EQUAL (key, '\r')) ? TRUE : FALSE; // We return bvTrue to inform that the dialog was closed after user pressed ENTER.
  // If instead user cancelled with the ESC key, we return bvFalse
  return rc;
}

BOOL16
dlg_yesno_dlg (const CHAR8 *prompt, const CHAR8 *definp)
{
  BOOL16 rc = FALSE;
  INT32 dlgres = dlg_input_dlg (prompt, definp);
  while (TRUE)
    {
      if (!dlgres)
        {
          break;
        }
      if (IS_TRUE (STRING_COMPARE (g_user_input, "yes")))
        {
          rc = TRUE;
          break;
        }
      if (IS_TRUE (STRING_COMPARE (g_user_input, "no")))
        {
          break;
        }
      dlgres = dlg_input_dlg (prompt, definp);
    }
  return rc;
}

void
dlg_msgbox (const CHAR8 *msg)
{
  INT32 y = ui_get_input_bar_y ();
  screen_draw_function funcp = (screen_draw_function) ui_redraw_screen;
  g_prompt_bar = conio_create_window (0x1, STRING_LEN (msg), y, FRAME_TOP_LEFT_X);
  if (conio_has_colors ())
    {
      wbkgd (g_prompt_bar, COLOR_PAIR (INPUT_BAR_COLOR_PAIR));
    }
  dlg_redraw_prompt_bar (msg);
  CALL_FUNCTION (funcp);
}

void
dlg_remove_input (void)
{
  dlg_remove_prompt_bar ();
  dlg_remove_input_bar ();
}