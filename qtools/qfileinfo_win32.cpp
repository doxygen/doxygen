/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2001 by Dimitri van Heesch.
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
 * Based on qfileinfo_unix.cpp 
 *
 * Copyright (C) 1992-2000 Trolltech AS.
 */

#include "qglobal.h"

#include "qfileinfo.h"
#include "qfiledefs_p.h"
#include "qdatetime.h"
#include "qdir.h"

static void reslashify( QString& n )
{
  for ( int i=0; i<(int)n.length(); i++ ) 
  {
     if ( n[i] == '/' )
          n[i] = '\\';
  }
}

void QFileInfo::slashify( QString& n )
{
  for ( int i=0; i<(int)n.length(); i++ ) 
  {
     if ( n[i] == '\\' )
          n[i] = '/';
  }
}

void QFileInfo::makeAbs( QString & )
{
  // TODO: what to do here?
  return;
}

extern bool qt_file_access( const QString& fn, int t );

/*!
  Returns TRUE if we are pointing to a real file.
  \sa isDir(), isSymLink()
*/
bool QFileInfo::isFile() const
{
    if ( !fic || !cache )
	doStat();
    return fic ? (fic->st.st_mode & STAT_MASK) == STAT_REG : FALSE;
}

/*!
  Returns TRUE if we are pointing to a directory or a symbolic link to
  a directory.
  \sa isFile(), isSymLink()
*/

bool QFileInfo::isDir() const
{
    if ( !fic || !cache )
	doStat();
    return fic ? (fic->st.st_mode & STAT_MASK) == STAT_DIR : FALSE;
}

/*!
  Returns TRUE if we are pointing to a symbolic link.
  \sa isFile(), isDir(), readLink()
*/

bool QFileInfo::isSymLink() const
{
    if ( !fic || !cache )
	doStat();
    return fic ? fic->isSymLink : FALSE;
}


/*!
  Returns the name a symlink points to, or a null QString if the
  object does not refer to a symbolic link.

  This name may not represent an existing file; it is only a string.
  QFileInfo::exists() returns TRUE if the symlink points to an
  existing file.

  \sa exists(), isSymLink(), isDir(), isFile()
*/

QString QFileInfo::readLink() const
{
    QString r;
    return r;
}

static const uint nobodyID = (uint) -2;

/*!
  Returns the owner of the file.

  On systems where files do not have owners this function returns 
  a null string.

  Note that this function can be time-consuming under UNIX. (in the order
  of milliseconds on a 486 DX2/66 running Linux).

  \sa ownerId(), group(), groupId()
*/

QString QFileInfo::owner() const
{
    return QString::null;
}

/*!
  Returns the id of the owner of the file.

  On systems where files do not have owners this function returns ((uint) -2).

  \sa owner(), group(), groupId()
*/

uint QFileInfo::ownerId() const
{
    return (uint)-2;
}

/*!
  Returns the group the file belongs to.

  On systems where files do not have groups this function always
  returns 0.

  Note that this function can be time-consuming under UNIX (in the order of
  milliseconds on a 486 DX2/66 running Linux).

  \sa groupId(), owner(), ownerId()
*/

QString QFileInfo::group() const
{
    return QString::null;
}

/*!
  Returns the id of the group the file belongs to.

  On systems where files do not have groups this function always
  returns ((uind) -2).

  \sa group(), owner(), ownerId()
*/

uint QFileInfo::groupId() const
{
    return (uint)-2;
}


/*!
  \fn bool QFileInfo::permission( int permissionSpec ) const

  Tests for file permissions.  The \e permissionSpec argument can be several
  flags of type PermissionSpec or'ed together to check for permission
  combinations.

  On systems where files do not have permissions this function always
  returns TRUE.

  Example:
  \code
    QFileInfo fi( "/tmp/tonsils" );
    if ( fi.permission( QFileInfo::WriteUser | QFileInfo::ReadGroup ) )
	qWarning( "Tonsils can be changed by me, and the group can read them.");
    if ( fi.permission( QFileInfo::WriteGroup | QFileInfo::WriteOther ) )
	qWarning( "Danger! Tonsils can be changed by the group or others!" );
  \endcode

  \sa isReadable(), isWritable(), isExecutable()
*/

bool QFileInfo::permission( int permissionSpec ) const
{
  return TRUE;
}

/*!
  Returns the file size in bytes, or 0 if the file does not exist if the size
  cannot be fetched.
*/

uint QFileInfo::size() const
{
    if ( !fic || !cache )
	doStat();
    if ( fic )
	return (uint)fic->st.st_size;
    else
	return 0;
}


/*!
  Returns the date and time when the file was last modified.
  \sa lastRead()
*/

QDateTime QFileInfo::lastModified() const
{
    QDateTime dt;
    if ( !fic || !cache )
	doStat();
    if ( fic )
	dt.setTime_t( fic->st.st_mtime );
    return dt;
}

/*!
  Returns the date and time when the file was last read (accessed).

  On systems that do not support last read times, the modification time is
  returned.

  \sa lastModified()
*/

QDateTime QFileInfo::lastRead() const
{
    QDateTime dt;
    if ( !fic || !cache )
	doStat();
    if ( fic )
	dt.setTime_t( fic->st.st_atime );
    return dt;
}


void QFileInfo::doStat() const
{
    QFileInfo *that = ((QFileInfo*)this);	// mutable function
    if ( !that->fic )
	that->fic = new QFileInfoCache;
    STATBUF *b = &that->fic->st;
    that->fic->isSymLink = FALSE;

#if defined(__CYGWIN32_)
    int r;

    r = STAT( QFile::encodeName(fn), b );

    if ( r != 0 ) {
	delete that->fic;
	that->fic = 0;
    }
#else
    QString file = fn;
    reslashify(file);
#ifdef QT_LARGEFILE_SUPPORT
    if ( _wstati64( (wchar_t*) file.ucs2(), b ) == -1 ) {
#else
    if ( _wstat( (wchar_t*) file.ucs2(), b ) == -1 ) {
#endif
      delete that->fic;
      that->fic = 0;
    }
#endif
}

/*!
  Returns the directory path of the file.

  If \e absPath is TRUE an absolute path is always returned.

  \sa dir(), filePath(), fileName(), isRelative()
*/
#ifndef QT_NO_DIR
QString QFileInfo::dirPath( bool absPath ) const
{
    QString s;
    if ( absPath )
	s = absFilePath();
    else
	s = fn;
    int pos = s.findRev( '/' );
    if ( pos == -1 ) {
	return QString::fromLatin1(".");
    } else {
	if ( pos == 0 )
	    return QString::fromLatin1( "/" );
	return s.left( pos );
    }
}
#endif
/*!
  Returns the name of the file, the file path is not included.

  Example:
  \code
     QFileInfo fi( "/tmp/abdomen.lower" );
     QString name = fi.fileName();		// name = "abdomen.lower"
  \endcode

  \sa isRelative(), filePath(), baseName(), extension()
*/

QString QFileInfo::fileName() const
{
    int p = fn.findRev( '/' );
    if ( p == -1 ) {
	return fn;
    } else {
	return fn.mid(p+1);
    }
}

/*!
  Returns the absolute path name.

  The absolute path name is the file name including the absolute path. If
  the QFileInfo is absolute (i.e. not relative) this function will return
  the same string as filePath().

  Note that this function can be time-consuming under UNIX. (in the order
  of milliseconds on a 486 DX2/66 running Linux).

  \sa isRelative(), filePath()
*/
#ifndef QT_NO_DIR
QString QFileInfo::absFilePath() const
{
    if ( QDir::isRelativePath(fn) ) {
	QString tmp = QDir::currentDirPath();
	tmp += '/';
	tmp += fn;
	makeAbs( tmp );
	return QDir::cleanDirPath( tmp );
    } else {
	QString tmp = fn;
	makeAbs( tmp );
	return QDir::cleanDirPath( tmp );
    }

}
#endif
