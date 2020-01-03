/************************************************************
 * PROJECT: KLEIN
 * FILE:    GLOBALS.H
 * PURPOSE:
 * VERSION: 0.8.5
 *
 ************************************************************/

#ifndef GLOBALS_H
#define GLOBALS_H

static const error_st g_error_list[] = {
  { es_malloc_failed,       "ERROR_MALLOC **:    malloc() failed"           },
  { es_bad_args,            "ERROR_ARGS **:      invalid argument"          },
  { es_bad_buffer,          "ERROR_BUFFER **:    buffer error"              },
  { es_save_failed,         "SAVE_ERROR **:      file save error"           },
  { es_bad_numeric_format,  "NUMERIC_ERROR **:   numeric value error"       },
  { es_bad_script,          "SCRIPTDIR_ERROR **: invalid script's workdir"  },
  { es_locate_line_failed,  "FIND_ERROR**:       failed to find line"       },
  { es_bad_config_file,     "CONFIG_ERROR **:    Config file error!"        },
  { 0x0,                    NULL                                            }
};

extern struct buffer_st     g_config_file;
extern struct buffer_st     g_buffer_storage[MAX_BUFFERS_COUNT];
extern INT32                g_buffers_count;
extern INT32                g_current_buffer_idx;
extern struct buffer_st     *g_current_buffer_ptr;
extern CHAR8                *g_text_clipboard;
extern struct buffer_st     g_line_clipboard;
extern struct buffer_st     *g_line_clipboard_ptr;
extern struct line_st       g_undo_clipboard;
extern CHAR8                g_user_input[BUFSIZ];
extern CHAR8                *g_search_str;
extern INT32                g_auto_indent;
extern INT32                g_tab_stop;
extern INT32                g_top_line_no;
extern INT32                g_current_line_no;
extern WINDOW               *g_text_area;
extern WINDOW               *g_title_bar;
extern WINDOW               *g_status_bar;
extern WINDOW               *g_prompt_bar;
extern WINDOW               *g_input_bar;
extern WINDOW               *g_hint_bar;

#endif