/************************************************************
 * PROJECT: KLEIN
 * FILE:    CONST.H
 * PURPOSE: CONSTANTS USED THROUGHOUT THE PROGRAM
 * VERSION: 0.8.5
 *
 ************************************************************/

#ifndef _CONST_H_INCLUDED
#define _CONST_H_INCLUDED

#define VERSION  "GNU klein 0.8.5"

#if !defined(TRUE)
#define TRUE                        (0x1)
#endif
#if !defined(FALSE)
#define FALSE                       (0x0)
#endif
#if !defined(NULL)
#define NULL                        ((void *) 0x0)
#endif
#define ALLOC_FAILED                ((void *) -0x1) 
#if !defined(EOF)
#define EOF                         (-0x1)
#endif
#define SYSTEM_ERROR_MSG            "[SystemError]"
#define SPACE_CHAR                  ' '
#define TAB_CHAR                    '\t'
#define LF_CHAR                     '\n'
#if !defined(BUFSIZ)
#define BUFSIZ                      0x200
#endif
#if (DEBUG_MODE_ENABLED)
#define MAX_BUFFERS_COUNT           0x2
#else
#define MAX_BUFFERS_COUNT           0xA
#endif
#define FRAME_TOP_LEFT_X            (0x0)
#define FRAME_TOP_LEFT_Y            (0x0)
#define FRAME_COLS_COUNT()          ((INT32) COLS)
#define FRAME_LINES_COUNT()         ((INT32) LINES - 0x2)
#define ABOUT_BOX_COLS_COUNT        math_asm_div(FRAME_COLS_COUNT(), 0x4)
#define ABOUT_BOX_LINES_COUNT       math_asm_div(FRAME_LINES_COUNT(), 0x4)
#define ABOUT_BOX_BORDER_CHAR       '+'
#define ABOUT_BOX_COLOR_PAIR        0x6
#define TEXT_AREA_COLOR_PAIR        0x1
#define INPUT_BAR_COLOR_PAIR        0x2
#define PROMPT_BAR_COLOR_PAIR       0x7
#define TITLE_BAR_COLOR_PAIR        0x4
#define STATUS_BAR_COLOR_PAIR       0x3
#define HINT_BAR_COLOR_PAIR         0x5

#endif