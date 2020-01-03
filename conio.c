/************************************************************
 * PROJECT: KLEIN
 * FILE:    CONIO.C
 * PURPOSE:
 * VERSION: 0.8.5
 *
 ************************************************************/

#include <stdlib.h>
#include "library.h"

void
conio_start (void)
{
  initscr ();
  (void) cbreak ();
  (void) nonl ();
  (void) noecho ();
}

BOOL16
conio_has_colors (void)
{
  return (BOOL16) has_colors ();
}

void
conio_start_color (void)
{
  (void) start_color ();
}

void
conio_init_pair (const INT32 pair, 
                 const INT32 f, 
                 const INT32 b)
{
  (void) init_pair (pair, f, b);
}

WINDOW *
conio_create_window (const INT32 nlines, 
                     const INT32 ncols, 
                     const INT32 starty,
                     const INT32 startx)
{
  WINDOW * winp = newwin (nlines, ncols, starty, startx);
  (void) keypad (winp, TRUE);
  (void) idlok (winp, TRUE);
  (void) intrflush (winp, TRUE);
  return winp;
}

WINDOW *
conio_create_subwindow (WINDOW *winp, 
                        const INT32 nlines, 
                        const INT32 ncols,
                        const INT32 starty, 
                        const INT32 startx)
{
  WINDOW * subwinp = subwin (winp, nlines, ncols, starty, startx);
  (void) keypad (subwinp, TRUE);
  (void) idlok (subwinp, TRUE);
  (void) intrflush (subwinp, TRUE);
  return subwinp;
}

WINDOW *
conio_get_stdscr (const INT32 pair)
{
  (void) keypad (stdscr, TRUE);
  (void) idlok (stdscr, TRUE);
  (void) intrflush (stdscr, TRUE);
  if (conio_has_colors ())
    {
      wbkgd (stdscr, COLOR_PAIR (pair));
      wattron (stdscr, A_BOLD);
    }
  return stdscr;
}

void
conio_delete_window (WINDOW *winp)
{
  (void) wmove (winp, 0x0, 0x0);
  wclrtoeol (winp);
  wclear (winp);
  SET_WINDOW_BKGD (winp, COLOR_PAIR (TEXT_AREA_COLOR_PAIR));
  conio_refresh_window (winp);
  (void) delwin (winp);
}

void
conio_empty_window (WINDOW *winp, 
                    const INT32 starty, 
                    const INT32 endy,
                    const char_index_t startx, 
                    const char_index_t endx)
{
  INT32 len = endx - startx + 0x1;
  size_t slen = sizeof (CHAR8) * len;
  CHAR8 *s = (CHAR8 *) malloc (slen);
  s = FILL_STR_WITH_SPACE(s, len);
  INT32 i;
  for (i = starty; i <= endy; i++)
    {
      (void) wmove (winp, i, startx);
      (void) waddstr (winp, s);
    }
}

void
conio_refresh_window (WINDOW *winp)
{
  (void) wnoutrefresh (winp);
  (void) doupdate ();
}

INT32
conio_get_key_length (const INT32 key)
{
  const CHAR8 *name = (const CHAR8 *) keyname (key);
  return strlen (name);
}

void
conio_print_str_on_window (WINDOW *winp,
                           const CHAR8 *s,
                           const INT32 y,
                           const char_index_t start,
                           const char_index_t end,
                           const char_index_t selstart,
                           const char_index_t selend)
{
  BOOL16 istxtarea = (winp == g_text_area);
  (void) wmove (winp, y, istxtarea ? FRAME_TOP_LEFT_X : 0x0);
  wclrtoeol (winp);
  RETURN_IF_TRUE (!s);
  INT32 x = istxtarea ? FRAME_TOP_LEFT_X : 0x0;
  INT32 pos = start;
  register const CHAR8 *sp = s + start;
  CHAR8 c;
  while (TRUE)
    {
      if (pos > end)
        {
          break;
        }
      c = *sp;
      if (!c)
        {
          break;
        }
      (void) wmove (winp, y, x);
      if (!IS_NEGATIVE (selstart) && !IS_NEGATIVE (selend))
        {
          (void) waddch (winp,
                         ((pos >= selstart) && (pos <= selend)) ? (c | A_STANDOUT) : c);
        }
      else if (!IS_NEGATIVE (selstart) || !IS_NEGATIVE (selend))
        {
          (void) waddch (winp,
                         ((pos == selstart) || (pos == selend)) ? c | A_STANDOUT : c);
        }
      else
        {
          (void) waddch (winp, c);
        }
      x++, pos++, sp++;
    }
}