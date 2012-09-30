/****************************************************************************
** 
**
** Implementation of QEucCodec class
**
** Created : 981015
**
** Copyright (C)1998-2000 Trolltech AS.  All rights reserved.
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

#include "qutfcodec.h"

#ifndef QT_NO_TEXTCODEC

int QUtf8Codec::mibEnum() const
{
    return 106;
}

QCString QUtf8Codec::fromUnicode(const QString& uc, int& len_in_out) const
{
    int l = QMIN((int)uc.length(),len_in_out);
    int rlen = l*3+1;
    QCString rstr(rlen);
    uchar* cursor = (uchar*)rstr.data();
    for (int i=0; i<l; i++) {
	QChar ch = uc[i];
	if ( !ch.row() && ch.cell() < 0x80 ) {
	    *cursor++ = ch.cell();
	} else {
	    uchar b = (ch.row() << 2) | (ch.cell() >> 6);
	    if ( ch.row() < 0x08 ) {
		*cursor++ = 0xc0 | b;
	    } else {
		*cursor++ = 0xe0 | (ch.row() >> 4);
		*cursor++ = 0x80 | (b&0x3f);
	    }
	    *cursor++ = 0x80 | (ch.cell()&0x3f);
	}
    }
    len_in_out = (int)(cursor - (uchar*)rstr.data());
    rstr.truncate(len_in_out);
    return rstr;
}

const char* QUtf8Codec::name() const
{
    return "UTF-8";
}

int QUtf8Codec::heuristicContentMatch(const char* chars, int len) const
{
    int score = 0;
    for (int i=0; i<len; i++) {
	uchar ch = chars[i];
	// No nulls allowed.
	if ( !ch )
	    return -1;
	if ( ch < 128 ) {
	    // Inconclusive
	    score++;
	} else if ( (ch&0xe0) == 0xc0 ) {
	    if ( i < len-1 ) {
		uchar c2 = chars[++i];
		if ( (c2&0xc0) != 0x80 )
		    return -1;
		score+=3;
	    }
	} else if ( (ch&0xf0) == 0xe0 ) {
	    if ( i < len-1 ) {
		uchar c2 = chars[++i];
		if ( (c2&0xc0) != 0x80 ) {
		    return -1;
#if 0
		    if ( i < len-1 ) {
			uchar c3 = chars[++i];
			if ( (c3&0xc0) != 0x80 )
			    return -1;
			score+=3;
		    }
#endif
		}
		score+=2;
	    }
	}
    }
    return score;
}




class QUtf8Decoder : public QTextDecoder {
    ushort uc;
    int need;
public:
    QUtf8Decoder() : need(0)
    {
    }

    QString toUnicode(const char* chars, int len)
    {
	QString result;
	for (int i=0; i<len; i++) {
	    uchar ch = chars[i];
	    if (need) {
		if ( (ch&0xc0) == 0x80 ) {
		    uc = (uc << 6) | (ch & 0x3f);
		    need--;
		    if ( !need ) {
			result += QChar(uc);
		    }
		} else {
		    // error
		    result += QChar::replacement;
		    need = 0;
		}
	    } else {
		if ( ch < 128 ) {
		    result += QChar(ch);
		} else if ( (ch&0xe0) == 0xc0 ) {
		    uc = ch &0x1f;
		    need = 1;
		} else if ( (ch&0xf0) == 0xe0 ) {
		    uc = ch &0x0f;
		    need = 2;
		}
	    }
	}
	return result;
    }
};

QTextDecoder* QUtf8Codec::makeDecoder() const
{
    return new QUtf8Decoder;
}






int QUtf16Codec::mibEnum() const
{
    return 1000;
}

const char* QUtf16Codec::name() const
{
    return "ISO-10646-UCS-2";
}

int QUtf16Codec::heuristicContentMatch(const char* chars, int len) const
{
    uchar* uchars = (uchar*)chars;
    if ( len >= 2 && ((uchars[0] == 0xff && uchars[1] == 0xfe) ||
		      (uchars[1] == 0xff && uchars[0] == 0xfe)) )
	return len;
    else
	return 0;
}




class QUtf16Encoder : public QTextEncoder {
    bool headerdone;
public:
    QUtf16Encoder() : headerdone(FALSE)
    {
    }

    QCString fromUnicode(const QString& uc, int& len_in_out)
    {
	if ( headerdone ) {
	    len_in_out = uc.length()*(int)sizeof(QChar);
	    QCString d(len_in_out);
	    memcpy(d.data(),uc.unicode(),len_in_out);
	    return d;
	} else {
	    headerdone = TRUE;
	    len_in_out = (1+uc.length())*(int)sizeof(QChar);
	    QCString d(len_in_out);
	    memcpy(d.data(),&QChar::byteOrderMark,sizeof(QChar));
	    memcpy(d.data()+sizeof(QChar),uc.unicode(),uc.length()*sizeof(QChar));
	    return d;
	}
    }
};

class QUtf16Decoder : public QTextDecoder {
    uchar buf;
    bool half;
    bool swap;
    bool headerdone;

public:
    QUtf16Decoder() : half(FALSE), swap(FALSE), headerdone(FALSE)
    {
    }

    QString toUnicode(const char* chars, int len)
    {
	QString r;

	while ( len-- ) {
	    if ( half ) {
		QChar ch;
		if ( swap ) {
		    ch.row() = *chars++;
		    ch.cell() = buf;
		} else {
		    ch.row() = buf;
		    ch.cell() = *chars++;
		}
		if ( !headerdone ) {
		    if ( ch == QChar::byteOrderSwapped ) {
			swap = !swap;
		    } else if ( ch == QChar::byteOrderMark ) {
			// Ignore ZWNBSP
		    } else {
			r += ch;
		    }
		    headerdone = TRUE;
		} else
		    r += ch;
		half = FALSE;
	    } else {
		buf = *chars++;
		half = TRUE;
	    }
	}

	return r;
    }
};

QTextDecoder* QUtf16Codec::makeDecoder() const
{
    return new QUtf16Decoder;
}

QTextEncoder* QUtf16Codec::makeEncoder() const
{
    return new QUtf16Encoder;
}

#endif // QT_NO_TEXTCODEC
