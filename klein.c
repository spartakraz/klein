/************************************************************
 * PROJECT: KLEIN
 * FILE:    KLEIN.C
 * PURPOSE:
 * VERSION: 0.8.5
 *
 ************************************************************/

#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include "library.h"

#define _add_ctrl_key_handler(no_, command_, handler_)  \
MACRO_START                                             \
    g_ctrl_key_handlers[no_].command = (command_);      \
    g_ctrl_key_handlers[no_].handler = (handler_);      \
MACRO_END

#define _add_meta_key_handler(no_, command_, handler_)  \
MACRO_START                                             \
    g_meta_key_handlers[no_].command = (command_);      \
    g_meta_key_handlers[no_].handler = (handler_);      \
MACRO_END

#define _warn_numeric_input()                                   \
MACRO_START                                                     \
    if (!IS_TRUE(str_is_numeric(g_user_input)))                 \
        {                                                       \
            dlg_msgbox(errtype_to_str(es_bad_numeric_format));  \
            return;                                             \
        }                                                       \
MACRO_END

#define _warn_switch_buffer()                           \
MACRO_START                                             \
    if (g_buffers_count <= 1)                           \
        {                                               \
            flash();                                    \
            return;                                     \
        }                                               \
MACRO_END

typedef void
command_handler_func (void);

struct command_handler_st
{
  command_key_binding_e command;
  command_handler_func *handler;
};

struct buffer_st                    g_config_file;
struct buffer_st                    g_buffer_storage[MAX_BUFFERS_COUNT];
INT32                               g_buffers_count         = 0x0;
INT32                               g_current_buffer_idx    = -0x1;
struct buffer_st                    *g_current_buffer_ptr   = NULL;
CHAR8                               *g_text_clipboard       = NULL;
struct buffer_st                    g_line_clipboard;
struct buffer_st                    *g_line_clipboard_ptr   = NULL;
struct line_st                      g_undo_clipboard;
INT32                               g_auto_indent;
INT32                               g_tab_stop;
INT32                               g_top_line_no;
INT32                               g_current_line_no;
WINDOW                              *g_text_area            = NULL;
WINDOW                              *g_title_bar            = NULL;
WINDOW                              *g_status_bar           = NULL;
WINDOW                              *g_prompt_bar           = NULL;
WINDOW                              *g_input_bar            = NULL;
WINDOW                              *g_hint_bar             = NULL;
CHAR8                               g_user_input[BUFSIZ];
CHAR8                               *g_search_str           = NULL;

static struct script_st             g_script_info;
static struct command_handler_st    g_ctrl_key_handlers[0x1E];
static struct command_handler_st    g_meta_key_handlers[0x8];

static void
abort_klein (void) NORETURN_ATTRIB;
static void
atexit_callback (void);
static inline void
check_termsize (void);
static inline void
flash_screen (void);
static void
register_buffer (const CHAR8 *);
static void
activate_buffer (const INT32);
static void
get_buffer_info (CHAR8 **, buffer_time_option_e);
static void
switch_to_console_mode (void);
static void
switch_to_edit_mode (void);
static void
init_globals (void);
static void
init_window (void);
static void
uninit_window (void);
static command_handler_func *
get_ctrl_key_handler (const INT32);
static command_handler_func *
get_meta_key_handler (const INT32);
static void
on_sigwinch ();
static void
on_sigint (INT32);
static void
on_show_help (void);
static void
on_show_about_box ();
static void
on_open_file (void);
static void
on_save_buffer (void);
static void
on_save_buffers (void);
static void
on_run_script (void);
static void
on_set_script_dir (void);
static void
on_show_buffer_info (void);
static void
on_list_buffers (void);
static void
on_drop_buffer (void);
static void
on_move_line_up (void);
static void
on_move_line_down (void);
static void
on_move_char_frwd (void);
static void
on_move_char_bkwd (void);
static void
on_move_to_bol (void);
static void
on_move_to_eol (void);
static void
on_move_page_down (void);
static void
on_move_page_up (void);
static void
on_to_first_page (void);
static void
on_mark_text(void);
static void
on_insert_tab_char (void);
static void
on_insert_cr_char (void);
static void
on_drop_prev_char (void);
static void
on_drop_next_char (void);
static void
on_drop_chars (void);
static void
on_drop_line (void);
static void
on_drop_lines (void);
static void
on_undrop_line (void);
static void
on_copy_chars (void);
static void
on_copy_lines (void);
static void
on_yank_chars (void);
static void
on_yank_lines (void);
static void
on_locate_by_str (void);
static void
on_locate_by_no (void);
static void
on_to_prev_buffer (void);
static void
on_to_next_buffer (void);
static void
on_refresh_screen (void);
static void
event_loop (void);

int
main (int argc, char **argv)
{
  (void) atexit (atexit_callback);
  if (argc != 2)
    {
      file_write_str (STDERR_FILENO, errtype_to_str (es_bad_args), TRUE);
      perror ("Bad args!");
      exit (EXIT_FAILURE);
    }
  register CHAR8 opt;
  while ((opt = getopt (argc, argv, "vh")) != -1)
    {
      if (IS_EQUAL(opt, 'v'))
        {
          file_write_str (STDOUT_FILENO, VERSION, TRUE);
          exit (EXIT_SUCCESS);
        }
      else if (IS_EQUAL(opt, 'h'))
        {
          (void) shell_run_cmd (NULL, "bash -c 'clear'");
          on_show_help ();
          exit (EXIT_SUCCESS);
        }
      else
        {
          file_write_str (STDERR_FILENO, errtype_to_str (es_bad_args), TRUE);
          perror ("Bad args!");
          exit (EXIT_FAILURE);
        }
    }
  init_globals ();
  if (!config_load ())
    {
#if defined(DEBUG_MODE_ENABLED)
      perror ("Bad config!");
#endif
      g_auto_indent = 0;
      g_tab_stop = 8;
    }
  else
    {
      text_read_config_file ();
    }
  undo_init ();
  signal (SIGINT, on_sigint);
  g_line_clipboard = buffer_new ();
  g_line_clipboard_ptr = &g_line_clipboard;
  buffer_reset (g_line_clipboard_ptr);
  g_line_clipboard_ptr->file_name = NULL;
  init_window ();
  check_termsize ();
  register_buffer (argv[1]);
  event_loop ();
  uninit_window ();
  struct buffer_st *bfrp = NULL;
  for (INT32 i = 0; i < g_buffers_count; i++)
    {
      bfrp = &g_buffer_storage[i];
      if (bfrp->line_count)
        {
          buffer_clear (bfrp);
        }
    }
  (void) shell_run_cmd (NULL, "bash -c 'clear'");
#if defined(EXIT_SUCCESS)
  return EXIT_SUCCESS;
#else
  return 0;
#endif
}

static void
abort_klein (void)
{
  abort ();
}

static void
atexit_callback (void)
{
  fputs ("\n\nExiting Klein\n\n", stderr);
}

static inline void
check_termsize (void)
{
  if ((FRAME_LINES_COUNT () < 25) || (FRAME_COLS_COUNT () < 80))
    {
      DIEF ("%s\n", "Bad screen size");
    }
}

static inline void
flash_screen (void)
{
  flash ();
}

static void
register_buffer (const CHAR8 *filename)
{
  INT32 found = file_exists (filename);
  g_buffers_count++;
  g_current_buffer_idx = g_buffers_count - 1;
  struct buffer_st newbf = buffer_new ();
  buffer_reset (&newbf);
  INT32 len = STRING_LEN (filename);
  str_alloc (&newbf.file_name, len);
  strncpy (newbf.file_name, filename, len);
  if (!IS_TRUE (found))
    {
      buffer_append_line (&newbf, line_new ());
      buffer_renum_lines (&newbf);
    }
  g_buffer_storage[g_current_buffer_idx] = newbf;
  g_current_buffer_ptr = &g_buffer_storage[g_current_buffer_idx];
  if (IS_TRUE (found))
    {
      if (!IS_TRUE (buffer_load_file (g_current_buffer_ptr)))
        {
          fprintf (stderr, "%s\n", errtype_to_str (es_bad_buffer));
          exit (EXIT_FAILURE);
        }
    }
  g_current_buffer_ptr->page_top_line = buffer_get_line (g_current_buffer_ptr, 0);
  g_current_buffer_ptr->current_line = g_current_buffer_ptr->page_top_line;
  ui_redraw_screen ();
}

static void
activate_buffer (const INT32 bfridx)
{
  g_current_buffer_idx = bfridx;
  g_current_buffer_ptr = &g_buffer_storage[g_current_buffer_idx];
  ui_redraw_screen ();
}

static void
get_buffer_info (CHAR8 **info, buffer_time_option_e flags)
{
  INT32 len = BUFSIZ;
  *info = NULL;
  str_alloc (&(*info), len);
  (void) sprintf (*info,
                  "(size: %d bytes)  (owner: %s) %s %s",
                  (INT32) file_get_size (g_current_buffer_ptr->file_name),
                  file_owner (g_current_buffer_ptr->file_name),
                  BIT_IS_SET (flags, bto_show_atime) ?
                  file_get_atime (g_current_buffer_ptr->file_name) : "",
                  BIT_IS_SET (flags, bto_show_mtime) ?
                  file_get_mtime (g_current_buffer_ptr->file_name) : "");
  dlg_msgbox (*info);
}

static void
switch_to_console_mode (void)
{
  (void) endwin ();
  (void) shell_run_cmd (NULL, "bash -c 'clear'");
}

static inline void
switch_to_edit_mode (void)
{
  CHAR8 *msg = "Press ENTER to return to KLEIN...\n";
  file_write_str (STDOUT_FILENO, msg, FALSE);
  (void) wgetch (g_text_area);
  conio_refresh_window (g_text_area);
}

static void
init_globals (void)
{
  _add_ctrl_key_handler (0,  CMD_SHOW_HELP,          &on_show_about_box  );
  _add_ctrl_key_handler (1,  CMD_OPEN_FILE,          &on_open_file       );
  _add_ctrl_key_handler (2,  CMD_SAVE_FILE,          &on_save_buffer     );
  _add_ctrl_key_handler (3,  CMD_SAVE_ALL_FILES,     &on_save_buffers    );
  _add_ctrl_key_handler (4,  CMD_RUN_SCRIPT,         &on_run_script      );
  _add_ctrl_key_handler (5,  CMD_SET_SCRIPT_DIR,     &on_set_script_dir  );
  _add_ctrl_key_handler (6,  CMD_SHOW_FILE_INFO,     &on_show_buffer_info);
  _add_ctrl_key_handler (7,  CMD_LIST_OPEN_FILES,    &on_list_buffers    );
  _add_ctrl_key_handler (8,  CMD_DROP_BUFFER,        &on_drop_buffer     );
  _add_ctrl_key_handler (9,  CMD_MOVE_LINE_UP,       &on_move_line_up    );
  _add_ctrl_key_handler (10, CMD_MOVE_LINE_DOWN,     &on_move_line_down  );
  _add_ctrl_key_handler (11, CMD_MOVE_CHAR_FRWD,     &on_move_char_frwd  );
  _add_ctrl_key_handler (12, CMD_MOVE_CHAR_BKWD,     &on_move_char_bkwd  );
  _add_ctrl_key_handler (13, CMD_MOVE_TO_BOL,        &on_move_to_bol     );
  _add_ctrl_key_handler (14, CMD_MOVE_TO_EOL,        &on_move_to_eol     );
  _add_ctrl_key_handler (15, CMD_SCROLL_PAGE_DOWN,   &on_move_page_down  );
  _add_ctrl_key_handler (16, CMD_SCROLL_PAGE_UP,     &on_move_page_up    );
  _add_ctrl_key_handler (17, CMD_TO_FIRST_PAGE,      &on_to_first_page   );
  _add_ctrl_key_handler (18, CMD_MARK_LINE_TEXT,          &on_mark_text       );
  _add_ctrl_key_handler (19, CMD_INSERT_TAB_CHAR,    &on_insert_tab_char );
  _add_ctrl_key_handler (20, CMD_INSERT_CR_CHAR,     &on_insert_cr_char  );
  _add_ctrl_key_handler (21, CMD_DROP_PREV_CHAR,     &on_drop_prev_char  );
  _add_ctrl_key_handler (22, CMD_DROP_NEXT_CHAR,     &on_drop_next_char  );
  _add_ctrl_key_handler (23, CMD_DROP_LINE,          &on_drop_line       );
  _add_ctrl_key_handler (24, CMD_UNDROP_LINE,        &on_undrop_line     );
  _add_ctrl_key_handler (25, CMD_COPY_CHARS,         &on_copy_chars      );
  _add_ctrl_key_handler (26, CMD_YANK_CHARS,         &on_yank_chars      );
  _add_ctrl_key_handler (27, CMD_LOCATE_BY_STR,      &on_locate_by_str   );
  _add_ctrl_key_handler (28, CMD_LOCATE_BY_NO,       &on_locate_by_no    );
  _add_ctrl_key_handler (29, 0,                      NULL                );
  _add_meta_key_handler (0,  CMD_DROP_CHARS,         &on_drop_chars      );
  _add_meta_key_handler (1,  CMD_DROP_LINES,         &on_drop_lines      );
  _add_meta_key_handler (2,  CMD_COPY_LINES,         &on_copy_lines      );
  _add_meta_key_handler (3,  CMD_YANK_LINES,         &on_yank_lines      );
  _add_meta_key_handler (4,  CMD_TO_NEXT_BUFFER,     &on_to_next_buffer  );
  _add_meta_key_handler (5,  CMD_TO_PREV_BUFFER,     &on_to_prev_buffer  );
  _add_meta_key_handler (6,  CMD_REFRESH_SCREEN,     &on_refresh_screen  );
  _add_meta_key_handler (7,  0,                      NULL                );
  g_script_info.work_dir    = NULL;
  g_script_info.base_name   = NULL;
  g_current_line_no         = 0;
  g_top_line_no             = 0;
}

static void
init_window (void)
{
  conio_start ();
  if (conio_has_colors ())
    {
      conio_start_color ();
      conio_init_pair (TEXT_AREA_COLOR_PAIR,    COLOR_WHITE, COLOR_BLACK);
      conio_init_pair (INPUT_BAR_COLOR_PAIR,    COLOR_WHITE, COLOR_BLACK);
      conio_init_pair (STATUS_BAR_COLOR_PAIR,   COLOR_BLACK, COLOR_WHITE);
      conio_init_pair (TITLE_BAR_COLOR_PAIR,    COLOR_BLACK, COLOR_WHITE);
      conio_init_pair (HINT_BAR_COLOR_PAIR,     COLOR_BLACK, COLOR_WHITE);
      conio_init_pair (ABOUT_BOX_COLOR_PAIR,    COLOR_BLACK, COLOR_WHITE);
      conio_init_pair (PROMPT_BAR_COLOR_PAIR,   COLOR_BLACK, COLOR_WHITE);
    }
  ui_create_text_area ();
  ui_create_status_bar ();
  ui_create_title_bar ();
  ui_create_hint_bar ();
}

static void
uninit_window (void)
{
  delwin (g_status_bar);
  delwin (g_title_bar);
  delwin (g_hint_bar);
  (void) endwin ();
}

static command_handler_func *
get_ctrl_key_handler (const INT32 command)
{
  INT32 i;
  for (i = 0; !IS_NULL (g_ctrl_key_handlers[i].handler); i++)
    {
      if (g_ctrl_key_handlers[i].command == command)
        {
          return g_ctrl_key_handlers[i].handler;
        }
    }
  return NULL;
}

static command_handler_func *
get_meta_key_handler (const INT32 command)
{
  INT32 i;
  for (i = 0; !IS_NULL (g_meta_key_handlers[i].handler); i++)
    {
      if (g_meta_key_handlers[i].command == command)
        {
          return g_meta_key_handlers[i].handler;
        }
    }
  return NULL;
}

static void
on_sigwinch ()
{
  text_to_first_page (NULL);
  struct line_st *linep;
  struct buffer_st *bfrp = g_current_buffer_ptr;
  for (linep = bfrp->first_line; !IS_NULL (linep); linep = linep->next_line)
    {
      linep->cursor_char = 0;
    }
  (void) endwin ();
  init_window ();
  ui_redraw_screen ();
}

static void
on_sigint (INT32 arg)
{
  flash_screen ();
}

static void
on_show_help (void)
{
  if (!IS_NULL (g_text_area))
    {
      switch_to_console_mode ();
    }
  INT32 i;
  INT32 nitems = HELP_ITEMS_COUNT;
  CHAR8 *s;
  for (i = 0; i < nitems; i++)
    {
      s = GET_HELP_ITEM (i);
      file_write_str (STDOUT_FILENO, s, TRUE);
      if (i == RIGHT_SHIFT_BITS(nitems, 1))
        {
          file_write_str (STDOUT_FILENO, "Press ENTER to get to the next page", TRUE);
          getchar ();
          (void) shell_run_cmd (NULL, "bash -c 'clear'");
        }
    }
  if (!IS_NULL (g_text_area))
    {
      switch_to_edit_mode ();
    }
}

static void
on_show_about_box ()
{
  curs_set (0x0);
  WINDOW *winp = newwin (ABOUT_BOX_LINES_COUNT,
                         ABOUT_BOX_COLS_COUNT,
                         0x1,
                         math_asm_div(FRAME_COLS_COUNT (), 0x2) - math_asm_div(ABOUT_BOX_COLS_COUNT, 0x2));
  SET_WINDOW_BKGD (winp, COLOR_PAIR (ABOUT_BOX_COLOR_PAIR));
  box (winp, ABOUT_BOX_BORDER_CHAR, ABOUT_BOX_BORDER_CHAR);
  mvwprintw (winp,
             math_asm_div(ABOUT_BOX_LINES_COUNT, 0x2),
             math_asm_div(ABOUT_BOX_COLS_COUNT, 0x2) - (math_asm_div(STRING_LEN (VERSION), 0x2)),
             "%s",
             VERSION);
  wrefresh (winp);
  sleep (0x2);
  conio_delete_window (winp);
  curs_set (0x1);
  touchwin (stdscr);
  wrefresh (stdscr);
}

static void
on_open_file (void)
{
  if (g_buffers_count == MAX_BUFFERS_COUNT)
    {
      dlg_msgbox (errtype_to_str (es_bad_buffer));
      return;
    }
  if (dlg_input_dlg ("Open File: ", g_current_buffer_ptr->file_name))
    {
      struct buffer_st *bfrp = NULL;
      INT32 i;
      for (i = 0; i < g_buffers_count; i++)
        {
          bfrp = &g_buffer_storage[i];
          if (strcmp (bfrp->file_name, g_user_input) == 0)
            {
              dlg_msgbox (errtype_to_str (es_bad_buffer));
              return;
            }
        }
      register_buffer (g_user_input);
    }
}

static void
on_save_buffer (void)
{
  if (dlg_input_dlg ("Save As: ", g_current_buffer_ptr->file_name))
    {
      INT32 len = STRING_LEN (g_user_input);
      str_alloc (&g_current_buffer_ptr->file_name, len);
      strncpy (g_current_buffer_ptr->file_name, g_user_input, len);
      if (!IS_TRUE (buffer_save_file (g_current_buffer_ptr)))
        {
          dlg_msgbox (errtype_to_str (es_save_failed));
        }
      else
        {
          ui_redraw_screen ();
        }
    }
}

static void
on_save_buffers (void)
{
  struct buffer_st *bfrp = NULL;
  INT32 i;
  for (i = 0; i < g_buffers_count; i++)
    {
      bfrp = &g_buffer_storage[i];
      if (!IS_TRUE (buffer_save_file (bfrp)))
        {
          dlg_msgbox (errtype_to_str (es_save_failed));
        }
    }
  ui_redraw_screen ();
}

static void
on_set_script_dir (void)
{
  if (IS_NULL (g_script_info.work_dir))
    {
      g_script_info.work_dir = (CHAR8 *) file_get_current_dir ();
    }
  if (dlg_input_dlg ("Workdir: ", g_script_info.work_dir))
    {
      CHAR8 *lastinput = g_user_input;
      INT32 len = STRING_LEN (lastinput);
      str_alloc (&g_script_info.work_dir, len);
      strcpy (g_script_info.work_dir, lastinput);
      if (!IS_TRUE (file_is_dir (g_script_info.work_dir)))
        {
          dlg_msgbox (errtype_to_str (es_bad_script));
        }
    }
  ui_redraw_screen ();
}

static void
on_run_script (void)
{
#if (DEBUG_MODE_ENABLED)
  dlg_msgbox (ui_get_input_bar_y (), "demo mode");
  return;
#endif
  if (IS_NULL (g_script_info.base_name))
    {
      g_script_info.base_name = "./script.sh";
    }
  INT32 len;
  if (IS_NULL (g_script_info.base_name))
    {
      dlg_msgbox (errtype_to_str (es_bad_buffer));
      return;
    }
  if (dlg_input_dlg ("Script:", g_script_info.base_name))
    {
      len = STRING_LEN (g_user_input);
      str_alloc (&g_script_info.base_name, len);
      strncpy (g_script_info.base_name, g_user_input, len);
      switch_to_console_mode ();
      const INT32 cmdlen = BUFSIZ;
      CHAR8 cmd[cmdlen];
      sprintf ((CHAR8 *) cmd, "bash -c '%s'", g_script_info.base_name);
      INT32 ret = shell_run_cmd (g_script_info.work_dir, (CHAR8 *) cmd);
      if (!IS_TRUE (ret))
        {
          /* gotta do sth here !! */
        }
      switch_to_edit_mode ();
    }
}

static void
on_show_buffer_info (void)
{
  CHAR8 *info = NULL;
  buffer_time_option_e flags = 0x0;
  SET_BIT (flags, bto_show_atime | bto_show_mtime);
  get_buffer_info ((CHAR8**) & info, flags);
  dlg_msgbox (info);
  ui_redraw_screen ();
}

static void
on_list_buffers (void)
{
  switch_to_console_mode ();
  struct buffer_st *bfrp = NULL;
  INT32 i;
  CHAR8 *curflg;
  CHAR8 *msg = "Buffer List:\n\n";
  if (IS_NEGATIVE (write (STDOUT_FILENO, msg, strlen (msg))))
    {
      return;
    }
  for (i = 0x0; i < g_buffers_count; i++)
    {
      bfrp = &g_buffer_storage[i];
      curflg = (g_current_buffer_idx == i) ? "[*]" : "";
      CHAR8 *item = NULL;
      str_alloc (&item, BUFSIZ);
      (void) sprintf (item, "%d. %s %s\n", i + 0x1, bfrp->file_name, curflg);
      if (IS_NEGATIVE (write (STDOUT_FILENO, item, STRING_LEN (item))))
        {
          return;
        }
      mem_free (item);
    }
  switch_to_edit_mode ();
}

static void
on_drop_buffer (void)
{
  if (g_buffers_count <= 0x1)
    {
      dlg_msgbox (errtype_to_str (es_bad_buffer));
    }
  else
    {
      if (IS_TRUE (g_current_buffer_ptr->is_dirty))
        {
          on_save_buffer ();
        }
      INT32 nextidx = (!g_current_buffer_idx) ? g_current_buffer_idx + 0x1 : g_current_buffer_idx - 0x1;
      buffer_clear (g_current_buffer_ptr);
      INT32 i;
      for (i = g_current_buffer_idx; i < g_buffers_count - 0x1; i++)
        {
          g_buffer_storage[i] = g_buffer_storage[i + 0x1];
        }
      g_buffers_count--;
      activate_buffer (nextidx);
    }
}

static void
on_move_line_up (void)
{
  text_move_line_up ();
}

static void
on_move_line_down (void)
{
  text_move_line_down ((screen_draw_function) ui_redraw_screen);
}

static void
on_move_char_frwd (void)
{
  text_move_char_frwd ();
}

static void
on_move_char_bkwd (void)
{
  text_move_char_bkwd ();
}

static void
on_move_to_bol (void)
{
  text_move_to_BOL ();
}

static void
on_move_to_eol (void)
{
  text_move_to_EOL ();
}

static void
on_move_page_down (void)
{
  text_scroll_page_down ((screen_draw_function) ui_redraw_screen);
}

static void
on_move_page_up (void)
{
  text_scroll_page_up (ui_get_status_bar_y () - ui_get_text_area_y ());
}

static void
on_to_first_page (void)
{
  text_to_first_page ((screen_draw_function) ui_redraw_screen);
}

static void
on_mark_text(void)
{
  text_mark_text ();
}

static void
on_insert_tab_char (void)
{
  text_accept_tab_key ();
}

static void
on_insert_cr_char (void)
{
  text_accept_return_key ();
}

static void
on_drop_prev_char (void)
{
  text_accept_backspace_key ();
}

static void
on_drop_next_char (void)
{
  text_accept_delete_key ();
}

static void
on_drop_chars (void)
{
  text_drop_chars ();
}

static void
on_drop_line (void)
{
  undo_set_line (g_current_buffer_ptr->current_line);
  text_drop_line ((screen_draw_function) ui_redraw_screen);
}

static void
on_drop_lines (void)
{
  text_drop_lines ();
}

static void
on_undrop_line (void)
{
  if (IS_TRUE (undo_restore_line (g_current_buffer_ptr)))
    {
      g_current_buffer_ptr->is_dirty = TRUE;
      ui_redraw_screen ();
    }
}

static void
on_copy_chars (void)
{
  text_copy_chars ();
}

static void
on_copy_lines (void)
{
  if (dlg_input_dlg ("N: ", "1"))
    {
      _warn_numeric_input ();
      INT32 count = atoi (g_user_input);
      INT32 curno = g_current_buffer_ptr->current_line->no;
      if (count + curno >= g_current_buffer_ptr->line_count)
        {
          dlg_msgbox (errtype_to_str (es_bad_numeric_format));
        }
      else
        {
          buffer_clear (g_line_clipboard_ptr);
          struct line_st *linep;
          struct line_st *newlinep;
          for (INT32 i = 0x0; i < count; i++)
            {
              linep = buffer_get_line (g_current_buffer_ptr, curno + i);
              newlinep = line_new ();
              CHAR8 *strp = linep->text;
              if (!IS_NULL (strp))
                {
                  line_append_str (newlinep, strp);
                }
              else
                {
                  newlinep->text = "";
                }
              buffer_append_line (g_line_clipboard_ptr, newlinep);
            }
          buffer_renum_lines (g_line_clipboard_ptr);
        }
    }
}

static void
on_yank_chars (void)
{
  text_yank_chars ();
}

static void
on_yank_lines (void)
{
  if (!g_line_clipboard_ptr->line_count)
    {
      dlg_msgbox (errtype_to_str (es_bad_buffer));
      return;
    }
  text_yank_lines (g_line_clipboard_ptr);
}

static void
on_locate_by_str (void)
{
  text_locate_by_str ();
}

static void
on_locate_by_no (void)
{
  if (dlg_input_dlg ("Line#: ", "1"))
    {
      _warn_numeric_input ();
      INT32 no = atoi (g_user_input) - 0x1;
      if (no > g_current_buffer_ptr->line_count - 0x1)
        {
          dlg_msgbox (errtype_to_str (es_bad_numeric_format));
        }
      else
        {
          text_locate_by_no (no);
        }
    }
}

static void
on_to_prev_buffer (void)
{
  _warn_switch_buffer ();
  if (!g_current_buffer_idx)
    {
      g_current_buffer_idx = g_buffers_count;
    }
  activate_buffer (g_current_buffer_idx - 0x1);
}

static void
on_to_next_buffer (void)
{
  _warn_switch_buffer ();
  if (g_current_buffer_idx == g_buffers_count - 0x1)
    {
      g_current_buffer_idx = -0x1;
    }
  activate_buffer (g_current_buffer_idx + 0x1);
}

static void
on_refresh_screen (void)
{
  on_sigwinch (0x0);
}

static void
event_loop ()
{
  command_handler_func *funcp = NULL;
  INT32 key, secbyte;
  while ((key = (INT32) wgetch (g_text_area)) != CMD_EXIT)
    {
      dlg_remove_input ();
      if (IS_EQUAL (key, CMD_CANCEL))
        {
          secbyte = (INT32) wgetch (g_text_area);
          funcp = (ERR != secbyte) ? get_meta_key_handler (secbyte) : flash_screen;
        }
      else if (IS_EQUAL (key, KEY_RESIZE))
        {
          on_sigwinch ();
        }
      else
        {
          funcp = get_ctrl_key_handler (key);
        }
      if (!IS_NULL (funcp))
        {
          funcp ();
        }
      else
        {
          text_accept_printable_key (key);
        }
    }
  if (!dlg_yesno_dlg ("Exit?(yes/no): ", "no"))
    {
      event_loop ();
    }
  on_save_buffers ();
}
