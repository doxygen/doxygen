/****************************************************************************
** 
**
** Global feature selection
**
** Created : 000417
**
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
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

#ifndef QFEATURES_H
#define QFEATURES_H

/*! \page features....html
    ...
*/

// Qt ships with a number of pre-defined configurations. If none suit
// your needs, define QCONFIG_LOCAL and create a "qconfig-local.h" file.
//
// Note that disabling some features will produce a libqt that is not
// compatible with other libqt builds. Such modifications are only
// supported on Qt/Embedded where reducing the library size is important
// and where the application-suite is often a fixed set.
//
#if defined(QCONFIG_LOCAL)
#include <qconfig-local.h>
#elif defined(QCONFIG_MINIMAL)
#include <qconfig-minimal.h>
#elif defined(QCONFIG_SMALL)
#include <qconfig-small.h>
#elif defined(QCONFIG_MEDIUM)
#include <qconfig-medium.h>
#elif defined(QCONFIG_LARGE)
#include <qconfig-large.h>
#else // everything...
#include <qconfig.h>
#endif


// Data structures
/*!
    QStringList
*/
//#define QT_NO_STRINGLIST

#if defined(QT_NO_IMAGE_SMOOTHSCALE)
/*!
    QIconSet
*/
# define QT_NO_ICONSET
#endif

// File I/O
#if defined(QT_NO_STRINGLIST)
    /*!
	QDir
    */
# define QT_NO_DIR
#endif

/*!
  Palettes
*/
//#define QT_NO_PALETTE

/*!
    QTextStream
*/
//#define QT_NO_TEXTSTREAM
/*!
    QDataStream
*/
//#define QT_NO_DATASTREAM

/*!
  Dynamic module linking
*/
//#define QT_NO_PLUGIN


// Images
/*!
    BMP image I/O
    <p>The Windows Bitmap (BMP) image format is common on MS-Windows.
    <p>This is an uncompressed image format
    offering few advantages over PNG or JPEG.
*/
#if defined(QT_NO_DATASTREAM)
# define QT_NO_IMAGEIO_BMP
#endif
/*!
    PPM image I/O
    <p>The Portable PixMap (PPM) image format is common on Unix.
    <p>This is an uncompressed image format
    offering few advantages over PNG or JPEG.
*/
//#define QT_NO_IMAGEIO_PPM
/*!
    XBM image I/O
    <p>The X11 BitMap (XBM) image format is common on X11.
    <p>This is an uncompressed monochrome image format.
    Qt uses this format for some internal images (eg. mouse cursors).
*/
//#define QT_NO_IMAGEIO_XBM
/*!
    XPM image I/O
    <p>The X11 PixMap (XPM) image format is common on X11.
    <p>This is an uncompressed image format.
    XPM images have the small advantage that they can be trivially
    included in source files as they are C code.
    Qt uses this format for some internal images (eg. QMessageBox icons).
*/
#if defined(QT_NO_TEXTSTREAM)
# define QT_NO_IMAGEIO_XPM
#endif
/*!
    PNG image I/O
    <p>The Portable Network Graphics (PNG) is a compressed image format.
    <p>See <a href=http://www.libpng.org/pub/png/>The PNG Home Site</a> for
    details of the format.
*/
//#define QT_NO_IMAGEIO_PNG
/*!
    MNG image I/O
    <p>The Multiple-image Network Graphics (MNG) is a compressed animation format.
    <p>See <a href=http://www.libpng.org/pub/mng/>The MNG Home Site</a> for
    details of the format.
*/
//#define QT_NO_IMAGEIO_MNG
/*!
    JPEG image I/O
    <p>The Joint Photographic Experts Group (JPEG) is a compressed lossy image format that gives high compression
	for real-world and photo-realistic images.
*/
//#define QT_NO_IMAGEIO_JPEG

/*!
    Asynchronous I/O
    <p>Allows push-driven data processing.
*/
//#define QT_NO_ASYNC_IO
/*!
    Asynchronous image I/O
    <p>Allows push-driven images.
*/
//#define QT_NO_ASYNC_IMAGE_IO
#if defined(QT_NO_ASYNC_IO) || defined(QT_NO_ASYNC_IMAGE_IO)
    /*!
	Animated images
	<p>This includes animated GIFs.
	<p><b>Note: this currently also requires <tt>QT_BUILTIN_GIF_READER</tt> to
	be defined when building Qt.</b>
    */
# define QT_NO_MOVIE
#endif

// Fonts
/*!
    TrueType font files
    <p>Scalable font format common on MS-Windows and becoming common on Unix.
    <p>Only supported on Qt/Embedded.
*/
//#define QT_NO_TRUETYPE
/*!
    BDF font files
    <p>The Bitmap Distribution Format (BDF) font file format, common
	on Unix.
    <p>Only supported on Qt/Embedded.
*/
#if defined(QT_NO_TEXTSTREAM) || defined(QT_NO_STRINGLIST)
# define QT_NO_BDF
#endif
/*!
    QFontDatabase
*/
#if defined(QT_NO_STRINGLIST)
# define QT_NO_FONTDATABASE
#endif

// Internationalization

/*!
    QObject::tr()
*/
#if defined(QT_NO_DATASTREAM)
# define QT_NO_TRANSLATION
#endif

/*!
    QTextCodec class and subclasses
*/
//#define QT_NO_TEXTCODEC

#if defined(QT_NO_TEXTCODEC)
    /*!
	QTextCodec classes
	<p>This includes some large conversion tables.
    */
# define QT_NO_CODECS
#endif
#if defined(QT_LITE_UNICODE)
    /*!
	Unicode property tables
	<p>These include some large tables.
    */
# define QT_NO_UNICODETABLES
#endif

/*!
    MIME
*/
#if defined(QT_NO_DIR)
# define QT_NO_MIME
#endif
#if defined(QT_NO_MIME) || defined(QT_NO_TEXTSTREAM) || defined(QT_NO_DRAWUTIL) || defined(QT_NO_IMAGE_SMOOTHSCALE)
    /*!
	RichText (HTML) display
    */
# define QT_NO_RICHTEXT
#endif

/*!
  XML
*/
#if defined(QT_NO_STRINGLIST) || defined(QT_NO_TEXTSTREAM) || defined(QT_NO_TEXTCODEC)
# define QT_NO_XML
#endif

/*!
  Document Object Model
*/
#if defined(QT_NO_XML) || defined(QT_NO_MIME)
# define QT_NO_DOM
#endif

// Sound
/*!
    Playing sounds
*/
//#define QT_NO_SOUND

/*!
    Properties
*/
#if defined(QT_NO_STRINGLIST) || defined(QT_NO_ICONSET)
# define QT_NO_PROPERTIES
#endif



// Networking

/*!
  Network support
*/
//#define QT_NO_NETWORK

#if defined(QT_NO_NETWORK) || defined(QT_NO_STRINGLIST) || defined(QT_NO_TEXTSTREAM)
   /*!
     DNS
   */
# define QT_NO_DNS
#endif
/*!
    Network file access
*/
#if defined(QT_NO_NETWORK) || defined(QT_NO_DIR) || defined(QT_NO_STRINGLIST)
# define QT_NO_NETWORKPROTOCOL
#endif
#if defined(QT_NO_NETWORKPROTOCOL) || defined(QT_NO_DNS)
    /*!
	FTP file access
    */
# define QT_NO_NETWORKPROTOCOL_FTP
    /*!
	HTTP file access
    */
# define QT_NO_NETWORKPROTOCOL_HTTP
#endif

/*!
  External process invocation.
*/
//#define QT_NO_PROCESS


// Qt/Embedded-specific

#if defined(QT_NO_NETWORK)
    /*!
      Multi-process support.
    */
# define QT_NO_QWS_MULTIPROCESS
#endif

#if defined(QT_NO_QWS_MULTIPROCESS) || defined(QT_NO_DATASTREAM)
    /*!
      Palmtop Communication Protocol
    */
# define QT_NO_COP
#endif

/*!
    Console keyboard support
*/
//#define QT_NO_QWS_KEYBOARD

/*!
    Visible cursor
*/
#if defined(QT_NO_CURSOR)
# define QT_NO_QWS_CURSOR
#endif

/*!
    Alpha-blended cursor
*/
//#define QT_NO_QWS_ALPHA_CURSOR
/*!
    Mach64 acceleration
*/
//#define QT_NO_QWS_MACH64
/*!
    Voodoo3 acceleration
*/
//#define QT_NO_QWS_VOODOO3
/*!
    Matrox MGA acceleration (Millennium/Millennium II/Mystique/G200/G400)
*/
//#define QT_NO_QWS_MATROX
/*!
    Virtual frame buffer
*/
                                        
//#define QT_NO_QWS_VFB
/*!
    Transformed frame buffer
*/
//#define QT_NO_QWS_TRANSFORMED
#if defined(QT_NO_NETWORK)
/*!
    Remote frame buffer (VNC)
*/
# ifndef QT_NO_QWS_VNC
#  define QT_NO_QWS_VNC
# endif
#endif
/*!
    1-bit monochrome
*/
//#define QT_NO_QWS_DEPTH_1
/*!
    4-bit greyscale
*/
//#define QT_NO_QWS_DEPTH_4
/*!
    4-bit VGA
*/
//#define QT_NO_QWS_VGA_16
/*!
    SVGALib Support
    Not implemented yet
*/
#define QT_NO_QWS_SVGALIB
/*!
    8-bit grayscale
*/
#define QT_NO_QWS_DEPTH_8GRAYSCALE
/*!
    8-bit color
*/
//#define QT_NO_QWS_DEPTH_8
/*!
    15 or 16-bit color (define QT_QWS_DEPTH16_RGB as 555 for 15-bit)
*/
//#define QT_NO_QWS_DEPTH_16
/*!
    24-bit color
*/
//#define QT_NO_QWS_DEPTH_24
/*!
    32-bit color
*/
//#define QT_NO_QWS_DEPTH_32

/*!
    Window Manager
*/
//#define QT_NO_QWS_MANAGER

/*!
    Window Manager Styles
*/
#define QT_NO_QWS_KDE2_WM_STYLE
#if defined( QT_NO_QWS_MANAGER ) || defined( QT_NO_IMAGEIO_XPM )
# define QT_NO_QWS_AQUA_WM_STYLE
# define QT_NO_QWS_BEOS_WM_STYLE
# define QT_NO_QWS_KDE_WM_STYLE
# define QT_NO_QWS_QPE_WM_STYLE
# define QT_NO_QWS_WINDOWS_WM_STYLE
#endif

/*!
    Saving of fonts
*/
//#define QT_NO_QWS_SAVEFONTS

/*!
    Favour code size over graphics speed
    <p>Smaller, slower code will be used for drawing operations.
    <p>Only supported on Qt/Embedded.
*/
//#define QT_NO_QWS_GFX_SPEED

/*!
    Qt/Embedded window system properties.
*/
//#define QT_NO_QWS_PROPERTIES

#if defined(QT_NO_QWS_PROPERTIES) || defined(QT_NO_MIME)
    /*!
      Cut and paste
    */
# define QT_NO_CLIPBOARD
#endif

#if defined(QT_NO_MIME) || defined(QT_NO_QWS_PROPERTIES)
    /*!
	Drag and drop
    */
# define QT_NO_DRAGANDDROP
#endif

#if defined(QT_NO_PROPERTIES)
    /*!
	SQL
    */
# define QT_NO_SQL
#endif

#if defined(QT_NO_CLIPBOARD) || defined(QT_NO_MIME) || defined(_WS_QWS_)
    /*!
	Cut and paste of complex data types (non-text)
	Not yet implemented for QWS. 
    */
# define QT_NO_MIMECLIPBOARD
#endif


/*!
    Drawing utility functions
*/
//#define QT_NO_DRAWUTIL
/*!
    TrueColor QImage
*/
//#define QT_NO_IMAGE_TRUECOLOR
/*!
    Smooth QImage scaling
*/
//#define QT_NO_IMAGE_SMOOTHSCALE
/*!
    Image file text strings
*/
#if defined(QT_NO_STRINGLIST)
# define QT_NO_IMAGE_TEXT
#endif

#if defined(QT_NO_IMAGE_TRUECOLOR)
    /*!
	16-bit QImage
    */
# define QT_NO_IMAGE_16_BIT
#endif
/*!
    Cursors
*/
//#define QT_NO_CURSOR

// Painting
/*!
    Named colors
*/
//#define QT_NO_COLORNAMES
/*!
    Scaling and rotation
*/
//#define QT_NO_TRANSFORMATIONS

/*!
    Printing
*/
#if defined(QT_NO_TEXTSTREAM)
# define QT_NO_PRINTER
#endif

/*!
    QPicture
*/
#if defined(QT_NO_DATASTREAM)
# define QT_NO_PICTURE
#endif

// Layout
/*!
    Automatic widget layout
*/
//#define QT_NO_LAYOUT

// Widgets
#if defined(QT_NO_DRAWUTIL) || defined(QT_NO_PALETTE)
/*!
    QStyle
*/
# define QT_NO_STYLE
#endif


/*!
  Dialogs
*/
//#define QT_NO_DIALOG
/*!
  Semi-modal dialogs
*/
//#define QT_NO_SEMIMODAL
/*!
  Framed widgets
*/
//#define QT_NO_FRAME

/*!
  Special widget effects (fading, scrolling)
*/
//#define QT_NO_EFFECTS


/*!
  QLabel
*/
#ifdef QT_NO_FRAME
# define QT_NO_LABEL
#endif

/*!
  Toolbars
*/
#ifdef QT_NO_LAYOUT
# define QT_NO_TOOLBAR
#endif

/*!
  Buttons
*/
#if defined(QT_NO_BUTTON) || defined(QT_NO_STYLE)
/*!
  Check-boxes
*/
# define QT_NO_CHECKBOX
/*!
  Radio-buttons
*/
# define QT_NO_RADIOBUTTON
#endif
#if defined(QT_NO_BUTTON) || defined(QT_NO_TOOLBAR) || defined(QT_NO_ICONSET)
/*!
  Tool-buttons
*/
# define QT_NO_TOOLBUTTON
#endif
/*!
  Grid layout widgets
*/
#ifdef QT_NO_FRAME
# define QT_NO_GRID
#endif
/*!
  Group boxes
*/
#ifdef QT_NO_FRAME
# define QT_NO_GROUPBOX
#endif
#if defined(QT_NO_GROUPBOX)
/*!
  Button groups
*/
# define QT_NO_BUTTONGROUP
/*!
  Horizontal group boxes
*/
# define QT_NO_HGROUPBOX
#endif
#if defined(QT_NO_HGROUPBOX)
/*!
  Vertical group boxes
*/
# define QT_NO_VGROUPBOX
#endif
#if defined(QT_NO_BUTTONGROUP)
/*!
  Horizontal button groups
*/
#  define QT_NO_HBUTTONGROUP
#endif
#if defined(QT_NO_HBUTTONGROUP)
/*!
  Vertical button groups
*/
#   define QT_NO_VBUTTONGROUP
#endif
/*!
  Horizonal box layout widgets
*/
#ifdef QT_NO_FRAME
# define QT_NO_HBOX
#endif
#if defined(QT_NO_HBOX)
/*!
  Vertical box layout widgets
*/
# define QT_NO_VBOX
#endif
/*!
  Single-line edits
*/
#if defined(QT_NO_PALETTE)
# define QT_NO_LINEEDIT
#endif
#if defined(QT_NO_TOOLBAR)
/*!
  Main-windows
*/
# define QT_NO_MAINWINDOW
#endif
#if defined(QT_NO_ICONSET)
/*!
  Menu-like widgets
*/
# define QT_NO_MENUDATA
#endif
#if defined(QT_NO_MENUDATA)
/*!
  Popup-menus
*/
# define QT_NO_POPUPMENU
/*!
  Menu bars
*/
# define QT_NO_MENUBAR
#endif
#if defined(QT_NO_BUTTON) || defined(QT_NO_ICONSET) || defined(QT_NO_POPUPMENU)
/*!
  Push-buttons
*/
# define QT_NO_PUSHBUTTON
#endif
/*!
  Progress bars
*/
#ifdef QT_NO_FRAME
# define QT_NO_PROGRESSBAR
#endif
/*!
  Range-control widgets
*/
//#define QT_NO_RANGECONTROL
#if defined(QT_NO_RANGECONTROL) || defined(QT_NO_STYLE)
/*!
  Scroll bars
*/
# define QT_NO_SCROLLBAR
/*!
  Sliders
*/
# define QT_NO_SLIDER
/*!
  Spin boxes
*/
# define QT_NO_SPINBOX
/*!
  Dials
*/
# define QT_NO_DIAL
#endif


#if defined(QT_NO_SCROLLBAR) || defined(QT_NO_FRAME)
/*!
  Scrollable view widgets
*/
# define QT_NO_SCROLLVIEW
#endif
#if defined(QT_NO_SCROLLVIEW)
/*!
    QCanvas
*/
# define QT_NO_CANVAS
/*!
    QIconView
*/
# define QT_NO_ICONVIEW
#endif

#if defined(QT_NO_SCROLLBAR)
/*!
  Table-like widgets
*/
# define QT_NO_TABLEVIEW
#endif
#if defined(QT_NO_TABLEVIEW)
/*!
  Multi-line edits
*/
# define QT_NO_MULTILINEEDIT
#endif

/*!
  Splitters
*/
#ifdef QT_NO_FRAME
# define QT_NO_SPLITTER
#endif
/*!
  Status bars
*/
#ifdef QT_NO_LAYOUT
# define QT_NO_STATUSBAR
#endif
/*!
  Tab-bars
*/
#if defined(QT_NO_ICONSET)
# define QT_NO_TABBAR
#endif
#if defined(QT_NO_TABBAR)
/*!
  Tab widgets
*/
# define QT_NO_TABWIDGET
#endif
/*!
  Tool tips
*/
#if defined( QT_NO_LABEL ) || defined( QT_NO_PALETTE )
# define QT_NO_TOOLTIP
#endif
/*!
  Input validators
*/
//#define QT_NO_VALIDATOR
/*!
  "What's this" help
*/
#if defined( QT_NO_TOOLTIP )
# define QT_NO_WHATSTHIS
#endif
/*!
  Widget stacks
*/
#ifdef QT_NO_FRAME
# define QT_NO_WIDGETSTACK
#endif

#if defined(QT_NO_RICHTEXT) || defined(QT_NO_SCROLLVIEW)
    /*!
	QTextView
    */
# define QT_NO_TEXTVIEW
#endif

#if defined(QT_NO_TEXTVIEW)
    /*!
	QTextBrowser
    */
# define QT_NO_TEXTBROWSER
#endif

#if defined(QT_NO_STYLE)
    /*!
	Windows style
    */
# define QT_NO_STYLE_WINDOWS
    /*!
	Motif style
    */
# define QT_NO_STYLE_MOTIF
#endif

#if defined(QT_NO_STYLE_MOTIF)
    /*!
	Motif-plus style
    */
# define QT_NO_STYLE_MOTIFPLUS
#endif


#if defined(QT_NO_SCROLLVIEW) || defined(QT_NO_STRINGLIST)
    /*!
	QListBox
    */
# define QT_NO_LISTBOX
#endif

/*!
    QAccel
*/
//#define QT_NO_ACCEL

/*!
    QSizeGrip
*/
#ifdef QT_NO_PALETTE
# define QT_NO_SIZEGRIP
#endif
/*!
    QHeader
*/
#ifdef QT_NO_ICONSET
# define QT_NO_HEADER
#endif
/*!
    QWorkSpace
*/
#ifdef QT_NO_FRAME
# define QT_NO_WORKSPACE
#endif
/*!
    QLCDNumber
*/
#ifdef QT_NO_FRAME
# define QT_NO_LCDNUMBER
#endif
/*!
    QAction
*/
//#define QT_NO_ACTION

#if defined(QT_NO_HEADER)
    /*!
	QTable
    */
# define QT_NO_TABLE
#endif

#if defined(QT_NO_LISTBOX)
    /*!
	QComboBox
    */
# define QT_NO_COMBOBOX
#endif    

#if defined(QT_NO_HEADER) || defined(QT_NO_SCROLLVIEW)
    /*!
	QListView
    */
# define QT_NO_LISTVIEW
#endif

#if defined(QT_NO_STYLE_WINDOWS)
    /*!
	Compact Windows style
    */
# define QT_NO_STYLE_COMPACT
#endif

#if defined(QT_NO_STYLE_MOTIF) || defined(QT_NO_TRANSFORMATIONS)
    /*!
	CDE style
    */
# define QT_NO_STYLE_CDE
    /*!
	SGI style
    */
# define QT_NO_STYLE_SGI
#endif
#if defined(QT_NO_STYLE_WINDOWS)
    /*!
	Platinum style
    */
# define QT_NO_STYLE_PLATINUM
#endif

/*!
    QColorDialog
*/
#if defined(QT_NO_LAYOUT) || defined(QT_NO_LABEL) || defined(QT_NO_PUSHBUTTON) || defined(QT_NO_DIALOG)
# define QT_NO_COLORDIALOG
#endif
#if defined(QT_NO_DIALOG)
/*!
    QMessageBox
*/
# define QT_NO_MESSAGEBOX
#endif
#if defined(QT_NO_DIALOG) || defined(QT_NO_TABBAR)
/*!
    QTabDialog
*/
#define QT_NO_TABDIALOG
#endif

#if defined(QT_NO_DIALOG)
/*!
    QWizard
*/
# define QT_NO_WIZARD
#endif

#if defined(QT_NO_DIALOG) || defined(QT_NO_LISTVIEW) || defined(QT_NO_NETWORKPROTOCOL) || defined(QT_NO_COMBOBOX) || defined(QT_NO_DIR) || defined(QT_NO_MESSAGEBOX) || defined(QT_NO_SEMIMODAL)
    /*!
	QFileDialog
    */
# define QT_NO_FILEDIALOG
#endif

#if defined(QT_NO_DIALOG) || defined(QT_NO_FONTDATABASE) || defined(QT_NO_COMBOBOX)
    /*!
	QFontDialog
    */
# define QT_NO_FONTDIALOG
#endif

#if defined(QT_NO_DIALOG) || defined(QT_NO_LISTVIEW) || defined(QT_NO_PRINTER) || defined(QT_NO_COMBOBOX) || defined(QT_NO_DIR) || defined(QT_NO_LAYOUT) || defined(QT_NO_LABEL)
    /*!
	QPrintDialog
    */
# define QT_NO_PRINTDIALOG
#endif

#if defined(QT_NO_SEMIMODAL)
    /*!
	QProgressDialog
    */
# define QT_NO_PROGRESSDIALOG
#endif
#if defined(QT_NO_DIALOG) || defined(QT_NO_COMBOBOX)
    /*!
	QInputDialog
    */
# define QT_NO_INPUTDIALOG
#endif

#if defined(QT_NO_STRINGLIST)
    /*!
	Session management support
    */
# define QT_NO_SESSIONMANAGER
#endif

#endif // QFEATURES_H
