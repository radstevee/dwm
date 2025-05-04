#include <stdarg.h>
#include <stdio.h>

#ifndef SLL_H
#define SLL_H

typedef enum {
  DEBUG, INFO, WARN, ERROR
} Level;

typedef char *(*levelstrhandler)(Level);

void initloggingdirat(int fd);
void initloggingdir(const char *dir);
void closelogfile(void);
void vlogmsg(Level level, const char *format, va_list args);
void logmsg(Level level, const char *format, ...) __attribute__ ((format (printf, 2, 3)));
void debug(const char *format, ...) __attribute__ ((format (printf, 1, 2)));
void info(const char *format, ...) __attribute__ ((format (printf, 1, 2)));
void warn(const char *format, ...) __attribute__ ((format (printf, 1, 2)));
void error(const char *format, ...) __attribute__ ((format (printf, 1, 2)));
void setlevelstrhandler(levelstrhandler handler);

#endif // SLL_H
