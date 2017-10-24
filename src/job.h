/* Definitions for managing subprocesses in GNU Make.
Copyright (C) 1992-2013 Free Software Foundation, Inc.
This file is part of GNU Make.

GNU Make is free software; you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation; either version 3 of the License, or (at your option) any later
version.

GNU Make is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program.  If not, see <http://www.gnu.org/licenses/>.  */

#ifdef HAVE_FCNTL_H
# include <fcntl.h>
#else
# include <sys/file.h>
#endif

enum cmd_state {
	cs_not_started = 0,     /* Not yet started.  Must be 0!  */
	cs_deps_running,        /* Dep commands running.  */
	cs_running,             /* Commands running.  */
	cs_finished,             /* Commands finished.  */
	us_failed
};

/* Structure describing a running or dead child process.  */

struct child
  {
    struct child *next;         /* Link in the chain.  */
    void (*command_ptr)();          /* Ptr into command_lines[command_line].  */

    const char *comname;              /* Temporary command file name */
    unsigned int command_state;

    pid_t         pid;          /* Child process's ID number.  */
  };

void job_spawn (const char*name,void (*fptr)());
void cleanup_jobserver();
