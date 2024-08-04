/******************************************************************************
 *
 *
 *
 *
 * Copyright (C) 1997-2015 by Dimitri van Heesch.
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

#include "doxygen.h"

/**
 * \brief code added by paule32, to check for chm viewer instances under MS-Windows 10, 11 is open.
 * \details
 * If you compile a CHM HTML Project, you would not be inform, if the WebViewer instance is open.
 * And as such, Doxygen produce undefined behavoiur. To workaround this, you get a message where
 * you can choose, if you would work through, or exit the Doxygen App.
 *
 * 1. CreateToolhelp32Snapshot:
 *    This funtion create a SnapShot of all running processes in the System.
 * 2. PROCESSENTRY32:
 *    A structure, they holds informations about the process.
 * 3. Process32First, and Process32Next:
 *    This function's will be used to iterate through all processes that are in the SnapShot.
 * 4. _tcscmp:
 *    A function, that compares two string's. In this case, the given string will be check, if a process
 *    with this name is available.
 */
#if defined(_WIN32) || defined(_WIN64)
# include <windows.h>
# include <tlhelp32.h>
# include <tchar.h>
# include <stdio.h>
# include <iostream>
# include <string>
using namespace std;

BOOL IsProcessRunning(const TCHAR *processName, DWORD *processId) {
    BOOL exists = FALSE;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 processEntry = {0};
    processEntry.dwSize = sizeof(PROCESSENTRY32);
    if (Process32First(snapshot, &processEntry)) {
        do {
            if (_tcscmp(processEntry.szExeFile, processName) == 0) {
                *processId = processEntry.th32ProcessID;
                exists = TRUE;
                break;
            }
        }
        while (Process32Next(snapshot, &processEntry));
    }
    CloseHandle(snapshot);
    return exists;
}
void TerminateProcessById(DWORD processId) {
    HANDLE processHandle = OpenProcess(PROCESS_TERMINATE, FALSE, processId);
    if (processHandle != NULL) {
        TerminateProcess(processHandle, 0);
        CloseHandle(processHandle);
    }
}
#endif

/*! \file
 *  \brief main entry point for doxygen
 *
 *  This file contains main()
 */

/*! Default main. The idea of separating this from the rest of doxygen,
 *  is to make it possible to write your own main, with a different
 *  generateOutput() function for instance.
 */
int main(int argc,char **argv)
{
  /**
    * \brief code added by paule32, to check for chm viewer instances under
    *        MS-Windows 10, 11 is open ...
    */
  #if defined(_WIN32) || defined(_WIN64)
  const TCHAR *processName = _T("msedgewebview2.exe");
  DWORD processId = 0;
  if (IsProcessRunning(processName, &processId)) {
      cout << "The HTML Viewer is already open.\nTry to close it..." << endl;
      TerminateProcessById(processId);
  }
  #endif
  initDoxygen();
  readConfiguration(argc,argv);
  checkConfiguration();
  adjustConfiguration();
  parseInput();
  generateOutput();
  return 0;
}
