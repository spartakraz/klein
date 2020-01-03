/************************************************************
 * PROJECT: KLEIN
 * FILE:    PROTO.H
 * PURPOSE: FUNCTION PROTOTYPES
 * VERSION: 0.8.5
 *
 ************************************************************/

#ifndef PROTO_H
#define PROTO_H

#include "typedefs.h"
#include "macros.h"

/// performs a mutliplication without using the * operator
inline int
math_qmult (INT32 num1, INT32 num2)
{
  INT32 retval = 0x0,
        count  = 0x0;
  while (num2)
    {
      if (IS_EQUAL (BIT_IS_SET(num2, 0x1), 0x1))
        {
          retval += LEFT_SHIFT_BITS(num1, count);
        }
      count++;
      num2 = RIGHT_SHIFT_BITS(num2, 1);
    }
  return retval;
}

/// performs division without using the / operator
inline INT32
math_qdiv (INT32 num1, INT32 num2)
{
  if (!num1)
    {
      return 0x0;
    }
  if (!num2)
    {
      return INT_MAX;
    }
  INT32 negres = FALSE;
  if (num1 < 0x0)
    {
      num1 = -num1;
      if (num2 < 0x0)
        {
          num2 = -num2;
        }
      else
        {
          negres = TRUE;
        }
    }
  else if (num2 < 0x0)
    {
      num2 = -num2;
      negres = TRUE;
    }
  INT32 q = 0x0;
  while (num1 >= num2)
    {
      num1 -= num2;
      q++;
    }
  if (negres)
    {
      q = -q;
    }
  return q;
}

/// finds a remainder without the % operator
inline INT32
math_qmod (INT32 num1, INT32 num2)
{
  INT32 q = math_qdiv (num1, num2);
  INT32 p = math_qmult (q, num2);
  return num1 - p;
}

/// In future versions this function will be used instead of math_qdiv()
/// to perform fast division without the use of DIV and IDIV. Right now
/// it is just a stub which does nothing useful yet
inline INT32
math_asm_div(int n1, int n2)
{

  int res = 0;
  int temp_edx = 0;

  asm (
      "div %3;"
      : "=a" (res),
        "+d" (temp_edx)
      : "0" (n1),
        "r" (n2)
      );
  return res;
}

/// wrapper around malloc())
extern void *
mem_alloc (const size_t);
/// wrapper around realloc())
extern void *
mem_realloc (void *, const size_t);
/// wrapper around free() returning TRUE if memory was freed successfully
extern BOOL16
mem_free (void *);

/// allocates memory for a string and returns TRUE on success
extern BOOL16
str_alloc (CHAR8 **, const size_t);
/// reallocates memory for a string and returns TRUE on success
extern BOOL16
str_realloc (CHAR8 **, const size_t);
/// returns TRUE if a string can be converted to number
extern BOOL16
str_is_numeric (const CHAR8 *);
/// returns the text description of an error symbol
extern const CHAR8 *
errtype_to_str (const error_symbol_e);

/// returns TRUE if a file exists
extern BOOL16
file_exists (const CHAR8 *);
/// reads the next character from a file and stores it in the second parameter
extern INT32
file_read_byte (INT32, CHAR8 *);
/// writes a character to a file
extern INT32
file_write_byte (INT32, CHAR8 *);
/// writes a string to a file and appends the new-line-character if the 3rd parameter
/// is set to TRUE
extern BOOL16
file_write_str (const INT32, const CHAR8 *, const INT32);
/// returns the size (in bytes) of a file
extern const size_t
file_get_size (const CHAR8 *name);
/// returns the last access time of a file
extern const CHAR8 *
file_get_atime (const CHAR8 *);
/// returns the last modification time of a file
extern const CHAR8 *
file_get_mtime (const CHAR8 *);
/// returns the owner of a file
extern const CHAR8 *
file_owner (const CHAR8 *);
/// returns TRUE if a file is a directory
extern BOOL16
file_is_dir (const CHAR8 *);
/// returns the user's home directory
extern const CHAR8 *
file_get_home_dir (void);
/// returns the current directory
extern const CHAR8 *
file_get_current_dir (void);

/// runs the command passed thru the 2nd param from the directory passed
/// thru the 1st param and returns TRUE on success
extern BOOL16
shell_run_cmd (const CHAR8 *, const CHAR8 *);

/// creates a new line
extern struct line_st *
line_new (void);
/// frees the memory allocated for a line
extern void
line_free (struct line_st *);
/// allocates memory for a line's text
extern void
line_text_alloc (struct line_st *, const INT32);
/// checks if a line's text is NULL
extern BOOL16
line_is_null (struct line_st *p);
/// returns the length of a line's text
extern INT32
line_len (struct line_st *p);
/// adds a character onto the beginning of a line
extern void
line_prepend_char (struct line_st *p, const CHAR8 c);
/// returns TRUE if a line contains specified string
extern BOOL16
line_strstr (struct line_st *p, CHAR8 *s, char_index_t *, char_index_t *);

/// creates a buffer
extern struct buffer_st
buffer_new (void);
/// resets a buffer's attributes to their defaults
extern void
buffer_reset (struct buffer_st *);
/// renumerates a buffer's lines
extern void
buffer_renum_lines (struct buffer_st *);
/// returns the line with given number in a buffer
extern struct line_st *
buffer_get_line (struct buffer_st *, const INT32);
/// adds a line onto the beginning of a buffer
extern void
buffer_prepend_line (struct buffer_st *, struct line_st *);
/// adds a line onto the end of a buffer
extern void
buffer_append_line (struct buffer_st *, struct line_st *);
/// inserts a line into the specified position in a buffer
extern void
buffer_insert_line (struct buffer_st *, const INT32, struct line_st *);
/// drops the line at the specified position in a buffer
extern void
buffer_drop_line (struct buffer_st *, const INT32);
/// clears a buffer of all lines
extern void
buffer_clear (struct buffer_st *);
/// loads a file to a buffer and returns TRUE on success
extern BOOL16
buffer_load_file (struct buffer_st *);
/// saves a buffer's file and returns TRUE on success
extern BOOL16
buffer_save_file (struct buffer_st *);

/// initializes the screen
extern void
conio_start (void);
/// checks if the screen supports color mode
extern BOOL16
conio_has_colors (void) CONST_ATTRIB;
/// starts color mode
extern void
conio_start_color (void);
/// initializes a color pair passed as the 1st argument with a foreground color
/// passed as the 2nd argument and a background color passed as the 3rd argument
extern void
conio_init_pair (const INT32, const INT32, const INT32);
/// Creates a curses window. The first two params are the number of lines and cols
/// respectively. The last two params are Y- and X- coordinates of the window's
/// upper left corner.
extern WINDOW *
conio_create_window (const INT32, const INT32, const INT32, const INT32);
/// Creates a subwindow of the given curses window. The first are two params the number of lines and cols
/// respectively. The last two params are Y- and X- coordinates of the window's
/// upper left corner.
extern WINDOW *
conio_create_subwindow (WINDOW *, const INT32, const INT32, const INT32, const INT32);
/// returns the pointer to the curses default window
extern WINDOW *
conio_get_stdscr (const INT32);
/// deletes a curses window
extern void
conio_delete_window (WINDOW *);
/// Imitates cleaning a window's area by filling it up with space characters. The first 
/// two params are the area's number of lines and cols respectively. And the last 
/// two params are Y- and X- coordinates of the area's upper left corner.
extern void
conio_empty_window (WINDOW *, 
                    const INT32, 
                    const INT32, 
                    const char_index_t, 
                    const char_index_t);
/// refreshes a window
extern void
conio_refresh_window (WINDOW * wp);
/// returns the length of a character string corresponding to the given key
extern INT32
conio_get_key_length (const INT32 key);
/// Outputs a substring of a string to the specified place within a window. The first three params
/// are the window, the string and the place's Y-coordinate. The 4th and 5th params are the indexes
/// of the characters at the start and the end of the substring respectively. If a part of the string
/// was selected by the CTRL^p shortcut then the last two params are the character indexes which indicate 
/// the start and the end of the selected part. If there is no such selection at all then these params
/// hold -1.
extern void
conio_print_str_on_window (WINDOW *,
                           const CHAR8 *,
                           const INT32,
                           const char_index_t,
                           const char_index_t,
                           const char_index_t,
                           const char_index_t);

/// returns TRUE if the cursor stands at the beginning of the current line
extern BOOL16
text_is_on_BOL (void);
/// returns TRUE if the cursor stands at the end of the current line
extern BOOL16
text_is_on_EOL (void);
/// returns TRUE if the cursor stands at the text area's left edge
extern BOOL16
text_is_on_left_edge (void);
/// returns TRUE if the cursor stands at the text area's right edge
extern BOOL16
text_is_on_right_edge (void);
/// moves the current line's cursor the text area's left edge
extern void
text_to_left_edge (void);
/// moves the current line's cursor the text area's right edge
extern void
text_to_right_edge (void);
/// scrolls the current line horizontally in the forward direction
extern void
text_scroll_frwd (void);
/// scrolls the current line horizontally in the backward direction
extern void
text_scroll_bkwd (void);
/// moves the cursor one line up in the text area
extern void
text_move_line_up (void);
/// moves the cursor one line down in the text area
extern void
text_move_line_down (screen_draw_function);
/// moves the cursor one character forward in the text area
extern void
text_move_char_frwd (void);
/// moves the cursor one character backward in the text area
extern void
text_move_char_bkwd (void);
/// moves the cursor to the beginning of the current line
extern void
text_move_to_BOL (void);
/// moves the cursor to the end of the current line
extern void
text_move_to_EOL (void);
/// scrolls the text area's contents one page up
extern void
text_scroll_page_up (const INT32);
/// scrolls the text area's contents one page down
extern void
text_scroll_page_down (screen_draw_function);
/// scrolls the text area's contents to the very first page
extern void
text_to_first_page (screen_draw_function);
extern void
text_mark_text(void);
/// returns the text area's X-coordinate where a line's character with given index
/// is to appear
inline INT32
text_char_idx_to_x (const char_index_t char_idx)
{
  RETURN_VAL_IF_TRUE (IS_NEGATIVE (char_idx), FRAME_TOP_LEFT_X);
  INT32 t1 = math_qdiv (char_idx, FRAME_COLS_COUNT ());
  register INT32 divisor = math_qmult (FRAME_COLS_COUNT (), t1);
  INT32 t2 = math_qmod (char_idx, FRAME_COLS_COUNT ());
  register INT32 x = !t2 ? 0x0 : char_idx - divisor;
  return FRAME_TOP_LEFT_X + GET_FIRST_TWO_BYTES (x);
}
/// the text area's reaction to hitting a printable key
extern void
text_accept_printable_key (INT32);
/// the text area's reaction to hitting the tab key
extern void
text_accept_tab_key (void);
/// the text area's reaction to hitting the RETURN key
extern void
text_accept_return_key (void);
/// the text area's reaction to hitting the backspace key
extern void
text_accept_backspace_key (void);
/// the text area's reaction to hitting the delete key
extern void
text_accept_delete_key (void);
/// deletes the current line's chars from the cursor position to the end of the line
extern void
text_drop_chars (void);
/// drops the current line
extern void
text_drop_line (screen_draw_function);
/// drops N consecutive lines standing below the current line
extern void
text_drop_lines (void);
/// copies a text from the current line to the text clipboard
extern void
text_copy_chars (void);
/// pastes text from the text clipboard
extern void
text_yank_chars (void);
/// pastes lines from the line clipboard
extern void
text_yank_lines (struct buffer_st *);
/// starting from the current line, search for the first line containing
/// a search text
extern void
text_locate_by_str (void);
/// Locates the line with given no
extern void
text_locate_by_no (const INT32);
/// use this function to read from the config file
extern void
text_read_config_file (void);

/// initializes the undo clipboard
extern void
undo_init (void);
/// sends a line to the undo clipboard
extern void
undo_set_line (struct line_st *);
/// sends a line stored in the undo clipboard back to the text area
extern BOOL16
undo_restore_line (struct buffer_st *);

/// returns the config file's path
extern CHAR8 *
config_get_path (void);
/// loads the config file and returns TRUE on success
extern BOOL16
config_load (void);
/// reads the value of an option from the config file and assigns it to the
/// 2nd param
extern INT32
config_read_option (const CHAR8 *, INT32 *);

/// returns the screen's Y-coordinate for the text area's top left corner
inline INT32
ui_get_text_area_y (void)
{
  return FRAME_TOP_LEFT_Y + 0x1;
}

/// returns the screen's Y-coordinate for the title bar's top left corner
inline INT32
ui_get_title_bar_y (void)
{
  return FRAME_TOP_LEFT_Y;
}

/// returns the screen's Y-coordinate for the input bar's top left corner
inline INT32
ui_get_input_bar_y (void)
{
  return (INT32) FRAME_LINES_COUNT () - 0x1;
}

/// returns the screen's Y-coordinate for the status bar's top left corner
inline INT32
ui_get_status_bar_y (void)
{
  return ui_get_input_bar_y () - 0x1;
}

/// returns the screen's Y-coordinate for the hint bar's top left corner
inline INT32
ui_get_hint_bar_y (void)
{
  return ui_get_input_bar_y () - 0x2;
}

/// returns the screen's Y-coordinate corresponding to a line with given no
inline INT32
ui_lineno_to_y (const INT32 lineno)
{
  return lineno - g_current_buffer_ptr->page_top_line->no + ui_get_text_area_y ();
}

/// draws a line on the screen
inline void
ui_draw_line (struct line_st *linep)
{
  conio_print_str_on_window (g_text_area,
                             linep->text,
                             ui_lineno_to_y (linep->no),
                             linep->first_visible_char,
                             linep->first_visible_char + +FRAME_COLS_COUNT () - 0x1,
                             linep->selstart_char,
                             linep->selend_char);
}

/// draws the cursor on a screen
inline void
ui_draw_cursor (void)
{
  INT32 y = ui_lineno_to_y (g_current_buffer_ptr->current_line->no);
  INT32 x = (!line_is_null (g_current_buffer_ptr->current_line)) ?
          text_char_idx_to_x (g_current_buffer_ptr->current_line->cursor_char) :
          FRAME_TOP_LEFT_X;
  (void) wmove (g_text_area, y, x);
}

/// creates the text area's window
extern void
ui_create_text_area (void);
/// creates the title bar's window
extern void
ui_create_title_bar (void);
/// creates the status bar's window
extern void
ui_create_status_bar (void);
/// creates the hint bar's window
extern void
ui_create_hint_bar (void);
/// redraws the title bar's window
extern void
ui_redraw_title_bar (void);
/// redraws the status bar's window
extern void
ui_redraw_status_bar (void);
/// redraws the hint bar's window
extern void
ui_redraw_hint_bar (void);
/// redraws the screen
extern void
ui_redraw_screen (void);

/// shows the input dialog with given prompt and default input values and
/// returns TRUE if the user closes the dialog hitting the RETURN key, FALSE
/// if the user cancels the dialog with the ESCAPE key
extern BOOL16
dlg_input_dlg (const CHAR8 *, const CHAR8 *);
/// shows the confirmation YES-NO dialog with given prompt and default input values and
/// returns TRUE if the user selects YES or FALSE if the user selects NO
extern BOOL16
dlg_yesno_dlg (const CHAR8 *, const CHAR8 *);
/// shows a simple message box
extern void
dlg_msgbox (const CHAR8 *);
/// removes the input dialog (if there is any) from the screen
extern void
dlg_remove_input (void);

#endif