/******************************************************************************
*
* Copyright (C) 1997-2019 by Dimitri van Heesch.
*
* Permission to use, copy, modify, and distribute this software and its
* documentation under the terms of the GNU General Public License is hereby
* granted. No representations are made about the suitability of this software
* for any purpose. It is provided "as is" without express or implied warranty.
* See the GNU General Public License for more details.
*
* Documents produced by Doxygen are derivative works derived from the
* input used in their production; they are not affected by this license.
*
*/

#ifndef DOTPROCESS_H
#define DOTPROCESS_H

#include <condition_variable>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "qcstring.h"

#ifdef _WIN32
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#endif

/** Manages a persistent 'dot -T<format>' child process.
 *
 *  One instance per thread per format is kept alive across many graphs,
 *  feeding each graph via stdin and reading the rendered result from stdout.
 *  This avoids fork+exec overhead per graph while remaining thread-safe
 *  (each thread owns its own DotProcess objects).
 */
class DotProcess
{
  public:
    DotProcess() = default;
    ~DotProcess();

    // Non-copyable, non-movable (owns OS handles)
    DotProcess(const DotProcess &) = delete;
    DotProcess &operator=(const DotProcess &) = delete;

    /** Launch "dotExe -T<format>".  Returns false if exec fails. */
    bool start(const QCString &dotExe, const QCString &format);

    /** Send dotContent to the process stdin, read rendered output from
     *  stdout until the format-specific end marker, write bytes to
     *  outputPath.  Returns false on error; marks process dead for restart. */
    bool render(const std::string &dotContent, const QCString &outputPath);

    bool isAlive() const { return m_alive; }

  private:
    /** Read from stdout pipe until the end marker for m_format.
     *  Appends bytes to buf.  On failure, sets capturedErrno to the OS error
     *  code (errno / GetLastError) or 0 on clean EOF.
     *  Returns false on EOF before finding marker. */
    bool readUntilEndMarker(std::vector<char> &buf, int &capturedErrno);

    /** Mark the process dead, log why (context + OS error code), and
     *  WNOHANG-waitpid the child: if it has already become a zombie,
     *  reap it and include its exit status / termination signal in the
     *  error message. */
    void markDeadAndReport(const char *context, int savedErrno);

#ifdef _WIN32
    HANDLE m_stdinW  = INVALID_HANDLE_VALUE;
    HANDLE m_stdoutR = INVALID_HANDLE_VALUE;
    HANDLE m_process = INVALID_HANDLE_VALUE;
#else
    int   m_stdinW  = -1;
    int   m_stdoutR = -1;
    pid_t m_pid     = -1;
#endif
    QCString m_format;
    bool     m_alive = false;

    // Persistent writer thread — created in start(), stopped in ~DotProcess()
    std::thread             m_writer;
    std::mutex              m_mu;
    std::condition_variable m_workCv;   // render() -> writer: new data ready
    std::condition_variable m_doneCv;   // writer -> render(): write finished
    const std::string      *m_writeData  = nullptr;
    bool                    m_writeOk    = true;
    int                     m_writeErrno = 0;
    bool                    m_writerStop = false;
};

/** Return (or lazily start) the persistent dot process for this thread and
 *  format.  Returns nullptr if the process cannot be started (caller should
 *  fall back to the subprocess path). */
DotProcess *getDotProcess(const QCString &dotExe, const QCString &format);

/** Returns true for formats where a reliable end-of-output marker exists
 *  and the persistent-process path can be used. */
bool isPersistentFormat(const QCString &format);

#endif
