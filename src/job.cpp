/* Job execution and handling for GNU Make.
   Copyright (C) 1988-2013 Free Software Foundation, Inc.
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

#include "job.h"
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/prctl.h>

int g_job_slots = 1;
static sigset_t fatal_signal_set;
extern bool g_successfulRun;

# define FD_STDIN       (fileno (stdin))
# define FD_STDOUT      (fileno (stdout))
# define FD_STDERR      (fileno (stderr))

#define EINTRLOOP(_v,_c)   while (((_v)=_c)==-1 && errno==EINTR)
#define WAIT_NOHANG(status)   wait3 ((status), WNOHANG, (struct rusage *) 0)
#define WAIT_T int
/* Different systems have different requirements for pid_t.
   Plus we have to support gettext string translation... Argh.  */
	static const char *
pid2str (pid_t pid)
{
	static char pidstring[100];
	sprintf (pidstring, "%lu", (unsigned long) pid);
	return pidstring;
}


static void block_sigs (void);
static void unblock_sigs (void);


/* Chain of all live (or recently deceased) children.  */
struct child *children = 0;

/* Number of children currently running.  */
static unsigned int job_slots_used = 0;



/* Reap all dead children, storing the returned status and the new command
   state ('cs_finished') in the 'file' member of the 'struct child' for the
   dead child, and removing the child from the chain.  In addition, if BLOCK
   nonzero, we block in this function until we've reaped at least one
   complete child, waiting for it to die if necessary.  If ERR is nonzero,
   print an error message first.  */


	void
reap_children (int block, int err)
{
	WAIT_T status;
	/* Initially, assume we have some.  */
	int reap_more = 1;

	/* As long as:

	   We have at least one child outstanding OR a shell function in progress,
	   AND
	   We're blocking for a complete child OR there are more children to reap

	   we'll keep reaping children.  */

	while ((children != NULL) && (block || reap_more))
	{
		pid_t pid;
		int exit_code, exit_sig, coredump;
		struct child *lastc, *c;
		int child_failed;

		if (block)
			EINTRLOOP(pid, wait (&status));
		else
			pid = WAIT_NOHANG (&status);

		if (pid < 0)
		{
			/* The wait*() failed miserably.  Punt.  */
			fprintf(stderr, "failed %s\n", strerror(errno));
			assert(0=="wait");
		}
		else if (pid > 0)
		{
			/* We got a child exit; chop the status word up.  */
			exit_code = WEXITSTATUS (status);
			exit_sig = WIFSIGNALED (status) ? WTERMSIG (status) : 0;
			coredump = WCOREDUMP (status);
		}
		else
		{
			/* No local children are dead.  */
			reap_more = 0;

			if (!block)
				break;

		}

		child_failed = exit_sig != 0 || exit_code != 0;

		/* Search for a child matching the deceased one.  */
		lastc = 0;
		for (c = children; c != 0; lastc = c, c = c->next) {
			if (c->pid == pid) break;
		}

		if (c == 0)
			/* An unknown child died.
			   Ignore it; it was inherited from our invoker.  */
			continue;


		if (child_failed)
			fprintf(stderr, "JOB Reaping losing child %p PID %s\n",
					c, pid2str (c->pid));
		else
			fprintf(stderr, "JOB Reaping winning child %p PID %s\n",
					c, pid2str (c->pid));

		/* There are no more commands.  We got through them all
		   without an unignored error.  Now the target has been
		   successfully updated.  */
		c->command_state = cs_finished;

		/* At this point c->file->update_status is success or failed.  But
		   c->file->command_state is still cs_running if all the commands
		   ran; notice_finish_file looks for cs_running to tell it that
		   it's interesting to check the file's modtime again now.  */

		fprintf(stderr, "JOB Removing child %p PID %s from chain.\n",
				c, pid2str (c->pid));

		/* Block fatal signals while frobnicating the list, so that
		   children and job_slots_used are always consistent.  Otherwise
		   a fatal signal arriving after the child is off the chain and
		   before job_slots_used is decremented would believe a child was
		   live and call reap_children again.  */
		block_sigs ();

		/* There is now another slot open.  */
		if (job_slots_used > 0)
			--job_slots_used;

		/* Remove the child from the chain and free it.  */
		if (lastc == 0)
			children = c->next;
		else
			lastc->next = c->next;

		delete c;

		unblock_sigs ();

		/* If the job failed, and the -k flag was not given, die,
		   unless we are already in the process of dying.  */
		if (!err && child_failed )
		{
			fprintf(stderr, "JOB * EXIT 2\n");
			exit (2);
		}

		/* Only block for one child.  */
		block = 0;
	}

	return;
}


	void
block_sigs ()
{
	(void) sigprocmask (SIG_BLOCK, &fatal_signal_set, (sigset_t *) 0);
}

	void
unblock_sigs ()
{
	sigset_t empty;
	sigemptyset (&empty);
	sigprocmask (SIG_SETMASK, &empty, (sigset_t *) 0);
}

	void
job_noop (int)
{
}


/* Start a job to run the commands specified in CHILD.
   CHILD is updated to reflect the commands and ID of the child process.

NOTE: On return fatal signals are blocked!  The caller is responsible
for calling 'unblock_sigs', once the new child is safely on the chain so
it can be cleaned up in the event of a fatal signal.  */

	static void
start_job_command (struct child *child)
{
	/* If we have a completely empty commandset, stop now.  */
	if (!child->command_ptr) {
		child->command_state = cs_finished;
		return;
	}

	/* Fork the child process.  */
	block_sigs ();

	child->pid = fork ();
	if (child->pid == 0)
	{
		/* We are the child side.  */
		unblock_sigs ();
		prctl(PR_SET_NAME, child->comname);

		fprintf(stderr, "JOB Child(%s) is running\n", child->comname);
		child->command_ptr();
		fprintf(stderr, "JOB Child(%s) is about to exit\n",child->comname);
		g_successfulRun = 1;
		exit(0);
	}
	else if (child->pid < 0)
	{
		/* Fork failed!  */
		unblock_sigs ();
		fprintf(stderr, "JOB fork");
		goto error;
	}
	/* We are the parent side.  Set the state to
	   say the commands are running and return.  */

	child->command_state = cs_running;

	return;

error:
	child->command_state = us_failed;
}

/* Try to start a child running.
   Returns nonzero if the child was started (and maybe finished), or zero if
   the load was too high and the child was put on the 'waiting_jobs' chain.  */

	static int
start_waiting_job (struct child *c)
{
	/* Start the first command; reap_children will run later command lines.  */
	start_job_command (c);

	switch (c->command_state)
	{
		case cs_running:
			c->next = children;
			children = c;
			/* One more job slot is in use.  */
			++job_slots_used;
			unblock_sigs ();
			break;

		case cs_finished:
			delete c;
			break;

		default:
			assert (c->command_state == cs_finished);
			break;
	}

	return 1;
}

/* Create a 'struct child' for FILE and start its commands running.  */

void
job_spawn (const char*name, void (*fptr)())
{
	if (g_job_slots == 1) {
		fptr();
		return;
	}
	/* Start the command sequence, record it in a new
	   'struct child', and add that to the chain.  */
	struct child *c;
	c = new child;
	c->command_ptr = fptr;
	c->comname = name;

	/* Wait for a job slot to be freed up.*/
	while (job_slots_used == g_job_slots)
		reap_children (1, 1);

	start_waiting_job (c);
}



void cleanup_jobserver()
{
	while (job_slots_used > 0)
		reap_children(1,1);
	fprintf(stderr, "all done'\n");
}
