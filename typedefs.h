/************************************************************
 * PROJECT: KLEIN
 * FILE:    TYPEDEFS.H
 * PURPOSE: CUSTOM TYPES
 * VERSION: 0.8.5
 *
 ************************************************************/

#ifndef _TYPEDEFS_H_INCLUDED
#define _TYPEDEFS_H_INCLUDED

#include "macros.h"


typedef enum
    {
      es_malloc_failed      =  (0x1),
      es_realloc_failed     =  (0x2),
      es_bad_args           =  (0x3),
      es_bad_buffer         =  (0x4),
      es_save_failed        =  (0x5),
      es_bad_numeric_format =  (0x6),
      es_bad_script         =  (0x7),
      es_locate_line_failed =  (0x8),
      es_bad_config_file    =  (0x9)
    } error_symbol_e;

typedef enum
    {
      CMD_EXIT              =  F10_SHORTCUT,
      CMD_SHOW_HELP         =  F1_SHORTCUT,
      CMD_OPEN_FILE         =  F2_SHORTCUT,
      CMD_SAVE_FILE         =  F3_SHORTCUT,
      CMD_SAVE_ALL_FILES    =  F4_SHORTCUT,
      CMD_LIST_OPEN_FILES   =  F5_SHORTCUT,
      CMD_DROP_BUFFER       =  F6_SHORTCUT,
      CMD_SHOW_FILE_INFO    =  F7_SHORTCUT,
      CMD_SET_SCRIPT_DIR    =  F8_SHORTCUT,
      CMD_RUN_SCRIPT        =  F9_SHORTCUT,
      CMD_MOVE_LINE_UP      =  ARROW_UP_SHORTCUT,
      CMD_MOVE_LINE_DOWN    =  ARROW_DOWN_SHORTCUT,
      CMD_MOVE_CHAR_FRWD    =  ARROW_RIGHT_SHORTCUT,
      CMD_MOVE_CHAR_BKWD    =  ARROW_LEFT_SHORTCUT,
      CMD_MOVE_TO_BOL       =  CTRL_A_SHORTCUT,
      CMD_MOVE_TO_EOL       =  CTRL_E_SHORTCUT,
      CMD_SCROLL_PAGE_UP    =  PAGE_UP_SHORTCUT,
      CMD_SCROLL_PAGE_DOWN  =  PAGE_DOWN_SHORTCUT,
      CMD_TO_FIRST_PAGE     =  HOME_SHORTCUT,
      CMD_MARK_LINE_TEXT         =  CTRL_L_SHORTCUT,        
      CMD_INSERT_TAB_CHAR   =  TAB_SHORTCUT,
      CMD_INSERT_CR_CHAR    =  RETURN_SHORTCUT,
      CMD_DROP_NEXT_CHAR    =  DELETE_SHORTCUT,
      CMD_DROP_PREV_CHAR    =  BACKSPACE_SHORTCUT,
      CMD_DROP_CHARS        =  META_D_SHORTCUT,
      CMD_DROP_LINE         =  CTRL_K_SHORTCUT,
      CMD_UNDROP_LINE       =  CTRL_U_SHORTCUT,
      CMD_DROP_LINES        =  META_K_SHORTCUT,
      CMD_COPY_CHARS        =  CTRL_P_SHORTCUT,
      CMD_YANK_CHARS        =  CTRL_Y_SHORTCUT,
      CMD_COPY_LINES        =  META_P_SHORTCUT,
      CMD_YANK_LINES        =  META_Y_SHORTCUT,
      CMD_LOCATE_BY_STR     =  CTRL_F_SHORTCUT,
      CMD_LOCATE_BY_NO      =  CTRL_G_SHORTCUT,
      CMD_CANCEL            =  ESC_SHORTCUT,
      CMD_TO_PREV_BUFFER    =  META_COMMA_SHORTCUT,
      CMD_TO_NEXT_BUFFER    =  META_DOT_SHORTCUT,
      CMD_REFRESH_SCREEN    =  META_R_SHORTCUT
    } command_key_binding_e;

typedef enum
    {
        bto_show_atime = 0x1,
        bto_show_mtime = 0x2
    } buffer_time_option_e;

typedef int     INT32;
typedef INT32   char_index_t;
typedef short   BOOL16;
typedef char    CHAR8;

typedef void (*screen_draw_function)(void);

typedef struct
    {
        error_symbol_e    symbol;
        CHAR8             *description;
    } error_st;

struct line_st
    {
        INT32 no;
        struct line_st    *prev_line;
        struct line_st    *next_line;
        CHAR8             *text;
        char_index_t      first_visible_char;
        char_index_t      cursor_char;
        char_index_t      selstart_char;
        char_index_t      selend_char;
    };

struct buffer_st
    {
        CHAR8             *file_name;
        INT32             line_count;
        struct line_st    *first_line;
        struct line_st    *current_line;
        struct line_st    *page_top_line;
        struct line_st    *page_bottom_line;
        BOOL16            is_dirty;
    };

struct script_st
    {
        CHAR8 *base_name;
        CHAR8 *work_dir;
    };

#endif