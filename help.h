/************************************************************
 * PROJECT: KLEIN
 * FILE:    HELP.H
 * PURPOSE: THE PROGRAM'S HELP
 * VERSION: 0.8.5
 *
 ************************************************************/

#ifndef _HELP_H_INCLUDED
#define _HELP_H_INCLUDED

#define HELP_ITEMS_COUNT        0x2D
#define MAX_HELP_ITEM_SIZE      0xC8
#define GET_HELP_ITEM(no_)      (_help_items[no_])

static const CHAR8 _help_items[HELP_ITEMS_COUNT][MAX_HELP_ITEM_SIZE] = {
  "Klein Manual",
  "F1               Help.",
  "F2               Open a file.",
  "F3               Save current buffer.",
  "F4               Save all open buffers.",
  "F5               List open buffers.",
  "F6               Drop current buffer.",
  "F7               Current buffer info.",
  "F8               Set script dir.",
  "F9               Run script from the dir set by F8.",
  "F10              Quit.",
  "*******************************************",
  "Up arrow         Move one line up",
  "Down arrow       Move one line down",
  "Right arrow      Move one character forward",
  "Left arrow       Move one character backward",
  "CTRL-a           Move to the beginning of line",
  "CTRL-e           Move to the end of line",
  "PG DWN           Scroll one page down",
  "PG UP            Scroll one page up",
  "HOME             Scroll to the very first page",
  "*******************************************",
  "RETURN           Standard behavior of the RETURN key",
  "TAB              Standard behavior of the TAB key",
  "BACKSPACE        Standard behavior of the BKSPC key",
  "DELETE           Delete the CHAR8 to the right of the cursor ",
  "CTRL-k           Delete the current line.",
  "CTRL-u           Undo deletion of the current line",
  "META^k           Delete all the lines below the current line",
  "*******************************************",
  "ESC              Cancel a dialog",
  "*******************************************",
  "CTRL-l           Selects a text from the current line.",
  "CTRL-p           Copies the current line's selected text to clipboard.", 
  "CTRL-y           Pastes from clipboard the text copied there by CTRL-p",
  "META-d           Removes the part of the line from current cursor position to the end of the line",
  "META-p           Copies to clipboard N lines starting from the current one. The value of N is specified in the dialog",
  "META-y           Pastes from clipboard N lines copied there by META-p",
  "*******************************************",
  "CTRL-g           Go to line by its number",
  "CTRL-f           Go to line by its text.",
  "META-<           Switch to previous buffer.",
  "META->           Switch to next buffer.",
  "META-r           Refresh.",
  "*******************************************"
};

#endif