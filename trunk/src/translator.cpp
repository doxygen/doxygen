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
const unsigned char Translator::Koi8RToWindows1251Tab[128] =
{ 128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,
  144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,
  160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,
  176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,
  254,224,225,246,228,229,244,227,245,232,233,234,235,236,237,238,
  239,255,240,241,242,243,230,226,252,251,231,248,253,249,247,250,
  222,192,193,214,196,197,212,195,213,200,201,202,203,204,205,206,
  207,223,208,209,210,211,198,194,220,219,199,216,221,217,215,218 
};


/*! The translation table used by Windows1251ToKoi8R() method. */
const unsigned char Translator::Windows1251ToKoi8RTab[128] =
{ 128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,
  144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,
  160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,
  176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,
  225,226,247,231,228,229,246,250,233,234,235,236,237,238,239,240,
  242,243,244,245,230,232,227,254,251,253,255,249,248,252,224,241,
  193,194,215,199,196,197,214,218,201,202,203,204,205,206,207,208,
  210,211,212,213,198,200,195,222,219,221,223,217,216,220,192,209
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

  QCString result(sInput);
  int len = sInput.length();

  const unsigned char * c = (const unsigned char *)(const char*)sInput;
  unsigned char *dc = (unsigned char*)(const char*)result;
  for( int i=0; i<len; i++ ) {
    if ( c[i] > 127 )
      dc[i] = Koi8RToWindows1251Tab[c[i]-128];
  }
  return result;
}


/*! returns the string converted from Windows-1251 to koi8-r */
/* See the comments of the Koi8RToWindows1251() method for details.
   Encoding table got from QT:qtextcodec.cpp */
QCString Translator::Windows1251ToKoi8R( const QCString & sInput )
{
  QCString result(sInput);
  int len = sInput.length();

  const unsigned char * c = (const unsigned char *)(const char*)sInput;
  unsigned char *dc = (unsigned char*)(const char*)result;
  for( int i=0; i<len; i++ ) {
    if ( c[i] > 127 )
      dc[i] = Windows1251ToKoi8RTab[c[i]-128];
  }
  return result;
}

/*! returns the caracter converted from hankaku-kana to zenkakukana. 
  Thanks Yongmao Ni http://alfin.mine.utsunomiya-u.ac.jp/~niy/algo/ */
unsigned int hankaku2zen(int hankaku)
{
    static unsigned int z[64] = {
        0x2121,0x2123,0x2156,0x2157,0x2122,0x2126,0x2572,0x2521,
        0x2523,0x2525,0x2527,0x2529,0x2563,0x2565,0x2567,0x2543,
        0x213c,0x2522,0x2524,0x2526,0x2528,0x252a,0x252b,0x252d,
        0x252f,0x2531,0x2533,0x2535,0x2537,0x2539,0x253b,0x253d,
        0x253f,0x2541,0x2544,0x2546,0x2548,0x254a,0x254b,0x254c,
        0x254d,0x254e,0x254f,0x2552,0x2555,0x2558,0x255b,0x255e,
        0x255f,0x2560,0x2561,0x2562,0x2564,0x2566,0x2568,0x2569,
        0x256a,0x256b,0x256c,0x256d,0x256f,0x2573,0x212b,0x212c };

    if (hankaku < 0xa0 || hankaku > 0xdf) return 0;
    return z[hankaku - 0xa0];
}

/*! returns the character converted from japaneseEUC to SJIS
  Thanks Yongmao Ni http://alfin.mine.utsunomiya-u.ac.jp/~niy/algo/ */
unsigned int euc2sjis(unsigned int euc)
{
    unsigned int jis;
    unsigned int hib, lob;

    if ((euc & 0xff00) == 0x8e00)
        jis = hankaku2zen(euc & 0xff);
    else jis = euc & ~0x8080;
    
    hib = (jis >> 8) & 0xff;
    lob = jis & 0xff;
    lob += (hib & 1) ? 0x1f : 0x7d;
    if (lob >= 0x7f) lob++;
    hib = ((hib - 0x21) >> 1) + 0x81;
    if (hib > 0x9f) hib += 0x40;

    return (hib << 8) | lob;
}


/*! returns the string converted from Japanese-EUC to SJIS */

QCString Translator::JapaneseEucToSjis( const QCString & sInput )
{
  QString result;
  int len = sInput.length();
  int c1,c2,sj;

  result.setUnicode(0, len);
  QChar* uc = (QChar*)result.unicode(); // const_cast
  const unsigned char * c = (const unsigned char *)(const char*)sInput;
  
  for( int i=0; i<len;)
    {
      c1 = c[i];

      if( c1 == EOF ) break;
      
      /* if MSB=0 then the character is ascii */
      if(!( c1 & 0x80))
	{
	  uc[i] = c[i];
	  i=i+1;
	}
      else
	{
	  c2 = c[i+1];
	  if( c2 == EOF ) break;
	  sj     = euc2sjis( (c1 << 8) + c2 );
	  uc[i]   = sj >> 8;
	  uc[i+1] = sj & 0xff;
	  i+=2;
	}
    }

  return result.latin1();

}
