#ifndef DOXYW_MSG_H
#define DOXYW_MSG_H

void config_err(const char *fmt, ...);
void config_term(const char *fmt, ...);
void config_warn(const char *fmt, ...);
void config_open();
void config_finish();

#endif
