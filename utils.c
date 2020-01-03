/************************************************************
 * PROJECT: KLEIN
 * FILE:    UTILS.C
 * PURPOSE:
 * VERSION: 0.8.5
 *
 ************************************************************/

#include <ctype.h>
#include <limits.h>
#include <pwd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include "library.h"

void *
mem_alloc (const size_t nbytes)
{
  RETURN_VAL_IF_TRUE (!nbytes, ALLOC_FAILED);
  void *memp = malloc (nbytes);
  return IS_NULL (memp) ? ALLOC_FAILED : memp;
}

void *
mem_realloc (void *srcmemp, const size_t nbytes)
{
  RETURN_VAL_IF_TRUE (!nbytes, ALLOC_FAILED);
  void *destmemp = IS_NULL (srcmemp) ? mem_alloc (nbytes) : realloc (srcmemp, nbytes);
  return IS_NULL (destmemp) ? ALLOC_FAILED : destmemp;
}

BOOL16
mem_free (void *memp)
{
  RETURN_VAL_IF_TRUE (IS_NULL (memp), FALSE);
  free (memp);
  return TRUE;
}

BOOL16
str_alloc (CHAR8 **spp, const size_t nbytes)
{
  size_t memsiz = STRING_MEM_SIZE (nbytes);
  *spp = (CHAR8 *) mem_alloc (memsiz);
  RETURN_VAL_IF_TRUE (IS_NULL (*spp), FALSE);
  FILL_STRPTR_WITH_ZERO (spp, memsiz);
  *(*spp + nbytes) = 0x0;
  return TRUE;
}

BOOL16
str_realloc (CHAR8 **spp, const size_t nbytes)
{
  size_t memsiz = STRING_MEM_SIZE (nbytes);
  *spp = (CHAR8 *) mem_realloc (*spp, memsiz);
  RETURN_VAL_IF_TRUE ((IS_NULL (*spp) || *spp == NULL), FALSE);
  *(*spp + nbytes) = 0x0;
  return TRUE;
}

BOOL16
str_is_numeric (const CHAR8 *s)
{
  RETURN_VAL_IF_TRUE (IS_NULL (s), FALSE);
  BOOL16 rc = TRUE;
  register const CHAR8 *strp = s;
  CHAR8 c;
  while (TRUE)
    {
      c = *strp++;
      if (!c)
        {
          break;
        }
      if (!isdigit (c))
        {
          rc = FALSE;
          break;
        }
    }
  strp = NULL;
  return rc;
}

BOOL16
file_exists (const CHAR8 *filename)
{
  BOOL16 rc = (!access (filename, F_OK)) ? TRUE : FALSE;
  return rc;
}

INT32
file_read_byte (INT32 fd, CHAR8 *c)
{
  return read (fd, c, 0x1);
}

INT32
file_write_byte (INT32 fd, CHAR8 *c)
{
  return write (fd, c, 0x1);
}

BOOL16
file_write_str (const INT32 fd, const CHAR8 *s, const INT32 newline)
{
  INT32 rc = write (fd, s, STRING_LEN (s));
  RETURN_VAL_IF_TRUE(IS_NEGATIVE(rc), FALSE);
  if (IS_TRUE (newline))
    {
      CHAR8 c = LF_CHAR;
      rc = file_write_byte (fd, &c);
      if (IS_NEGATIVE (rc))
        {
          return FALSE;
        }
    }
  return TRUE;
}

const size_t
file_get_size (const CHAR8 *filename)
{
  struct stat st;
  stat (filename, (struct stat *) &st);
  return (const size_t) st.st_size;
}

const CHAR8 *
file_get_atime (const CHAR8 *filename)
{
  struct stat filest;
  CHAR8 *buffer = NULL;
  str_alloc ((CHAR8 **) &buffer, BUFSIZ);
  FILL_STR_WITH_ZERO (buffer, BUFSIZ);
  if (IS_NEGATIVE (stat (filename, (struct stat *) &filest)))
    {
      strcpy (buffer, "(atime: 0000-00-00)");
    }
  else
    {
      struct tm *ptm = localtime ((const time_t *) &filest.st_atime);
      strftime (buffer, BUFSIZ, "(atime: %d/%m/%Y %T)", ptm);
    }
  return buffer;
}

const CHAR8 *
file_get_mtime (const CHAR8 *filename)
{
  struct stat filest;
  CHAR8 *buffer = NULL;
  str_alloc ((CHAR8 **) &buffer, BUFSIZ);
  FILL_STR_WITH_ZERO (buffer, BUFSIZ);
  if (IS_NEGATIVE (stat (filename, (struct stat *) &filest)))
    {
      strcpy (buffer, "(mtime: 0000-00-00)");
    }
  else
    {
      struct tm *ptm = localtime ((const time_t *) &filest.st_mtime);
      strftime (buffer, BUFSIZ, "(mtime: %d/%m/%Y %T)", ptm);
    }
  return buffer;
}

const CHAR8 *
file_owner (const CHAR8 *filename)
{
  struct stat st;
  RETURN_VAL_IF_TRUE (IS_NULL (filename), NULL);
  stat (filename, (struct stat *) &st);
  uid_t uid = st.st_uid;
  struct passwd *pwd = getpwuid (uid);
  return (const CHAR8 *) pwd->pw_name;
}

BOOL16
file_is_dir (const CHAR8 *filename)
{
  struct stat st;
  stat (filename, (struct stat *) &st);
  mode_t md = st.st_mode;
  INT32 rc1 = (INT32) S_ISDIR (md);
  INT32 rc2 = rc1 ? TRUE : FALSE;
  return rc2;
}

const CHAR8 *
file_get_home_dir (void)
{
  CHAR8 *env = getenv ("HOME");
  const CHAR8 *homedir = !IS_NULL (env) ? env : getpwuid (getuid ())->pw_dir;
  return homedir;
}

extern const CHAR8 *
file_get_current_dir (void)
{
  CHAR8 *buffer = NULL;
  str_alloc ((CHAR8 **) &buffer, BUFSIZ);
  return getcwd (buffer, BUFSIZ) ? (const CHAR8 *) buffer : file_get_home_dir ();
}

BOOL16
shell_run_cmd (const CHAR8 *workdir, const CHAR8 *cmd)
{
  RETURN_VAL_IF_TRUE (IS_NULL (cmd), FALSE);
  CHAR8 *buf = NULL;
  str_alloc ((CHAR8 **) &buf, BUFSIZ);
  buf = getcwd (buf, BUFSIZ);
  if (IS_NULL (buf))
    {
      return FALSE;
    }
  if (IS_NEGATIVE (system ("bash -c 'clear'")))
    {
      mem_free (buf);
      return FALSE;
    }
  if (!IS_NULL (workdir))
    {
      if (IS_NEGATIVE (chdir (workdir)))
        {
          mem_free (buf);
          return FALSE;
        }
    }
  if (IS_NEGATIVE (system (cmd)))
    {
      return FALSE;
    }
  if (!IS_NULL (workdir))
    {
      if (IS_NEGATIVE (chdir (buf)))
        {
          mem_free (buf);
          return FALSE;
        }
    }
  return TRUE;
}
