#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>

#include "sll.h"

char *
defaultlevelstrhandler(Level level)
{
  switch (level) {
    case DEBUG: return "[debug]";
    case INFO: return "[info]";
    case WARN: return "[warn]";
    case ERROR: return "[error]";
  };
  return "unknown";
}

static levelstrhandler levelstr = defaultlevelstrhandler;
static int logfilefd = -999;

const char *
svlog(Level level, const char *format, va_list args)
{
  char *b;
  const char *lvl = levelstr ? levelstr(level) : "[unknown]";

  va_list args_copy;
  va_copy(args_copy, args);
  int needed = vsnprintf(NULL, 0, format, args_copy);
  va_end(args_copy);

  if (needed < 0)
    return NULL;

  size_t total = strlen(lvl) + 1 + needed + 2;
  b = malloc(total);
  if (!b)
    return NULL;

  int offset = sprintf(b, "%s ", lvl);
  vsprintf(b + offset, format, args);
  strcat(b, "\n");

  return b;
}

void
vlogmsg(Level level, const char *format, va_list args)
{
  const char *m = svlog(level, format, args);
  if (level == ERROR) {
    fputs(m, stderr);
  } else {
    fputs(m, stdout);
  }

  if (logfilefd != -999)
    write(logfilefd, m, strlen(m));

  free((char *) m);
}

void
initloggingdirat(int fd)
{
  char logfilename[64];
  int logfile;
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);

  snprintf(logfilename, sizeof(logfilename), "%d-%02d-%02d_%02d_%02d_%02d.log",
           tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
  logfile = openat(fd, logfilename, O_CREAT | O_WRONLY | O_TRUNC | O_APPEND, 0655);

  if (logfile == -1) {
    error("failed opening logfile at %s", logfilename);
    return;
  }

  logfilefd = logfile;
}

void
initloggingdir(const char *dir)
{
  mkdir(dir, 0755);

  int fd = open(dir, O_RDONLY | O_DIRECTORY);
  if (fd == -1) {
    error("failed opening directory %s", dir);
    return;
  }

  initloggingdirat(fd);
}

void
closelogfile(void)
{
  if (logfilefd == -999)
    return;

  close(logfilefd);
}

void
logmsg(Level level, const char *format, ...)
{
  va_list args;
  va_start(args, format);
  vlogmsg(level, format, args);
  va_end(args);
}

void
debug(const char *format, ...)
{
  va_list args;
  va_start(args, format);
  vlogmsg(DEBUG, format, args);
  va_end(args);
}

void
info(const char *format, ...)
{
  va_list args;
  va_start(args, format);
  vlogmsg(INFO, format, args);
  va_end(args);
}

void
warn(const char *format, ...)
{
  va_list args;
  va_start(args, format);
  vlogmsg(WARN, format, args);
  va_end(args);
}

void
error(const char *format, ...)
{
  va_list args;
  va_start(args, format);
  vlogmsg(ERROR, format, args);
  va_end(args);
}

void
setlevelstrhandler(levelstrhandler handler)
{
  levelstr = handler;
}

