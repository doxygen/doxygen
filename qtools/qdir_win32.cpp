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
 * Based on qdir_unix.cpp 
 *
 * Copyright (C) 1992-2000 Trolltech AS.
 */


#include "qglobal.h"

#include "qdir.h"
#ifndef QT_NO_DIR


#include "qfileinfo.h"
#include "qfiledefs_p.h"
#include "qregexp.h"
#include "qstringlist.h"
#include <stdlib.h>
#include <ctype.h>
#if defined(_OS_WIN32_)
#if defined(_CC_BOOL_DEF_)
#undef  bool
#include <windows.h>
#define bool int
#else
#include <windows.h>
#endif
#endif
#if defined(_OS_OS2EMX_)
extern Q_UINT32 DosQueryCurrentDisk(Q_UINT32*,Q_UINT32*);
#define NO_ERROR 0
#endif

extern QStringList qt_makeFilterList( const QString &filter );

extern int qt_cmp_si_sortSpec;

#if defined(Q_C_CALLBACKS)
extern "C" {
#endif

extern int qt_cmp_si( const void *, const void * );

#if defined(Q_C_CALLBACKS)
}
#endif


void QDir::slashify( QString& n )
{
  for ( int i=0; i<(int)n.length(); i++ ) 
  {
     if ( n[i] == '\\' )
          n[i] = '/';
  }
}

QString QDir::homeDirPath()
{
    QString d;
    d = QFile::decodeName(getenv("HOME"));
    slashify( d );
    if ( d.isNull() )
	d = rootDirPath();
    return d;
}

QString QDir::canonicalPath() const
{
    QString r;

    char cur[PATH_MAX];
    char tmp[PATH_MAX];
    GETCWD( cur, PATH_MAX );
    if ( CHDIR(QFile::encodeName(dPath)) >= 0 ) {
	GETCWD( tmp, PATH_MAX );
	r = QFile::decodeName(tmp);
    }
    CHDIR( cur );

    slashify( r );
    return r;
}

bool QDir::mkdir( const QString &dirName, bool acceptAbsPath ) const
{
#if defined(__CYGWIN32_)
    return MKDIR( QFile::encodeName(filePath(dirName,acceptAbsPath)), 0777 ) 
	== 0;
#else
    return MKDIR( QFile::encodeName(filePath(dirName,acceptAbsPath)) ) == 0;

#endif
}

bool QDir::rmdir( const QString &dirName, bool acceptAbsPath ) const
{
    return RMDIR( QFile::encodeName(filePath(dirName,acceptAbsPath)) ) == 0;
}

bool QDir::isReadable() const
{
    return ACCESS( QFile::encodeName(dPath), R_OK ) == 0;
}

bool QDir::isRoot() const
{
    return dPath == QString::fromLatin1("/");
}

bool QDir::rename( const QString &name, const QString &newName,
		   bool acceptAbsPaths	)
{
    if ( name.isEmpty() || newName.isEmpty() ) {
#if defined(CHECK_NULL)
	qWarning( "QDir::rename: Empty or null file name(s)" );
#endif
	return FALSE;
    }
    QString fn1 = filePath( name, acceptAbsPaths );
    QString fn2 = filePath( newName, acceptAbsPaths );
    return ::rename( QFile::encodeName(fn1),
		     QFile::encodeName(fn2) ) == 0;
}

bool QDir::setCurrent( const QString &path )
{
    int r;
    r = CHDIR( QFile::encodeName(path) );
    return r >= 0;
}

QString QDir::currentDirPath()
{
    QString result;

    STATBUF st;
    if ( STAT( ".", &st ) == 0 ) {
	char currentName[PATH_MAX];
	if ( GETCWD( currentName, PATH_MAX ) != 0 )
	    result = QFile::decodeName(currentName);
#if defined(DEBUG)
	if ( result.isNull() )
	    qWarning( "QDir::currentDirPath: getcwd() failed" );
#endif
    } else {
#if defined(DEBUG)
	qWarning( "QDir::currentDirPath: stat(\".\") failed" );
#endif
    }
    slashify( result );
    return result;
}

QString QDir::rootDirPath()
{
    QString d = QString::fromLatin1( "/" );
    return d;
}

bool QDir::isRelativePath( const QString &path )
{
    int len = path.length();
    if ( len == 0 )
	return TRUE;
    int i = 0;
    if ( isalpha(path[0]) && path[1] == ':' )           // drive, e.g. a:
        i = 2;
    return path[i] != '/' && path[i] != '\\';
}

#undef  IS_SUBDIR
#undef  IS_RDONLY
#undef  IS_ARCH
#undef  IS_HIDDEN
#undef  IS_SYSTEM
#undef  FF_GETFIRST
#undef  FF_GETNEXT
#undef  FF_ERROR

#if defined(_OS_WIN32_)
#define IS_SUBDIR   FILE_ATTRIBUTE_DIRECTORY
#define IS_RDONLY   FILE_ATTRIBUTE_READONLY
#define IS_ARCH     FILE_ATTRIBUTE_ARCHIVE
#define IS_HIDDEN   FILE_ATTRIBUTE_HIDDEN
#define IS_SYSTEM   FILE_ATTRIBUTE_SYSTEM
#define FF_GETFIRST FindFirstFile
#define FF_GETNEXT  FindNextFile
#define FF_ERROR    INVALID_HANDLE_VALUE
#else
#define IS_SUBDIR   _A_SUBDIR
#define IS_RDONLY   _A_RDONLY
#define IS_ARCH     _A_ARCH
#define IS_HIDDEN   _A_HIDDEN
#define IS_SYSTEM   _A_SYSTEM
#define FF_GETFIRST _findfirst
#define FF_GETNEXT  _findnext
#define FF_ERROR    -1
#endif


bool QDir::readDirEntries( const QString &nameFilter,
			   int filterSpec, int sortSpec )
{
    int i;
    if ( !fList ) {
	fList  = new QStringList;
	CHECK_PTR( fList );
	fiList = new QFileInfoList;
	CHECK_PTR( fiList );
	fiList->setAutoDelete( TRUE );
    } else {
	fList->clear();
	fiList->clear();
    }

    QStringList filters = qt_makeFilterList( nameFilter );

    bool doDirs	    = (filterSpec & Dirs)	!= 0;
    bool doFiles    = (filterSpec & Files)	!= 0;
    bool noSymLinks = (filterSpec & NoSymLinks) != 0;
    bool doReadable = (filterSpec & Readable)	!= 0;
    bool doWritable = (filterSpec & Writable)	!= 0;
    bool doExecable = (filterSpec & Executable) != 0;
    bool doHidden   = (filterSpec & Hidden)	!= 0;
    // show hidden files if the user asks explicitly for e.g. .*
    if ( !doHidden && !nameFilter.isEmpty() && nameFilter[0] == '.' )
         doHidden = TRUE;
    bool doModified = (filterSpec & Modified)   != 0;
    bool doSystem   = (filterSpec & System)     != 0;

    QRegExp   wc( nameFilter.data(), FALSE, TRUE );    // wild card, case insensitive
    bool      first = TRUE;
    QString   p = dPath.copy();
    int       plen = p.length();
#if defined(_OS_WIN32_)
    HANDLE    ff;
    WIN32_FIND_DATA finfo;
#else
    long      ff;
    _finddata_t finfo;
#endif
    QFileInfo fi;
    if ( plen == 0 )
    {
#if defined(CHECK_NULL)
      warning( "QDir::readDirEntries: No directory name specified" );
#endif
      return FALSE;
    }
    if ( p.at(plen-1) != '/' && p.at(plen-1) != '\\' )
          p += '/';
    p += "*.*";

    ff = FF_GETFIRST( p.data(), &finfo );
    if ( ff == FF_ERROR ) 
    {
#if defined(DEBUG)
      warning( "QDir::readDirEntries: Cannot read the directory: %s",
                           (const char *)dPath );
#endif
    return FALSE;
    }
    
    while ( TRUE ) 
    {
	if ( first )
	    first = FALSE;
	else 
        {
#if defined(_OS_WIN32_)
	    if ( !FF_GETNEXT(ff,&finfo) )
		break;
#else
	    if ( FF_GETNEXT(ff,&finfo) == -1 )
		break;
#endif
	}
#if defined(_OS_WIN32_)
	int  attrib = finfo.dwFileAttributes;
#else
	int  attrib = finfo.attrib;
#endif
	bool isDir	= (attrib & IS_SUBDIR) != 0;
	bool isFile	= !isDir;
	bool isSymLink	= FALSE;
	bool isReadable = TRUE;
	bool isWritable = (attrib & IS_RDONLY) == 0;
	bool isExecable = FALSE;
	bool isModified = (attrib & IS_ARCH)   != 0;
	bool isHidden	= (attrib & IS_HIDDEN) != 0;
	bool isSystem	= (attrib & IS_SYSTEM) != 0;

#if defined(_OS_WIN32_)
	const char *fname = finfo.cFileName;
#else
	const char *fname = finfo.name;
#endif
	if ( wc.match(fname) == -1 && !(allDirs && isDir) )
	    continue;

	QString name = fname;
	if ( doExecable ) 
        {
	    QString ext = name.right(4).lower();
	    if ( ext == ".exe" || ext == ".com" || ext == ".bat" ||
		 ext == ".pif" || ext == ".cmd" )
		isExecable = TRUE;
	}

	if  ( (doDirs && isDir) || (doFiles && isFile) ) 
        {
	    if ( noSymLinks && isSymLink )
		continue;
	    if ( (filterSpec & RWEMask) != 0 )
		if ( (doReadable && !isReadable) ||
		     (doWritable && !isWritable) ||
		     (doExecable && !isExecable) )
		    continue;
	    if ( doModified && !isModified )
		continue;
	    if ( !doHidden && isHidden )
		continue;
	    if ( !doSystem && isSystem )
		continue;
	    fi.setFile( *this, name );
	    fiList->append( new QFileInfo( fi ) );
	}
    }
#if defined(_OS_WIN32_)
    FindClose( ff );
#else
    _findclose( ff );
#endif

    // Sort...
    QDirSortItem* si= new QDirSortItem[fiList->count()];
    QFileInfo* itm;
    i=0;
    for (itm = fiList->first(); itm; itm = fiList->next())
	si[i++].item = itm;
    qt_cmp_si_sortSpec = sortSpec;
    qsort( si, i, sizeof(si[0]), qt_cmp_si );
    // put them back in the list
    fiList->setAutoDelete( FALSE );
    fiList->clear();
    int j;
    for ( j=0; j<i; j++ ) {
	fiList->append( si[j].item );
	fList->append( si[j].item->fileName() );
    }
    delete [] si;
    fiList->setAutoDelete( TRUE );

    if ( filterSpec == (FilterSpec)filtS && sortSpec == (SortSpec)sortS &&
	 nameFilter == nameFilt )
	dirty = FALSE;
    else
	dirty = TRUE;
    return TRUE;
}

const QFileInfoList * QDir::drives()
{
    // at most one instance of QFileInfoList is leaked, and this variable
    // points to that list
    static QFileInfoList * knownMemoryLeak = 0;

    if ( !knownMemoryLeak ) {
	knownMemoryLeak = new QFileInfoList;

#if defined(_OS_WIN32_)
	Q_UINT32 driveBits = (Q_UINT32) GetLogicalDrives() & 0x3ffffff;
#elif defined(_OS_OS2EMX_)
	Q_UINT32 driveBits, cur;
	if (DosQueryCurrentDisk(&cur,&driveBits) != NO_ERROR)
	    exit(1);
	driveBits &= 0x3ffffff;
#endif
	char driveName[4];
	qstrcpy( driveName, "a:/" );
	while( driveBits ) {
	    if ( driveBits & 1 )
		knownMemoryLeak->append( new QFileInfo( driveName ) );
	    driveName[0]++;
	    driveBits = driveBits >> 1;
	}
    }

    return knownMemoryLeak;
}
#endif //QT_NO_DIR
