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

#include "dotprocess.h"
#include "message.h"
#include "portable.h"

#include <cerrno>
#include <cstring>
#include <map>

#ifndef _WIN32
#  include <unistd.h>
#  include <sys/wait.h>
#  include <fcntl.h>
#  include <signal.h>  // strsignal
#endif

//-----------------------------------------------------------------------------------------
// End-marker tables
//-----------------------------------------------------------------------------------------

struct EndMarker
{
  const char *bytes;
  size_t      len;
};

static EndMarker getEndMarker(const QCString &format)
{
  if (format == "svg")   return { "</svg>\n",  7 };
  if (format == "cmapx") return { "</map>\n",  7 };
  if (format == "png")
  {
    // PNG IEND chunk: length(4)=0, type="IEND", crc=AE426082
    static const char pngIend[] = "\x00\x00\x00\x00IEND\xAE\x42\x60\x82";
    return { pngIend, 12 };
  }
  if (format.startsWith("pdf") || format == "ps" || format == "eps")
    return { "%%EOF", 5 };
  return { nullptr, 0 };
}

bool isPersistentFormat(const QCString &format)
{
  return getEndMarker(format).bytes != nullptr;
}

//-----------------------------------------------------------------------------------------
// DotProcess implementation
//-----------------------------------------------------------------------------------------

DotProcess::~DotProcess()
{
  // Stop the writer thread first (before closing the stdin handle it writes to)
  if (m_writer.joinable())
  {
    {
      std::unique_lock<std::mutex> lock(m_mu);
      m_writerStop = true;
      m_workCv.notify_one();
    }
    m_writer.join();
  }

  if (!m_alive) return;
#ifdef _WIN32
  if (m_stdinW != INVALID_HANDLE_VALUE)
  {
    CloseHandle(m_stdinW);
    m_stdinW = INVALID_HANDLE_VALUE;
  }
  if (m_process != INVALID_HANDLE_VALUE)
  {
    WaitForSingleObject(m_process, 5000);
    CloseHandle(m_process);
    m_process = INVALID_HANDLE_VALUE;
  }
  if (m_stdoutR != INVALID_HANDLE_VALUE)
  {
    CloseHandle(m_stdoutR);
    m_stdoutR = INVALID_HANDLE_VALUE;
  }
#else
  if (m_stdinW != -1)
  {
    ::close(m_stdinW);
    m_stdinW = -1;
  }
  if (m_pid != -1)
  {
    int status = 0;
    ::waitpid(m_pid, &status, 0);
    m_pid = -1;
  }
  if (m_stdoutR != -1)
  {
    ::close(m_stdoutR);
    m_stdoutR = -1;
  }
#endif
  m_alive = false;
}

//-----------------------------------------------------------------------------------------

#ifdef _WIN32

bool DotProcess::start(const QCString &dotExe, const QCString &format)
{
  // Stop any writer thread left over from a previous (dead) process instance
  if (m_writer.joinable())
  {
    {
      std::unique_lock<std::mutex> lock(m_mu);
      m_writerStop = true;
      m_workCv.notify_one();
    }
    m_writer.join();
  }

  m_format = format;
  m_alive  = false;

  HANDLE stdinR  = INVALID_HANDLE_VALUE;
  HANDLE stdoutW = INVALID_HANDLE_VALUE;

  SECURITY_ATTRIBUTES sa;
  sa.nLength              = sizeof(sa);
  sa.lpSecurityDescriptor = nullptr;
  sa.bInheritHandle       = TRUE;

  if (!CreatePipe(&stdinR, &m_stdinW, &sa, 0))   return false;
  if (!CreatePipe(&m_stdoutR, &stdoutW, &sa, 0))
  {
    CloseHandle(stdinR);
    CloseHandle(m_stdinW);
    m_stdinW = INVALID_HANDLE_VALUE;
    return false;
  }

  // Parent ends must not be inherited
  SetHandleInformation(m_stdinW,  HANDLE_FLAG_INHERIT, 0);
  SetHandleInformation(m_stdoutR, HANDLE_FLAG_INHERIT, 0);

  QCString cmdLine = QCString("\"") + dotExe + "\" -T" + format;

  STARTUPINFOW si;
  ZeroMemory(&si, sizeof(si));
  si.cb         = sizeof(si);
  si.dwFlags    = STARTF_USESTDHANDLES;
  si.hStdInput  = stdinR;
  si.hStdOutput = stdoutW;
  si.hStdError  = GetStdHandle(STD_ERROR_HANDLE);

  PROCESS_INFORMATION pi;
  ZeroMemory(&pi, sizeof(pi));

  uint16_t *wcmdLine = nullptr;
  size_t wlen = Portable::recodeUtf8StringToW(cmdLine, &wcmdLine);
  bool ok = (wlen > 0) &&
            CreateProcessW(nullptr, reinterpret_cast<LPWSTR>(wcmdLine),
                           nullptr, nullptr, TRUE, 0, nullptr, nullptr, &si, &pi);
  free(wcmdLine);

  CloseHandle(stdinR);
  CloseHandle(stdoutW);

  if (!ok)
  {
    CloseHandle(m_stdinW);  m_stdinW  = INVALID_HANDLE_VALUE;
    CloseHandle(m_stdoutR); m_stdoutR = INVALID_HANDLE_VALUE;
    return false;
  }

  CloseHandle(pi.hThread);
  m_process = pi.hProcess;
  m_alive      = true;
  m_writerStop = false;
  m_writeData  = nullptr;
  m_writer = std::thread([this]() {
    std::unique_lock<std::mutex> lock(m_mu);
    while (true)
    {
      m_workCv.wait(lock, [this] { return m_writeData || m_writerStop; });
      if (m_writerStop) break;
      const std::string *data = m_writeData;
      lock.unlock();

      bool ok = true;
      int  err = 0;
      const char *ptr = data->data();
      size_t left = data->size();
      while (left > 0)
      {
        DWORD written = 0;
        if (!WriteFile(m_stdinW, ptr, static_cast<DWORD>(left), &written, nullptr))
        { err = static_cast<int>(GetLastError()); ok = false; break; }
        ptr  += written;
        left -= written;
      }

      lock.lock();
      m_writeData  = nullptr;
      m_writeOk    = ok;
      m_writeErrno = err;
      m_doneCv.notify_one();
    }
  });
  return true;
}

#else // POSIX

// On systems without pipe2(), this mutex is held across pipe()+fcntl()
// and the subsequent fork() so that no concurrent fork() can observe the
// file descriptors before O_CLOEXEC is set.  Any other code in this
// process that calls fork() should also acquire this lock.
#if !defined(__linux__)
static std::mutex g_forkMu;
#endif

bool DotProcess::start(const QCString &dotExe, const QCString &format)
{
  // Stop any writer thread left over from a previous (dead) process instance
  if (m_writer.joinable())
  {
    {
      std::unique_lock<std::mutex> lock(m_mu);
      m_writerStop = true;
      m_workCv.notify_one();
    }
    m_writer.join();
  }

  m_format = format;
  m_alive  = false;

  int stdinPipe[2]  = { -1, -1 };
  int stdoutPipe[2] = { -1, -1 };
  pid_t pid;

#if defined(__linux__)
  // pipe2 atomically creates the pipe and sets O_CLOEXEC on both ends,
  // so no lock is needed to guard against a concurrent fork().
  if (::pipe2(stdinPipe,  O_CLOEXEC) == -1) return false;
  if (::pipe2(stdoutPipe, O_CLOEXEC) == -1)
  {
    ::close(stdinPipe[0]);
    ::close(stdinPipe[1]);
    return false;
  }
  pid = ::fork();
#else
  // pipe() does not set O_CLOEXEC atomically.  Hold g_forkMu from before
  // pipe() until after fork() so that no concurrent fork() runs while any
  // of our four fds are still missing their CLOEXEC flag.
  {
    std::unique_lock<std::mutex> lock(g_forkMu);
    if (::pipe(stdinPipe)  == -1) return false;
    if (::pipe(stdoutPipe) == -1)
    {
      ::close(stdinPipe[0]);
      ::close(stdinPipe[1]);
      return false;
    }
    ::fcntl(stdinPipe[0],  F_SETFD, FD_CLOEXEC);
    ::fcntl(stdinPipe[1],  F_SETFD, FD_CLOEXEC);
    ::fcntl(stdoutPipe[0], F_SETFD, FD_CLOEXEC);
    ::fcntl(stdoutPipe[1], F_SETFD, FD_CLOEXEC);
    pid = ::fork();
    // Lock released here; child has already seen the fds with CLOEXEC set.
  }
#endif

  if (pid < 0)
  {
    ::close(stdinPipe[0]);  ::close(stdinPipe[1]);
    ::close(stdoutPipe[0]); ::close(stdoutPipe[1]);
    return false;
  }

  if (pid == 0)
  {
    // Child
    ::dup2(stdinPipe[0],  STDIN_FILENO);
    ::dup2(stdoutPipe[1], STDOUT_FILENO);
    ::close(stdinPipe[0]);  ::close(stdinPipe[1]);
    ::close(stdoutPipe[0]); ::close(stdoutPipe[1]);

    QCString fmtArg = QCString("-T") + format;
    const char *argv[] = { dotExe.data(), fmtArg.data(), nullptr };
    ::execv(dotExe.data(), const_cast<char *const *>(argv));
    // If execv returns, something went wrong
    _exit(127);
  }

  // Parent
  ::close(stdinPipe[0]);
  ::close(stdoutPipe[1]);
  m_stdinW     = stdinPipe[1];
  m_stdoutR    = stdoutPipe[0];
  m_pid        = pid;
  m_alive      = true;
  m_writerStop = false;
  m_writeData  = nullptr;
  m_writer = std::thread([this]() {
    std::unique_lock<std::mutex> lock(m_mu);
    while (true)
    {
      m_workCv.wait(lock, [this] { return m_writeData || m_writerStop; });
      if (m_writerStop) break;
      const std::string *data = m_writeData;
      lock.unlock();

      bool ok = true;
      int  err = 0;
      const char *ptr = data->data();
      size_t left = data->size();
      while (left > 0)
      {
        ssize_t n = ::write(m_stdinW, ptr, left);
        if (n <= 0) { err = errno; ok = false; break; }
        ptr  += n;
        left -= static_cast<size_t>(n);
      }

      lock.lock();
      m_writeData  = nullptr;
      m_writeOk    = ok;
      m_writeErrno = err;
      m_doneCv.notify_one();
    }
  });
  return true;
}

#endif // _WIN32

//-----------------------------------------------------------------------------------------

void DotProcess::markDeadAndReport(const char *context, int savedErrno)
{
  m_alive = false;

#ifdef _WIN32
  // Build OS error string
  std::string osErr;
  if (savedErrno)
  {
    char msgBuf[512] = {};
    FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                   nullptr, static_cast<DWORD>(savedErrno), 0,
                   msgBuf, sizeof(msgBuf), nullptr);
    // strip trailing newline that FormatMessage appends
    size_t n = strlen(msgBuf);
    while (n > 0 && (msgBuf[n-1] == '\r' || msgBuf[n-1] == '\n')) msgBuf[--n] = '\0';
    osErr = msgBuf;
  }

  // Check whether the child has already exited (STILL_ACTIVE = 259)
  if (m_process != INVALID_HANDLE_VALUE)
  {
    DWORD exitCode = STILL_ACTIVE;
    if (GetExitCodeProcess(m_process, &exitCode) && exitCode != STILL_ACTIVE)
    {
      if (osErr.empty())
        err("dot process (format '{}'): {} — process exited with code {}; will restart.\n",
            m_format, context, exitCode);
      else
        err("dot process (format '{}'): {} ({})"
            " — process exited with code {}; will restart.\n",
            m_format, context, osErr, exitCode);
      return;
    }
  }

  if (osErr.empty())
    err("dot process (format '{}'): {}; will restart.\n", m_format, context);
  else
    err("dot process (format '{}'): {} ({}); will restart.\n", m_format, context, osErr);

#else // POSIX

  // WNOHANG: reap zombie if process has already exited, otherwise just report
  int status = 0;
  pid_t ret = (m_pid != -1) ? ::waitpid(m_pid, &status, WNOHANG) : 0;
  if (ret > 0)
    m_pid = -1; // reaped

  // Build the "why the syscall failed" part
  std::string syscallErr = savedErrno ? std::string(": ") + std::strerror(savedErrno) : std::string();

  if (ret > 0)
  {
    // Child exited — report its status
    if (WIFEXITED(status))
      err("dot process (format '{}'): {}{}"
          " — process exited with code {}; will restart.\n",
          m_format, context, syscallErr, WEXITSTATUS(status));
    else if (WIFSIGNALED(status))
      err("dot process (format '{}'): {}{}"
          " — process killed by signal {} ({}); will restart.\n",
          m_format, context, syscallErr, WTERMSIG(status), ::strsignal(WTERMSIG(status)));
    else
      err("dot process (format '{}'): {}{}"
          " — process ended with status {:#x}; will restart.\n",
          m_format, context, syscallErr, status);
  }
  else
  {
    // Child still running (or no pid); syscall failure is the only clue
    err("dot process (format '{}'): {}{}; will restart.\n",
        m_format, context, syscallErr);
  }
#endif
}

//-----------------------------------------------------------------------------------------

bool DotProcess::readUntilEndMarker(std::vector<char> &buf, int &capturedErrno)
{
  capturedErrno = 0;
  EndMarker em = getEndMarker(m_format);
  if (!em.bytes) return false;

  char readBuf[4096];
  while (true)
  {
#ifdef _WIN32
    DWORD nRead = 0;
    BOOL ok = ReadFile(m_stdoutR, readBuf, sizeof(readBuf), &nRead, nullptr);
    if (!ok || nRead == 0)
    {
      capturedErrno = ok ? 0 : static_cast<int>(GetLastError());
      return false;
    }
#else
    ssize_t nRead = ::read(m_stdoutR, readBuf, sizeof(readBuf));
    if (nRead < 0)  { capturedErrno = errno; return false; }
    if (nRead == 0) { capturedErrno = 0;     return false; } // EOF
#endif
    buf.insert(buf.end(), readBuf, readBuf + nRead);

    // Check whether the end marker is present at the tail of buf.
    // The marker must end exactly at or after the newly appended bytes.
    if (buf.size() >= em.len)
    {
      // For text markers (svg, cmapx, pdf/ps) a simple suffix check suffices
      // because dot always terminates the output with the marker.
      if (std::memcmp(buf.data() + buf.size() - em.len, em.bytes, em.len) == 0)
        return true;

      // For %%EOF we also accept "%%EOF\n"
      if (m_format.startsWith("pdf") || m_format == "ps" || m_format == "eps")
      {
        // Check again without optional trailing newline already included
        static const char eof_nl[] = "%%EOF\n";
        if (buf.size() >= 6 &&
            std::memcmp(buf.data() + buf.size() - 6, eof_nl, 6) == 0)
          return true;
      }
    }
  }
}

//-----------------------------------------------------------------------------------------

bool DotProcess::render(const std::string &dotContent, const QCString &outputPath)
{
  if (!m_alive) return false;

  // Hand dotContent to the persistent writer thread (avoids fork+exec per graph
  // and the classic bidirectional-pipe deadlock).
  {
    std::unique_lock<std::mutex> lock(m_mu);
    m_writeData  = &dotContent;
    m_writeOk    = true;
    m_writeErrno = 0;
    m_workCv.notify_one();
  }

  std::vector<char> buf;
  int readErrno = 0;
  bool ok = readUntilEndMarker(buf, readErrno);

  // Wait for the writer to finish (ensures write errors are visible and the
  // dotContent reference remains valid until the writer is done with it).
  bool writeOk;
  int  writeErrno;
  {
    std::unique_lock<std::mutex> lock(m_mu);
    m_doneCv.wait(lock, [this] { return m_writeData == nullptr; });
    writeOk    = m_writeOk;
    writeErrno = m_writeErrno;
  }

  if (!ok)
  {
    markDeadAndReport("read from dot stdout failed", readErrno);
    return false;
  }
  if (!writeOk)
  {
    markDeadAndReport("write to dot stdin failed", writeErrno);
    return false;
  }

  // Write directly via OS primitives to avoid the internal lock (flockfile)
  // that libc acquires on every fwrite() call.
#ifdef _WIN32
  uint16_t *wpath = nullptr;
  Portable::recodeUtf8StringToW(outputPath, &wpath);
  HANDLE fh = CreateFileW(reinterpret_cast<LPCWSTR>(wpath),
                          GENERIC_WRITE, 0, nullptr,
                          CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
  free(wpath);
  if (fh == INVALID_HANDLE_VALUE)
  {
    markDeadAndReport(("cannot open output file '" + outputPath + "'").data(),
                      static_cast<int>(GetLastError()));
    return false;
  }
  DWORD written = 0;
  WriteFile(fh, buf.data(), static_cast<DWORD>(buf.size()), &written, nullptr);
  CloseHandle(fh);
#else
  int fd = ::open(outputPath.data(), O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC, 0666);
  if (fd == -1)
  {
    markDeadAndReport(("cannot open output file '" + outputPath + "'").data(), errno);
    return false;
  }
  const char *ptr = buf.data();
  size_t left = buf.size();
  while (left > 0)
  {
    ssize_t n = ::write(fd, ptr, left);
    if (n <= 0) break;
    ptr  += n;
    left -= static_cast<size_t>(n);
  }
  ::close(fd);
#endif
  return true;
}

//-----------------------------------------------------------------------------------------
// Thread-local process cache
//-----------------------------------------------------------------------------------------

namespace
{

struct ThreadDotProcesses
{
  std::map<std::string, DotProcess> byFormat;
};

static thread_local ThreadDotProcesses tl_dotProcs;

} // anonymous namespace

DotProcess *getDotProcess(const QCString &dotExe, const QCString &format)
{
  auto &proc = tl_dotProcs.byFormat[format.str()];
  if (!proc.isAlive())
  {
    if (!proc.start(dotExe, format)) return nullptr;
  }
  return &proc;
}
