/****************************************************************************
** 
**
** Common macros and system include files for QFile, QFileInfo and QDir.
**
** Created : 930812
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of the tools module of the Qt GUI Toolkit.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition or Qt Professional Edition
** licenses may use this file in accordance with the Qt Commercial License
** Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/qpl/ for QPL licensing information.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef QFILEDEFS_P_H
#define QFILEDEFS_P_H


//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of qfile.cpp, qfileinfo.cpp and qdir.cpp.
// This header file may change from version to version without notice,
// or even be removed.
//
//
#if defined(_CC_MWERKS_)
# include <stdlib.h>
# include <stat.h>
#elif !defined(_OS_MAC_) 
# include <sys/types.h>
# include <sys/stat.h>
#elif defined(_OS_MAC_)
# include <sys/types.h>
# include <sys/stat.h>
# define _OS_UNIX_
#endif
#include <fcntl.h>
#include <errno.h>
#if defined(_OS_UNIX_)
# include <dirent.h>
# include <unistd.h>
#endif
#if defined(_OS_MSDOS_) || defined(_OS_WIN32_) || defined(_OS_OS2_)
# define _OS_FATFS_
# if defined(__CYGWIN32__)
#  include <dirent.h>
#  include <unistd.h>
#  if !defined(_OS_UNIX_)
#   define _OS_UNIX_
#  endif
# else
#  include <io.h>
#  if !defined(_CC_MWERKS_)
#   include <dos.h>
#  endif
#  include <direct.h>
# endif
#endif
#include <limits.h>


#if !defined(PATH_MAX)
#if defined( MAXPATHLEN )
#define PATH_MAX MAXPATHLEN
#else
#define PATH_MAX 1024
#endif
#endif


#undef STATBUF
#undef STAT
#undef STAT_REG
#undef STAT_DIR
#undef STAT_LNK
#undef STAT_MASK
#undef FILENO
#undef OPEN
#undef CLOSE
#undef LSEEK
#undef READ
#undef WRITE
#undef ACCESS
#undef GETCWD
#undef CHDIR
#undef MKDIR
#undef RMDIR
#undef OPEN_RDONLY
#undef OPEN_WRONLY
#undef OPEN_CREAT
#undef OPEN_TRUNC
#undef OPEN_APPEND
#undef OPEN_TEXT
#undef OPEN_BINARY


#if defined(_CC_MSVC_) || defined(_CC_SYM_)

# define STATBUF	struct _stat		// non-ANSI defs
# define STATBUF4TSTAT	struct _stat		// non-ANSI defs
# define STAT		::_stat
# define FSTAT		::_fstat
# define STAT_REG	_S_IFREG
# define STAT_DIR	_S_IFDIR
# define STAT_MASK	_S_IFMT
# if defined(_S_IFLNK)
#  define STAT_LNK	_S_IFLNK
# endif
# define FILENO		_fileno
# define OPEN		::_open
# define CLOSE		::_close
# define LSEEK		::_lseek
# define READ		::_read
# define WRITE		::_write
# define ACCESS		::_access
# define GETCWD		::_getcwd
# define CHDIR		::_chdir
# define MKDIR		::_mkdir
# define RMDIR		::_rmdir
# define OPEN_RDONLY	_O_RDONLY
# define OPEN_WRONLY	_O_WRONLY
# define OPEN_RDWR	_O_RDWR
# define OPEN_CREAT	_O_CREAT
# define OPEN_TRUNC	_O_TRUNC
# define OPEN_APPEND	_O_APPEND
# if defined(O_TEXT)
#  define OPEN_TEXT	_O_TEXT
#  define OPEN_BINARY	_O_BINARY
# endif

#elif defined(_CC_BOR_) && __BORLANDC__ >= 0x550

# define STATBUF	struct stat		// non-ANSI defs
# define STATBUF4TSTAT	struct _stat		// non-ANSI defs
# define STAT		::stat
# define FSTAT		::fstat
# define STAT_REG	_S_IFREG
# define STAT_DIR	_S_IFDIR
# define STAT_MASK	_S_IFMT
# if defined(_S_IFLNK)
#  define STAT_LNK	_S_IFLNK
# endif
# define FILENO		_fileno
# define OPEN		::open
# define CLOSE		::_close
# define LSEEK		::_lseek
# define READ		::_read
# define WRITE		::_write
# define ACCESS		::_access
# define GETCWD		::_getcwd
# define CHDIR		::chdir
# define MKDIR		::_mkdir
# define RMDIR		::_rmdir
# define OPEN_RDONLY	_O_RDONLY
# define OPEN_WRONLY	_O_WRONLY
# define OPEN_RDWR	_O_RDWR
# define OPEN_CREAT	_O_CREAT
# define OPEN_TRUNC	_O_TRUNC
# define OPEN_APPEND	_O_APPEND
# if defined(O_TEXT)
#  define OPEN_TEXT	_O_TEXT
#  define OPEN_BINARY	_O_BINARY
# endif

#else						// all other systems

# define STATBUF	struct stat
# define STATBUF4TSTAT	struct stat
# define STAT		::stat
# define FSTAT		::fstat
# define STAT_REG	S_IFREG
# define STAT_DIR	S_IFDIR
# define STAT_MASK	S_IFMT
# if defined(S_IFLNK)
#  define STAT_LNK	S_IFLNK
# endif
# define FILENO		fileno
# define OPEN		::open
# define CLOSE		::close
# define LSEEK		::lseek
# define READ		::read
# define WRITE		::write
# define ACCESS		::access
# if defined(_OS_OS2EMX_)
#  define GETCWD	::_getcwd2
#  define CHDIR		::_chdir2
# else
#  define GETCWD	::getcwd
#  define CHDIR		::chdir
# endif
# define MKDIR		::mkdir
# define RMDIR		::rmdir
# define OPEN_RDONLY	O_RDONLY
# define OPEN_WRONLY	O_WRONLY
# define OPEN_RDWR	O_RDWR
# define OPEN_CREAT	O_CREAT
# define OPEN_TRUNC	O_TRUNC
# define OPEN_APPEND	O_APPEND
# if defined(O_TEXT)
#  define OPEN_TEXT	O_TEXT
#  define OPEN_BINARY	O_BINARY
# endif
#endif

#if defined(_CC_MWERKS_)
#undef mkdir
#undef MKDIR
#define MKDIR _mkdir
#undef rmdir
#undef RMDIR
#define RMDIR _rmdir
#endif


#if defined(_OS_FATFS_)
# define F_OK	0
# define X_OK	1
# define W_OK	2
# define R_OK	4
#endif

#if defined(_OS_MAC_) && !defined(_OS_UNIX_)
# define F_OK	0
# define X_OK	1
# define W_OK	2
# define R_OK	4
#endif

struct QFileInfoCache
{
    STATBUF st;
    bool isSymLink;
};

#endif
