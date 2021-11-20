#include <QString>
#include "config_msg.h"
#include "doxywizard.h"

static QString warning_str = QString::fromLatin1("warning: ");
static QString error_str = QString::fromLatin1("error: ");

void config_err(const char *fmt, ...)
{
  QString msg = error_str;

  msg.append(QString::fromLatin1(fmt));
  va_list args;
  va_start(args, fmt);
  char debugOut[1000]; // this size should be sufficient
  vsnprintf(debugOut, 1000,qPrintable(msg), args);
  MainWindow::instance().outputLogText(QString::fromLatin1(debugOut));
  va_end(args);
}

void config_term(const char *fmt, ...)
{
  QString msg = error_str;

  msg.append(QString::fromLatin1(fmt));
  va_list args;
  va_start(args, fmt);
  char debugOut[1000]; // this size should be sufficient
  vsnprintf(debugOut, 1000,qPrintable(msg), args);
  MainWindow::instance().outputLogText(QString::fromLatin1(debugOut));
  va_end(args);
  exit(1);
}

void config_warn(const char *fmt, ...)
{
  QString msg = warning_str;

  msg.append(QString::fromLatin1(fmt));
  va_list args;
  va_start(args, fmt);
  char debugOut[1000];
  vsnprintf(debugOut, 1000,qPrintable(msg), args);
  MainWindow::instance().outputLogText(QString::fromLatin1(debugOut));
  va_end(args);
}

void config_open()
{
  MainWindow::instance().outputLogStart();
}

void config_finish()
{
  MainWindow::instance().outputLogFinish();
}
