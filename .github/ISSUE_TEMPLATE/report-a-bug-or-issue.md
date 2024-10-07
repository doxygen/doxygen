---
name: Report a bug or issue
about: Create a report to help us improve doxygen
title: ''
labels: ''
assignees: ''

---

**Describe the bug**
Describe what you see that (you think) is wrong.

**Screenshots**
If useful, add screenshots to help explain your problem.

**To Reproduce**
Attach a self contained example that allows us to reproduce the problem.
Such an example typically exist of some source code (can be dummy code) and a doxygen configuration file used (you can strip it using `doxygen -s -u`). After you verified the example demonstrates the problem, put it in a zip (or tarball) and attach it to the bug report. Try to avoid linking to external sources, since they might disappear in the future.

**Expected behavior**
Describe what you would have expected or think is correct.

**Version**
Mention the version of doxygen used (output of `doxygen --version`) and the platform on which you run doxygen (e.g. Windows 10, 64 bit). If you run doxygen under Linux please also mention the name and version of the distribution used (output of `lsb_release -a`) and mention if you compiled doxygen yourself or that you use a binary that comes with the distribution or from the doxygen website.

**Stack trace**
If you encounter a crash and can build doxygen from sources yourself with debug info (`-DCMAKE_BUILD_TYPE=Debug`), a stack trace can be very helpful (especially if it is not possible to capture the problem in a small example that can be shared).

**Additional context**
Add any other context about the problem here.
