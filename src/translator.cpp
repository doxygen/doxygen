/*! \file translator.cpp 
 *  \brief Implementation of generally used translator methods.
 * 
 * This file contains implementation of the translator methods that
 * are not expected to be reimplemented by derived translator classes.
 * It also contains static data tables used by the methods.
 *  
 */
#include "translator.h"

/*! The translation table used by Win1250ToISO88592() method. */
const char Translator::Win1250ToISO88592Tab[] = 
{
  '\x80', '\x81', '\x82', '\x83', '\x84', '\x85', '\x86', '\x87',
  '\x88', '\x89', '\xA9', '\x8B', '\xA6', '\xAB', '\xAE', '\xAC',
  '\x90', '\x91', '\x92', '\x93', '\x94', '\x2E', '\x96', '\x97',
  '\x98', '\x99', '\xB9', '\x9B', '\xB6', '\xBB', '\xBE', '\xBC',
  '\xA0', '\x20', '\x20', '\xA3', '\xA4', '\xA1', '\xA6', '\xA7',
  '\x22', '\xA9', '\xAA', '\x3C', '\xAC', '\x2D', '\xAE', '\xAF',
  '\x2E', '\x2B', '\x20', '\xB3', '\x27', '\x75', '\xB6', '\xB7',
  '\x20', '\xB1', '\xBA', '\x3E', '\xA5', '\x22', '\xB5', '\xBF',
  '\xC0', '\xC1', '\xC2', '\xC3', '\xC4', '\xC5', '\xC6', '\xC7',
  '\xC8', '\xC9', '\xCA', '\xCB', '\xCC', '\xCD', '\xCE', '\xCF',
  '\xD0', '\xD1', '\xD2', '\xD3', '\xD4', '\xD5', '\xD6', '\xD7',
  '\xD8', '\xD9', '\xDA', '\xDB', '\xDC', '\xDD', '\xDE', '\xDF',
  '\xE0', '\xE1', '\xE2', '\xE3', '\xE4', '\xE5', '\xE6', '\xE7',
  '\xE8', '\xE9', '\xEA', '\xEB', '\xEC', '\xED', '\xEE', '\xEF',
  '\xF0', '\xF1', '\xF2', '\xF3', '\xF4', '\xF5', '\xF6', '\x2D',
  '\xF8', '\xF9', '\xFA', '\xFB', '\xFC', '\xFD', '\xFE', '\xFF',
  '\0'
};


/*! The translation table used by ISO88592ToWin1250() method. */
const char Translator::ISO88592ToWin1250Tab[] = {
  '\x80', '\x81', '\x82', '\x83', '\x84', '\x85', '\x86', '\x87',
  '\x88', '\x89', '\x8A', '\x8B', '\x8C', '\x8D', '\x8E', '\x8F',
  '\x90', '\x91', '\x92', '\x93', '\x94', '\x95', '\x96', '\x97',
  '\x98', '\x99', '\x9A', '\x9B', '\x9C', '\x9D', '\x9E', '\x9F',
  '\xA0', '\xA5', '\xA2', '\xA3', '\xA4', '\xBC', '\x8C', '\xA7',
  '\xA8', '\x8A', '\xAA', '\x8D', '\x8F', '\xAD', '\x8E', '\xAF',
  '\xB0', '\xB9', '\xB2', '\xB3', '\xB4', '\xBE', '\x9C', '\xB7',
  '\xB8', '\x9A', '\xBA', '\x9D', '\x9F', '\xBD', '\x9E', '\xBF',
  '\xC0', '\xC1', '\xC2', '\xC3', '\xC4', '\xC5', '\xC6', '\xC7',
  '\xC8', '\xC9', '\xCA', '\xCB', '\xCC', '\xCD', '\xCE', '\xCF',
  '\xD0', '\xD1', '\xD2', '\xD3', '\xD4', '\xD5', '\xD6', '\xD7',
  '\xD8', '\xD9', '\xDA', '\xDB', '\xDC', '\xDD', '\xDE', '\xDF',
  '\xE0', '\xE1', '\xE2', '\xE3', '\xE4', '\xE5', '\xE6', '\xE7',
  '\xE8', '\xE9', '\xEA', '\xEB', '\xEC', '\xED', '\xEE', '\xEF',
  '\xF0', '\xF1', '\xF2', '\xF3', '\xF4', '\xF5', '\xF6', '\xF7',
  '\xF8', '\xF9', '\xFA', '\xFB', '\xFC', '\xFD', '\xFE', '\xFF',
  '\0'
};


/*! The translation table used by Koi8RToWindows1251() method. */
Q_UINT16 Translator::Koi8RToWindows1251Tab[128] = 
{ 0x2500, 0x2502, 0x250C, 0x2510, 0x2514, 0x2518, 0x251C, 0x2524,
  0x252C, 0x2534, 0x253C, 0x2580, 0x2584, 0x2588, 0x258C, 0x2590,
  0x2591, 0x2592, 0x2593, 0x2320, 0x25A0, 0x2219/**/, 0x221A, 0x2248,
  0x2264, 0x2265, 0x00A0, 0x2321, 0x00B0, 0x00B2, 0x00B7, 0x00F7,
  0x2550, 0x2551, 0x2552, 0x0451, 0x2553, 0x2554, 0x2555, 0x2556,
  0x2557, 0x2558, 0x2559, 0x255A, 0x255B, 0x255C, 0x255D, 0x255E,
  0x255F, 0x2560, 0x2561, 0x0401, 0x2562, 0x2563, 0x2564, 0x2565,
  0x2566, 0x2567, 0x2568, 0x2569, 0x256A, 0x256B, 0x256C, 0x00A9,
  0x044E, 0x0430, 0x0431, 0x0446, 0x0434, 0x0435, 0x0444, 0x0433,
  0x0445, 0x0438, 0x0439, 0x043A, 0x043B, 0x043C, 0x043D, 0x043E,
  0x043F, 0x044F, 0x0440, 0x0441, 0x0442, 0x0443, 0x0436, 0x0432,
  0x044C, 0x044B, 0x0437, 0x0448, 0x044D, 0x0449, 0x0447, 0x044A,
  0x042E, 0x0410, 0x0411, 0x0426, 0x0414, 0x0415, 0x0424, 0x0413,
  0x0425, 0x0418, 0x0419, 0x041A, 0x041B, 0x041C, 0x041D, 0x041E,
  0x041F, 0x042F, 0x0420, 0x0421, 0x0422, 0x0423, 0x0416, 0x0412,
  0x042C, 0x042B, 0x0417, 0x0428, 0x042D, 0x0429, 0x0427, 0x042A 
};


/*! The translation table used by Windows1251ToKoi8R() method. */
Q_UINT16 Translator::Windows1251ToKoi8RTab[128] = 
{ 0x0402, 0x0403, 0x201A, 0x0453, 0x201E, 0x2026, 0x2020, 0x2021,
  0x20AC, 0x2030, 0x0409, 0x2039, 0x040A, 0x040C, 0x040B, 0x040F,
  0x0452, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014,
  0xFFFD, 0x2122, 0x0459, 0x203A, 0x045A, 0x045C, 0x045B, 0x045F,
  0x00A0, 0x040E, 0x045E, 0x0408, 0x00A4, 0x0490, 0x00A6, 0x00A7,
  0x0401, 0x00A9, 0x0404, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x0407,
  0x00B0, 0x00B1, 0x0406, 0x0456, 0x0491, 0x00B5, 0x00B6, 0x00B7,
  0x0451, 0x2116, 0x0454, 0x00BB, 0x0458, 0x0405, 0x0455, 0x0457,
  0x0410, 0x0411, 0x0412, 0x0413, 0x0414, 0x0415, 0x0416, 0x0417,
  0x0418, 0x0419, 0x041A, 0x041B, 0x041C, 0x041D, 0x041E, 0x041F,
  0x0420, 0x0421, 0x0422, 0x0423, 0x0424, 0x0425, 0x0426, 0x0427,
  0x0428, 0x0429, 0x042A, 0x042B, 0x042C, 0x042D, 0x042E, 0x042F,
  0x0430, 0x0431, 0x0432, 0x0433, 0x0434, 0x0435, 0x0436, 0x0437,
  0x0438, 0x0439, 0x043A, 0x043B, 0x043C, 0x043D, 0x043E, 0x043F,
  0x0440, 0x0441, 0x0442, 0x0443, 0x0444, 0x0445, 0x0446, 0x0447,
  0x0448, 0x0449, 0x044A, 0x044B, 0x044C, 0x044D, 0x044E, 0x044F
};


/*! Returns the string converted from windows-1250 to iso-8859-2. */
/* The method was designed initially for translator_cz.h. 
 * It is used for on-line encoding conversion related to
 * conditional compilation in Unix/MS Windows environments
 * (both use different encoding).  Later, the translator_hr.h
 * (by Boris Bralo) used and improved the same style. As the
 * method with the translation table was the same, the
 * decision to move it to this base class was made. The same
 * holds for ISO88592ToWin1250() method. 
 * 
 * Alexandr Chelpanov used the same approach for
 * Koi8RToWindows1251() and Windows1251ToKoi8R() methods.  Notice,
 * that he uses Unicode tables.
 * 
 * It is recommended for possibly other similar methods in future.
 */
QCString Translator::Win1250ToISO88592(const QCString & sInput)
{
  // The conversion table for characters >127
  // 
  
  QCString result;
  int len = sInput.length();

  for (int i = 0; i < len; ++i)
  {
    unsigned int c = sInput[i];  
    result += (c > 127) ? Win1250ToISO88592Tab[c & 0x7F] : c;
  }
  return result;
}


/*! returns the string converted from iso-8859-2 to windows-1250 */
/* See the comments of the Win1250ToISO88592() method for details. */
QCString Translator::ISO88592ToWin1250(const QCString & sInput)
{
  // The conversion table for characters >127
  // 
  QCString result;
  int len = sInput.length();

  for (int i = 0; i < len; ++i)
  {
    unsigned int c = sInput[i];  
    result += (c > 127) ? ISO88592ToWin1250Tab[c & 0x7F] : c;
  }
  return result;
}


/*! Returns the string converted from koi8-r to windows-1251. */
/* The method was designed initially for translator_cz.h. 
   It is used for on-line encoding conversion related to conditional
   compilation in Unix/MS Windows environments (both use different
   encoding). Encoding table got from QT:qtextcodec.cpp
 */
QCString Translator::Koi8RToWindows1251( const QCString & sInput )
{

  QString result;
  int len = sInput.length();

  result.setUnicode(0, len);
  QChar* uc = (QChar*)result.unicode(); // const_cast
  const unsigned char * c = (const unsigned char *)(const char*)sInput;
  for( int i=0; i<len; i++ ) {
    if ( c[i] > 127 )
      uc[i] = Koi8RToWindows1251Tab[c[i]-128];
    else
      uc[i] = c[i];
  }
  return result.local8Bit();
}


/*! returns the string converted from Windows-1251 to koi8-r */
/* See the comments of the Koi8RToWindows1251() method for details.
   Encoding table got from QT:qtextcodec.cpp */
QCString Translator::Windows1251ToKoi8R( const QCString & sInput )
{
  QString result;
  int len = sInput.length();

  result.setUnicode(0, len);
  QChar* uc = (QChar*)result.unicode(); // const_cast
  const unsigned char * c = (const unsigned char *)(const char*)sInput;
  for( int i=0; i<len; i++ ) {
    if ( c[i] > 127 )
      uc[i] = Windows1251ToKoi8RTab[c[i]-128];
    else
      uc[i] = c[i];
  }
  return result.local8Bit();
}

