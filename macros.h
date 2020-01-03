/************************************************************
 * PROJECT: KLEIN
 * FILE:    MACROS.H
 * PURPOSE: MACROS USED THROUGHOUT THE PROGRAM
 * VERSION: 0.8.5
 *
 ************************************************************/

#ifndef _MACROS_H_INCLUDED
#define _MACROS_H_INCLUDED

#include "systems.h"
#include "const.h"

#define BIT_IS_SET(var_, bit_)                  ((var_) &   (bit_))
#define SET_BIT(var_, bit_)                     ((var_) |=  (bit_))
#define LEFT_SHIFT_BITS(var_, nbits_)           ((var_) << (nbits_))
#define RIGHT_SHIFT_BITS(var_, nbits_)          ((var_) >> (nbits_))
#define GET_FIRST_BYTE(var_)                    ((var_)  & 0xff)
#define GET_FIRST_TWO_BYTES(var_)               ((var_)  & 0xffff)
#define IS_EQUAL(var_, const_)                  ((INT32)(var_) == (INT32)(const_))
#define IS_NEGATIVE(var_)                       IS_EQUAL((var_), -0x1)
#define IS_TRUE(var_)                           IS_EQUAL((var_), TRUE)
#define IS_NULL(ptr_)                           (((ptr_) == NULL) || ((ptr_) == ALLOC_FAILED))
#define ERRNO_TO_STR()                          perror(errno ? strerror(errno) : SYSTEM_ERROR_MSG)
#define FILL_VAR_WITH_ZERO(var_, type_)         memset (&var_, 0x0, sizeof (type_))
#define FILL_PTR_WITH_ZERO(ptr_, type_)         memset (ptr_, 0x0, sizeof (type_))
#define FILL_STR_WITH_ZERO(str_, size_)         memset (str_, 0x0, size_)
#define FILL_STR_WITH_SPACE(str_, size_)        memset (str_, SPACE_CHAR, size_)
#define FILL_STRPTR_WITH_ZERO(strp_, size_)     memset (*strp_, 0x0, size_)
#define STRING_LEN(str_)                        (!IS_NULL(str_) ? strlen(str_) : 0x0)
#define STRING_COMPARE(lhstr_, rhstr_)          (!(strcmp((lhstr_), (rhstr_))) ? TRUE : FALSE)
#define STRING_MEM_SIZE(len_)                   (sizeof(CHAR8) * ((len_) + 0x1))

#define CALC_NSPACES(tabstop_, colno_)          (tabstop_ - math_qmod(colno_ - 0x1,  tabstop_))      

#define RETURN_IF_TRUE(expr_)                                                               \
MACRO_START                                                                                 \
    if (IS_TRUE(expr_))                                                                     \
        {                                                                                   \
            return;                                                                         \
        }                                                                                   \
MACRO_END

#define RETURN_VAL_IF_TRUE(expr_, val_)                                                     \
MACRO_START                                                                                 \
    if (IS_TRUE(expr_))                                                                     \
        {                                                                                   \
            return (val_);                                                                  \
        }                                                                                   \
MACRO_END

#define CALL_FUNCTION(funcp_)                                                               \
MACRO_START                                                                                 \
    if (!IS_NULL((funcp_)))                                                                 \
        {                                                                                   \
            funcp_();                                                                       \
        }                                                                                   \
MACRO_END

#define line_append_char(linep_, char_)                                                     \
MACRO_START                                                                                 \
    INT32 len = (line_is_null((linep_))) ? 0x1 : line_len((linep_)) + 0x1;                  \
    line_text_alloc((linep_), len);                                                         \
    line_edit_char((linep_), len - 0x1, (char_));                                           \
MACRO_END

#define line_insert_char(linep_, pos_, char_)                                               \
MACRO_START                                                                                 \
    INT32 len = line_len((linep_)) + 0x1;                                                   \
    line_text_alloc((linep_), len);                                                         \
    memmove((linep_)->text + (pos_) + 0x1,                                                  \
            (linep_)->text + (pos_),                                                        \
            len - (pos_));                                                                  \
    line_edit_char((linep_), (pos_), (char_));                                              \
MACRO_END

#define line_drop_char(linep_, pos_)                                                        \
MACRO_START                                                                                 \
    if (!line_is_null((linep_)))                                                            \
        {                                                                                   \
            INT32 len = line_len((linep_));                                                 \
            memmove((linep_)->text + (pos_),                                                \
                    (linep_)->text + (pos_) + 0x1,                                          \
                    len - (pos_)-1);                                                        \
            line_text_alloc((linep_), --len);                                               \
        }                                                                                   \
MACRO_END

#define line_edit_char(linep_, pos_, val_)                                                  \
MACRO_START                                                                                 \
    (*((linep_)->text + (pos_)) = (val_));                                                  \
MACRO_END

#define line_append_str(linep_, s_)                                                         \
MACRO_START                                                                                 \
    while (*s_)                                                                             \
        {                                                                                   \
            line_append_char(linep_, *s_++);                                                \
        }                                                                                   \
MACRO_END

#define line_disable_selection(linep_)                                                      \
MACRO_START                                                                                 \
    {                                                                                       \
        (linep_)->selend_char = (linep_)->selstart_char = -0x1;                             \
        _text_redraw_line(linep_);                                                           \
    }                                                                                       \
MACRO_END

#define SET_WINDOW_BKGD(winp_, color_pair_)                                                 \
MACRO_START                                                                                 \
    if (IS_TRUE(conio_has_colors()))                                                        \
        {                                                                                   \
            wbkgd(winp_, color_pair_);                                                      \
        }                                                                                   \
MACRO_END

#define CTRL_KEY_CODE(key_)    (GET_FIRST_BYTE(key_) - 0x40)
#define META_KEY_CODE(key_)    GET_FIRST_BYTE(key_)
#define SINGLE_KEY_CODE(key_)  (key_)

#define F1_SHORTCUT            SINGLE_KEY_CODE(KEY_F(0x1))
#define F2_SHORTCUT            SINGLE_KEY_CODE(KEY_F(0x2))
#define F3_SHORTCUT            SINGLE_KEY_CODE(KEY_F(0x3))
#define F4_SHORTCUT            SINGLE_KEY_CODE(KEY_F(0x4))
#define F5_SHORTCUT            SINGLE_KEY_CODE(KEY_F(0x5))
#define F6_SHORTCUT            SINGLE_KEY_CODE(KEY_F(0x6))
#define F7_SHORTCUT            SINGLE_KEY_CODE(KEY_F(0x7))
#define F8_SHORTCUT            SINGLE_KEY_CODE(KEY_F(0x8))
#define F9_SHORTCUT            SINGLE_KEY_CODE(KEY_F(0x9))
#define F10_SHORTCUT           SINGLE_KEY_CODE(KEY_F(0xA))
#define CTRL_A_SHORTCUT        CTRL_KEY_CODE('A')
#define CTRL_E_SHORTCUT        CTRL_KEY_CODE('E')
#define CTRL_F_SHORTCUT        CTRL_KEY_CODE('F')
#define CTRL_G_SHORTCUT        CTRL_KEY_CODE('G')
#define CTRL_K_SHORTCUT        CTRL_KEY_CODE('K')
#define CTRL_L_SHORTCUT        CTRL_KEY_CODE('L')
#define CTRL_P_SHORTCUT        CTRL_KEY_CODE('P')
#define CTRL_U_SHORTCUT        CTRL_KEY_CODE('U')
#define CTRL_Y_SHORTCUT        CTRL_KEY_CODE('Y')
#define META_D_SHORTCUT        META_KEY_CODE('d')
#define META_K_SHORTCUT        META_KEY_CODE('k')
#define META_P_SHORTCUT        META_KEY_CODE('p')
#define META_R_SHORTCUT        META_KEY_CODE('r')
#define META_Y_SHORTCUT        META_KEY_CODE('y')
#define META_COMMA_SHORTCUT    META_KEY_CODE(',')
#define META_DOT_SHORTCUT      META_KEY_CODE('.')
#define ARROW_DOWN_SHORTCUT    SINGLE_KEY_CODE(KEY_DOWN)
#define ARROW_LEFT_SHORTCUT    SINGLE_KEY_CODE(KEY_LEFT)
#define ARROW_RIGHT_SHORTCUT   SINGLE_KEY_CODE(KEY_RIGHT)
#define ARROW_UP_SHORTCUT      SINGLE_KEY_CODE(KEY_UP)
#define BACKSPACE_SHORTCUT     SINGLE_KEY_CODE(KEY_BACKSPACE)
#define DELETE_SHORTCUT        SINGLE_KEY_CODE(KEY_DC)
#define ESC_SHORTCUT           SINGLE_KEY_CODE(0x1B)
#define HOME_SHORTCUT          SINGLE_KEY_CODE(KEY_HOME)
#define PAGE_DOWN_SHORTCUT     SINGLE_KEY_CODE(0x152)
#define PAGE_UP_SHORTCUT       SINGLE_KEY_CODE(0x153)
#define RETURN_SHORTCUT        SINGLE_KEY_CODE('\r')
#define TAB_SHORTCUT           SINGLE_KEY_CODE('\t')

#endif
