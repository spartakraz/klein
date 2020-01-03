/************************************************************
 * PROJECT: KLEIN
 * FILE:    CONFIG.C
 * PURPOSE:
 * VERSION: 0.8.5
 *
 ************************************************************/

#include "library.h"
#include <stdlib.h>

#define CONFIG_FILE_NAME "/.kleinrc"

CHAR8 *
config_get_path (void)
{
  const CHAR8 *home = getenv ("HOME");
  INT32 len = STRING_LEN (CONFIG_FILE_NAME) + STRING_LEN (home) + 0x1;
  CHAR8 *retval = NULL;
  str_alloc (&retval, len);
  (void) sprintf (retval, "%s%s", home, CONFIG_FILE_NAME);
  return retval;
}

BOOL16
config_load (void)
{
  CHAR8 *path = config_get_path ();
  if (!file_exists (path))
    {
      return FALSE;
    }
  g_config_file = buffer_new ();
  buffer_reset (&g_config_file);
  INT32 len = STRING_LEN (path);
  str_alloc (&g_config_file.file_name, len);
  strcpy (g_config_file.file_name, path);
  BOOL16 rc = buffer_load_file (&g_config_file);
  if (IS_TRUE (rc))
    {
      buffer_renum_lines (&g_config_file);
    }
  return rc;
}

INT32
config_read_option (const CHAR8 *name, INT32 *val)
{
  const CHAR8 *sep = "=";
  CHAR8 *token = NULL;
  INT32 i;
  *val = 0x0;
  struct line_st *linep = NULL;
  for (i = 0; i < g_config_file.line_count; i++)
    {
      token = NULL;
      linep = buffer_get_line (&g_config_file, i);
      if (line_is_null (linep))
        {
          continue;
        }
      token = strtok (linep->text, sep);
      if (token)
        {
          if (!STRING_COMPARE (token, name))
            {
              token = strtok (NULL, sep);
              if (token)
                {
                  *val = atoi (token);
                  return TRUE;
                }
              break;
            }
        }
    }
  return FALSE;
}