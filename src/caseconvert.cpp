// This is generated code
// see github issue 8375 and the accompanying pull request for more information

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "caseconvert.h"
int StrToUprExt(unsigned char* pString, CaseBuffer *cb)
{
  size_t pStrLen = strlen((char *)pString);
  if (cb->curBufLen < pStrLen + 40)
  {
    cb->outBuf = (unsigned char *)realloc(cb->outBuf,pStrLen + 41);
    cb->curBufLen = pStrLen + 40;
  }

  size_t curLen = 0;
  unsigned char tmpBuf[4];
  if (pString && *pString)
  {
    size_t replacementLen = 0;
    unsigned char* p = pString;
    while (*p)
    {
      replacementLen = 0;
      switch(*(p+0))
      {
        case 0x61: tmpBuf[0] = 0x41; replacementLen = 1; break;
        case 0x62: tmpBuf[0] = 0x42; replacementLen = 1; break;
        case 0x63: tmpBuf[0] = 0x43; replacementLen = 1; break;
        case 0x64: tmpBuf[0] = 0x44; replacementLen = 1; break;
        case 0x65: tmpBuf[0] = 0x45; replacementLen = 1; break;
        case 0x66: tmpBuf[0] = 0x46; replacementLen = 1; break;
        case 0x67: tmpBuf[0] = 0x47; replacementLen = 1; break;
        case 0x68: tmpBuf[0] = 0x48; replacementLen = 1; break;
        case 0x69: tmpBuf[0] = 0x49; replacementLen = 1; break;
        case 0x6a: tmpBuf[0] = 0x4a; replacementLen = 1; break;
        case 0x6b: tmpBuf[0] = 0x4b; replacementLen = 1; break;
        case 0x6c: tmpBuf[0] = 0x4c; replacementLen = 1; break;
        case 0x6d: tmpBuf[0] = 0x4d; replacementLen = 1; break;
        case 0x6e: tmpBuf[0] = 0x4e; replacementLen = 1; break;
        case 0x6f: tmpBuf[0] = 0x4f; replacementLen = 1; break;
        case 0x70: tmpBuf[0] = 0x50; replacementLen = 1; break;
        case 0x71: tmpBuf[0] = 0x51; replacementLen = 1; break;
        case 0x72: tmpBuf[0] = 0x52; replacementLen = 1; break;
        case 0x73: tmpBuf[0] = 0x53; replacementLen = 1; break;
        case 0x74: tmpBuf[0] = 0x54; replacementLen = 1; break;
        case 0x75: tmpBuf[0] = 0x55; replacementLen = 1; break;
        case 0x76: tmpBuf[0] = 0x56; replacementLen = 1; break;
        case 0x77: tmpBuf[0] = 0x57; replacementLen = 1; break;
        case 0x78: tmpBuf[0] = 0x58; replacementLen = 1; break;
        case 0x79: tmpBuf[0] = 0x59; replacementLen = 1; break;
        case 0x7a: tmpBuf[0] = 0x5a; replacementLen = 1; break;
        case 0xc3:
          switch(*(p+1))
          {
            case 0x9f: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xba; tmpBuf[2] = 0x9e; replacementLen = 3; break;
            case 0xa0: tmpBuf[0] = 0xc3; tmpBuf[1] = 0x80; replacementLen = 2; break;
            case 0xa1: tmpBuf[0] = 0xc3; tmpBuf[1] = 0x81; replacementLen = 2; break;
            case 0xa2: tmpBuf[0] = 0xc3; tmpBuf[1] = 0x82; replacementLen = 2; break;
            case 0xa3: tmpBuf[0] = 0xc3; tmpBuf[1] = 0x83; replacementLen = 2; break;
            case 0xa4: tmpBuf[0] = 0xc3; tmpBuf[1] = 0x84; replacementLen = 2; break;
            case 0xa5: tmpBuf[0] = 0xc3; tmpBuf[1] = 0x85; replacementLen = 2; break;
            case 0xa6: tmpBuf[0] = 0xc3; tmpBuf[1] = 0x86; replacementLen = 2; break;
            case 0xa7: tmpBuf[0] = 0xc3; tmpBuf[1] = 0x87; replacementLen = 2; break;
            case 0xa8: tmpBuf[0] = 0xc3; tmpBuf[1] = 0x88; replacementLen = 2; break;
            case 0xa9: tmpBuf[0] = 0xc3; tmpBuf[1] = 0x89; replacementLen = 2; break;
            case 0xaa: tmpBuf[0] = 0xc3; tmpBuf[1] = 0x8a; replacementLen = 2; break;
            case 0xab: tmpBuf[0] = 0xc3; tmpBuf[1] = 0x8b; replacementLen = 2; break;
            case 0xac: tmpBuf[0] = 0xc3; tmpBuf[1] = 0x8c; replacementLen = 2; break;
            case 0xad: tmpBuf[0] = 0xc3; tmpBuf[1] = 0x8d; replacementLen = 2; break;
            case 0xae: tmpBuf[0] = 0xc3; tmpBuf[1] = 0x8e; replacementLen = 2; break;
            case 0xaf: tmpBuf[0] = 0xc3; tmpBuf[1] = 0x8f; replacementLen = 2; break;
            case 0xb0: tmpBuf[0] = 0xc3; tmpBuf[1] = 0x90; replacementLen = 2; break;
            case 0xb1: tmpBuf[0] = 0xc3; tmpBuf[1] = 0x91; replacementLen = 2; break;
            case 0xb2: tmpBuf[0] = 0xc3; tmpBuf[1] = 0x92; replacementLen = 2; break;
            case 0xb3: tmpBuf[0] = 0xc3; tmpBuf[1] = 0x93; replacementLen = 2; break;
            case 0xb4: tmpBuf[0] = 0xc3; tmpBuf[1] = 0x94; replacementLen = 2; break;
            case 0xb5: tmpBuf[0] = 0xc3; tmpBuf[1] = 0x95; replacementLen = 2; break;
            case 0xb6: tmpBuf[0] = 0xc3; tmpBuf[1] = 0x96; replacementLen = 2; break;
            case 0xb8: tmpBuf[0] = 0xc3; tmpBuf[1] = 0x98; replacementLen = 2; break;
            case 0xb9: tmpBuf[0] = 0xc3; tmpBuf[1] = 0x99; replacementLen = 2; break;
            case 0xba: tmpBuf[0] = 0xc3; tmpBuf[1] = 0x9a; replacementLen = 2; break;
            case 0xbb: tmpBuf[0] = 0xc3; tmpBuf[1] = 0x9b; replacementLen = 2; break;
            case 0xbc: tmpBuf[0] = 0xc3; tmpBuf[1] = 0x9c; replacementLen = 2; break;
            case 0xbd: tmpBuf[0] = 0xc3; tmpBuf[1] = 0x9d; replacementLen = 2; break;
            case 0xbe: tmpBuf[0] = 0xc3; tmpBuf[1] = 0x9e; replacementLen = 2; break;
            case 0xbf: tmpBuf[0] = 0xc5; tmpBuf[1] = 0xb8; replacementLen = 2; break;
            default:
              break;
          }
          break;
        case 0xc4:
          switch(*(p+1))
          {
            case 0x81: tmpBuf[0] = 0xc4; tmpBuf[1] = 0x80; replacementLen = 2; break;
            case 0x83: tmpBuf[0] = 0xc4; tmpBuf[1] = 0x82; replacementLen = 2; break;
            case 0x85: tmpBuf[0] = 0xc4; tmpBuf[1] = 0x84; replacementLen = 2; break;
            case 0x87: tmpBuf[0] = 0xc4; tmpBuf[1] = 0x86; replacementLen = 2; break;
            case 0x89: tmpBuf[0] = 0xc4; tmpBuf[1] = 0x88; replacementLen = 2; break;
            case 0x8b: tmpBuf[0] = 0xc4; tmpBuf[1] = 0x8a; replacementLen = 2; break;
            case 0x8d: tmpBuf[0] = 0xc4; tmpBuf[1] = 0x8c; replacementLen = 2; break;
            case 0x8f: tmpBuf[0] = 0xc4; tmpBuf[1] = 0x8e; replacementLen = 2; break;
            case 0x91: tmpBuf[0] = 0xc4; tmpBuf[1] = 0x90; replacementLen = 2; break;
            case 0x93: tmpBuf[0] = 0xc4; tmpBuf[1] = 0x92; replacementLen = 2; break;
            case 0x95: tmpBuf[0] = 0xc4; tmpBuf[1] = 0x94; replacementLen = 2; break;
            case 0x97: tmpBuf[0] = 0xc4; tmpBuf[1] = 0x96; replacementLen = 2; break;
            case 0x99: tmpBuf[0] = 0xc4; tmpBuf[1] = 0x98; replacementLen = 2; break;
            case 0x9b: tmpBuf[0] = 0xc4; tmpBuf[1] = 0x9a; replacementLen = 2; break;
            case 0x9d: tmpBuf[0] = 0xc4; tmpBuf[1] = 0x9c; replacementLen = 2; break;
            case 0x9f: tmpBuf[0] = 0xc4; tmpBuf[1] = 0x9e; replacementLen = 2; break;
            case 0xa1: tmpBuf[0] = 0xc4; tmpBuf[1] = 0xa0; replacementLen = 2; break;
            case 0xa3: tmpBuf[0] = 0xc4; tmpBuf[1] = 0xa2; replacementLen = 2; break;
            case 0xa5: tmpBuf[0] = 0xc4; tmpBuf[1] = 0xa4; replacementLen = 2; break;
            case 0xa7: tmpBuf[0] = 0xc4; tmpBuf[1] = 0xa6; replacementLen = 2; break;
            case 0xa9: tmpBuf[0] = 0xc4; tmpBuf[1] = 0xa8; replacementLen = 2; break;
            case 0xab: tmpBuf[0] = 0xc4; tmpBuf[1] = 0xaa; replacementLen = 2; break;
            case 0xad: tmpBuf[0] = 0xc4; tmpBuf[1] = 0xac; replacementLen = 2; break;
            case 0xaf: tmpBuf[0] = 0xc4; tmpBuf[1] = 0xae; replacementLen = 2; break;
            case 0xb1: tmpBuf[0] = 0x49; replacementLen = 1; break;
            case 0xb3: tmpBuf[0] = 0xc4; tmpBuf[1] = 0xb2; replacementLen = 2; break;
            case 0xb5: tmpBuf[0] = 0xc4; tmpBuf[1] = 0xb4; replacementLen = 2; break;
            case 0xb7: tmpBuf[0] = 0xc4; tmpBuf[1] = 0xb6; replacementLen = 2; break;
            case 0xba: tmpBuf[0] = 0xc4; tmpBuf[1] = 0xb9; replacementLen = 2; break;
            case 0xbc: tmpBuf[0] = 0xc4; tmpBuf[1] = 0xbb; replacementLen = 2; break;
            case 0xbe: tmpBuf[0] = 0xc4; tmpBuf[1] = 0xbd; replacementLen = 2; break;
            default:
              break;
          }
          break;
        case 0xc5:
          switch(*(p+1))
          {
            case 0x80: tmpBuf[0] = 0xc4; tmpBuf[1] = 0xbf; replacementLen = 2; break;
            case 0x82: tmpBuf[0] = 0xc5; tmpBuf[1] = 0x81; replacementLen = 2; break;
            case 0x84: tmpBuf[0] = 0xc5; tmpBuf[1] = 0x83; replacementLen = 2; break;
            case 0x86: tmpBuf[0] = 0xc5; tmpBuf[1] = 0x85; replacementLen = 2; break;
            case 0x88: tmpBuf[0] = 0xc5; tmpBuf[1] = 0x87; replacementLen = 2; break;
            case 0x8b: tmpBuf[0] = 0xc5; tmpBuf[1] = 0x8a; replacementLen = 2; break;
            case 0x8d: tmpBuf[0] = 0xc5; tmpBuf[1] = 0x8c; replacementLen = 2; break;
            case 0x8f: tmpBuf[0] = 0xc5; tmpBuf[1] = 0x8e; replacementLen = 2; break;
            case 0x91: tmpBuf[0] = 0xc5; tmpBuf[1] = 0x90; replacementLen = 2; break;
            case 0x93: tmpBuf[0] = 0xc5; tmpBuf[1] = 0x92; replacementLen = 2; break;
            case 0x95: tmpBuf[0] = 0xc5; tmpBuf[1] = 0x94; replacementLen = 2; break;
            case 0x97: tmpBuf[0] = 0xc5; tmpBuf[1] = 0x96; replacementLen = 2; break;
            case 0x99: tmpBuf[0] = 0xc5; tmpBuf[1] = 0x98; replacementLen = 2; break;
            case 0x9b: tmpBuf[0] = 0xc5; tmpBuf[1] = 0x9a; replacementLen = 2; break;
            case 0x9d: tmpBuf[0] = 0xc5; tmpBuf[1] = 0x9c; replacementLen = 2; break;
            case 0x9f: tmpBuf[0] = 0xc5; tmpBuf[1] = 0x9e; replacementLen = 2; break;
            case 0xa1: tmpBuf[0] = 0xc5; tmpBuf[1] = 0xa0; replacementLen = 2; break;
            case 0xa3: tmpBuf[0] = 0xc5; tmpBuf[1] = 0xa2; replacementLen = 2; break;
            case 0xa5: tmpBuf[0] = 0xc5; tmpBuf[1] = 0xa4; replacementLen = 2; break;
            case 0xa7: tmpBuf[0] = 0xc5; tmpBuf[1] = 0xa6; replacementLen = 2; break;
            case 0xa9: tmpBuf[0] = 0xc5; tmpBuf[1] = 0xa8; replacementLen = 2; break;
            case 0xab: tmpBuf[0] = 0xc5; tmpBuf[1] = 0xaa; replacementLen = 2; break;
            case 0xad: tmpBuf[0] = 0xc5; tmpBuf[1] = 0xac; replacementLen = 2; break;
            case 0xaf: tmpBuf[0] = 0xc5; tmpBuf[1] = 0xae; replacementLen = 2; break;
            case 0xb1: tmpBuf[0] = 0xc5; tmpBuf[1] = 0xb0; replacementLen = 2; break;
            case 0xb3: tmpBuf[0] = 0xc5; tmpBuf[1] = 0xb2; replacementLen = 2; break;
            case 0xb5: tmpBuf[0] = 0xc5; tmpBuf[1] = 0xb4; replacementLen = 2; break;
            case 0xb7: tmpBuf[0] = 0xc5; tmpBuf[1] = 0xb6; replacementLen = 2; break;
            case 0xba: tmpBuf[0] = 0xc5; tmpBuf[1] = 0xb9; replacementLen = 2; break;
            case 0xbc: tmpBuf[0] = 0xc5; tmpBuf[1] = 0xbb; replacementLen = 2; break;
            case 0xbe: tmpBuf[0] = 0xc5; tmpBuf[1] = 0xbd; replacementLen = 2; break;
            case 0xbf: tmpBuf[0] = 0x53; replacementLen = 1; break;
            default:
              break;
          }
          break;
        case 0xc6:
          switch(*(p+1))
          {
            case 0x80: tmpBuf[0] = 0xc9; tmpBuf[1] = 0x83; replacementLen = 2; break;
            case 0x83: tmpBuf[0] = 0xc6; tmpBuf[1] = 0x82; replacementLen = 2; break;
            case 0x85: tmpBuf[0] = 0xc6; tmpBuf[1] = 0x84; replacementLen = 2; break;
            case 0x88: tmpBuf[0] = 0xc6; tmpBuf[1] = 0x87; replacementLen = 2; break;
            case 0x8c: tmpBuf[0] = 0xc6; tmpBuf[1] = 0x8b; replacementLen = 2; break;
            case 0x92: tmpBuf[0] = 0xc6; tmpBuf[1] = 0x91; replacementLen = 2; break;
            case 0x95: tmpBuf[0] = 0xc7; tmpBuf[1] = 0xb6; replacementLen = 2; break;
            case 0x99: tmpBuf[0] = 0xc6; tmpBuf[1] = 0x98; replacementLen = 2; break;
            case 0x9a: tmpBuf[0] = 0xc8; tmpBuf[1] = 0xbd; replacementLen = 2; break;
            case 0x9e: tmpBuf[0] = 0xc8; tmpBuf[1] = 0xa0; replacementLen = 2; break;
            case 0xa1: tmpBuf[0] = 0xc6; tmpBuf[1] = 0xa0; replacementLen = 2; break;
            case 0xa3: tmpBuf[0] = 0xc6; tmpBuf[1] = 0xa2; replacementLen = 2; break;
            case 0xa5: tmpBuf[0] = 0xc6; tmpBuf[1] = 0xa4; replacementLen = 2; break;
            case 0xa8: tmpBuf[0] = 0xc6; tmpBuf[1] = 0xa7; replacementLen = 2; break;
            case 0xad: tmpBuf[0] = 0xc6; tmpBuf[1] = 0xac; replacementLen = 2; break;
            case 0xb0: tmpBuf[0] = 0xc6; tmpBuf[1] = 0xaf; replacementLen = 2; break;
            case 0xb4: tmpBuf[0] = 0xc6; tmpBuf[1] = 0xb3; replacementLen = 2; break;
            case 0xb6: tmpBuf[0] = 0xc6; tmpBuf[1] = 0xb5; replacementLen = 2; break;
            case 0xb9: tmpBuf[0] = 0xc6; tmpBuf[1] = 0xb8; replacementLen = 2; break;
            case 0xbd: tmpBuf[0] = 0xc6; tmpBuf[1] = 0xbc; replacementLen = 2; break;
            case 0xbf: tmpBuf[0] = 0xc7; tmpBuf[1] = 0xb7; replacementLen = 2; break;
            default:
              break;
          }
          break;
        case 0xc7:
          switch(*(p+1))
          {
            case 0x85: tmpBuf[0] = 0xc7; tmpBuf[1] = 0x84; replacementLen = 2; break;
            case 0x86: tmpBuf[0] = 0xc7; tmpBuf[1] = 0x84; replacementLen = 2; break;
            case 0x88: tmpBuf[0] = 0xc7; tmpBuf[1] = 0x87; replacementLen = 2; break;
            case 0x89: tmpBuf[0] = 0xc7; tmpBuf[1] = 0x87; replacementLen = 2; break;
            case 0x8b: tmpBuf[0] = 0xc7; tmpBuf[1] = 0x8a; replacementLen = 2; break;
            case 0x8c: tmpBuf[0] = 0xc7; tmpBuf[1] = 0x8a; replacementLen = 2; break;
            case 0x8e: tmpBuf[0] = 0xc7; tmpBuf[1] = 0x8d; replacementLen = 2; break;
            case 0x90: tmpBuf[0] = 0xc7; tmpBuf[1] = 0x8f; replacementLen = 2; break;
            case 0x92: tmpBuf[0] = 0xc7; tmpBuf[1] = 0x91; replacementLen = 2; break;
            case 0x94: tmpBuf[0] = 0xc7; tmpBuf[1] = 0x93; replacementLen = 2; break;
            case 0x96: tmpBuf[0] = 0xc7; tmpBuf[1] = 0x95; replacementLen = 2; break;
            case 0x98: tmpBuf[0] = 0xc7; tmpBuf[1] = 0x97; replacementLen = 2; break;
            case 0x9a: tmpBuf[0] = 0xc7; tmpBuf[1] = 0x99; replacementLen = 2; break;
            case 0x9c: tmpBuf[0] = 0xc7; tmpBuf[1] = 0x9b; replacementLen = 2; break;
            case 0x9d: tmpBuf[0] = 0xc6; tmpBuf[1] = 0x8e; replacementLen = 2; break;
            case 0x9f: tmpBuf[0] = 0xc7; tmpBuf[1] = 0x9e; replacementLen = 2; break;
            case 0xa1: tmpBuf[0] = 0xc7; tmpBuf[1] = 0xa0; replacementLen = 2; break;
            case 0xa3: tmpBuf[0] = 0xc7; tmpBuf[1] = 0xa2; replacementLen = 2; break;
            case 0xa5: tmpBuf[0] = 0xc7; tmpBuf[1] = 0xa4; replacementLen = 2; break;
            case 0xa7: tmpBuf[0] = 0xc7; tmpBuf[1] = 0xa6; replacementLen = 2; break;
            case 0xa9: tmpBuf[0] = 0xc7; tmpBuf[1] = 0xa8; replacementLen = 2; break;
            case 0xab: tmpBuf[0] = 0xc7; tmpBuf[1] = 0xaa; replacementLen = 2; break;
            case 0xad: tmpBuf[0] = 0xc7; tmpBuf[1] = 0xac; replacementLen = 2; break;
            case 0xaf: tmpBuf[0] = 0xc7; tmpBuf[1] = 0xae; replacementLen = 2; break;
            case 0xb2: tmpBuf[0] = 0xc7; tmpBuf[1] = 0xb1; replacementLen = 2; break;
            case 0xb3: tmpBuf[0] = 0xc7; tmpBuf[1] = 0xb1; replacementLen = 2; break;
            case 0xb5: tmpBuf[0] = 0xc7; tmpBuf[1] = 0xb4; replacementLen = 2; break;
            case 0xb9: tmpBuf[0] = 0xc7; tmpBuf[1] = 0xb8; replacementLen = 2; break;
            case 0xbb: tmpBuf[0] = 0xc7; tmpBuf[1] = 0xba; replacementLen = 2; break;
            case 0xbd: tmpBuf[0] = 0xc7; tmpBuf[1] = 0xbc; replacementLen = 2; break;
            case 0xbf: tmpBuf[0] = 0xc7; tmpBuf[1] = 0xbe; replacementLen = 2; break;
            default:
              break;
          }
          break;
        case 0xc8:
          switch(*(p+1))
          {
            case 0x81: tmpBuf[0] = 0xc8; tmpBuf[1] = 0x80; replacementLen = 2; break;
            case 0x83: tmpBuf[0] = 0xc8; tmpBuf[1] = 0x82; replacementLen = 2; break;
            case 0x85: tmpBuf[0] = 0xc8; tmpBuf[1] = 0x84; replacementLen = 2; break;
            case 0x87: tmpBuf[0] = 0xc8; tmpBuf[1] = 0x86; replacementLen = 2; break;
            case 0x89: tmpBuf[0] = 0xc8; tmpBuf[1] = 0x88; replacementLen = 2; break;
            case 0x8b: tmpBuf[0] = 0xc8; tmpBuf[1] = 0x8a; replacementLen = 2; break;
            case 0x8d: tmpBuf[0] = 0xc8; tmpBuf[1] = 0x8c; replacementLen = 2; break;
            case 0x8f: tmpBuf[0] = 0xc8; tmpBuf[1] = 0x8e; replacementLen = 2; break;
            case 0x91: tmpBuf[0] = 0xc8; tmpBuf[1] = 0x90; replacementLen = 2; break;
            case 0x93: tmpBuf[0] = 0xc8; tmpBuf[1] = 0x92; replacementLen = 2; break;
            case 0x95: tmpBuf[0] = 0xc8; tmpBuf[1] = 0x94; replacementLen = 2; break;
            case 0x97: tmpBuf[0] = 0xc8; tmpBuf[1] = 0x96; replacementLen = 2; break;
            case 0x99: tmpBuf[0] = 0xc8; tmpBuf[1] = 0x98; replacementLen = 2; break;
            case 0x9b: tmpBuf[0] = 0xc8; tmpBuf[1] = 0x9a; replacementLen = 2; break;
            case 0x9d: tmpBuf[0] = 0xc8; tmpBuf[1] = 0x9c; replacementLen = 2; break;
            case 0x9f: tmpBuf[0] = 0xc8; tmpBuf[1] = 0x9e; replacementLen = 2; break;
            case 0xa3: tmpBuf[0] = 0xc8; tmpBuf[1] = 0xa2; replacementLen = 2; break;
            case 0xa5: tmpBuf[0] = 0xc8; tmpBuf[1] = 0xa4; replacementLen = 2; break;
            case 0xa7: tmpBuf[0] = 0xc8; tmpBuf[1] = 0xa6; replacementLen = 2; break;
            case 0xa9: tmpBuf[0] = 0xc8; tmpBuf[1] = 0xa8; replacementLen = 2; break;
            case 0xab: tmpBuf[0] = 0xc8; tmpBuf[1] = 0xaa; replacementLen = 2; break;
            case 0xad: tmpBuf[0] = 0xc8; tmpBuf[1] = 0xac; replacementLen = 2; break;
            case 0xaf: tmpBuf[0] = 0xc8; tmpBuf[1] = 0xae; replacementLen = 2; break;
            case 0xb1: tmpBuf[0] = 0xc8; tmpBuf[1] = 0xb0; replacementLen = 2; break;
            case 0xb3: tmpBuf[0] = 0xc8; tmpBuf[1] = 0xb2; replacementLen = 2; break;
            case 0xbc: tmpBuf[0] = 0xc8; tmpBuf[1] = 0xbb; replacementLen = 2; break;
            case 0xbf: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb1; tmpBuf[2] = 0xbe; replacementLen = 3; break;
            default:
              break;
          }
          break;
        case 0xc9:
          switch(*(p+1))
          {
            case 0x80: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb1; tmpBuf[2] = 0xbf; replacementLen = 3; break;
            case 0x82: tmpBuf[0] = 0xc9; tmpBuf[1] = 0x81; replacementLen = 2; break;
            case 0x87: tmpBuf[0] = 0xc9; tmpBuf[1] = 0x86; replacementLen = 2; break;
            case 0x89: tmpBuf[0] = 0xc9; tmpBuf[1] = 0x88; replacementLen = 2; break;
            case 0x8b: tmpBuf[0] = 0xc9; tmpBuf[1] = 0x8a; replacementLen = 2; break;
            case 0x8d: tmpBuf[0] = 0xc9; tmpBuf[1] = 0x8c; replacementLen = 2; break;
            case 0x8f: tmpBuf[0] = 0xc9; tmpBuf[1] = 0x8e; replacementLen = 2; break;
            case 0x90: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb1; tmpBuf[2] = 0xaf; replacementLen = 3; break;
            case 0x91: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb1; tmpBuf[2] = 0xad; replacementLen = 3; break;
            case 0x92: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb1; tmpBuf[2] = 0xb0; replacementLen = 3; break;
            case 0x93: tmpBuf[0] = 0xc6; tmpBuf[1] = 0x81; replacementLen = 2; break;
            case 0x94: tmpBuf[0] = 0xc6; tmpBuf[1] = 0x86; replacementLen = 2; break;
            case 0x96: tmpBuf[0] = 0xc6; tmpBuf[1] = 0x89; replacementLen = 2; break;
            case 0x97: tmpBuf[0] = 0xc6; tmpBuf[1] = 0x8a; replacementLen = 2; break;
            case 0x99: tmpBuf[0] = 0xc6; tmpBuf[1] = 0x8f; replacementLen = 2; break;
            case 0x9b: tmpBuf[0] = 0xc6; tmpBuf[1] = 0x90; replacementLen = 2; break;
            case 0x9c: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xab; replacementLen = 3; break;
            case 0xa0: tmpBuf[0] = 0xc6; tmpBuf[1] = 0x93; replacementLen = 2; break;
            case 0xa1: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xac; replacementLen = 3; break;
            case 0xa3: tmpBuf[0] = 0xc6; tmpBuf[1] = 0x94; replacementLen = 2; break;
            case 0xa5: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9e; tmpBuf[2] = 0x8d; replacementLen = 3; break;
            case 0xa6: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xaa; replacementLen = 3; break;
            case 0xa8: tmpBuf[0] = 0xc6; tmpBuf[1] = 0x97; replacementLen = 2; break;
            case 0xa9: tmpBuf[0] = 0xc6; tmpBuf[1] = 0x96; replacementLen = 2; break;
            case 0xaa: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xae; replacementLen = 3; break;
            case 0xab: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb1; tmpBuf[2] = 0xa2; replacementLen = 3; break;
            case 0xac: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xad; replacementLen = 3; break;
            case 0xaf: tmpBuf[0] = 0xc6; tmpBuf[1] = 0x9c; replacementLen = 2; break;
            case 0xb1: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb1; tmpBuf[2] = 0xae; replacementLen = 3; break;
            case 0xb2: tmpBuf[0] = 0xc6; tmpBuf[1] = 0x9d; replacementLen = 2; break;
            case 0xb5: tmpBuf[0] = 0xc6; tmpBuf[1] = 0x9f; replacementLen = 2; break;
            case 0xbd: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb1; tmpBuf[2] = 0xa4; replacementLen = 3; break;
            default:
              break;
          }
          break;
        case 0xca:
          switch(*(p+1))
          {
            case 0x80: tmpBuf[0] = 0xc6; tmpBuf[1] = 0xa6; replacementLen = 2; break;
            case 0x82: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9f; tmpBuf[2] = 0x85; replacementLen = 3; break;
            case 0x83: tmpBuf[0] = 0xc6; tmpBuf[1] = 0xa9; replacementLen = 2; break;
            case 0x87: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xb1; replacementLen = 3; break;
            case 0x88: tmpBuf[0] = 0xc6; tmpBuf[1] = 0xae; replacementLen = 2; break;
            case 0x89: tmpBuf[0] = 0xc9; tmpBuf[1] = 0x84; replacementLen = 2; break;
            case 0x8a: tmpBuf[0] = 0xc6; tmpBuf[1] = 0xb1; replacementLen = 2; break;
            case 0x8b: tmpBuf[0] = 0xc6; tmpBuf[1] = 0xb2; replacementLen = 2; break;
            case 0x8c: tmpBuf[0] = 0xc9; tmpBuf[1] = 0x85; replacementLen = 2; break;
            case 0x92: tmpBuf[0] = 0xc6; tmpBuf[1] = 0xb7; replacementLen = 2; break;
            case 0x9d: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xb2; replacementLen = 3; break;
            case 0x9e: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xb0; replacementLen = 3; break;
            default:
              break;
          }
          break;
        case 0xcd:
          switch(*(p+1))
          {
            case 0xb1: tmpBuf[0] = 0xcd; tmpBuf[1] = 0xb0; replacementLen = 2; break;
            case 0xb3: tmpBuf[0] = 0xcd; tmpBuf[1] = 0xb2; replacementLen = 2; break;
            case 0xb7: tmpBuf[0] = 0xcd; tmpBuf[1] = 0xb6; replacementLen = 2; break;
            case 0xbb: tmpBuf[0] = 0xcf; tmpBuf[1] = 0xbd; replacementLen = 2; break;
            case 0xbc: tmpBuf[0] = 0xcf; tmpBuf[1] = 0xbe; replacementLen = 2; break;
            case 0xbd: tmpBuf[0] = 0xcf; tmpBuf[1] = 0xbf; replacementLen = 2; break;
            default:
              break;
          }
          break;
        case 0xce:
          switch(*(p+1))
          {
            case 0xac: tmpBuf[0] = 0xce; tmpBuf[1] = 0x86; replacementLen = 2; break;
            case 0xad: tmpBuf[0] = 0xce; tmpBuf[1] = 0x88; replacementLen = 2; break;
            case 0xae: tmpBuf[0] = 0xce; tmpBuf[1] = 0x89; replacementLen = 2; break;
            case 0xaf: tmpBuf[0] = 0xce; tmpBuf[1] = 0x8a; replacementLen = 2; break;
            case 0xb1: tmpBuf[0] = 0xce; tmpBuf[1] = 0x91; replacementLen = 2; break;
            case 0xb2: tmpBuf[0] = 0xce; tmpBuf[1] = 0x92; replacementLen = 2; break;
            case 0xb3: tmpBuf[0] = 0xce; tmpBuf[1] = 0x93; replacementLen = 2; break;
            case 0xb4: tmpBuf[0] = 0xce; tmpBuf[1] = 0x94; replacementLen = 2; break;
            case 0xb5: tmpBuf[0] = 0xce; tmpBuf[1] = 0x95; replacementLen = 2; break;
            case 0xb6: tmpBuf[0] = 0xce; tmpBuf[1] = 0x96; replacementLen = 2; break;
            case 0xb7: tmpBuf[0] = 0xce; tmpBuf[1] = 0x97; replacementLen = 2; break;
            case 0xb8: tmpBuf[0] = 0xce; tmpBuf[1] = 0x98; replacementLen = 2; break;
            case 0xb9: tmpBuf[0] = 0xce; tmpBuf[1] = 0x99; replacementLen = 2; break;
            case 0xba: tmpBuf[0] = 0xce; tmpBuf[1] = 0x9a; replacementLen = 2; break;
            case 0xbb: tmpBuf[0] = 0xce; tmpBuf[1] = 0x9b; replacementLen = 2; break;
            case 0xbc: tmpBuf[0] = 0xce; tmpBuf[1] = 0x9c; replacementLen = 2; break;
            case 0xbd: tmpBuf[0] = 0xce; tmpBuf[1] = 0x9d; replacementLen = 2; break;
            case 0xbe: tmpBuf[0] = 0xce; tmpBuf[1] = 0x9e; replacementLen = 2; break;
            case 0xbf: tmpBuf[0] = 0xce; tmpBuf[1] = 0x9f; replacementLen = 2; break;
            default:
              break;
          }
          break;
        case 0xcf:
          switch(*(p+1))
          {
            case 0x80: tmpBuf[0] = 0xce; tmpBuf[1] = 0xa0; replacementLen = 2; break;
            case 0x81: tmpBuf[0] = 0xce; tmpBuf[1] = 0xa1; replacementLen = 2; break;
            case 0x82: tmpBuf[0] = 0xce; tmpBuf[1] = 0xa3; replacementLen = 2; break;
            case 0x83: tmpBuf[0] = 0xce; tmpBuf[1] = 0xa3; replacementLen = 2; break;
            case 0x84: tmpBuf[0] = 0xce; tmpBuf[1] = 0xa4; replacementLen = 2; break;
            case 0x85: tmpBuf[0] = 0xce; tmpBuf[1] = 0xa5; replacementLen = 2; break;
            case 0x86: tmpBuf[0] = 0xce; tmpBuf[1] = 0xa6; replacementLen = 2; break;
            case 0x87: tmpBuf[0] = 0xce; tmpBuf[1] = 0xa7; replacementLen = 2; break;
            case 0x88: tmpBuf[0] = 0xce; tmpBuf[1] = 0xa8; replacementLen = 2; break;
            case 0x89: tmpBuf[0] = 0xce; tmpBuf[1] = 0xa9; replacementLen = 2; break;
            case 0x8a: tmpBuf[0] = 0xce; tmpBuf[1] = 0xaa; replacementLen = 2; break;
            case 0x8b: tmpBuf[0] = 0xce; tmpBuf[1] = 0xab; replacementLen = 2; break;
            case 0x8c: tmpBuf[0] = 0xce; tmpBuf[1] = 0x8c; replacementLen = 2; break;
            case 0x8d: tmpBuf[0] = 0xce; tmpBuf[1] = 0x8e; replacementLen = 2; break;
            case 0x8e: tmpBuf[0] = 0xce; tmpBuf[1] = 0x8f; replacementLen = 2; break;
            case 0x90: tmpBuf[0] = 0xce; tmpBuf[1] = 0x92; replacementLen = 2; break;
            case 0x91: tmpBuf[0] = 0xce; tmpBuf[1] = 0x98; replacementLen = 2; break;
            case 0x95: tmpBuf[0] = 0xce; tmpBuf[1] = 0xa6; replacementLen = 2; break;
            case 0x96: tmpBuf[0] = 0xce; tmpBuf[1] = 0xa0; replacementLen = 2; break;
            case 0x97: tmpBuf[0] = 0xcf; tmpBuf[1] = 0x8f; replacementLen = 2; break;
            case 0x99: tmpBuf[0] = 0xcf; tmpBuf[1] = 0x98; replacementLen = 2; break;
            case 0x9b: tmpBuf[0] = 0xcf; tmpBuf[1] = 0x9a; replacementLen = 2; break;
            case 0x9d: tmpBuf[0] = 0xcf; tmpBuf[1] = 0x9c; replacementLen = 2; break;
            case 0x9f: tmpBuf[0] = 0xcf; tmpBuf[1] = 0x9e; replacementLen = 2; break;
            case 0xa1: tmpBuf[0] = 0xcf; tmpBuf[1] = 0xa0; replacementLen = 2; break;
            case 0xa3: tmpBuf[0] = 0xcf; tmpBuf[1] = 0xa2; replacementLen = 2; break;
            case 0xa5: tmpBuf[0] = 0xcf; tmpBuf[1] = 0xa4; replacementLen = 2; break;
            case 0xa7: tmpBuf[0] = 0xcf; tmpBuf[1] = 0xa6; replacementLen = 2; break;
            case 0xa9: tmpBuf[0] = 0xcf; tmpBuf[1] = 0xa8; replacementLen = 2; break;
            case 0xab: tmpBuf[0] = 0xcf; tmpBuf[1] = 0xaa; replacementLen = 2; break;
            case 0xad: tmpBuf[0] = 0xcf; tmpBuf[1] = 0xac; replacementLen = 2; break;
            case 0xaf: tmpBuf[0] = 0xcf; tmpBuf[1] = 0xae; replacementLen = 2; break;
            case 0xb0: tmpBuf[0] = 0xce; tmpBuf[1] = 0x9a; replacementLen = 2; break;
            case 0xb1: tmpBuf[0] = 0xce; tmpBuf[1] = 0xa1; replacementLen = 2; break;
            case 0xb2: tmpBuf[0] = 0xcf; tmpBuf[1] = 0xb9; replacementLen = 2; break;
            case 0xb3: tmpBuf[0] = 0xcd; tmpBuf[1] = 0xbf; replacementLen = 2; break;
            case 0xb5: tmpBuf[0] = 0xce; tmpBuf[1] = 0x95; replacementLen = 2; break;
            case 0xb8: tmpBuf[0] = 0xcf; tmpBuf[1] = 0xb7; replacementLen = 2; break;
            case 0xbb: tmpBuf[0] = 0xcf; tmpBuf[1] = 0xba; replacementLen = 2; break;
            default:
              break;
          }
          break;
        case 0xd0:
          switch(*(p+1))
          {
            case 0xb0: tmpBuf[0] = 0xd0; tmpBuf[1] = 0x90; replacementLen = 2; break;
            case 0xb1: tmpBuf[0] = 0xd0; tmpBuf[1] = 0x91; replacementLen = 2; break;
            case 0xb2: tmpBuf[0] = 0xd0; tmpBuf[1] = 0x92; replacementLen = 2; break;
            case 0xb3: tmpBuf[0] = 0xd0; tmpBuf[1] = 0x93; replacementLen = 2; break;
            case 0xb4: tmpBuf[0] = 0xd0; tmpBuf[1] = 0x94; replacementLen = 2; break;
            case 0xb5: tmpBuf[0] = 0xd0; tmpBuf[1] = 0x95; replacementLen = 2; break;
            case 0xb6: tmpBuf[0] = 0xd0; tmpBuf[1] = 0x96; replacementLen = 2; break;
            case 0xb7: tmpBuf[0] = 0xd0; tmpBuf[1] = 0x97; replacementLen = 2; break;
            case 0xb8: tmpBuf[0] = 0xd0; tmpBuf[1] = 0x98; replacementLen = 2; break;
            case 0xb9: tmpBuf[0] = 0xd0; tmpBuf[1] = 0x99; replacementLen = 2; break;
            case 0xba: tmpBuf[0] = 0xd0; tmpBuf[1] = 0x9a; replacementLen = 2; break;
            case 0xbb: tmpBuf[0] = 0xd0; tmpBuf[1] = 0x9b; replacementLen = 2; break;
            case 0xbc: tmpBuf[0] = 0xd0; tmpBuf[1] = 0x9c; replacementLen = 2; break;
            case 0xbd: tmpBuf[0] = 0xd0; tmpBuf[1] = 0x9d; replacementLen = 2; break;
            case 0xbe: tmpBuf[0] = 0xd0; tmpBuf[1] = 0x9e; replacementLen = 2; break;
            case 0xbf: tmpBuf[0] = 0xd0; tmpBuf[1] = 0x9f; replacementLen = 2; break;
            default:
              break;
          }
          break;
        case 0xd1:
          switch(*(p+1))
          {
            case 0x80: tmpBuf[0] = 0xd0; tmpBuf[1] = 0xa0; replacementLen = 2; break;
            case 0x81: tmpBuf[0] = 0xd0; tmpBuf[1] = 0xa1; replacementLen = 2; break;
            case 0x82: tmpBuf[0] = 0xd0; tmpBuf[1] = 0xa2; replacementLen = 2; break;
            case 0x83: tmpBuf[0] = 0xd0; tmpBuf[1] = 0xa3; replacementLen = 2; break;
            case 0x84: tmpBuf[0] = 0xd0; tmpBuf[1] = 0xa4; replacementLen = 2; break;
            case 0x85: tmpBuf[0] = 0xd0; tmpBuf[1] = 0xa5; replacementLen = 2; break;
            case 0x86: tmpBuf[0] = 0xd0; tmpBuf[1] = 0xa6; replacementLen = 2; break;
            case 0x87: tmpBuf[0] = 0xd0; tmpBuf[1] = 0xa7; replacementLen = 2; break;
            case 0x88: tmpBuf[0] = 0xd0; tmpBuf[1] = 0xa8; replacementLen = 2; break;
            case 0x89: tmpBuf[0] = 0xd0; tmpBuf[1] = 0xa9; replacementLen = 2; break;
            case 0x8a: tmpBuf[0] = 0xd0; tmpBuf[1] = 0xaa; replacementLen = 2; break;
            case 0x8b: tmpBuf[0] = 0xd0; tmpBuf[1] = 0xab; replacementLen = 2; break;
            case 0x8c: tmpBuf[0] = 0xd0; tmpBuf[1] = 0xac; replacementLen = 2; break;
            case 0x8d: tmpBuf[0] = 0xd0; tmpBuf[1] = 0xad; replacementLen = 2; break;
            case 0x8e: tmpBuf[0] = 0xd0; tmpBuf[1] = 0xae; replacementLen = 2; break;
            case 0x8f: tmpBuf[0] = 0xd0; tmpBuf[1] = 0xaf; replacementLen = 2; break;
            case 0x90: tmpBuf[0] = 0xd0; tmpBuf[1] = 0x80; replacementLen = 2; break;
            case 0x91: tmpBuf[0] = 0xd0; tmpBuf[1] = 0x81; replacementLen = 2; break;
            case 0x92: tmpBuf[0] = 0xd0; tmpBuf[1] = 0x82; replacementLen = 2; break;
            case 0x93: tmpBuf[0] = 0xd0; tmpBuf[1] = 0x83; replacementLen = 2; break;
            case 0x94: tmpBuf[0] = 0xd0; tmpBuf[1] = 0x84; replacementLen = 2; break;
            case 0x95: tmpBuf[0] = 0xd0; tmpBuf[1] = 0x85; replacementLen = 2; break;
            case 0x96: tmpBuf[0] = 0xd0; tmpBuf[1] = 0x86; replacementLen = 2; break;
            case 0x97: tmpBuf[0] = 0xd0; tmpBuf[1] = 0x87; replacementLen = 2; break;
            case 0x98: tmpBuf[0] = 0xd0; tmpBuf[1] = 0x88; replacementLen = 2; break;
            case 0x99: tmpBuf[0] = 0xd0; tmpBuf[1] = 0x89; replacementLen = 2; break;
            case 0x9a: tmpBuf[0] = 0xd0; tmpBuf[1] = 0x8a; replacementLen = 2; break;
            case 0x9b: tmpBuf[0] = 0xd0; tmpBuf[1] = 0x8b; replacementLen = 2; break;
            case 0x9c: tmpBuf[0] = 0xd0; tmpBuf[1] = 0x8c; replacementLen = 2; break;
            case 0x9d: tmpBuf[0] = 0xd0; tmpBuf[1] = 0x8d; replacementLen = 2; break;
            case 0x9e: tmpBuf[0] = 0xd0; tmpBuf[1] = 0x8e; replacementLen = 2; break;
            case 0x9f: tmpBuf[0] = 0xd0; tmpBuf[1] = 0x8f; replacementLen = 2; break;
            case 0xa1: tmpBuf[0] = 0xd1; tmpBuf[1] = 0xa0; replacementLen = 2; break;
            case 0xa3: tmpBuf[0] = 0xd1; tmpBuf[1] = 0xa2; replacementLen = 2; break;
            case 0xa5: tmpBuf[0] = 0xd1; tmpBuf[1] = 0xa4; replacementLen = 2; break;
            case 0xa7: tmpBuf[0] = 0xd1; tmpBuf[1] = 0xa6; replacementLen = 2; break;
            case 0xa9: tmpBuf[0] = 0xd1; tmpBuf[1] = 0xa8; replacementLen = 2; break;
            case 0xab: tmpBuf[0] = 0xd1; tmpBuf[1] = 0xaa; replacementLen = 2; break;
            case 0xad: tmpBuf[0] = 0xd1; tmpBuf[1] = 0xac; replacementLen = 2; break;
            case 0xaf: tmpBuf[0] = 0xd1; tmpBuf[1] = 0xae; replacementLen = 2; break;
            case 0xb1: tmpBuf[0] = 0xd1; tmpBuf[1] = 0xb0; replacementLen = 2; break;
            case 0xb3: tmpBuf[0] = 0xd1; tmpBuf[1] = 0xb2; replacementLen = 2; break;
            case 0xb5: tmpBuf[0] = 0xd1; tmpBuf[1] = 0xb4; replacementLen = 2; break;
            case 0xb7: tmpBuf[0] = 0xd1; tmpBuf[1] = 0xb6; replacementLen = 2; break;
            case 0xb9: tmpBuf[0] = 0xd1; tmpBuf[1] = 0xb8; replacementLen = 2; break;
            case 0xbb: tmpBuf[0] = 0xd1; tmpBuf[1] = 0xba; replacementLen = 2; break;
            case 0xbd: tmpBuf[0] = 0xd1; tmpBuf[1] = 0xbc; replacementLen = 2; break;
            case 0xbf: tmpBuf[0] = 0xd1; tmpBuf[1] = 0xbe; replacementLen = 2; break;
            default:
              break;
          }
          break;
        case 0xd2:
          switch(*(p+1))
          {
            case 0x81: tmpBuf[0] = 0xd2; tmpBuf[1] = 0x80; replacementLen = 2; break;
            case 0x8b: tmpBuf[0] = 0xd2; tmpBuf[1] = 0x8a; replacementLen = 2; break;
            case 0x8d: tmpBuf[0] = 0xd2; tmpBuf[1] = 0x8c; replacementLen = 2; break;
            case 0x8f: tmpBuf[0] = 0xd2; tmpBuf[1] = 0x8e; replacementLen = 2; break;
            case 0x91: tmpBuf[0] = 0xd2; tmpBuf[1] = 0x90; replacementLen = 2; break;
            case 0x93: tmpBuf[0] = 0xd2; tmpBuf[1] = 0x92; replacementLen = 2; break;
            case 0x95: tmpBuf[0] = 0xd2; tmpBuf[1] = 0x94; replacementLen = 2; break;
            case 0x97: tmpBuf[0] = 0xd2; tmpBuf[1] = 0x96; replacementLen = 2; break;
            case 0x99: tmpBuf[0] = 0xd2; tmpBuf[1] = 0x98; replacementLen = 2; break;
            case 0x9b: tmpBuf[0] = 0xd2; tmpBuf[1] = 0x9a; replacementLen = 2; break;
            case 0x9d: tmpBuf[0] = 0xd2; tmpBuf[1] = 0x9c; replacementLen = 2; break;
            case 0x9f: tmpBuf[0] = 0xd2; tmpBuf[1] = 0x9e; replacementLen = 2; break;
            case 0xa1: tmpBuf[0] = 0xd2; tmpBuf[1] = 0xa0; replacementLen = 2; break;
            case 0xa3: tmpBuf[0] = 0xd2; tmpBuf[1] = 0xa2; replacementLen = 2; break;
            case 0xa5: tmpBuf[0] = 0xd2; tmpBuf[1] = 0xa4; replacementLen = 2; break;
            case 0xa7: tmpBuf[0] = 0xd2; tmpBuf[1] = 0xa6; replacementLen = 2; break;
            case 0xa9: tmpBuf[0] = 0xd2; tmpBuf[1] = 0xa8; replacementLen = 2; break;
            case 0xab: tmpBuf[0] = 0xd2; tmpBuf[1] = 0xaa; replacementLen = 2; break;
            case 0xad: tmpBuf[0] = 0xd2; tmpBuf[1] = 0xac; replacementLen = 2; break;
            case 0xaf: tmpBuf[0] = 0xd2; tmpBuf[1] = 0xae; replacementLen = 2; break;
            case 0xb1: tmpBuf[0] = 0xd2; tmpBuf[1] = 0xb0; replacementLen = 2; break;
            case 0xb3: tmpBuf[0] = 0xd2; tmpBuf[1] = 0xb2; replacementLen = 2; break;
            case 0xb5: tmpBuf[0] = 0xd2; tmpBuf[1] = 0xb4; replacementLen = 2; break;
            case 0xb7: tmpBuf[0] = 0xd2; tmpBuf[1] = 0xb6; replacementLen = 2; break;
            case 0xb9: tmpBuf[0] = 0xd2; tmpBuf[1] = 0xb8; replacementLen = 2; break;
            case 0xbb: tmpBuf[0] = 0xd2; tmpBuf[1] = 0xba; replacementLen = 2; break;
            case 0xbd: tmpBuf[0] = 0xd2; tmpBuf[1] = 0xbc; replacementLen = 2; break;
            case 0xbf: tmpBuf[0] = 0xd2; tmpBuf[1] = 0xbe; replacementLen = 2; break;
            default:
              break;
          }
          break;
        case 0xd3:
          switch(*(p+1))
          {
            case 0x82: tmpBuf[0] = 0xd3; tmpBuf[1] = 0x81; replacementLen = 2; break;
            case 0x84: tmpBuf[0] = 0xd3; tmpBuf[1] = 0x83; replacementLen = 2; break;
            case 0x86: tmpBuf[0] = 0xd3; tmpBuf[1] = 0x85; replacementLen = 2; break;
            case 0x88: tmpBuf[0] = 0xd3; tmpBuf[1] = 0x87; replacementLen = 2; break;
            case 0x8a: tmpBuf[0] = 0xd3; tmpBuf[1] = 0x89; replacementLen = 2; break;
            case 0x8c: tmpBuf[0] = 0xd3; tmpBuf[1] = 0x8b; replacementLen = 2; break;
            case 0x8e: tmpBuf[0] = 0xd3; tmpBuf[1] = 0x8d; replacementLen = 2; break;
            case 0x8f: tmpBuf[0] = 0xd3; tmpBuf[1] = 0x80; replacementLen = 2; break;
            case 0x91: tmpBuf[0] = 0xd3; tmpBuf[1] = 0x90; replacementLen = 2; break;
            case 0x93: tmpBuf[0] = 0xd3; tmpBuf[1] = 0x92; replacementLen = 2; break;
            case 0x95: tmpBuf[0] = 0xd3; tmpBuf[1] = 0x94; replacementLen = 2; break;
            case 0x97: tmpBuf[0] = 0xd3; tmpBuf[1] = 0x96; replacementLen = 2; break;
            case 0x99: tmpBuf[0] = 0xd3; tmpBuf[1] = 0x98; replacementLen = 2; break;
            case 0x9b: tmpBuf[0] = 0xd3; tmpBuf[1] = 0x9a; replacementLen = 2; break;
            case 0x9d: tmpBuf[0] = 0xd3; tmpBuf[1] = 0x9c; replacementLen = 2; break;
            case 0x9f: tmpBuf[0] = 0xd3; tmpBuf[1] = 0x9e; replacementLen = 2; break;
            case 0xa1: tmpBuf[0] = 0xd3; tmpBuf[1] = 0xa0; replacementLen = 2; break;
            case 0xa3: tmpBuf[0] = 0xd3; tmpBuf[1] = 0xa2; replacementLen = 2; break;
            case 0xa5: tmpBuf[0] = 0xd3; tmpBuf[1] = 0xa4; replacementLen = 2; break;
            case 0xa7: tmpBuf[0] = 0xd3; tmpBuf[1] = 0xa6; replacementLen = 2; break;
            case 0xa9: tmpBuf[0] = 0xd3; tmpBuf[1] = 0xa8; replacementLen = 2; break;
            case 0xab: tmpBuf[0] = 0xd3; tmpBuf[1] = 0xaa; replacementLen = 2; break;
            case 0xad: tmpBuf[0] = 0xd3; tmpBuf[1] = 0xac; replacementLen = 2; break;
            case 0xaf: tmpBuf[0] = 0xd3; tmpBuf[1] = 0xae; replacementLen = 2; break;
            case 0xb1: tmpBuf[0] = 0xd3; tmpBuf[1] = 0xb0; replacementLen = 2; break;
            case 0xb3: tmpBuf[0] = 0xd3; tmpBuf[1] = 0xb2; replacementLen = 2; break;
            case 0xb5: tmpBuf[0] = 0xd3; tmpBuf[1] = 0xb4; replacementLen = 2; break;
            case 0xb7: tmpBuf[0] = 0xd3; tmpBuf[1] = 0xb6; replacementLen = 2; break;
            case 0xb9: tmpBuf[0] = 0xd3; tmpBuf[1] = 0xb8; replacementLen = 2; break;
            case 0xbb: tmpBuf[0] = 0xd3; tmpBuf[1] = 0xba; replacementLen = 2; break;
            case 0xbd: tmpBuf[0] = 0xd3; tmpBuf[1] = 0xbc; replacementLen = 2; break;
            case 0xbf: tmpBuf[0] = 0xd3; tmpBuf[1] = 0xbe; replacementLen = 2; break;
            default:
              break;
          }
          break;
        case 0xd4:
          switch(*(p+1))
          {
            case 0x81: tmpBuf[0] = 0xd4; tmpBuf[1] = 0x80; replacementLen = 2; break;
            case 0x83: tmpBuf[0] = 0xd4; tmpBuf[1] = 0x82; replacementLen = 2; break;
            case 0x85: tmpBuf[0] = 0xd4; tmpBuf[1] = 0x84; replacementLen = 2; break;
            case 0x87: tmpBuf[0] = 0xd4; tmpBuf[1] = 0x86; replacementLen = 2; break;
            case 0x89: tmpBuf[0] = 0xd4; tmpBuf[1] = 0x88; replacementLen = 2; break;
            case 0x8b: tmpBuf[0] = 0xd4; tmpBuf[1] = 0x8a; replacementLen = 2; break;
            case 0x8d: tmpBuf[0] = 0xd4; tmpBuf[1] = 0x8c; replacementLen = 2; break;
            case 0x8f: tmpBuf[0] = 0xd4; tmpBuf[1] = 0x8e; replacementLen = 2; break;
            case 0x91: tmpBuf[0] = 0xd4; tmpBuf[1] = 0x90; replacementLen = 2; break;
            case 0x93: tmpBuf[0] = 0xd4; tmpBuf[1] = 0x92; replacementLen = 2; break;
            case 0x95: tmpBuf[0] = 0xd4; tmpBuf[1] = 0x94; replacementLen = 2; break;
            case 0x97: tmpBuf[0] = 0xd4; tmpBuf[1] = 0x96; replacementLen = 2; break;
            case 0x99: tmpBuf[0] = 0xd4; tmpBuf[1] = 0x98; replacementLen = 2; break;
            case 0x9b: tmpBuf[0] = 0xd4; tmpBuf[1] = 0x9a; replacementLen = 2; break;
            case 0x9d: tmpBuf[0] = 0xd4; tmpBuf[1] = 0x9c; replacementLen = 2; break;
            case 0x9f: tmpBuf[0] = 0xd4; tmpBuf[1] = 0x9e; replacementLen = 2; break;
            case 0xa1: tmpBuf[0] = 0xd4; tmpBuf[1] = 0xa0; replacementLen = 2; break;
            case 0xa3: tmpBuf[0] = 0xd4; tmpBuf[1] = 0xa2; replacementLen = 2; break;
            case 0xa5: tmpBuf[0] = 0xd4; tmpBuf[1] = 0xa4; replacementLen = 2; break;
            case 0xa7: tmpBuf[0] = 0xd4; tmpBuf[1] = 0xa6; replacementLen = 2; break;
            case 0xa9: tmpBuf[0] = 0xd4; tmpBuf[1] = 0xa8; replacementLen = 2; break;
            case 0xab: tmpBuf[0] = 0xd4; tmpBuf[1] = 0xaa; replacementLen = 2; break;
            case 0xad: tmpBuf[0] = 0xd4; tmpBuf[1] = 0xac; replacementLen = 2; break;
            case 0xaf: tmpBuf[0] = 0xd4; tmpBuf[1] = 0xae; replacementLen = 2; break;
            default:
              break;
          }
          break;
        case 0xd5:
          switch(*(p+1))
          {
            case 0xa1: tmpBuf[0] = 0xd4; tmpBuf[1] = 0xb1; replacementLen = 2; break;
            case 0xa2: tmpBuf[0] = 0xd4; tmpBuf[1] = 0xb2; replacementLen = 2; break;
            case 0xa3: tmpBuf[0] = 0xd4; tmpBuf[1] = 0xb3; replacementLen = 2; break;
            case 0xa4: tmpBuf[0] = 0xd4; tmpBuf[1] = 0xb4; replacementLen = 2; break;
            case 0xa5: tmpBuf[0] = 0xd4; tmpBuf[1] = 0xb5; replacementLen = 2; break;
            case 0xa6: tmpBuf[0] = 0xd4; tmpBuf[1] = 0xb6; replacementLen = 2; break;
            case 0xa7: tmpBuf[0] = 0xd4; tmpBuf[1] = 0xb7; replacementLen = 2; break;
            case 0xa8: tmpBuf[0] = 0xd4; tmpBuf[1] = 0xb8; replacementLen = 2; break;
            case 0xa9: tmpBuf[0] = 0xd4; tmpBuf[1] = 0xb9; replacementLen = 2; break;
            case 0xaa: tmpBuf[0] = 0xd4; tmpBuf[1] = 0xba; replacementLen = 2; break;
            case 0xab: tmpBuf[0] = 0xd4; tmpBuf[1] = 0xbb; replacementLen = 2; break;
            case 0xac: tmpBuf[0] = 0xd4; tmpBuf[1] = 0xbc; replacementLen = 2; break;
            case 0xad: tmpBuf[0] = 0xd4; tmpBuf[1] = 0xbd; replacementLen = 2; break;
            case 0xae: tmpBuf[0] = 0xd4; tmpBuf[1] = 0xbe; replacementLen = 2; break;
            case 0xaf: tmpBuf[0] = 0xd4; tmpBuf[1] = 0xbf; replacementLen = 2; break;
            case 0xb0: tmpBuf[0] = 0xd5; tmpBuf[1] = 0x80; replacementLen = 2; break;
            case 0xb1: tmpBuf[0] = 0xd5; tmpBuf[1] = 0x81; replacementLen = 2; break;
            case 0xb2: tmpBuf[0] = 0xd5; tmpBuf[1] = 0x82; replacementLen = 2; break;
            case 0xb3: tmpBuf[0] = 0xd5; tmpBuf[1] = 0x83; replacementLen = 2; break;
            case 0xb4: tmpBuf[0] = 0xd5; tmpBuf[1] = 0x84; replacementLen = 2; break;
            case 0xb5: tmpBuf[0] = 0xd5; tmpBuf[1] = 0x85; replacementLen = 2; break;
            case 0xb6: tmpBuf[0] = 0xd5; tmpBuf[1] = 0x86; replacementLen = 2; break;
            case 0xb7: tmpBuf[0] = 0xd5; tmpBuf[1] = 0x87; replacementLen = 2; break;
            case 0xb8: tmpBuf[0] = 0xd5; tmpBuf[1] = 0x88; replacementLen = 2; break;
            case 0xb9: tmpBuf[0] = 0xd5; tmpBuf[1] = 0x89; replacementLen = 2; break;
            case 0xba: tmpBuf[0] = 0xd5; tmpBuf[1] = 0x8a; replacementLen = 2; break;
            case 0xbb: tmpBuf[0] = 0xd5; tmpBuf[1] = 0x8b; replacementLen = 2; break;
            case 0xbc: tmpBuf[0] = 0xd5; tmpBuf[1] = 0x8c; replacementLen = 2; break;
            case 0xbd: tmpBuf[0] = 0xd5; tmpBuf[1] = 0x8d; replacementLen = 2; break;
            case 0xbe: tmpBuf[0] = 0xd5; tmpBuf[1] = 0x8e; replacementLen = 2; break;
            case 0xbf: tmpBuf[0] = 0xd5; tmpBuf[1] = 0x8f; replacementLen = 2; break;
            default:
              break;
          }
          break;
        case 0xd6:
          switch(*(p+1))
          {
            case 0x80: tmpBuf[0] = 0xd5; tmpBuf[1] = 0x90; replacementLen = 2; break;
            case 0x81: tmpBuf[0] = 0xd5; tmpBuf[1] = 0x91; replacementLen = 2; break;
            case 0x82: tmpBuf[0] = 0xd5; tmpBuf[1] = 0x92; replacementLen = 2; break;
            case 0x83: tmpBuf[0] = 0xd5; tmpBuf[1] = 0x93; replacementLen = 2; break;
            case 0x84: tmpBuf[0] = 0xd5; tmpBuf[1] = 0x94; replacementLen = 2; break;
            case 0x85: tmpBuf[0] = 0xd5; tmpBuf[1] = 0x95; replacementLen = 2; break;
            case 0x86: tmpBuf[0] = 0xd5; tmpBuf[1] = 0x96; replacementLen = 2; break;
            default:
              break;
          }
          break;
        case 0xe1:
          switch(*(p+1))
          {
            case 0x83:
              switch(*(p+2))
              {
                case 0x90: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb2; tmpBuf[2] = 0x90; replacementLen = 3; break;
                case 0x91: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb2; tmpBuf[2] = 0x91; replacementLen = 3; break;
                case 0x92: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb2; tmpBuf[2] = 0x92; replacementLen = 3; break;
                case 0x93: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb2; tmpBuf[2] = 0x93; replacementLen = 3; break;
                case 0x94: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb2; tmpBuf[2] = 0x94; replacementLen = 3; break;
                case 0x95: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb2; tmpBuf[2] = 0x95; replacementLen = 3; break;
                case 0x96: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb2; tmpBuf[2] = 0x96; replacementLen = 3; break;
                case 0x97: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb2; tmpBuf[2] = 0x97; replacementLen = 3; break;
                case 0x98: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb2; tmpBuf[2] = 0x98; replacementLen = 3; break;
                case 0x99: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb2; tmpBuf[2] = 0x99; replacementLen = 3; break;
                case 0x9a: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb2; tmpBuf[2] = 0x9a; replacementLen = 3; break;
                case 0x9b: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb2; tmpBuf[2] = 0x9b; replacementLen = 3; break;
                case 0x9c: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb2; tmpBuf[2] = 0x9c; replacementLen = 3; break;
                case 0x9d: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb2; tmpBuf[2] = 0x9d; replacementLen = 3; break;
                case 0x9e: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb2; tmpBuf[2] = 0x9e; replacementLen = 3; break;
                case 0x9f: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb2; tmpBuf[2] = 0x9f; replacementLen = 3; break;
                case 0xa0: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb2; tmpBuf[2] = 0xa0; replacementLen = 3; break;
                case 0xa1: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb2; tmpBuf[2] = 0xa1; replacementLen = 3; break;
                case 0xa2: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb2; tmpBuf[2] = 0xa2; replacementLen = 3; break;
                case 0xa3: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb2; tmpBuf[2] = 0xa3; replacementLen = 3; break;
                case 0xa4: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb2; tmpBuf[2] = 0xa4; replacementLen = 3; break;
                case 0xa5: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb2; tmpBuf[2] = 0xa5; replacementLen = 3; break;
                case 0xa6: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb2; tmpBuf[2] = 0xa6; replacementLen = 3; break;
                case 0xa7: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb2; tmpBuf[2] = 0xa7; replacementLen = 3; break;
                case 0xa8: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb2; tmpBuf[2] = 0xa8; replacementLen = 3; break;
                case 0xa9: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb2; tmpBuf[2] = 0xa9; replacementLen = 3; break;
                case 0xaa: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb2; tmpBuf[2] = 0xaa; replacementLen = 3; break;
                case 0xab: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb2; tmpBuf[2] = 0xab; replacementLen = 3; break;
                case 0xac: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb2; tmpBuf[2] = 0xac; replacementLen = 3; break;
                case 0xad: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb2; tmpBuf[2] = 0xad; replacementLen = 3; break;
                case 0xae: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb2; tmpBuf[2] = 0xae; replacementLen = 3; break;
                case 0xaf: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb2; tmpBuf[2] = 0xaf; replacementLen = 3; break;
                case 0xb0: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb2; tmpBuf[2] = 0xb0; replacementLen = 3; break;
                case 0xb1: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb2; tmpBuf[2] = 0xb1; replacementLen = 3; break;
                case 0xb2: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb2; tmpBuf[2] = 0xb2; replacementLen = 3; break;
                case 0xb3: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb2; tmpBuf[2] = 0xb3; replacementLen = 3; break;
                case 0xb4: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb2; tmpBuf[2] = 0xb4; replacementLen = 3; break;
                case 0xb5: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb2; tmpBuf[2] = 0xb5; replacementLen = 3; break;
                case 0xb6: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb2; tmpBuf[2] = 0xb6; replacementLen = 3; break;
                case 0xb7: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb2; tmpBuf[2] = 0xb7; replacementLen = 3; break;
                case 0xb8: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb2; tmpBuf[2] = 0xb8; replacementLen = 3; break;
                case 0xb9: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb2; tmpBuf[2] = 0xb9; replacementLen = 3; break;
                case 0xba: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb2; tmpBuf[2] = 0xba; replacementLen = 3; break;
                case 0xbd: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb2; tmpBuf[2] = 0xbd; replacementLen = 3; break;
                case 0xbe: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb2; tmpBuf[2] = 0xbe; replacementLen = 3; break;
                case 0xbf: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb2; tmpBuf[2] = 0xbf; replacementLen = 3; break;
                default:
                  break;
              }
              break;
            case 0x8f:
              switch(*(p+2))
              {
                case 0xb8: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8f; tmpBuf[2] = 0xb0; replacementLen = 3; break;
                case 0xb9: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8f; tmpBuf[2] = 0xb1; replacementLen = 3; break;
                case 0xba: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8f; tmpBuf[2] = 0xb2; replacementLen = 3; break;
                case 0xbb: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8f; tmpBuf[2] = 0xb3; replacementLen = 3; break;
                case 0xbc: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8f; tmpBuf[2] = 0xb4; replacementLen = 3; break;
                case 0xbd: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8f; tmpBuf[2] = 0xb5; replacementLen = 3; break;
                default:
                  break;
              }
              break;
            case 0xb2:
              switch(*(p+2))
              {
                case 0x80: tmpBuf[0] = 0xd0; tmpBuf[1] = 0x92; replacementLen = 2; break;
                case 0x81: tmpBuf[0] = 0xd0; tmpBuf[1] = 0x94; replacementLen = 2; break;
                case 0x82: tmpBuf[0] = 0xd0; tmpBuf[1] = 0x9e; replacementLen = 2; break;
                case 0x83: tmpBuf[0] = 0xd0; tmpBuf[1] = 0xa1; replacementLen = 2; break;
                case 0x84: tmpBuf[0] = 0xd0; tmpBuf[1] = 0xa2; replacementLen = 2; break;
                case 0x85: tmpBuf[0] = 0xd0; tmpBuf[1] = 0xa2; replacementLen = 2; break;
                case 0x86: tmpBuf[0] = 0xd0; tmpBuf[1] = 0xaa; replacementLen = 2; break;
                case 0x87: tmpBuf[0] = 0xd1; tmpBuf[1] = 0xa2; replacementLen = 2; break;
                case 0x88: tmpBuf[0] = 0xea; tmpBuf[1] = 0x99; tmpBuf[2] = 0x8a; replacementLen = 3; break;
                default:
                  break;
              }
              break;
            case 0xb5:
              switch(*(p+2))
              {
                case 0xb9: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9d; tmpBuf[2] = 0xbd; replacementLen = 3; break;
                case 0xbd: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb1; tmpBuf[2] = 0xa3; replacementLen = 3; break;
                default:
                  break;
              }
              break;
            case 0xb6:
              switch(*(p+2))
              {
                case 0x8e: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9f; tmpBuf[2] = 0x86; replacementLen = 3; break;
                default:
                  break;
              }
              break;
            case 0xb8:
              switch(*(p+2))
              {
                case 0x81: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb8; tmpBuf[2] = 0x80; replacementLen = 3; break;
                case 0x83: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb8; tmpBuf[2] = 0x82; replacementLen = 3; break;
                case 0x85: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb8; tmpBuf[2] = 0x84; replacementLen = 3; break;
                case 0x87: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb8; tmpBuf[2] = 0x86; replacementLen = 3; break;
                case 0x89: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb8; tmpBuf[2] = 0x88; replacementLen = 3; break;
                case 0x8b: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb8; tmpBuf[2] = 0x8a; replacementLen = 3; break;
                case 0x8d: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb8; tmpBuf[2] = 0x8c; replacementLen = 3; break;
                case 0x8f: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb8; tmpBuf[2] = 0x8e; replacementLen = 3; break;
                case 0x91: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb8; tmpBuf[2] = 0x90; replacementLen = 3; break;
                case 0x93: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb8; tmpBuf[2] = 0x92; replacementLen = 3; break;
                case 0x95: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb8; tmpBuf[2] = 0x94; replacementLen = 3; break;
                case 0x97: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb8; tmpBuf[2] = 0x96; replacementLen = 3; break;
                case 0x99: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb8; tmpBuf[2] = 0x98; replacementLen = 3; break;
                case 0x9b: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb8; tmpBuf[2] = 0x9a; replacementLen = 3; break;
                case 0x9d: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb8; tmpBuf[2] = 0x9c; replacementLen = 3; break;
                case 0x9f: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb8; tmpBuf[2] = 0x9e; replacementLen = 3; break;
                case 0xa1: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb8; tmpBuf[2] = 0xa0; replacementLen = 3; break;
                case 0xa3: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb8; tmpBuf[2] = 0xa2; replacementLen = 3; break;
                case 0xa5: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb8; tmpBuf[2] = 0xa4; replacementLen = 3; break;
                case 0xa7: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb8; tmpBuf[2] = 0xa6; replacementLen = 3; break;
                case 0xa9: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb8; tmpBuf[2] = 0xa8; replacementLen = 3; break;
                case 0xab: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb8; tmpBuf[2] = 0xaa; replacementLen = 3; break;
                case 0xad: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb8; tmpBuf[2] = 0xac; replacementLen = 3; break;
                case 0xaf: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb8; tmpBuf[2] = 0xae; replacementLen = 3; break;
                case 0xb1: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb8; tmpBuf[2] = 0xb0; replacementLen = 3; break;
                case 0xb3: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb8; tmpBuf[2] = 0xb2; replacementLen = 3; break;
                case 0xb5: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb8; tmpBuf[2] = 0xb4; replacementLen = 3; break;
                case 0xb7: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb8; tmpBuf[2] = 0xb6; replacementLen = 3; break;
                case 0xb9: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb8; tmpBuf[2] = 0xb8; replacementLen = 3; break;
                case 0xbb: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb8; tmpBuf[2] = 0xba; replacementLen = 3; break;
                case 0xbd: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb8; tmpBuf[2] = 0xbc; replacementLen = 3; break;
                case 0xbf: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb8; tmpBuf[2] = 0xbe; replacementLen = 3; break;
                default:
                  break;
              }
              break;
            case 0xb9:
              switch(*(p+2))
              {
                case 0x81: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb9; tmpBuf[2] = 0x80; replacementLen = 3; break;
                case 0x83: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb9; tmpBuf[2] = 0x82; replacementLen = 3; break;
                case 0x85: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb9; tmpBuf[2] = 0x84; replacementLen = 3; break;
                case 0x87: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb9; tmpBuf[2] = 0x86; replacementLen = 3; break;
                case 0x89: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb9; tmpBuf[2] = 0x88; replacementLen = 3; break;
                case 0x8b: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb9; tmpBuf[2] = 0x8a; replacementLen = 3; break;
                case 0x8d: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb9; tmpBuf[2] = 0x8c; replacementLen = 3; break;
                case 0x8f: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb9; tmpBuf[2] = 0x8e; replacementLen = 3; break;
                case 0x91: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb9; tmpBuf[2] = 0x90; replacementLen = 3; break;
                case 0x93: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb9; tmpBuf[2] = 0x92; replacementLen = 3; break;
                case 0x95: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb9; tmpBuf[2] = 0x94; replacementLen = 3; break;
                case 0x97: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb9; tmpBuf[2] = 0x96; replacementLen = 3; break;
                case 0x99: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb9; tmpBuf[2] = 0x98; replacementLen = 3; break;
                case 0x9b: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb9; tmpBuf[2] = 0x9a; replacementLen = 3; break;
                case 0x9d: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb9; tmpBuf[2] = 0x9c; replacementLen = 3; break;
                case 0x9f: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb9; tmpBuf[2] = 0x9e; replacementLen = 3; break;
                case 0xa1: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb9; tmpBuf[2] = 0xa0; replacementLen = 3; break;
                case 0xa3: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb9; tmpBuf[2] = 0xa2; replacementLen = 3; break;
                case 0xa5: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb9; tmpBuf[2] = 0xa4; replacementLen = 3; break;
                case 0xa7: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb9; tmpBuf[2] = 0xa6; replacementLen = 3; break;
                case 0xa9: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb9; tmpBuf[2] = 0xa8; replacementLen = 3; break;
                case 0xab: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb9; tmpBuf[2] = 0xaa; replacementLen = 3; break;
                case 0xad: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb9; tmpBuf[2] = 0xac; replacementLen = 3; break;
                case 0xaf: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb9; tmpBuf[2] = 0xae; replacementLen = 3; break;
                case 0xb1: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb9; tmpBuf[2] = 0xb0; replacementLen = 3; break;
                case 0xb3: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb9; tmpBuf[2] = 0xb2; replacementLen = 3; break;
                case 0xb5: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb9; tmpBuf[2] = 0xb4; replacementLen = 3; break;
                case 0xb7: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb9; tmpBuf[2] = 0xb6; replacementLen = 3; break;
                case 0xb9: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb9; tmpBuf[2] = 0xb8; replacementLen = 3; break;
                case 0xbb: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb9; tmpBuf[2] = 0xba; replacementLen = 3; break;
                case 0xbd: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb9; tmpBuf[2] = 0xbc; replacementLen = 3; break;
                case 0xbf: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb9; tmpBuf[2] = 0xbe; replacementLen = 3; break;
                default:
                  break;
              }
              break;
            case 0xba:
              switch(*(p+2))
              {
                case 0x81: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xba; tmpBuf[2] = 0x80; replacementLen = 3; break;
                case 0x83: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xba; tmpBuf[2] = 0x82; replacementLen = 3; break;
                case 0x85: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xba; tmpBuf[2] = 0x84; replacementLen = 3; break;
                case 0x87: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xba; tmpBuf[2] = 0x86; replacementLen = 3; break;
                case 0x89: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xba; tmpBuf[2] = 0x88; replacementLen = 3; break;
                case 0x8b: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xba; tmpBuf[2] = 0x8a; replacementLen = 3; break;
                case 0x8d: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xba; tmpBuf[2] = 0x8c; replacementLen = 3; break;
                case 0x8f: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xba; tmpBuf[2] = 0x8e; replacementLen = 3; break;
                case 0x91: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xba; tmpBuf[2] = 0x90; replacementLen = 3; break;
                case 0x93: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xba; tmpBuf[2] = 0x92; replacementLen = 3; break;
                case 0x95: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xba; tmpBuf[2] = 0x94; replacementLen = 3; break;
                case 0x9b: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb9; tmpBuf[2] = 0xa0; replacementLen = 3; break;
                case 0xa1: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xba; tmpBuf[2] = 0xa0; replacementLen = 3; break;
                case 0xa3: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xba; tmpBuf[2] = 0xa2; replacementLen = 3; break;
                case 0xa5: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xba; tmpBuf[2] = 0xa4; replacementLen = 3; break;
                case 0xa7: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xba; tmpBuf[2] = 0xa6; replacementLen = 3; break;
                case 0xa9: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xba; tmpBuf[2] = 0xa8; replacementLen = 3; break;
                case 0xab: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xba; tmpBuf[2] = 0xaa; replacementLen = 3; break;
                case 0xad: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xba; tmpBuf[2] = 0xac; replacementLen = 3; break;
                case 0xaf: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xba; tmpBuf[2] = 0xae; replacementLen = 3; break;
                case 0xb1: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xba; tmpBuf[2] = 0xb0; replacementLen = 3; break;
                case 0xb3: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xba; tmpBuf[2] = 0xb2; replacementLen = 3; break;
                case 0xb5: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xba; tmpBuf[2] = 0xb4; replacementLen = 3; break;
                case 0xb7: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xba; tmpBuf[2] = 0xb6; replacementLen = 3; break;
                case 0xb9: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xba; tmpBuf[2] = 0xb8; replacementLen = 3; break;
                case 0xbb: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xba; tmpBuf[2] = 0xba; replacementLen = 3; break;
                case 0xbd: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xba; tmpBuf[2] = 0xbc; replacementLen = 3; break;
                case 0xbf: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xba; tmpBuf[2] = 0xbe; replacementLen = 3; break;
                default:
                  break;
              }
              break;
            case 0xbb:
              switch(*(p+2))
              {
                case 0x81: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbb; tmpBuf[2] = 0x80; replacementLen = 3; break;
                case 0x83: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbb; tmpBuf[2] = 0x82; replacementLen = 3; break;
                case 0x85: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbb; tmpBuf[2] = 0x84; replacementLen = 3; break;
                case 0x87: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbb; tmpBuf[2] = 0x86; replacementLen = 3; break;
                case 0x89: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbb; tmpBuf[2] = 0x88; replacementLen = 3; break;
                case 0x8b: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbb; tmpBuf[2] = 0x8a; replacementLen = 3; break;
                case 0x8d: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbb; tmpBuf[2] = 0x8c; replacementLen = 3; break;
                case 0x8f: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbb; tmpBuf[2] = 0x8e; replacementLen = 3; break;
                case 0x91: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbb; tmpBuf[2] = 0x90; replacementLen = 3; break;
                case 0x93: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbb; tmpBuf[2] = 0x92; replacementLen = 3; break;
                case 0x95: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbb; tmpBuf[2] = 0x94; replacementLen = 3; break;
                case 0x97: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbb; tmpBuf[2] = 0x96; replacementLen = 3; break;
                case 0x99: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbb; tmpBuf[2] = 0x98; replacementLen = 3; break;
                case 0x9b: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbb; tmpBuf[2] = 0x9a; replacementLen = 3; break;
                case 0x9d: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbb; tmpBuf[2] = 0x9c; replacementLen = 3; break;
                case 0x9f: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbb; tmpBuf[2] = 0x9e; replacementLen = 3; break;
                case 0xa1: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbb; tmpBuf[2] = 0xa0; replacementLen = 3; break;
                case 0xa3: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbb; tmpBuf[2] = 0xa2; replacementLen = 3; break;
                case 0xa5: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbb; tmpBuf[2] = 0xa4; replacementLen = 3; break;
                case 0xa7: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbb; tmpBuf[2] = 0xa6; replacementLen = 3; break;
                case 0xa9: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbb; tmpBuf[2] = 0xa8; replacementLen = 3; break;
                case 0xab: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbb; tmpBuf[2] = 0xaa; replacementLen = 3; break;
                case 0xad: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbb; tmpBuf[2] = 0xac; replacementLen = 3; break;
                case 0xaf: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbb; tmpBuf[2] = 0xae; replacementLen = 3; break;
                case 0xb1: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbb; tmpBuf[2] = 0xb0; replacementLen = 3; break;
                case 0xb3: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbb; tmpBuf[2] = 0xb2; replacementLen = 3; break;
                case 0xb5: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbb; tmpBuf[2] = 0xb4; replacementLen = 3; break;
                case 0xb7: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbb; tmpBuf[2] = 0xb6; replacementLen = 3; break;
                case 0xb9: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbb; tmpBuf[2] = 0xb8; replacementLen = 3; break;
                case 0xbb: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbb; tmpBuf[2] = 0xba; replacementLen = 3; break;
                case 0xbd: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbb; tmpBuf[2] = 0xbc; replacementLen = 3; break;
                case 0xbf: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbb; tmpBuf[2] = 0xbe; replacementLen = 3; break;
                default:
                  break;
              }
              break;
            case 0xbc:
              switch(*(p+2))
              {
                case 0x80: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbc; tmpBuf[2] = 0x88; replacementLen = 3; break;
                case 0x81: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbc; tmpBuf[2] = 0x89; replacementLen = 3; break;
                case 0x82: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbc; tmpBuf[2] = 0x8a; replacementLen = 3; break;
                case 0x83: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbc; tmpBuf[2] = 0x8b; replacementLen = 3; break;
                case 0x84: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbc; tmpBuf[2] = 0x8c; replacementLen = 3; break;
                case 0x85: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbc; tmpBuf[2] = 0x8d; replacementLen = 3; break;
                case 0x86: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbc; tmpBuf[2] = 0x8e; replacementLen = 3; break;
                case 0x87: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbc; tmpBuf[2] = 0x8f; replacementLen = 3; break;
                case 0x90: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbc; tmpBuf[2] = 0x98; replacementLen = 3; break;
                case 0x91: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbc; tmpBuf[2] = 0x99; replacementLen = 3; break;
                case 0x92: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbc; tmpBuf[2] = 0x9a; replacementLen = 3; break;
                case 0x93: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbc; tmpBuf[2] = 0x9b; replacementLen = 3; break;
                case 0x94: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbc; tmpBuf[2] = 0x9c; replacementLen = 3; break;
                case 0x95: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbc; tmpBuf[2] = 0x9d; replacementLen = 3; break;
                case 0xa0: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbc; tmpBuf[2] = 0xa8; replacementLen = 3; break;
                case 0xa1: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbc; tmpBuf[2] = 0xa9; replacementLen = 3; break;
                case 0xa2: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbc; tmpBuf[2] = 0xaa; replacementLen = 3; break;
                case 0xa3: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbc; tmpBuf[2] = 0xab; replacementLen = 3; break;
                case 0xa4: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbc; tmpBuf[2] = 0xac; replacementLen = 3; break;
                case 0xa5: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbc; tmpBuf[2] = 0xad; replacementLen = 3; break;
                case 0xa6: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbc; tmpBuf[2] = 0xae; replacementLen = 3; break;
                case 0xa7: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbc; tmpBuf[2] = 0xaf; replacementLen = 3; break;
                case 0xb0: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbc; tmpBuf[2] = 0xb8; replacementLen = 3; break;
                case 0xb1: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbc; tmpBuf[2] = 0xb9; replacementLen = 3; break;
                case 0xb2: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbc; tmpBuf[2] = 0xba; replacementLen = 3; break;
                case 0xb3: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbc; tmpBuf[2] = 0xbb; replacementLen = 3; break;
                case 0xb4: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbc; tmpBuf[2] = 0xbc; replacementLen = 3; break;
                case 0xb5: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbc; tmpBuf[2] = 0xbd; replacementLen = 3; break;
                case 0xb6: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbc; tmpBuf[2] = 0xbe; replacementLen = 3; break;
                case 0xb7: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbc; tmpBuf[2] = 0xbf; replacementLen = 3; break;
                default:
                  break;
              }
              break;
            case 0xbd:
              switch(*(p+2))
              {
                case 0x80: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbd; tmpBuf[2] = 0x88; replacementLen = 3; break;
                case 0x81: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbd; tmpBuf[2] = 0x89; replacementLen = 3; break;
                case 0x82: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbd; tmpBuf[2] = 0x8a; replacementLen = 3; break;
                case 0x83: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbd; tmpBuf[2] = 0x8b; replacementLen = 3; break;
                case 0x84: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbd; tmpBuf[2] = 0x8c; replacementLen = 3; break;
                case 0x85: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbd; tmpBuf[2] = 0x8d; replacementLen = 3; break;
                case 0x91: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbd; tmpBuf[2] = 0x99; replacementLen = 3; break;
                case 0x93: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbd; tmpBuf[2] = 0x9b; replacementLen = 3; break;
                case 0x95: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbd; tmpBuf[2] = 0x9d; replacementLen = 3; break;
                case 0x97: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbd; tmpBuf[2] = 0x9f; replacementLen = 3; break;
                case 0xa0: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbd; tmpBuf[2] = 0xa8; replacementLen = 3; break;
                case 0xa1: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbd; tmpBuf[2] = 0xa9; replacementLen = 3; break;
                case 0xa2: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbd; tmpBuf[2] = 0xaa; replacementLen = 3; break;
                case 0xa3: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbd; tmpBuf[2] = 0xab; replacementLen = 3; break;
                case 0xa4: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbd; tmpBuf[2] = 0xac; replacementLen = 3; break;
                case 0xa5: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbd; tmpBuf[2] = 0xad; replacementLen = 3; break;
                case 0xa6: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbd; tmpBuf[2] = 0xae; replacementLen = 3; break;
                case 0xa7: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbd; tmpBuf[2] = 0xaf; replacementLen = 3; break;
                case 0xb0: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbe; tmpBuf[2] = 0xba; replacementLen = 3; break;
                case 0xb1: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbe; tmpBuf[2] = 0xbb; replacementLen = 3; break;
                case 0xb2: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbf; tmpBuf[2] = 0x88; replacementLen = 3; break;
                case 0xb3: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbf; tmpBuf[2] = 0x89; replacementLen = 3; break;
                case 0xb4: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbf; tmpBuf[2] = 0x8a; replacementLen = 3; break;
                case 0xb5: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbf; tmpBuf[2] = 0x8b; replacementLen = 3; break;
                case 0xb6: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbf; tmpBuf[2] = 0x9a; replacementLen = 3; break;
                case 0xb7: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbf; tmpBuf[2] = 0x9b; replacementLen = 3; break;
                case 0xb8: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbf; tmpBuf[2] = 0xb8; replacementLen = 3; break;
                case 0xb9: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbf; tmpBuf[2] = 0xb9; replacementLen = 3; break;
                case 0xba: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbf; tmpBuf[2] = 0xaa; replacementLen = 3; break;
                case 0xbb: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbf; tmpBuf[2] = 0xab; replacementLen = 3; break;
                case 0xbc: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbf; tmpBuf[2] = 0xba; replacementLen = 3; break;
                case 0xbd: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbf; tmpBuf[2] = 0xbb; replacementLen = 3; break;
                default:
                  break;
              }
              break;
            case 0xbe:
              switch(*(p+2))
              {
                case 0x80: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbe; tmpBuf[2] = 0x88; replacementLen = 3; break;
                case 0x81: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbe; tmpBuf[2] = 0x89; replacementLen = 3; break;
                case 0x82: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbe; tmpBuf[2] = 0x8a; replacementLen = 3; break;
                case 0x83: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbe; tmpBuf[2] = 0x8b; replacementLen = 3; break;
                case 0x84: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbe; tmpBuf[2] = 0x8c; replacementLen = 3; break;
                case 0x85: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbe; tmpBuf[2] = 0x8d; replacementLen = 3; break;
                case 0x86: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbe; tmpBuf[2] = 0x8e; replacementLen = 3; break;
                case 0x87: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbe; tmpBuf[2] = 0x8f; replacementLen = 3; break;
                case 0x90: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbe; tmpBuf[2] = 0x98; replacementLen = 3; break;
                case 0x91: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbe; tmpBuf[2] = 0x99; replacementLen = 3; break;
                case 0x92: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbe; tmpBuf[2] = 0x9a; replacementLen = 3; break;
                case 0x93: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbe; tmpBuf[2] = 0x9b; replacementLen = 3; break;
                case 0x94: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbe; tmpBuf[2] = 0x9c; replacementLen = 3; break;
                case 0x95: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbe; tmpBuf[2] = 0x9d; replacementLen = 3; break;
                case 0x96: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbe; tmpBuf[2] = 0x9e; replacementLen = 3; break;
                case 0x97: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbe; tmpBuf[2] = 0x9f; replacementLen = 3; break;
                case 0xa0: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbe; tmpBuf[2] = 0xa8; replacementLen = 3; break;
                case 0xa1: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbe; tmpBuf[2] = 0xa9; replacementLen = 3; break;
                case 0xa2: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbe; tmpBuf[2] = 0xaa; replacementLen = 3; break;
                case 0xa3: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbe; tmpBuf[2] = 0xab; replacementLen = 3; break;
                case 0xa4: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbe; tmpBuf[2] = 0xac; replacementLen = 3; break;
                case 0xa5: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbe; tmpBuf[2] = 0xad; replacementLen = 3; break;
                case 0xa6: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbe; tmpBuf[2] = 0xae; replacementLen = 3; break;
                case 0xa7: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbe; tmpBuf[2] = 0xaf; replacementLen = 3; break;
                case 0xb0: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbe; tmpBuf[2] = 0xb8; replacementLen = 3; break;
                case 0xb1: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbe; tmpBuf[2] = 0xb9; replacementLen = 3; break;
                case 0xb3: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbe; tmpBuf[2] = 0xbc; replacementLen = 3; break;
                case 0xbe: tmpBuf[0] = 0xce; tmpBuf[1] = 0x99; replacementLen = 2; break;
                default:
                  break;
              }
              break;
            case 0xbf:
              switch(*(p+2))
              {
                case 0x83: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbf; tmpBuf[2] = 0x8c; replacementLen = 3; break;
                case 0x90: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbf; tmpBuf[2] = 0x98; replacementLen = 3; break;
                case 0x91: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbf; tmpBuf[2] = 0x99; replacementLen = 3; break;
                case 0xa0: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbf; tmpBuf[2] = 0xa8; replacementLen = 3; break;
                case 0xa1: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbf; tmpBuf[2] = 0xa9; replacementLen = 3; break;
                case 0xa5: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbf; tmpBuf[2] = 0xac; replacementLen = 3; break;
                case 0xb3: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbf; tmpBuf[2] = 0xbc; replacementLen = 3; break;
                default:
                  break;
              }
              break;
            default:
              break;
          }
          break;
        case 0xe2:
          switch(*(p+1))
          {
            case 0x85:
              switch(*(p+2))
              {
                case 0x8e: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x84; tmpBuf[2] = 0xb2; replacementLen = 3; break;
                case 0xb0: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x85; tmpBuf[2] = 0xa0; replacementLen = 3; break;
                case 0xb1: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x85; tmpBuf[2] = 0xa1; replacementLen = 3; break;
                case 0xb2: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x85; tmpBuf[2] = 0xa2; replacementLen = 3; break;
                case 0xb3: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x85; tmpBuf[2] = 0xa3; replacementLen = 3; break;
                case 0xb4: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x85; tmpBuf[2] = 0xa4; replacementLen = 3; break;
                case 0xb5: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x85; tmpBuf[2] = 0xa5; replacementLen = 3; break;
                case 0xb6: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x85; tmpBuf[2] = 0xa6; replacementLen = 3; break;
                case 0xb7: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x85; tmpBuf[2] = 0xa7; replacementLen = 3; break;
                case 0xb8: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x85; tmpBuf[2] = 0xa8; replacementLen = 3; break;
                case 0xb9: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x85; tmpBuf[2] = 0xa9; replacementLen = 3; break;
                case 0xba: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x85; tmpBuf[2] = 0xaa; replacementLen = 3; break;
                case 0xbb: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x85; tmpBuf[2] = 0xab; replacementLen = 3; break;
                case 0xbc: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x85; tmpBuf[2] = 0xac; replacementLen = 3; break;
                case 0xbd: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x85; tmpBuf[2] = 0xad; replacementLen = 3; break;
                case 0xbe: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x85; tmpBuf[2] = 0xae; replacementLen = 3; break;
                case 0xbf: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x85; tmpBuf[2] = 0xaf; replacementLen = 3; break;
                default:
                  break;
              }
              break;
            case 0x86:
              switch(*(p+2))
              {
                case 0x84: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x86; tmpBuf[2] = 0x83; replacementLen = 3; break;
                default:
                  break;
              }
              break;
            case 0x93:
              switch(*(p+2))
              {
                case 0x90: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x92; tmpBuf[2] = 0xb6; replacementLen = 3; break;
                case 0x91: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x92; tmpBuf[2] = 0xb7; replacementLen = 3; break;
                case 0x92: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x92; tmpBuf[2] = 0xb8; replacementLen = 3; break;
                case 0x93: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x92; tmpBuf[2] = 0xb9; replacementLen = 3; break;
                case 0x94: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x92; tmpBuf[2] = 0xba; replacementLen = 3; break;
                case 0x95: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x92; tmpBuf[2] = 0xbb; replacementLen = 3; break;
                case 0x96: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x92; tmpBuf[2] = 0xbc; replacementLen = 3; break;
                case 0x97: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x92; tmpBuf[2] = 0xbd; replacementLen = 3; break;
                case 0x98: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x92; tmpBuf[2] = 0xbe; replacementLen = 3; break;
                case 0x99: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x92; tmpBuf[2] = 0xbf; replacementLen = 3; break;
                case 0x9a: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x93; tmpBuf[2] = 0x80; replacementLen = 3; break;
                case 0x9b: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x93; tmpBuf[2] = 0x81; replacementLen = 3; break;
                case 0x9c: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x93; tmpBuf[2] = 0x82; replacementLen = 3; break;
                case 0x9d: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x93; tmpBuf[2] = 0x83; replacementLen = 3; break;
                case 0x9e: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x93; tmpBuf[2] = 0x84; replacementLen = 3; break;
                case 0x9f: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x93; tmpBuf[2] = 0x85; replacementLen = 3; break;
                case 0xa0: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x93; tmpBuf[2] = 0x86; replacementLen = 3; break;
                case 0xa1: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x93; tmpBuf[2] = 0x87; replacementLen = 3; break;
                case 0xa2: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x93; tmpBuf[2] = 0x88; replacementLen = 3; break;
                case 0xa3: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x93; tmpBuf[2] = 0x89; replacementLen = 3; break;
                case 0xa4: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x93; tmpBuf[2] = 0x8a; replacementLen = 3; break;
                case 0xa5: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x93; tmpBuf[2] = 0x8b; replacementLen = 3; break;
                case 0xa6: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x93; tmpBuf[2] = 0x8c; replacementLen = 3; break;
                case 0xa7: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x93; tmpBuf[2] = 0x8d; replacementLen = 3; break;
                case 0xa8: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x93; tmpBuf[2] = 0x8e; replacementLen = 3; break;
                case 0xa9: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x93; tmpBuf[2] = 0x8f; replacementLen = 3; break;
                default:
                  break;
              }
              break;
            case 0xb0:
              switch(*(p+2))
              {
                case 0xb0: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb0; tmpBuf[2] = 0x80; replacementLen = 3; break;
                case 0xb1: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb0; tmpBuf[2] = 0x81; replacementLen = 3; break;
                case 0xb2: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb0; tmpBuf[2] = 0x82; replacementLen = 3; break;
                case 0xb3: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb0; tmpBuf[2] = 0x83; replacementLen = 3; break;
                case 0xb4: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb0; tmpBuf[2] = 0x84; replacementLen = 3; break;
                case 0xb5: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb0; tmpBuf[2] = 0x85; replacementLen = 3; break;
                case 0xb6: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb0; tmpBuf[2] = 0x86; replacementLen = 3; break;
                case 0xb7: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb0; tmpBuf[2] = 0x87; replacementLen = 3; break;
                case 0xb8: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb0; tmpBuf[2] = 0x88; replacementLen = 3; break;
                case 0xb9: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb0; tmpBuf[2] = 0x89; replacementLen = 3; break;
                case 0xba: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb0; tmpBuf[2] = 0x8a; replacementLen = 3; break;
                case 0xbb: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb0; tmpBuf[2] = 0x8b; replacementLen = 3; break;
                case 0xbc: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb0; tmpBuf[2] = 0x8c; replacementLen = 3; break;
                case 0xbd: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb0; tmpBuf[2] = 0x8d; replacementLen = 3; break;
                case 0xbe: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb0; tmpBuf[2] = 0x8e; replacementLen = 3; break;
                case 0xbf: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb0; tmpBuf[2] = 0x8f; replacementLen = 3; break;
                default:
                  break;
              }
              break;
            case 0xb1:
              switch(*(p+2))
              {
                case 0x80: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb0; tmpBuf[2] = 0x90; replacementLen = 3; break;
                case 0x81: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb0; tmpBuf[2] = 0x91; replacementLen = 3; break;
                case 0x82: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb0; tmpBuf[2] = 0x92; replacementLen = 3; break;
                case 0x83: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb0; tmpBuf[2] = 0x93; replacementLen = 3; break;
                case 0x84: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb0; tmpBuf[2] = 0x94; replacementLen = 3; break;
                case 0x85: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb0; tmpBuf[2] = 0x95; replacementLen = 3; break;
                case 0x86: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb0; tmpBuf[2] = 0x96; replacementLen = 3; break;
                case 0x87: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb0; tmpBuf[2] = 0x97; replacementLen = 3; break;
                case 0x88: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb0; tmpBuf[2] = 0x98; replacementLen = 3; break;
                case 0x89: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb0; tmpBuf[2] = 0x99; replacementLen = 3; break;
                case 0x8a: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb0; tmpBuf[2] = 0x9a; replacementLen = 3; break;
                case 0x8b: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb0; tmpBuf[2] = 0x9b; replacementLen = 3; break;
                case 0x8c: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb0; tmpBuf[2] = 0x9c; replacementLen = 3; break;
                case 0x8d: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb0; tmpBuf[2] = 0x9d; replacementLen = 3; break;
                case 0x8e: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb0; tmpBuf[2] = 0x9e; replacementLen = 3; break;
                case 0x8f: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb0; tmpBuf[2] = 0x9f; replacementLen = 3; break;
                case 0x90: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb0; tmpBuf[2] = 0xa0; replacementLen = 3; break;
                case 0x91: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb0; tmpBuf[2] = 0xa1; replacementLen = 3; break;
                case 0x92: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb0; tmpBuf[2] = 0xa2; replacementLen = 3; break;
                case 0x93: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb0; tmpBuf[2] = 0xa3; replacementLen = 3; break;
                case 0x94: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb0; tmpBuf[2] = 0xa4; replacementLen = 3; break;
                case 0x95: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb0; tmpBuf[2] = 0xa5; replacementLen = 3; break;
                case 0x96: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb0; tmpBuf[2] = 0xa6; replacementLen = 3; break;
                case 0x97: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb0; tmpBuf[2] = 0xa7; replacementLen = 3; break;
                case 0x98: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb0; tmpBuf[2] = 0xa8; replacementLen = 3; break;
                case 0x99: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb0; tmpBuf[2] = 0xa9; replacementLen = 3; break;
                case 0x9a: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb0; tmpBuf[2] = 0xaa; replacementLen = 3; break;
                case 0x9b: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb0; tmpBuf[2] = 0xab; replacementLen = 3; break;
                case 0x9c: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb0; tmpBuf[2] = 0xac; replacementLen = 3; break;
                case 0x9d: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb0; tmpBuf[2] = 0xad; replacementLen = 3; break;
                case 0x9e: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb0; tmpBuf[2] = 0xae; replacementLen = 3; break;
                case 0xa1: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb1; tmpBuf[2] = 0xa0; replacementLen = 3; break;
                case 0xa5: tmpBuf[0] = 0xc8; tmpBuf[1] = 0xba; replacementLen = 2; break;
                case 0xa6: tmpBuf[0] = 0xc8; tmpBuf[1] = 0xbe; replacementLen = 2; break;
                case 0xa8: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb1; tmpBuf[2] = 0xa7; replacementLen = 3; break;
                case 0xaa: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb1; tmpBuf[2] = 0xa9; replacementLen = 3; break;
                case 0xac: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb1; tmpBuf[2] = 0xab; replacementLen = 3; break;
                case 0xb3: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb1; tmpBuf[2] = 0xb2; replacementLen = 3; break;
                case 0xb6: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb1; tmpBuf[2] = 0xb5; replacementLen = 3; break;
                default:
                  break;
              }
              break;
            case 0xb2:
              switch(*(p+2))
              {
                case 0x81: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb2; tmpBuf[2] = 0x80; replacementLen = 3; break;
                case 0x83: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb2; tmpBuf[2] = 0x82; replacementLen = 3; break;
                case 0x85: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb2; tmpBuf[2] = 0x84; replacementLen = 3; break;
                case 0x87: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb2; tmpBuf[2] = 0x86; replacementLen = 3; break;
                case 0x89: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb2; tmpBuf[2] = 0x88; replacementLen = 3; break;
                case 0x8b: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb2; tmpBuf[2] = 0x8a; replacementLen = 3; break;
                case 0x8d: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb2; tmpBuf[2] = 0x8c; replacementLen = 3; break;
                case 0x8f: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb2; tmpBuf[2] = 0x8e; replacementLen = 3; break;
                case 0x91: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb2; tmpBuf[2] = 0x90; replacementLen = 3; break;
                case 0x93: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb2; tmpBuf[2] = 0x92; replacementLen = 3; break;
                case 0x95: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb2; tmpBuf[2] = 0x94; replacementLen = 3; break;
                case 0x97: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb2; tmpBuf[2] = 0x96; replacementLen = 3; break;
                case 0x99: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb2; tmpBuf[2] = 0x98; replacementLen = 3; break;
                case 0x9b: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb2; tmpBuf[2] = 0x9a; replacementLen = 3; break;
                case 0x9d: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb2; tmpBuf[2] = 0x9c; replacementLen = 3; break;
                case 0x9f: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb2; tmpBuf[2] = 0x9e; replacementLen = 3; break;
                case 0xa1: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb2; tmpBuf[2] = 0xa0; replacementLen = 3; break;
                case 0xa3: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb2; tmpBuf[2] = 0xa2; replacementLen = 3; break;
                case 0xa5: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb2; tmpBuf[2] = 0xa4; replacementLen = 3; break;
                case 0xa7: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb2; tmpBuf[2] = 0xa6; replacementLen = 3; break;
                case 0xa9: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb2; tmpBuf[2] = 0xa8; replacementLen = 3; break;
                case 0xab: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb2; tmpBuf[2] = 0xaa; replacementLen = 3; break;
                case 0xad: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb2; tmpBuf[2] = 0xac; replacementLen = 3; break;
                case 0xaf: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb2; tmpBuf[2] = 0xae; replacementLen = 3; break;
                case 0xb1: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb2; tmpBuf[2] = 0xb0; replacementLen = 3; break;
                case 0xb3: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb2; tmpBuf[2] = 0xb2; replacementLen = 3; break;
                case 0xb5: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb2; tmpBuf[2] = 0xb4; replacementLen = 3; break;
                case 0xb7: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb2; tmpBuf[2] = 0xb6; replacementLen = 3; break;
                case 0xb9: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb2; tmpBuf[2] = 0xb8; replacementLen = 3; break;
                case 0xbb: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb2; tmpBuf[2] = 0xba; replacementLen = 3; break;
                case 0xbd: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb2; tmpBuf[2] = 0xbc; replacementLen = 3; break;
                case 0xbf: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb2; tmpBuf[2] = 0xbe; replacementLen = 3; break;
                default:
                  break;
              }
              break;
            case 0xb3:
              switch(*(p+2))
              {
                case 0x81: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb3; tmpBuf[2] = 0x80; replacementLen = 3; break;
                case 0x83: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb3; tmpBuf[2] = 0x82; replacementLen = 3; break;
                case 0x85: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb3; tmpBuf[2] = 0x84; replacementLen = 3; break;
                case 0x87: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb3; tmpBuf[2] = 0x86; replacementLen = 3; break;
                case 0x89: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb3; tmpBuf[2] = 0x88; replacementLen = 3; break;
                case 0x8b: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb3; tmpBuf[2] = 0x8a; replacementLen = 3; break;
                case 0x8d: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb3; tmpBuf[2] = 0x8c; replacementLen = 3; break;
                case 0x8f: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb3; tmpBuf[2] = 0x8e; replacementLen = 3; break;
                case 0x91: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb3; tmpBuf[2] = 0x90; replacementLen = 3; break;
                case 0x93: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb3; tmpBuf[2] = 0x92; replacementLen = 3; break;
                case 0x95: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb3; tmpBuf[2] = 0x94; replacementLen = 3; break;
                case 0x97: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb3; tmpBuf[2] = 0x96; replacementLen = 3; break;
                case 0x99: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb3; tmpBuf[2] = 0x98; replacementLen = 3; break;
                case 0x9b: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb3; tmpBuf[2] = 0x9a; replacementLen = 3; break;
                case 0x9d: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb3; tmpBuf[2] = 0x9c; replacementLen = 3; break;
                case 0x9f: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb3; tmpBuf[2] = 0x9e; replacementLen = 3; break;
                case 0xa1: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb3; tmpBuf[2] = 0xa0; replacementLen = 3; break;
                case 0xa3: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb3; tmpBuf[2] = 0xa2; replacementLen = 3; break;
                case 0xac: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb3; tmpBuf[2] = 0xab; replacementLen = 3; break;
                case 0xae: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb3; tmpBuf[2] = 0xad; replacementLen = 3; break;
                case 0xb3: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb3; tmpBuf[2] = 0xb2; replacementLen = 3; break;
                default:
                  break;
              }
              break;
            case 0xb4:
              switch(*(p+2))
              {
                case 0x80: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x82; tmpBuf[2] = 0xa0; replacementLen = 3; break;
                case 0x81: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x82; tmpBuf[2] = 0xa1; replacementLen = 3; break;
                case 0x82: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x82; tmpBuf[2] = 0xa2; replacementLen = 3; break;
                case 0x83: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x82; tmpBuf[2] = 0xa3; replacementLen = 3; break;
                case 0x84: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x82; tmpBuf[2] = 0xa4; replacementLen = 3; break;
                case 0x85: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x82; tmpBuf[2] = 0xa5; replacementLen = 3; break;
                case 0x86: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x82; tmpBuf[2] = 0xa6; replacementLen = 3; break;
                case 0x87: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x82; tmpBuf[2] = 0xa7; replacementLen = 3; break;
                case 0x88: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x82; tmpBuf[2] = 0xa8; replacementLen = 3; break;
                case 0x89: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x82; tmpBuf[2] = 0xa9; replacementLen = 3; break;
                case 0x8a: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x82; tmpBuf[2] = 0xaa; replacementLen = 3; break;
                case 0x8b: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x82; tmpBuf[2] = 0xab; replacementLen = 3; break;
                case 0x8c: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x82; tmpBuf[2] = 0xac; replacementLen = 3; break;
                case 0x8d: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x82; tmpBuf[2] = 0xad; replacementLen = 3; break;
                case 0x8e: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x82; tmpBuf[2] = 0xae; replacementLen = 3; break;
                case 0x8f: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x82; tmpBuf[2] = 0xaf; replacementLen = 3; break;
                case 0x90: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x82; tmpBuf[2] = 0xb0; replacementLen = 3; break;
                case 0x91: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x82; tmpBuf[2] = 0xb1; replacementLen = 3; break;
                case 0x92: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x82; tmpBuf[2] = 0xb2; replacementLen = 3; break;
                case 0x93: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x82; tmpBuf[2] = 0xb3; replacementLen = 3; break;
                case 0x94: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x82; tmpBuf[2] = 0xb4; replacementLen = 3; break;
                case 0x95: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x82; tmpBuf[2] = 0xb5; replacementLen = 3; break;
                case 0x96: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x82; tmpBuf[2] = 0xb6; replacementLen = 3; break;
                case 0x97: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x82; tmpBuf[2] = 0xb7; replacementLen = 3; break;
                case 0x98: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x82; tmpBuf[2] = 0xb8; replacementLen = 3; break;
                case 0x99: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x82; tmpBuf[2] = 0xb9; replacementLen = 3; break;
                case 0x9a: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x82; tmpBuf[2] = 0xba; replacementLen = 3; break;
                case 0x9b: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x82; tmpBuf[2] = 0xbb; replacementLen = 3; break;
                case 0x9c: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x82; tmpBuf[2] = 0xbc; replacementLen = 3; break;
                case 0x9d: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x82; tmpBuf[2] = 0xbd; replacementLen = 3; break;
                case 0x9e: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x82; tmpBuf[2] = 0xbe; replacementLen = 3; break;
                case 0x9f: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x82; tmpBuf[2] = 0xbf; replacementLen = 3; break;
                case 0xa0: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x83; tmpBuf[2] = 0x80; replacementLen = 3; break;
                case 0xa1: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x83; tmpBuf[2] = 0x81; replacementLen = 3; break;
                case 0xa2: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x83; tmpBuf[2] = 0x82; replacementLen = 3; break;
                case 0xa3: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x83; tmpBuf[2] = 0x83; replacementLen = 3; break;
                case 0xa4: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x83; tmpBuf[2] = 0x84; replacementLen = 3; break;
                case 0xa5: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x83; tmpBuf[2] = 0x85; replacementLen = 3; break;
                case 0xa7: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x83; tmpBuf[2] = 0x87; replacementLen = 3; break;
                case 0xad: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x83; tmpBuf[2] = 0x8d; replacementLen = 3; break;
                default:
                  break;
              }
              break;
            default:
              break;
          }
          break;
        case 0xea:
          switch(*(p+1))
          {
            case 0x99:
              switch(*(p+2))
              {
                case 0x81: tmpBuf[0] = 0xea; tmpBuf[1] = 0x99; tmpBuf[2] = 0x80; replacementLen = 3; break;
                case 0x83: tmpBuf[0] = 0xea; tmpBuf[1] = 0x99; tmpBuf[2] = 0x82; replacementLen = 3; break;
                case 0x85: tmpBuf[0] = 0xea; tmpBuf[1] = 0x99; tmpBuf[2] = 0x84; replacementLen = 3; break;
                case 0x87: tmpBuf[0] = 0xea; tmpBuf[1] = 0x99; tmpBuf[2] = 0x86; replacementLen = 3; break;
                case 0x89: tmpBuf[0] = 0xea; tmpBuf[1] = 0x99; tmpBuf[2] = 0x88; replacementLen = 3; break;
                case 0x8b: tmpBuf[0] = 0xea; tmpBuf[1] = 0x99; tmpBuf[2] = 0x8a; replacementLen = 3; break;
                case 0x8d: tmpBuf[0] = 0xea; tmpBuf[1] = 0x99; tmpBuf[2] = 0x8c; replacementLen = 3; break;
                case 0x8f: tmpBuf[0] = 0xea; tmpBuf[1] = 0x99; tmpBuf[2] = 0x8e; replacementLen = 3; break;
                case 0x91: tmpBuf[0] = 0xea; tmpBuf[1] = 0x99; tmpBuf[2] = 0x90; replacementLen = 3; break;
                case 0x93: tmpBuf[0] = 0xea; tmpBuf[1] = 0x99; tmpBuf[2] = 0x92; replacementLen = 3; break;
                case 0x95: tmpBuf[0] = 0xea; tmpBuf[1] = 0x99; tmpBuf[2] = 0x94; replacementLen = 3; break;
                case 0x97: tmpBuf[0] = 0xea; tmpBuf[1] = 0x99; tmpBuf[2] = 0x96; replacementLen = 3; break;
                case 0x99: tmpBuf[0] = 0xea; tmpBuf[1] = 0x99; tmpBuf[2] = 0x98; replacementLen = 3; break;
                case 0x9b: tmpBuf[0] = 0xea; tmpBuf[1] = 0x99; tmpBuf[2] = 0x9a; replacementLen = 3; break;
                case 0x9d: tmpBuf[0] = 0xea; tmpBuf[1] = 0x99; tmpBuf[2] = 0x9c; replacementLen = 3; break;
                case 0x9f: tmpBuf[0] = 0xea; tmpBuf[1] = 0x99; tmpBuf[2] = 0x9e; replacementLen = 3; break;
                case 0xa1: tmpBuf[0] = 0xea; tmpBuf[1] = 0x99; tmpBuf[2] = 0xa0; replacementLen = 3; break;
                case 0xa3: tmpBuf[0] = 0xea; tmpBuf[1] = 0x99; tmpBuf[2] = 0xa2; replacementLen = 3; break;
                case 0xa5: tmpBuf[0] = 0xea; tmpBuf[1] = 0x99; tmpBuf[2] = 0xa4; replacementLen = 3; break;
                case 0xa7: tmpBuf[0] = 0xea; tmpBuf[1] = 0x99; tmpBuf[2] = 0xa6; replacementLen = 3; break;
                case 0xa9: tmpBuf[0] = 0xea; tmpBuf[1] = 0x99; tmpBuf[2] = 0xa8; replacementLen = 3; break;
                case 0xab: tmpBuf[0] = 0xea; tmpBuf[1] = 0x99; tmpBuf[2] = 0xaa; replacementLen = 3; break;
                case 0xad: tmpBuf[0] = 0xea; tmpBuf[1] = 0x99; tmpBuf[2] = 0xac; replacementLen = 3; break;
                default:
                  break;
              }
              break;
            case 0x9a:
              switch(*(p+2))
              {
                case 0x81: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9a; tmpBuf[2] = 0x80; replacementLen = 3; break;
                case 0x83: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9a; tmpBuf[2] = 0x82; replacementLen = 3; break;
                case 0x85: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9a; tmpBuf[2] = 0x84; replacementLen = 3; break;
                case 0x87: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9a; tmpBuf[2] = 0x86; replacementLen = 3; break;
                case 0x89: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9a; tmpBuf[2] = 0x88; replacementLen = 3; break;
                case 0x8b: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9a; tmpBuf[2] = 0x8a; replacementLen = 3; break;
                case 0x8d: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9a; tmpBuf[2] = 0x8c; replacementLen = 3; break;
                case 0x8f: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9a; tmpBuf[2] = 0x8e; replacementLen = 3; break;
                case 0x91: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9a; tmpBuf[2] = 0x90; replacementLen = 3; break;
                case 0x93: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9a; tmpBuf[2] = 0x92; replacementLen = 3; break;
                case 0x95: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9a; tmpBuf[2] = 0x94; replacementLen = 3; break;
                case 0x97: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9a; tmpBuf[2] = 0x96; replacementLen = 3; break;
                case 0x99: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9a; tmpBuf[2] = 0x98; replacementLen = 3; break;
                case 0x9b: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9a; tmpBuf[2] = 0x9a; replacementLen = 3; break;
                default:
                  break;
              }
              break;
            case 0x9c:
              switch(*(p+2))
              {
                case 0xa3: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9c; tmpBuf[2] = 0xa2; replacementLen = 3; break;
                case 0xa5: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9c; tmpBuf[2] = 0xa4; replacementLen = 3; break;
                case 0xa7: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9c; tmpBuf[2] = 0xa6; replacementLen = 3; break;
                case 0xa9: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9c; tmpBuf[2] = 0xa8; replacementLen = 3; break;
                case 0xab: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9c; tmpBuf[2] = 0xaa; replacementLen = 3; break;
                case 0xad: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9c; tmpBuf[2] = 0xac; replacementLen = 3; break;
                case 0xaf: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9c; tmpBuf[2] = 0xae; replacementLen = 3; break;
                case 0xb3: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9c; tmpBuf[2] = 0xb2; replacementLen = 3; break;
                case 0xb5: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9c; tmpBuf[2] = 0xb4; replacementLen = 3; break;
                case 0xb7: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9c; tmpBuf[2] = 0xb6; replacementLen = 3; break;
                case 0xb9: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9c; tmpBuf[2] = 0xb8; replacementLen = 3; break;
                case 0xbb: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9c; tmpBuf[2] = 0xba; replacementLen = 3; break;
                case 0xbd: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9c; tmpBuf[2] = 0xbc; replacementLen = 3; break;
                case 0xbf: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9c; tmpBuf[2] = 0xbe; replacementLen = 3; break;
                default:
                  break;
              }
              break;
            case 0x9d:
              switch(*(p+2))
              {
                case 0x81: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9d; tmpBuf[2] = 0x80; replacementLen = 3; break;
                case 0x83: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9d; tmpBuf[2] = 0x82; replacementLen = 3; break;
                case 0x85: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9d; tmpBuf[2] = 0x84; replacementLen = 3; break;
                case 0x87: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9d; tmpBuf[2] = 0x86; replacementLen = 3; break;
                case 0x89: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9d; tmpBuf[2] = 0x88; replacementLen = 3; break;
                case 0x8b: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9d; tmpBuf[2] = 0x8a; replacementLen = 3; break;
                case 0x8d: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9d; tmpBuf[2] = 0x8c; replacementLen = 3; break;
                case 0x8f: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9d; tmpBuf[2] = 0x8e; replacementLen = 3; break;
                case 0x91: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9d; tmpBuf[2] = 0x90; replacementLen = 3; break;
                case 0x93: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9d; tmpBuf[2] = 0x92; replacementLen = 3; break;
                case 0x95: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9d; tmpBuf[2] = 0x94; replacementLen = 3; break;
                case 0x97: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9d; tmpBuf[2] = 0x96; replacementLen = 3; break;
                case 0x99: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9d; tmpBuf[2] = 0x98; replacementLen = 3; break;
                case 0x9b: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9d; tmpBuf[2] = 0x9a; replacementLen = 3; break;
                case 0x9d: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9d; tmpBuf[2] = 0x9c; replacementLen = 3; break;
                case 0x9f: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9d; tmpBuf[2] = 0x9e; replacementLen = 3; break;
                case 0xa1: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9d; tmpBuf[2] = 0xa0; replacementLen = 3; break;
                case 0xa3: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9d; tmpBuf[2] = 0xa2; replacementLen = 3; break;
                case 0xa5: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9d; tmpBuf[2] = 0xa4; replacementLen = 3; break;
                case 0xa7: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9d; tmpBuf[2] = 0xa6; replacementLen = 3; break;
                case 0xa9: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9d; tmpBuf[2] = 0xa8; replacementLen = 3; break;
                case 0xab: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9d; tmpBuf[2] = 0xaa; replacementLen = 3; break;
                case 0xad: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9d; tmpBuf[2] = 0xac; replacementLen = 3; break;
                case 0xaf: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9d; tmpBuf[2] = 0xae; replacementLen = 3; break;
                case 0xba: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9d; tmpBuf[2] = 0xb9; replacementLen = 3; break;
                case 0xbc: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9d; tmpBuf[2] = 0xbb; replacementLen = 3; break;
                case 0xbf: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9d; tmpBuf[2] = 0xbe; replacementLen = 3; break;
                default:
                  break;
              }
              break;
            case 0x9e:
              switch(*(p+2))
              {
                case 0x81: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9e; tmpBuf[2] = 0x80; replacementLen = 3; break;
                case 0x83: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9e; tmpBuf[2] = 0x82; replacementLen = 3; break;
                case 0x85: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9e; tmpBuf[2] = 0x84; replacementLen = 3; break;
                case 0x87: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9e; tmpBuf[2] = 0x86; replacementLen = 3; break;
                case 0x8c: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9e; tmpBuf[2] = 0x8b; replacementLen = 3; break;
                case 0x91: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9e; tmpBuf[2] = 0x90; replacementLen = 3; break;
                case 0x93: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9e; tmpBuf[2] = 0x92; replacementLen = 3; break;
                case 0x94: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9f; tmpBuf[2] = 0x84; replacementLen = 3; break;
                case 0x97: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9e; tmpBuf[2] = 0x96; replacementLen = 3; break;
                case 0x99: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9e; tmpBuf[2] = 0x98; replacementLen = 3; break;
                case 0x9b: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9e; tmpBuf[2] = 0x9a; replacementLen = 3; break;
                case 0x9d: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9e; tmpBuf[2] = 0x9c; replacementLen = 3; break;
                case 0x9f: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9e; tmpBuf[2] = 0x9e; replacementLen = 3; break;
                case 0xa1: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa0; replacementLen = 3; break;
                case 0xa3: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa2; replacementLen = 3; break;
                case 0xa5: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa4; replacementLen = 3; break;
                case 0xa7: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa6; replacementLen = 3; break;
                case 0xa9: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa8; replacementLen = 3; break;
                case 0xb5: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xb4; replacementLen = 3; break;
                case 0xb7: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xb6; replacementLen = 3; break;
                case 0xb9: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xb8; replacementLen = 3; break;
                case 0xbb: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xba; replacementLen = 3; break;
                case 0xbd: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xbc; replacementLen = 3; break;
                case 0xbf: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xbe; replacementLen = 3; break;
                default:
                  break;
              }
              break;
            case 0x9f:
              switch(*(p+2))
              {
                case 0x83: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9f; tmpBuf[2] = 0x82; replacementLen = 3; break;
                case 0x88: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9f; tmpBuf[2] = 0x87; replacementLen = 3; break;
                case 0x8a: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9f; tmpBuf[2] = 0x89; replacementLen = 3; break;
                case 0xb6: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9f; tmpBuf[2] = 0xb5; replacementLen = 3; break;
                default:
                  break;
              }
              break;
            case 0xad:
              switch(*(p+2))
              {
                case 0x93: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xb3; replacementLen = 3; break;
                case 0xb0: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8e; tmpBuf[2] = 0xa0; replacementLen = 3; break;
                case 0xb1: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8e; tmpBuf[2] = 0xa1; replacementLen = 3; break;
                case 0xb2: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8e; tmpBuf[2] = 0xa2; replacementLen = 3; break;
                case 0xb3: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8e; tmpBuf[2] = 0xa3; replacementLen = 3; break;
                case 0xb4: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8e; tmpBuf[2] = 0xa4; replacementLen = 3; break;
                case 0xb5: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8e; tmpBuf[2] = 0xa5; replacementLen = 3; break;
                case 0xb6: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8e; tmpBuf[2] = 0xa6; replacementLen = 3; break;
                case 0xb7: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8e; tmpBuf[2] = 0xa7; replacementLen = 3; break;
                case 0xb8: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8e; tmpBuf[2] = 0xa8; replacementLen = 3; break;
                case 0xb9: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8e; tmpBuf[2] = 0xa9; replacementLen = 3; break;
                case 0xba: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8e; tmpBuf[2] = 0xaa; replacementLen = 3; break;
                case 0xbb: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8e; tmpBuf[2] = 0xab; replacementLen = 3; break;
                case 0xbc: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8e; tmpBuf[2] = 0xac; replacementLen = 3; break;
                case 0xbd: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8e; tmpBuf[2] = 0xad; replacementLen = 3; break;
                case 0xbe: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8e; tmpBuf[2] = 0xae; replacementLen = 3; break;
                case 0xbf: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8e; tmpBuf[2] = 0xaf; replacementLen = 3; break;
                default:
                  break;
              }
              break;
            case 0xae:
              switch(*(p+2))
              {
                case 0x80: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8e; tmpBuf[2] = 0xb0; replacementLen = 3; break;
                case 0x81: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8e; tmpBuf[2] = 0xb1; replacementLen = 3; break;
                case 0x82: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8e; tmpBuf[2] = 0xb2; replacementLen = 3; break;
                case 0x83: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8e; tmpBuf[2] = 0xb3; replacementLen = 3; break;
                case 0x84: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8e; tmpBuf[2] = 0xb4; replacementLen = 3; break;
                case 0x85: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8e; tmpBuf[2] = 0xb5; replacementLen = 3; break;
                case 0x86: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8e; tmpBuf[2] = 0xb6; replacementLen = 3; break;
                case 0x87: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8e; tmpBuf[2] = 0xb7; replacementLen = 3; break;
                case 0x88: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8e; tmpBuf[2] = 0xb8; replacementLen = 3; break;
                case 0x89: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8e; tmpBuf[2] = 0xb9; replacementLen = 3; break;
                case 0x8a: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8e; tmpBuf[2] = 0xba; replacementLen = 3; break;
                case 0x8b: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8e; tmpBuf[2] = 0xbb; replacementLen = 3; break;
                case 0x8c: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8e; tmpBuf[2] = 0xbc; replacementLen = 3; break;
                case 0x8d: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8e; tmpBuf[2] = 0xbd; replacementLen = 3; break;
                case 0x8e: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8e; tmpBuf[2] = 0xbe; replacementLen = 3; break;
                case 0x8f: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8e; tmpBuf[2] = 0xbf; replacementLen = 3; break;
                case 0x90: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8f; tmpBuf[2] = 0x80; replacementLen = 3; break;
                case 0x91: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8f; tmpBuf[2] = 0x81; replacementLen = 3; break;
                case 0x92: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8f; tmpBuf[2] = 0x82; replacementLen = 3; break;
                case 0x93: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8f; tmpBuf[2] = 0x83; replacementLen = 3; break;
                case 0x94: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8f; tmpBuf[2] = 0x84; replacementLen = 3; break;
                case 0x95: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8f; tmpBuf[2] = 0x85; replacementLen = 3; break;
                case 0x96: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8f; tmpBuf[2] = 0x86; replacementLen = 3; break;
                case 0x97: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8f; tmpBuf[2] = 0x87; replacementLen = 3; break;
                case 0x98: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8f; tmpBuf[2] = 0x88; replacementLen = 3; break;
                case 0x99: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8f; tmpBuf[2] = 0x89; replacementLen = 3; break;
                case 0x9a: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8f; tmpBuf[2] = 0x8a; replacementLen = 3; break;
                case 0x9b: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8f; tmpBuf[2] = 0x8b; replacementLen = 3; break;
                case 0x9c: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8f; tmpBuf[2] = 0x8c; replacementLen = 3; break;
                case 0x9d: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8f; tmpBuf[2] = 0x8d; replacementLen = 3; break;
                case 0x9e: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8f; tmpBuf[2] = 0x8e; replacementLen = 3; break;
                case 0x9f: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8f; tmpBuf[2] = 0x8f; replacementLen = 3; break;
                case 0xa0: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8f; tmpBuf[2] = 0x90; replacementLen = 3; break;
                case 0xa1: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8f; tmpBuf[2] = 0x91; replacementLen = 3; break;
                case 0xa2: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8f; tmpBuf[2] = 0x92; replacementLen = 3; break;
                case 0xa3: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8f; tmpBuf[2] = 0x93; replacementLen = 3; break;
                case 0xa4: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8f; tmpBuf[2] = 0x94; replacementLen = 3; break;
                case 0xa5: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8f; tmpBuf[2] = 0x95; replacementLen = 3; break;
                case 0xa6: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8f; tmpBuf[2] = 0x96; replacementLen = 3; break;
                case 0xa7: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8f; tmpBuf[2] = 0x97; replacementLen = 3; break;
                case 0xa8: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8f; tmpBuf[2] = 0x98; replacementLen = 3; break;
                case 0xa9: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8f; tmpBuf[2] = 0x99; replacementLen = 3; break;
                case 0xaa: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8f; tmpBuf[2] = 0x9a; replacementLen = 3; break;
                case 0xab: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8f; tmpBuf[2] = 0x9b; replacementLen = 3; break;
                case 0xac: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8f; tmpBuf[2] = 0x9c; replacementLen = 3; break;
                case 0xad: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8f; tmpBuf[2] = 0x9d; replacementLen = 3; break;
                case 0xae: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8f; tmpBuf[2] = 0x9e; replacementLen = 3; break;
                case 0xaf: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8f; tmpBuf[2] = 0x9f; replacementLen = 3; break;
                case 0xb0: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8f; tmpBuf[2] = 0xa0; replacementLen = 3; break;
                case 0xb1: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8f; tmpBuf[2] = 0xa1; replacementLen = 3; break;
                case 0xb2: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8f; tmpBuf[2] = 0xa2; replacementLen = 3; break;
                case 0xb3: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8f; tmpBuf[2] = 0xa3; replacementLen = 3; break;
                case 0xb4: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8f; tmpBuf[2] = 0xa4; replacementLen = 3; break;
                case 0xb5: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8f; tmpBuf[2] = 0xa5; replacementLen = 3; break;
                case 0xb6: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8f; tmpBuf[2] = 0xa6; replacementLen = 3; break;
                case 0xb7: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8f; tmpBuf[2] = 0xa7; replacementLen = 3; break;
                case 0xb8: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8f; tmpBuf[2] = 0xa8; replacementLen = 3; break;
                case 0xb9: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8f; tmpBuf[2] = 0xa9; replacementLen = 3; break;
                case 0xba: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8f; tmpBuf[2] = 0xaa; replacementLen = 3; break;
                case 0xbb: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8f; tmpBuf[2] = 0xab; replacementLen = 3; break;
                case 0xbc: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8f; tmpBuf[2] = 0xac; replacementLen = 3; break;
                case 0xbd: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8f; tmpBuf[2] = 0xad; replacementLen = 3; break;
                case 0xbe: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8f; tmpBuf[2] = 0xae; replacementLen = 3; break;
                case 0xbf: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8f; tmpBuf[2] = 0xaf; replacementLen = 3; break;
                default:
                  break;
              }
              break;
            default:
              break;
          }
          break;
        case 0xef:
          switch(*(p+1))
          {
            case 0xbd:
              switch(*(p+2))
              {
                case 0x81: tmpBuf[0] = 0xef; tmpBuf[1] = 0xbc; tmpBuf[2] = 0xa1; replacementLen = 3; break;
                case 0x82: tmpBuf[0] = 0xef; tmpBuf[1] = 0xbc; tmpBuf[2] = 0xa2; replacementLen = 3; break;
                case 0x83: tmpBuf[0] = 0xef; tmpBuf[1] = 0xbc; tmpBuf[2] = 0xa3; replacementLen = 3; break;
                case 0x84: tmpBuf[0] = 0xef; tmpBuf[1] = 0xbc; tmpBuf[2] = 0xa4; replacementLen = 3; break;
                case 0x85: tmpBuf[0] = 0xef; tmpBuf[1] = 0xbc; tmpBuf[2] = 0xa5; replacementLen = 3; break;
                case 0x86: tmpBuf[0] = 0xef; tmpBuf[1] = 0xbc; tmpBuf[2] = 0xa6; replacementLen = 3; break;
                case 0x87: tmpBuf[0] = 0xef; tmpBuf[1] = 0xbc; tmpBuf[2] = 0xa7; replacementLen = 3; break;
                case 0x88: tmpBuf[0] = 0xef; tmpBuf[1] = 0xbc; tmpBuf[2] = 0xa8; replacementLen = 3; break;
                case 0x89: tmpBuf[0] = 0xef; tmpBuf[1] = 0xbc; tmpBuf[2] = 0xa9; replacementLen = 3; break;
                case 0x8a: tmpBuf[0] = 0xef; tmpBuf[1] = 0xbc; tmpBuf[2] = 0xaa; replacementLen = 3; break;
                case 0x8b: tmpBuf[0] = 0xef; tmpBuf[1] = 0xbc; tmpBuf[2] = 0xab; replacementLen = 3; break;
                case 0x8c: tmpBuf[0] = 0xef; tmpBuf[1] = 0xbc; tmpBuf[2] = 0xac; replacementLen = 3; break;
                case 0x8d: tmpBuf[0] = 0xef; tmpBuf[1] = 0xbc; tmpBuf[2] = 0xad; replacementLen = 3; break;
                case 0x8e: tmpBuf[0] = 0xef; tmpBuf[1] = 0xbc; tmpBuf[2] = 0xae; replacementLen = 3; break;
                case 0x8f: tmpBuf[0] = 0xef; tmpBuf[1] = 0xbc; tmpBuf[2] = 0xaf; replacementLen = 3; break;
                case 0x90: tmpBuf[0] = 0xef; tmpBuf[1] = 0xbc; tmpBuf[2] = 0xb0; replacementLen = 3; break;
                case 0x91: tmpBuf[0] = 0xef; tmpBuf[1] = 0xbc; tmpBuf[2] = 0xb1; replacementLen = 3; break;
                case 0x92: tmpBuf[0] = 0xef; tmpBuf[1] = 0xbc; tmpBuf[2] = 0xb2; replacementLen = 3; break;
                case 0x93: tmpBuf[0] = 0xef; tmpBuf[1] = 0xbc; tmpBuf[2] = 0xb3; replacementLen = 3; break;
                case 0x94: tmpBuf[0] = 0xef; tmpBuf[1] = 0xbc; tmpBuf[2] = 0xb4; replacementLen = 3; break;
                case 0x95: tmpBuf[0] = 0xef; tmpBuf[1] = 0xbc; tmpBuf[2] = 0xb5; replacementLen = 3; break;
                case 0x96: tmpBuf[0] = 0xef; tmpBuf[1] = 0xbc; tmpBuf[2] = 0xb6; replacementLen = 3; break;
                case 0x97: tmpBuf[0] = 0xef; tmpBuf[1] = 0xbc; tmpBuf[2] = 0xb7; replacementLen = 3; break;
                case 0x98: tmpBuf[0] = 0xef; tmpBuf[1] = 0xbc; tmpBuf[2] = 0xb8; replacementLen = 3; break;
                case 0x99: tmpBuf[0] = 0xef; tmpBuf[1] = 0xbc; tmpBuf[2] = 0xb9; replacementLen = 3; break;
                case 0x9a: tmpBuf[0] = 0xef; tmpBuf[1] = 0xbc; tmpBuf[2] = 0xba; replacementLen = 3; break;
                default:
                  break;
              }
              break;
            default:
              break;
          }
          break;
        case 0xf0:
          switch(*(p+1))
          {
            case 0x90:
              switch(*(p+2))
              {
                case 0x90:
                  switch(*(p+3))
                  {
                    case 0xa8: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x90; tmpBuf[3] = 0x80; replacementLen = 4; break;
                    case 0xa9: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x90; tmpBuf[3] = 0x81; replacementLen = 4; break;
                    case 0xaa: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x90; tmpBuf[3] = 0x82; replacementLen = 4; break;
                    case 0xab: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x90; tmpBuf[3] = 0x83; replacementLen = 4; break;
                    case 0xac: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x90; tmpBuf[3] = 0x84; replacementLen = 4; break;
                    case 0xad: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x90; tmpBuf[3] = 0x85; replacementLen = 4; break;
                    case 0xae: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x90; tmpBuf[3] = 0x86; replacementLen = 4; break;
                    case 0xaf: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x90; tmpBuf[3] = 0x87; replacementLen = 4; break;
                    case 0xb0: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x90; tmpBuf[3] = 0x88; replacementLen = 4; break;
                    case 0xb1: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x90; tmpBuf[3] = 0x89; replacementLen = 4; break;
                    case 0xb2: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x90; tmpBuf[3] = 0x8a; replacementLen = 4; break;
                    case 0xb3: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x90; tmpBuf[3] = 0x8b; replacementLen = 4; break;
                    case 0xb4: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x90; tmpBuf[3] = 0x8c; replacementLen = 4; break;
                    case 0xb5: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x90; tmpBuf[3] = 0x8d; replacementLen = 4; break;
                    case 0xb6: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x90; tmpBuf[3] = 0x8e; replacementLen = 4; break;
                    case 0xb7: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x90; tmpBuf[3] = 0x8f; replacementLen = 4; break;
                    case 0xb8: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x90; tmpBuf[3] = 0x90; replacementLen = 4; break;
                    case 0xb9: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x90; tmpBuf[3] = 0x91; replacementLen = 4; break;
                    case 0xba: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x90; tmpBuf[3] = 0x92; replacementLen = 4; break;
                    case 0xbb: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x90; tmpBuf[3] = 0x93; replacementLen = 4; break;
                    case 0xbc: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x90; tmpBuf[3] = 0x94; replacementLen = 4; break;
                    case 0xbd: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x90; tmpBuf[3] = 0x95; replacementLen = 4; break;
                    case 0xbe: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x90; tmpBuf[3] = 0x96; replacementLen = 4; break;
                    case 0xbf: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x90; tmpBuf[3] = 0x97; replacementLen = 4; break;
                    default:
                      break;
                  }
                  break;
                case 0x91:
                  switch(*(p+3))
                  {
                    case 0x80: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x90; tmpBuf[3] = 0x98; replacementLen = 4; break;
                    case 0x81: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x90; tmpBuf[3] = 0x99; replacementLen = 4; break;
                    case 0x82: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x90; tmpBuf[3] = 0x9a; replacementLen = 4; break;
                    case 0x83: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x90; tmpBuf[3] = 0x9b; replacementLen = 4; break;
                    case 0x84: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x90; tmpBuf[3] = 0x9c; replacementLen = 4; break;
                    case 0x85: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x90; tmpBuf[3] = 0x9d; replacementLen = 4; break;
                    case 0x86: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x90; tmpBuf[3] = 0x9e; replacementLen = 4; break;
                    case 0x87: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x90; tmpBuf[3] = 0x9f; replacementLen = 4; break;
                    case 0x88: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x90; tmpBuf[3] = 0xa0; replacementLen = 4; break;
                    case 0x89: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x90; tmpBuf[3] = 0xa1; replacementLen = 4; break;
                    case 0x8a: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x90; tmpBuf[3] = 0xa2; replacementLen = 4; break;
                    case 0x8b: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x90; tmpBuf[3] = 0xa3; replacementLen = 4; break;
                    case 0x8c: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x90; tmpBuf[3] = 0xa4; replacementLen = 4; break;
                    case 0x8d: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x90; tmpBuf[3] = 0xa5; replacementLen = 4; break;
                    case 0x8e: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x90; tmpBuf[3] = 0xa6; replacementLen = 4; break;
                    case 0x8f: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x90; tmpBuf[3] = 0xa7; replacementLen = 4; break;
                    default:
                      break;
                  }
                  break;
                case 0x93:
                  switch(*(p+3))
                  {
                    case 0x98: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x92; tmpBuf[3] = 0xb0; replacementLen = 4; break;
                    case 0x99: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x92; tmpBuf[3] = 0xb1; replacementLen = 4; break;
                    case 0x9a: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x92; tmpBuf[3] = 0xb2; replacementLen = 4; break;
                    case 0x9b: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x92; tmpBuf[3] = 0xb3; replacementLen = 4; break;
                    case 0x9c: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x92; tmpBuf[3] = 0xb4; replacementLen = 4; break;
                    case 0x9d: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x92; tmpBuf[3] = 0xb5; replacementLen = 4; break;
                    case 0x9e: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x92; tmpBuf[3] = 0xb6; replacementLen = 4; break;
                    case 0x9f: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x92; tmpBuf[3] = 0xb7; replacementLen = 4; break;
                    case 0xa0: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x92; tmpBuf[3] = 0xb8; replacementLen = 4; break;
                    case 0xa1: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x92; tmpBuf[3] = 0xb9; replacementLen = 4; break;
                    case 0xa2: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x92; tmpBuf[3] = 0xba; replacementLen = 4; break;
                    case 0xa3: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x92; tmpBuf[3] = 0xbb; replacementLen = 4; break;
                    case 0xa4: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x92; tmpBuf[3] = 0xbc; replacementLen = 4; break;
                    case 0xa5: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x92; tmpBuf[3] = 0xbd; replacementLen = 4; break;
                    case 0xa6: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x92; tmpBuf[3] = 0xbe; replacementLen = 4; break;
                    case 0xa7: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x92; tmpBuf[3] = 0xbf; replacementLen = 4; break;
                    case 0xa8: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x93; tmpBuf[3] = 0x80; replacementLen = 4; break;
                    case 0xa9: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x93; tmpBuf[3] = 0x81; replacementLen = 4; break;
                    case 0xaa: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x93; tmpBuf[3] = 0x82; replacementLen = 4; break;
                    case 0xab: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x93; tmpBuf[3] = 0x83; replacementLen = 4; break;
                    case 0xac: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x93; tmpBuf[3] = 0x84; replacementLen = 4; break;
                    case 0xad: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x93; tmpBuf[3] = 0x85; replacementLen = 4; break;
                    case 0xae: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x93; tmpBuf[3] = 0x86; replacementLen = 4; break;
                    case 0xaf: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x93; tmpBuf[3] = 0x87; replacementLen = 4; break;
                    case 0xb0: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x93; tmpBuf[3] = 0x88; replacementLen = 4; break;
                    case 0xb1: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x93; tmpBuf[3] = 0x89; replacementLen = 4; break;
                    case 0xb2: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x93; tmpBuf[3] = 0x8a; replacementLen = 4; break;
                    case 0xb3: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x93; tmpBuf[3] = 0x8b; replacementLen = 4; break;
                    case 0xb4: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x93; tmpBuf[3] = 0x8c; replacementLen = 4; break;
                    case 0xb5: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x93; tmpBuf[3] = 0x8d; replacementLen = 4; break;
                    case 0xb6: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x93; tmpBuf[3] = 0x8e; replacementLen = 4; break;
                    case 0xb7: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x93; tmpBuf[3] = 0x8f; replacementLen = 4; break;
                    case 0xb8: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x93; tmpBuf[3] = 0x90; replacementLen = 4; break;
                    case 0xb9: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x93; tmpBuf[3] = 0x91; replacementLen = 4; break;
                    case 0xba: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x93; tmpBuf[3] = 0x92; replacementLen = 4; break;
                    case 0xbb: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x93; tmpBuf[3] = 0x93; replacementLen = 4; break;
                    default:
                      break;
                  }
                  break;
                case 0xb3:
                  switch(*(p+3))
                  {
                    case 0x80: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb2; tmpBuf[3] = 0x80; replacementLen = 4; break;
                    case 0x81: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb2; tmpBuf[3] = 0x81; replacementLen = 4; break;
                    case 0x82: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb2; tmpBuf[3] = 0x82; replacementLen = 4; break;
                    case 0x83: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb2; tmpBuf[3] = 0x83; replacementLen = 4; break;
                    case 0x84: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb2; tmpBuf[3] = 0x84; replacementLen = 4; break;
                    case 0x85: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb2; tmpBuf[3] = 0x85; replacementLen = 4; break;
                    case 0x86: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb2; tmpBuf[3] = 0x86; replacementLen = 4; break;
                    case 0x87: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb2; tmpBuf[3] = 0x87; replacementLen = 4; break;
                    case 0x88: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb2; tmpBuf[3] = 0x88; replacementLen = 4; break;
                    case 0x89: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb2; tmpBuf[3] = 0x89; replacementLen = 4; break;
                    case 0x8a: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb2; tmpBuf[3] = 0x8a; replacementLen = 4; break;
                    case 0x8b: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb2; tmpBuf[3] = 0x8b; replacementLen = 4; break;
                    case 0x8c: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb2; tmpBuf[3] = 0x8c; replacementLen = 4; break;
                    case 0x8d: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb2; tmpBuf[3] = 0x8d; replacementLen = 4; break;
                    case 0x8e: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb2; tmpBuf[3] = 0x8e; replacementLen = 4; break;
                    case 0x8f: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb2; tmpBuf[3] = 0x8f; replacementLen = 4; break;
                    case 0x90: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb2; tmpBuf[3] = 0x90; replacementLen = 4; break;
                    case 0x91: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb2; tmpBuf[3] = 0x91; replacementLen = 4; break;
                    case 0x92: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb2; tmpBuf[3] = 0x92; replacementLen = 4; break;
                    case 0x93: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb2; tmpBuf[3] = 0x93; replacementLen = 4; break;
                    case 0x94: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb2; tmpBuf[3] = 0x94; replacementLen = 4; break;
                    case 0x95: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb2; tmpBuf[3] = 0x95; replacementLen = 4; break;
                    case 0x96: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb2; tmpBuf[3] = 0x96; replacementLen = 4; break;
                    case 0x97: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb2; tmpBuf[3] = 0x97; replacementLen = 4; break;
                    case 0x98: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb2; tmpBuf[3] = 0x98; replacementLen = 4; break;
                    case 0x99: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb2; tmpBuf[3] = 0x99; replacementLen = 4; break;
                    case 0x9a: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb2; tmpBuf[3] = 0x9a; replacementLen = 4; break;
                    case 0x9b: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb2; tmpBuf[3] = 0x9b; replacementLen = 4; break;
                    case 0x9c: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb2; tmpBuf[3] = 0x9c; replacementLen = 4; break;
                    case 0x9d: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb2; tmpBuf[3] = 0x9d; replacementLen = 4; break;
                    case 0x9e: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb2; tmpBuf[3] = 0x9e; replacementLen = 4; break;
                    case 0x9f: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb2; tmpBuf[3] = 0x9f; replacementLen = 4; break;
                    case 0xa0: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb2; tmpBuf[3] = 0xa0; replacementLen = 4; break;
                    case 0xa1: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb2; tmpBuf[3] = 0xa1; replacementLen = 4; break;
                    case 0xa2: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb2; tmpBuf[3] = 0xa2; replacementLen = 4; break;
                    case 0xa3: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb2; tmpBuf[3] = 0xa3; replacementLen = 4; break;
                    case 0xa4: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb2; tmpBuf[3] = 0xa4; replacementLen = 4; break;
                    case 0xa5: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb2; tmpBuf[3] = 0xa5; replacementLen = 4; break;
                    case 0xa6: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb2; tmpBuf[3] = 0xa6; replacementLen = 4; break;
                    case 0xa7: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb2; tmpBuf[3] = 0xa7; replacementLen = 4; break;
                    case 0xa8: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb2; tmpBuf[3] = 0xa8; replacementLen = 4; break;
                    case 0xa9: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb2; tmpBuf[3] = 0xa9; replacementLen = 4; break;
                    case 0xaa: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb2; tmpBuf[3] = 0xaa; replacementLen = 4; break;
                    case 0xab: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb2; tmpBuf[3] = 0xab; replacementLen = 4; break;
                    case 0xac: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb2; tmpBuf[3] = 0xac; replacementLen = 4; break;
                    case 0xad: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb2; tmpBuf[3] = 0xad; replacementLen = 4; break;
                    case 0xae: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb2; tmpBuf[3] = 0xae; replacementLen = 4; break;
                    case 0xaf: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb2; tmpBuf[3] = 0xaf; replacementLen = 4; break;
                    case 0xb0: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb2; tmpBuf[3] = 0xb0; replacementLen = 4; break;
                    case 0xb1: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb2; tmpBuf[3] = 0xb1; replacementLen = 4; break;
                    case 0xb2: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb2; tmpBuf[3] = 0xb2; replacementLen = 4; break;
                    default:
                      break;
                  }
                  break;
                default:
                  break;
              }
              break;
            case 0x91:
              switch(*(p+2))
              {
                case 0xa3:
                  switch(*(p+3))
                  {
                    case 0x80: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x91; tmpBuf[2] = 0xa2; tmpBuf[3] = 0xa0; replacementLen = 4; break;
                    case 0x81: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x91; tmpBuf[2] = 0xa2; tmpBuf[3] = 0xa1; replacementLen = 4; break;
                    case 0x82: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x91; tmpBuf[2] = 0xa2; tmpBuf[3] = 0xa2; replacementLen = 4; break;
                    case 0x83: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x91; tmpBuf[2] = 0xa2; tmpBuf[3] = 0xa3; replacementLen = 4; break;
                    case 0x84: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x91; tmpBuf[2] = 0xa2; tmpBuf[3] = 0xa4; replacementLen = 4; break;
                    case 0x85: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x91; tmpBuf[2] = 0xa2; tmpBuf[3] = 0xa5; replacementLen = 4; break;
                    case 0x86: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x91; tmpBuf[2] = 0xa2; tmpBuf[3] = 0xa6; replacementLen = 4; break;
                    case 0x87: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x91; tmpBuf[2] = 0xa2; tmpBuf[3] = 0xa7; replacementLen = 4; break;
                    case 0x88: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x91; tmpBuf[2] = 0xa2; tmpBuf[3] = 0xa8; replacementLen = 4; break;
                    case 0x89: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x91; tmpBuf[2] = 0xa2; tmpBuf[3] = 0xa9; replacementLen = 4; break;
                    case 0x8a: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x91; tmpBuf[2] = 0xa2; tmpBuf[3] = 0xaa; replacementLen = 4; break;
                    case 0x8b: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x91; tmpBuf[2] = 0xa2; tmpBuf[3] = 0xab; replacementLen = 4; break;
                    case 0x8c: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x91; tmpBuf[2] = 0xa2; tmpBuf[3] = 0xac; replacementLen = 4; break;
                    case 0x8d: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x91; tmpBuf[2] = 0xa2; tmpBuf[3] = 0xad; replacementLen = 4; break;
                    case 0x8e: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x91; tmpBuf[2] = 0xa2; tmpBuf[3] = 0xae; replacementLen = 4; break;
                    case 0x8f: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x91; tmpBuf[2] = 0xa2; tmpBuf[3] = 0xaf; replacementLen = 4; break;
                    case 0x90: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x91; tmpBuf[2] = 0xa2; tmpBuf[3] = 0xb0; replacementLen = 4; break;
                    case 0x91: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x91; tmpBuf[2] = 0xa2; tmpBuf[3] = 0xb1; replacementLen = 4; break;
                    case 0x92: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x91; tmpBuf[2] = 0xa2; tmpBuf[3] = 0xb2; replacementLen = 4; break;
                    case 0x93: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x91; tmpBuf[2] = 0xa2; tmpBuf[3] = 0xb3; replacementLen = 4; break;
                    case 0x94: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x91; tmpBuf[2] = 0xa2; tmpBuf[3] = 0xb4; replacementLen = 4; break;
                    case 0x95: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x91; tmpBuf[2] = 0xa2; tmpBuf[3] = 0xb5; replacementLen = 4; break;
                    case 0x96: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x91; tmpBuf[2] = 0xa2; tmpBuf[3] = 0xb6; replacementLen = 4; break;
                    case 0x97: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x91; tmpBuf[2] = 0xa2; tmpBuf[3] = 0xb7; replacementLen = 4; break;
                    case 0x98: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x91; tmpBuf[2] = 0xa2; tmpBuf[3] = 0xb8; replacementLen = 4; break;
                    case 0x99: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x91; tmpBuf[2] = 0xa2; tmpBuf[3] = 0xb9; replacementLen = 4; break;
                    case 0x9a: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x91; tmpBuf[2] = 0xa2; tmpBuf[3] = 0xba; replacementLen = 4; break;
                    case 0x9b: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x91; tmpBuf[2] = 0xa2; tmpBuf[3] = 0xbb; replacementLen = 4; break;
                    case 0x9c: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x91; tmpBuf[2] = 0xa2; tmpBuf[3] = 0xbc; replacementLen = 4; break;
                    case 0x9d: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x91; tmpBuf[2] = 0xa2; tmpBuf[3] = 0xbd; replacementLen = 4; break;
                    case 0x9e: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x91; tmpBuf[2] = 0xa2; tmpBuf[3] = 0xbe; replacementLen = 4; break;
                    case 0x9f: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x91; tmpBuf[2] = 0xa2; tmpBuf[3] = 0xbf; replacementLen = 4; break;
                    default:
                      break;
                  }
                  break;
                default:
                  break;
              }
              break;
            case 0x96:
              switch(*(p+2))
              {
                case 0xb9:
                  switch(*(p+3))
                  {
                    case 0xa0: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x96; tmpBuf[2] = 0xb9; tmpBuf[3] = 0x80; replacementLen = 4; break;
                    case 0xa1: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x96; tmpBuf[2] = 0xb9; tmpBuf[3] = 0x81; replacementLen = 4; break;
                    case 0xa2: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x96; tmpBuf[2] = 0xb9; tmpBuf[3] = 0x82; replacementLen = 4; break;
                    case 0xa3: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x96; tmpBuf[2] = 0xb9; tmpBuf[3] = 0x83; replacementLen = 4; break;
                    case 0xa4: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x96; tmpBuf[2] = 0xb9; tmpBuf[3] = 0x84; replacementLen = 4; break;
                    case 0xa5: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x96; tmpBuf[2] = 0xb9; tmpBuf[3] = 0x85; replacementLen = 4; break;
                    case 0xa6: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x96; tmpBuf[2] = 0xb9; tmpBuf[3] = 0x86; replacementLen = 4; break;
                    case 0xa7: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x96; tmpBuf[2] = 0xb9; tmpBuf[3] = 0x87; replacementLen = 4; break;
                    case 0xa8: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x96; tmpBuf[2] = 0xb9; tmpBuf[3] = 0x88; replacementLen = 4; break;
                    case 0xa9: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x96; tmpBuf[2] = 0xb9; tmpBuf[3] = 0x89; replacementLen = 4; break;
                    case 0xaa: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x96; tmpBuf[2] = 0xb9; tmpBuf[3] = 0x8a; replacementLen = 4; break;
                    case 0xab: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x96; tmpBuf[2] = 0xb9; tmpBuf[3] = 0x8b; replacementLen = 4; break;
                    case 0xac: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x96; tmpBuf[2] = 0xb9; tmpBuf[3] = 0x8c; replacementLen = 4; break;
                    case 0xad: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x96; tmpBuf[2] = 0xb9; tmpBuf[3] = 0x8d; replacementLen = 4; break;
                    case 0xae: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x96; tmpBuf[2] = 0xb9; tmpBuf[3] = 0x8e; replacementLen = 4; break;
                    case 0xaf: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x96; tmpBuf[2] = 0xb9; tmpBuf[3] = 0x8f; replacementLen = 4; break;
                    case 0xb0: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x96; tmpBuf[2] = 0xb9; tmpBuf[3] = 0x90; replacementLen = 4; break;
                    case 0xb1: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x96; tmpBuf[2] = 0xb9; tmpBuf[3] = 0x91; replacementLen = 4; break;
                    case 0xb2: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x96; tmpBuf[2] = 0xb9; tmpBuf[3] = 0x92; replacementLen = 4; break;
                    case 0xb3: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x96; tmpBuf[2] = 0xb9; tmpBuf[3] = 0x93; replacementLen = 4; break;
                    case 0xb4: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x96; tmpBuf[2] = 0xb9; tmpBuf[3] = 0x94; replacementLen = 4; break;
                    case 0xb5: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x96; tmpBuf[2] = 0xb9; tmpBuf[3] = 0x95; replacementLen = 4; break;
                    case 0xb6: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x96; tmpBuf[2] = 0xb9; tmpBuf[3] = 0x96; replacementLen = 4; break;
                    case 0xb7: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x96; tmpBuf[2] = 0xb9; tmpBuf[3] = 0x97; replacementLen = 4; break;
                    case 0xb8: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x96; tmpBuf[2] = 0xb9; tmpBuf[3] = 0x98; replacementLen = 4; break;
                    case 0xb9: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x96; tmpBuf[2] = 0xb9; tmpBuf[3] = 0x99; replacementLen = 4; break;
                    case 0xba: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x96; tmpBuf[2] = 0xb9; tmpBuf[3] = 0x9a; replacementLen = 4; break;
                    case 0xbb: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x96; tmpBuf[2] = 0xb9; tmpBuf[3] = 0x9b; replacementLen = 4; break;
                    case 0xbc: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x96; tmpBuf[2] = 0xb9; tmpBuf[3] = 0x9c; replacementLen = 4; break;
                    case 0xbd: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x96; tmpBuf[2] = 0xb9; tmpBuf[3] = 0x9d; replacementLen = 4; break;
                    case 0xbe: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x96; tmpBuf[2] = 0xb9; tmpBuf[3] = 0x9e; replacementLen = 4; break;
                    case 0xbf: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x96; tmpBuf[2] = 0xb9; tmpBuf[3] = 0x9f; replacementLen = 4; break;
                    default:
                      break;
                  }
                  break;
                default:
                  break;
              }
              break;
            case 0x9e:
              switch(*(p+2))
              {
                case 0xa4:
                  switch(*(p+3))
                  {
                    case 0xa2: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa4; tmpBuf[3] = 0x80; replacementLen = 4; break;
                    case 0xa3: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa4; tmpBuf[3] = 0x81; replacementLen = 4; break;
                    case 0xa4: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa4; tmpBuf[3] = 0x82; replacementLen = 4; break;
                    case 0xa5: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa4; tmpBuf[3] = 0x83; replacementLen = 4; break;
                    case 0xa6: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa4; tmpBuf[3] = 0x84; replacementLen = 4; break;
                    case 0xa7: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa4; tmpBuf[3] = 0x85; replacementLen = 4; break;
                    case 0xa8: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa4; tmpBuf[3] = 0x86; replacementLen = 4; break;
                    case 0xa9: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa4; tmpBuf[3] = 0x87; replacementLen = 4; break;
                    case 0xaa: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa4; tmpBuf[3] = 0x88; replacementLen = 4; break;
                    case 0xab: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa4; tmpBuf[3] = 0x89; replacementLen = 4; break;
                    case 0xac: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa4; tmpBuf[3] = 0x8a; replacementLen = 4; break;
                    case 0xad: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa4; tmpBuf[3] = 0x8b; replacementLen = 4; break;
                    case 0xae: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa4; tmpBuf[3] = 0x8c; replacementLen = 4; break;
                    case 0xaf: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa4; tmpBuf[3] = 0x8d; replacementLen = 4; break;
                    case 0xb0: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa4; tmpBuf[3] = 0x8e; replacementLen = 4; break;
                    case 0xb1: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa4; tmpBuf[3] = 0x8f; replacementLen = 4; break;
                    case 0xb2: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa4; tmpBuf[3] = 0x90; replacementLen = 4; break;
                    case 0xb3: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa4; tmpBuf[3] = 0x91; replacementLen = 4; break;
                    case 0xb4: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa4; tmpBuf[3] = 0x92; replacementLen = 4; break;
                    case 0xb5: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa4; tmpBuf[3] = 0x93; replacementLen = 4; break;
                    case 0xb6: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa4; tmpBuf[3] = 0x94; replacementLen = 4; break;
                    case 0xb7: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa4; tmpBuf[3] = 0x95; replacementLen = 4; break;
                    case 0xb8: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa4; tmpBuf[3] = 0x96; replacementLen = 4; break;
                    case 0xb9: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa4; tmpBuf[3] = 0x97; replacementLen = 4; break;
                    case 0xba: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa4; tmpBuf[3] = 0x98; replacementLen = 4; break;
                    case 0xbb: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa4; tmpBuf[3] = 0x99; replacementLen = 4; break;
                    case 0xbc: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa4; tmpBuf[3] = 0x9a; replacementLen = 4; break;
                    case 0xbd: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa4; tmpBuf[3] = 0x9b; replacementLen = 4; break;
                    case 0xbe: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa4; tmpBuf[3] = 0x9c; replacementLen = 4; break;
                    case 0xbf: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa4; tmpBuf[3] = 0x9d; replacementLen = 4; break;
                    default:
                      break;
                  }
                  break;
                case 0xa5:
                  switch(*(p+3))
                  {
                    case 0x80: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa4; tmpBuf[3] = 0x9e; replacementLen = 4; break;
                    case 0x81: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa4; tmpBuf[3] = 0x9f; replacementLen = 4; break;
                    case 0x82: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa4; tmpBuf[3] = 0xa0; replacementLen = 4; break;
                    case 0x83: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa4; tmpBuf[3] = 0xa1; replacementLen = 4; break;
                    default:
                      break;
                  }
                  break;
                default:
                  break;
              }
              break;
            default:
              break;
          }
          break;
        default:
          break;
      }
      int incr = 1;
      if ((*p&0xE0)==0xC0)
      {
        incr=2; // 110x.xxxx: >=2 byte character
      }
      if ((*p&0xF0)==0xE0)
      {
        incr=3; // 1110.xxxx: >=3 byte character
      }
      if ((*p&0xF8)==0xF0)
      {
        incr=4; // 1111.0xxx: >=4 byte character
      }
      if (replacementLen == 0)
      {
        for (int i = 0; i < incr; i++)
        {
          tmpBuf[i] = *(p+i);
        }
        replacementLen = incr;
      }
      p += incr;
      if (cb->curBufLen < curLen + replacementLen)
      {
        cb->outBuf = (unsigned char *)realloc(cb->outBuf,cb->curBufLen + 41);
        cb->curBufLen = cb->curBufLen + 40;
      }
      for (size_t i = 0; i < replacementLen; i++)
      {
        cb->outBuf[curLen + i] = tmpBuf[i];
      }
      curLen += replacementLen;
      cb->outBuf[curLen] = 0;
    }
  }
  return curLen;
}
int StrToLwrExt(unsigned char* pString, CaseBuffer *cb)
{
  size_t pStrLen = strlen((char *)pString);
  if (cb->curBufLen < pStrLen + 40)
  {
    cb->outBuf = (unsigned char *)realloc(cb->outBuf,pStrLen + 41);
    cb->curBufLen = pStrLen + 40;
  }

  size_t curLen = 0;
  unsigned char tmpBuf[4];
  if (pString && *pString)
  {
    size_t replacementLen = 0;
    unsigned char* p = pString;
    while (*p)
    {
      replacementLen = 0;
      switch(*(p+0))
      {
        case 0x41: tmpBuf[0] = 0x61; replacementLen = 1; break;
        case 0x42: tmpBuf[0] = 0x62; replacementLen = 1; break;
        case 0x43: tmpBuf[0] = 0x63; replacementLen = 1; break;
        case 0x44: tmpBuf[0] = 0x64; replacementLen = 1; break;
        case 0x45: tmpBuf[0] = 0x65; replacementLen = 1; break;
        case 0x46: tmpBuf[0] = 0x66; replacementLen = 1; break;
        case 0x47: tmpBuf[0] = 0x67; replacementLen = 1; break;
        case 0x48: tmpBuf[0] = 0x68; replacementLen = 1; break;
        case 0x49: tmpBuf[0] = 0x69; replacementLen = 1; break;
        case 0x4a: tmpBuf[0] = 0x6a; replacementLen = 1; break;
        case 0x4b: tmpBuf[0] = 0x6b; replacementLen = 1; break;
        case 0x4c: tmpBuf[0] = 0x6c; replacementLen = 1; break;
        case 0x4d: tmpBuf[0] = 0x6d; replacementLen = 1; break;
        case 0x4e: tmpBuf[0] = 0x6e; replacementLen = 1; break;
        case 0x4f: tmpBuf[0] = 0x6f; replacementLen = 1; break;
        case 0x50: tmpBuf[0] = 0x70; replacementLen = 1; break;
        case 0x51: tmpBuf[0] = 0x71; replacementLen = 1; break;
        case 0x52: tmpBuf[0] = 0x72; replacementLen = 1; break;
        case 0x53: tmpBuf[0] = 0x73; replacementLen = 1; break;
        case 0x54: tmpBuf[0] = 0x74; replacementLen = 1; break;
        case 0x55: tmpBuf[0] = 0x75; replacementLen = 1; break;
        case 0x56: tmpBuf[0] = 0x76; replacementLen = 1; break;
        case 0x57: tmpBuf[0] = 0x77; replacementLen = 1; break;
        case 0x58: tmpBuf[0] = 0x78; replacementLen = 1; break;
        case 0x59: tmpBuf[0] = 0x79; replacementLen = 1; break;
        case 0x5a: tmpBuf[0] = 0x7a; replacementLen = 1; break;
        case 0xc3:
          switch(*(p+1))
          {
            case 0x80: tmpBuf[0] = 0xc3; tmpBuf[1] = 0xa0; replacementLen = 2; break;
            case 0x81: tmpBuf[0] = 0xc3; tmpBuf[1] = 0xa1; replacementLen = 2; break;
            case 0x82: tmpBuf[0] = 0xc3; tmpBuf[1] = 0xa2; replacementLen = 2; break;
            case 0x83: tmpBuf[0] = 0xc3; tmpBuf[1] = 0xa3; replacementLen = 2; break;
            case 0x84: tmpBuf[0] = 0xc3; tmpBuf[1] = 0xa4; replacementLen = 2; break;
            case 0x85: tmpBuf[0] = 0xc3; tmpBuf[1] = 0xa5; replacementLen = 2; break;
            case 0x86: tmpBuf[0] = 0xc3; tmpBuf[1] = 0xa6; replacementLen = 2; break;
            case 0x87: tmpBuf[0] = 0xc3; tmpBuf[1] = 0xa7; replacementLen = 2; break;
            case 0x88: tmpBuf[0] = 0xc3; tmpBuf[1] = 0xa8; replacementLen = 2; break;
            case 0x89: tmpBuf[0] = 0xc3; tmpBuf[1] = 0xa9; replacementLen = 2; break;
            case 0x8a: tmpBuf[0] = 0xc3; tmpBuf[1] = 0xaa; replacementLen = 2; break;
            case 0x8b: tmpBuf[0] = 0xc3; tmpBuf[1] = 0xab; replacementLen = 2; break;
            case 0x8c: tmpBuf[0] = 0xc3; tmpBuf[1] = 0xac; replacementLen = 2; break;
            case 0x8d: tmpBuf[0] = 0xc3; tmpBuf[1] = 0xad; replacementLen = 2; break;
            case 0x8e: tmpBuf[0] = 0xc3; tmpBuf[1] = 0xae; replacementLen = 2; break;
            case 0x8f: tmpBuf[0] = 0xc3; tmpBuf[1] = 0xaf; replacementLen = 2; break;
            case 0x90: tmpBuf[0] = 0xc3; tmpBuf[1] = 0xb0; replacementLen = 2; break;
            case 0x91: tmpBuf[0] = 0xc3; tmpBuf[1] = 0xb1; replacementLen = 2; break;
            case 0x92: tmpBuf[0] = 0xc3; tmpBuf[1] = 0xb2; replacementLen = 2; break;
            case 0x93: tmpBuf[0] = 0xc3; tmpBuf[1] = 0xb3; replacementLen = 2; break;
            case 0x94: tmpBuf[0] = 0xc3; tmpBuf[1] = 0xb4; replacementLen = 2; break;
            case 0x95: tmpBuf[0] = 0xc3; tmpBuf[1] = 0xb5; replacementLen = 2; break;
            case 0x96: tmpBuf[0] = 0xc3; tmpBuf[1] = 0xb6; replacementLen = 2; break;
            case 0x98: tmpBuf[0] = 0xc3; tmpBuf[1] = 0xb8; replacementLen = 2; break;
            case 0x99: tmpBuf[0] = 0xc3; tmpBuf[1] = 0xb9; replacementLen = 2; break;
            case 0x9a: tmpBuf[0] = 0xc3; tmpBuf[1] = 0xba; replacementLen = 2; break;
            case 0x9b: tmpBuf[0] = 0xc3; tmpBuf[1] = 0xbb; replacementLen = 2; break;
            case 0x9c: tmpBuf[0] = 0xc3; tmpBuf[1] = 0xbc; replacementLen = 2; break;
            case 0x9d: tmpBuf[0] = 0xc3; tmpBuf[1] = 0xbd; replacementLen = 2; break;
            case 0x9e: tmpBuf[0] = 0xc3; tmpBuf[1] = 0xbe; replacementLen = 2; break;
            default:
              break;
          }
          break;
        case 0xc4:
          switch(*(p+1))
          {
            case 0x80: tmpBuf[0] = 0xc4; tmpBuf[1] = 0x81; replacementLen = 2; break;
            case 0x82: tmpBuf[0] = 0xc4; tmpBuf[1] = 0x83; replacementLen = 2; break;
            case 0x84: tmpBuf[0] = 0xc4; tmpBuf[1] = 0x85; replacementLen = 2; break;
            case 0x86: tmpBuf[0] = 0xc4; tmpBuf[1] = 0x87; replacementLen = 2; break;
            case 0x88: tmpBuf[0] = 0xc4; tmpBuf[1] = 0x89; replacementLen = 2; break;
            case 0x8a: tmpBuf[0] = 0xc4; tmpBuf[1] = 0x8b; replacementLen = 2; break;
            case 0x8c: tmpBuf[0] = 0xc4; tmpBuf[1] = 0x8d; replacementLen = 2; break;
            case 0x8e: tmpBuf[0] = 0xc4; tmpBuf[1] = 0x8f; replacementLen = 2; break;
            case 0x90: tmpBuf[0] = 0xc4; tmpBuf[1] = 0x91; replacementLen = 2; break;
            case 0x92: tmpBuf[0] = 0xc4; tmpBuf[1] = 0x93; replacementLen = 2; break;
            case 0x94: tmpBuf[0] = 0xc4; tmpBuf[1] = 0x95; replacementLen = 2; break;
            case 0x96: tmpBuf[0] = 0xc4; tmpBuf[1] = 0x97; replacementLen = 2; break;
            case 0x98: tmpBuf[0] = 0xc4; tmpBuf[1] = 0x99; replacementLen = 2; break;
            case 0x9a: tmpBuf[0] = 0xc4; tmpBuf[1] = 0x9b; replacementLen = 2; break;
            case 0x9c: tmpBuf[0] = 0xc4; tmpBuf[1] = 0x9d; replacementLen = 2; break;
            case 0x9e: tmpBuf[0] = 0xc4; tmpBuf[1] = 0x9f; replacementLen = 2; break;
            case 0xa0: tmpBuf[0] = 0xc4; tmpBuf[1] = 0xa1; replacementLen = 2; break;
            case 0xa2: tmpBuf[0] = 0xc4; tmpBuf[1] = 0xa3; replacementLen = 2; break;
            case 0xa4: tmpBuf[0] = 0xc4; tmpBuf[1] = 0xa5; replacementLen = 2; break;
            case 0xa6: tmpBuf[0] = 0xc4; tmpBuf[1] = 0xa7; replacementLen = 2; break;
            case 0xa8: tmpBuf[0] = 0xc4; tmpBuf[1] = 0xa9; replacementLen = 2; break;
            case 0xaa: tmpBuf[0] = 0xc4; tmpBuf[1] = 0xab; replacementLen = 2; break;
            case 0xac: tmpBuf[0] = 0xc4; tmpBuf[1] = 0xad; replacementLen = 2; break;
            case 0xae: tmpBuf[0] = 0xc4; tmpBuf[1] = 0xaf; replacementLen = 2; break;
            case 0xb0: tmpBuf[0] = 0x69; replacementLen = 1; break;
            case 0xb2: tmpBuf[0] = 0xc4; tmpBuf[1] = 0xb3; replacementLen = 2; break;
            case 0xb4: tmpBuf[0] = 0xc4; tmpBuf[1] = 0xb5; replacementLen = 2; break;
            case 0xb6: tmpBuf[0] = 0xc4; tmpBuf[1] = 0xb7; replacementLen = 2; break;
            case 0xb9: tmpBuf[0] = 0xc4; tmpBuf[1] = 0xba; replacementLen = 2; break;
            case 0xbb: tmpBuf[0] = 0xc4; tmpBuf[1] = 0xbc; replacementLen = 2; break;
            case 0xbd: tmpBuf[0] = 0xc4; tmpBuf[1] = 0xbe; replacementLen = 2; break;
            case 0xbf: tmpBuf[0] = 0xc5; tmpBuf[1] = 0x80; replacementLen = 2; break;
            default:
              break;
          }
          break;
        case 0xc5:
          switch(*(p+1))
          {
            case 0x81: tmpBuf[0] = 0xc5; tmpBuf[1] = 0x82; replacementLen = 2; break;
            case 0x83: tmpBuf[0] = 0xc5; tmpBuf[1] = 0x84; replacementLen = 2; break;
            case 0x85: tmpBuf[0] = 0xc5; tmpBuf[1] = 0x86; replacementLen = 2; break;
            case 0x87: tmpBuf[0] = 0xc5; tmpBuf[1] = 0x88; replacementLen = 2; break;
            case 0x8a: tmpBuf[0] = 0xc5; tmpBuf[1] = 0x8b; replacementLen = 2; break;
            case 0x8c: tmpBuf[0] = 0xc5; tmpBuf[1] = 0x8d; replacementLen = 2; break;
            case 0x8e: tmpBuf[0] = 0xc5; tmpBuf[1] = 0x8f; replacementLen = 2; break;
            case 0x90: tmpBuf[0] = 0xc5; tmpBuf[1] = 0x91; replacementLen = 2; break;
            case 0x92: tmpBuf[0] = 0xc5; tmpBuf[1] = 0x93; replacementLen = 2; break;
            case 0x94: tmpBuf[0] = 0xc5; tmpBuf[1] = 0x95; replacementLen = 2; break;
            case 0x96: tmpBuf[0] = 0xc5; tmpBuf[1] = 0x97; replacementLen = 2; break;
            case 0x98: tmpBuf[0] = 0xc5; tmpBuf[1] = 0x99; replacementLen = 2; break;
            case 0x9a: tmpBuf[0] = 0xc5; tmpBuf[1] = 0x9b; replacementLen = 2; break;
            case 0x9c: tmpBuf[0] = 0xc5; tmpBuf[1] = 0x9d; replacementLen = 2; break;
            case 0x9e: tmpBuf[0] = 0xc5; tmpBuf[1] = 0x9f; replacementLen = 2; break;
            case 0xa0: tmpBuf[0] = 0xc5; tmpBuf[1] = 0xa1; replacementLen = 2; break;
            case 0xa2: tmpBuf[0] = 0xc5; tmpBuf[1] = 0xa3; replacementLen = 2; break;
            case 0xa4: tmpBuf[0] = 0xc5; tmpBuf[1] = 0xa5; replacementLen = 2; break;
            case 0xa6: tmpBuf[0] = 0xc5; tmpBuf[1] = 0xa7; replacementLen = 2; break;
            case 0xa8: tmpBuf[0] = 0xc5; tmpBuf[1] = 0xa9; replacementLen = 2; break;
            case 0xaa: tmpBuf[0] = 0xc5; tmpBuf[1] = 0xab; replacementLen = 2; break;
            case 0xac: tmpBuf[0] = 0xc5; tmpBuf[1] = 0xad; replacementLen = 2; break;
            case 0xae: tmpBuf[0] = 0xc5; tmpBuf[1] = 0xaf; replacementLen = 2; break;
            case 0xb0: tmpBuf[0] = 0xc5; tmpBuf[1] = 0xb1; replacementLen = 2; break;
            case 0xb2: tmpBuf[0] = 0xc5; tmpBuf[1] = 0xb3; replacementLen = 2; break;
            case 0xb4: tmpBuf[0] = 0xc5; tmpBuf[1] = 0xb5; replacementLen = 2; break;
            case 0xb6: tmpBuf[0] = 0xc5; tmpBuf[1] = 0xb7; replacementLen = 2; break;
            case 0xb8: tmpBuf[0] = 0xc3; tmpBuf[1] = 0xbf; replacementLen = 2; break;
            case 0xb9: tmpBuf[0] = 0xc5; tmpBuf[1] = 0xba; replacementLen = 2; break;
            case 0xbb: tmpBuf[0] = 0xc5; tmpBuf[1] = 0xbc; replacementLen = 2; break;
            case 0xbd: tmpBuf[0] = 0xc5; tmpBuf[1] = 0xbe; replacementLen = 2; break;
            default:
              break;
          }
          break;
        case 0xc6:
          switch(*(p+1))
          {
            case 0x81: tmpBuf[0] = 0xc9; tmpBuf[1] = 0x93; replacementLen = 2; break;
            case 0x82: tmpBuf[0] = 0xc6; tmpBuf[1] = 0x83; replacementLen = 2; break;
            case 0x84: tmpBuf[0] = 0xc6; tmpBuf[1] = 0x85; replacementLen = 2; break;
            case 0x86: tmpBuf[0] = 0xc9; tmpBuf[1] = 0x94; replacementLen = 2; break;
            case 0x87: tmpBuf[0] = 0xc6; tmpBuf[1] = 0x88; replacementLen = 2; break;
            case 0x89: tmpBuf[0] = 0xc9; tmpBuf[1] = 0x96; replacementLen = 2; break;
            case 0x8a: tmpBuf[0] = 0xc9; tmpBuf[1] = 0x97; replacementLen = 2; break;
            case 0x8b: tmpBuf[0] = 0xc6; tmpBuf[1] = 0x8c; replacementLen = 2; break;
            case 0x8e: tmpBuf[0] = 0xc7; tmpBuf[1] = 0x9d; replacementLen = 2; break;
            case 0x8f: tmpBuf[0] = 0xc9; tmpBuf[1] = 0x99; replacementLen = 2; break;
            case 0x90: tmpBuf[0] = 0xc9; tmpBuf[1] = 0x9b; replacementLen = 2; break;
            case 0x91: tmpBuf[0] = 0xc6; tmpBuf[1] = 0x92; replacementLen = 2; break;
            case 0x93: tmpBuf[0] = 0xc9; tmpBuf[1] = 0xa0; replacementLen = 2; break;
            case 0x94: tmpBuf[0] = 0xc9; tmpBuf[1] = 0xa3; replacementLen = 2; break;
            case 0x96: tmpBuf[0] = 0xc9; tmpBuf[1] = 0xa9; replacementLen = 2; break;
            case 0x97: tmpBuf[0] = 0xc9; tmpBuf[1] = 0xa8; replacementLen = 2; break;
            case 0x98: tmpBuf[0] = 0xc6; tmpBuf[1] = 0x99; replacementLen = 2; break;
            case 0x9c: tmpBuf[0] = 0xc9; tmpBuf[1] = 0xaf; replacementLen = 2; break;
            case 0x9d: tmpBuf[0] = 0xc9; tmpBuf[1] = 0xb2; replacementLen = 2; break;
            case 0x9f: tmpBuf[0] = 0xc9; tmpBuf[1] = 0xb5; replacementLen = 2; break;
            case 0xa0: tmpBuf[0] = 0xc6; tmpBuf[1] = 0xa1; replacementLen = 2; break;
            case 0xa2: tmpBuf[0] = 0xc6; tmpBuf[1] = 0xa3; replacementLen = 2; break;
            case 0xa4: tmpBuf[0] = 0xc6; tmpBuf[1] = 0xa5; replacementLen = 2; break;
            case 0xa6: tmpBuf[0] = 0xca; tmpBuf[1] = 0x80; replacementLen = 2; break;
            case 0xa7: tmpBuf[0] = 0xc6; tmpBuf[1] = 0xa8; replacementLen = 2; break;
            case 0xa9: tmpBuf[0] = 0xca; tmpBuf[1] = 0x83; replacementLen = 2; break;
            case 0xac: tmpBuf[0] = 0xc6; tmpBuf[1] = 0xad; replacementLen = 2; break;
            case 0xae: tmpBuf[0] = 0xca; tmpBuf[1] = 0x88; replacementLen = 2; break;
            case 0xaf: tmpBuf[0] = 0xc6; tmpBuf[1] = 0xb0; replacementLen = 2; break;
            case 0xb1: tmpBuf[0] = 0xca; tmpBuf[1] = 0x8a; replacementLen = 2; break;
            case 0xb2: tmpBuf[0] = 0xca; tmpBuf[1] = 0x8b; replacementLen = 2; break;
            case 0xb3: tmpBuf[0] = 0xc6; tmpBuf[1] = 0xb4; replacementLen = 2; break;
            case 0xb5: tmpBuf[0] = 0xc6; tmpBuf[1] = 0xb6; replacementLen = 2; break;
            case 0xb7: tmpBuf[0] = 0xca; tmpBuf[1] = 0x92; replacementLen = 2; break;
            case 0xb8: tmpBuf[0] = 0xc6; tmpBuf[1] = 0xb9; replacementLen = 2; break;
            case 0xbc: tmpBuf[0] = 0xc6; tmpBuf[1] = 0xbd; replacementLen = 2; break;
            default:
              break;
          }
          break;
        case 0xc7:
          switch(*(p+1))
          {
            case 0x84: tmpBuf[0] = 0xc7; tmpBuf[1] = 0x86; replacementLen = 2; break;
            case 0x87: tmpBuf[0] = 0xc7; tmpBuf[1] = 0x89; replacementLen = 2; break;
            case 0x8a: tmpBuf[0] = 0xc7; tmpBuf[1] = 0x8c; replacementLen = 2; break;
            case 0x8d: tmpBuf[0] = 0xc7; tmpBuf[1] = 0x8e; replacementLen = 2; break;
            case 0x8f: tmpBuf[0] = 0xc7; tmpBuf[1] = 0x90; replacementLen = 2; break;
            case 0x91: tmpBuf[0] = 0xc7; tmpBuf[1] = 0x92; replacementLen = 2; break;
            case 0x93: tmpBuf[0] = 0xc7; tmpBuf[1] = 0x94; replacementLen = 2; break;
            case 0x95: tmpBuf[0] = 0xc7; tmpBuf[1] = 0x96; replacementLen = 2; break;
            case 0x97: tmpBuf[0] = 0xc7; tmpBuf[1] = 0x98; replacementLen = 2; break;
            case 0x99: tmpBuf[0] = 0xc7; tmpBuf[1] = 0x9a; replacementLen = 2; break;
            case 0x9b: tmpBuf[0] = 0xc7; tmpBuf[1] = 0x9c; replacementLen = 2; break;
            case 0x9e: tmpBuf[0] = 0xc7; tmpBuf[1] = 0x9f; replacementLen = 2; break;
            case 0xa0: tmpBuf[0] = 0xc7; tmpBuf[1] = 0xa1; replacementLen = 2; break;
            case 0xa2: tmpBuf[0] = 0xc7; tmpBuf[1] = 0xa3; replacementLen = 2; break;
            case 0xa4: tmpBuf[0] = 0xc7; tmpBuf[1] = 0xa5; replacementLen = 2; break;
            case 0xa6: tmpBuf[0] = 0xc7; tmpBuf[1] = 0xa7; replacementLen = 2; break;
            case 0xa8: tmpBuf[0] = 0xc7; tmpBuf[1] = 0xa9; replacementLen = 2; break;
            case 0xaa: tmpBuf[0] = 0xc7; tmpBuf[1] = 0xab; replacementLen = 2; break;
            case 0xac: tmpBuf[0] = 0xc7; tmpBuf[1] = 0xad; replacementLen = 2; break;
            case 0xae: tmpBuf[0] = 0xc7; tmpBuf[1] = 0xaf; replacementLen = 2; break;
            case 0xb1: tmpBuf[0] = 0xc7; tmpBuf[1] = 0xb3; replacementLen = 2; break;
            case 0xb4: tmpBuf[0] = 0xc7; tmpBuf[1] = 0xb5; replacementLen = 2; break;
            case 0xb6: tmpBuf[0] = 0xc6; tmpBuf[1] = 0x95; replacementLen = 2; break;
            case 0xb7: tmpBuf[0] = 0xc6; tmpBuf[1] = 0xbf; replacementLen = 2; break;
            case 0xb8: tmpBuf[0] = 0xc7; tmpBuf[1] = 0xb9; replacementLen = 2; break;
            case 0xba: tmpBuf[0] = 0xc7; tmpBuf[1] = 0xbb; replacementLen = 2; break;
            case 0xbc: tmpBuf[0] = 0xc7; tmpBuf[1] = 0xbd; replacementLen = 2; break;
            case 0xbe: tmpBuf[0] = 0xc7; tmpBuf[1] = 0xbf; replacementLen = 2; break;
            default:
              break;
          }
          break;
        case 0xc8:
          switch(*(p+1))
          {
            case 0x80: tmpBuf[0] = 0xc8; tmpBuf[1] = 0x81; replacementLen = 2; break;
            case 0x82: tmpBuf[0] = 0xc8; tmpBuf[1] = 0x83; replacementLen = 2; break;
            case 0x84: tmpBuf[0] = 0xc8; tmpBuf[1] = 0x85; replacementLen = 2; break;
            case 0x86: tmpBuf[0] = 0xc8; tmpBuf[1] = 0x87; replacementLen = 2; break;
            case 0x88: tmpBuf[0] = 0xc8; tmpBuf[1] = 0x89; replacementLen = 2; break;
            case 0x8a: tmpBuf[0] = 0xc8; tmpBuf[1] = 0x8b; replacementLen = 2; break;
            case 0x8c: tmpBuf[0] = 0xc8; tmpBuf[1] = 0x8d; replacementLen = 2; break;
            case 0x8e: tmpBuf[0] = 0xc8; tmpBuf[1] = 0x8f; replacementLen = 2; break;
            case 0x90: tmpBuf[0] = 0xc8; tmpBuf[1] = 0x91; replacementLen = 2; break;
            case 0x92: tmpBuf[0] = 0xc8; tmpBuf[1] = 0x93; replacementLen = 2; break;
            case 0x94: tmpBuf[0] = 0xc8; tmpBuf[1] = 0x95; replacementLen = 2; break;
            case 0x96: tmpBuf[0] = 0xc8; tmpBuf[1] = 0x97; replacementLen = 2; break;
            case 0x98: tmpBuf[0] = 0xc8; tmpBuf[1] = 0x99; replacementLen = 2; break;
            case 0x9a: tmpBuf[0] = 0xc8; tmpBuf[1] = 0x9b; replacementLen = 2; break;
            case 0x9c: tmpBuf[0] = 0xc8; tmpBuf[1] = 0x9d; replacementLen = 2; break;
            case 0x9e: tmpBuf[0] = 0xc8; tmpBuf[1] = 0x9f; replacementLen = 2; break;
            case 0xa0: tmpBuf[0] = 0xc6; tmpBuf[1] = 0x9e; replacementLen = 2; break;
            case 0xa2: tmpBuf[0] = 0xc8; tmpBuf[1] = 0xa3; replacementLen = 2; break;
            case 0xa4: tmpBuf[0] = 0xc8; tmpBuf[1] = 0xa5; replacementLen = 2; break;
            case 0xa6: tmpBuf[0] = 0xc8; tmpBuf[1] = 0xa7; replacementLen = 2; break;
            case 0xa8: tmpBuf[0] = 0xc8; tmpBuf[1] = 0xa9; replacementLen = 2; break;
            case 0xaa: tmpBuf[0] = 0xc8; tmpBuf[1] = 0xab; replacementLen = 2; break;
            case 0xac: tmpBuf[0] = 0xc8; tmpBuf[1] = 0xad; replacementLen = 2; break;
            case 0xae: tmpBuf[0] = 0xc8; tmpBuf[1] = 0xaf; replacementLen = 2; break;
            case 0xb0: tmpBuf[0] = 0xc8; tmpBuf[1] = 0xb1; replacementLen = 2; break;
            case 0xb2: tmpBuf[0] = 0xc8; tmpBuf[1] = 0xb3; replacementLen = 2; break;
            case 0xba: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb1; tmpBuf[2] = 0xa5; replacementLen = 3; break;
            case 0xbb: tmpBuf[0] = 0xc8; tmpBuf[1] = 0xbc; replacementLen = 2; break;
            case 0xbd: tmpBuf[0] = 0xc6; tmpBuf[1] = 0x9a; replacementLen = 2; break;
            case 0xbe: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb1; tmpBuf[2] = 0xa6; replacementLen = 3; break;
            default:
              break;
          }
          break;
        case 0xc9:
          switch(*(p+1))
          {
            case 0x81: tmpBuf[0] = 0xc9; tmpBuf[1] = 0x82; replacementLen = 2; break;
            case 0x83: tmpBuf[0] = 0xc6; tmpBuf[1] = 0x80; replacementLen = 2; break;
            case 0x84: tmpBuf[0] = 0xca; tmpBuf[1] = 0x89; replacementLen = 2; break;
            case 0x85: tmpBuf[0] = 0xca; tmpBuf[1] = 0x8c; replacementLen = 2; break;
            case 0x86: tmpBuf[0] = 0xc9; tmpBuf[1] = 0x87; replacementLen = 2; break;
            case 0x88: tmpBuf[0] = 0xc9; tmpBuf[1] = 0x89; replacementLen = 2; break;
            case 0x8a: tmpBuf[0] = 0xc9; tmpBuf[1] = 0x8b; replacementLen = 2; break;
            case 0x8c: tmpBuf[0] = 0xc9; tmpBuf[1] = 0x8d; replacementLen = 2; break;
            case 0x8e: tmpBuf[0] = 0xc9; tmpBuf[1] = 0x8f; replacementLen = 2; break;
            default:
              break;
          }
          break;
        case 0xcd:
          switch(*(p+1))
          {
            case 0xb0: tmpBuf[0] = 0xcd; tmpBuf[1] = 0xb1; replacementLen = 2; break;
            case 0xb2: tmpBuf[0] = 0xcd; tmpBuf[1] = 0xb3; replacementLen = 2; break;
            case 0xb6: tmpBuf[0] = 0xcd; tmpBuf[1] = 0xb7; replacementLen = 2; break;
            case 0xbf: tmpBuf[0] = 0xcf; tmpBuf[1] = 0xb3; replacementLen = 2; break;
            default:
              break;
          }
          break;
        case 0xce:
          switch(*(p+1))
          {
            case 0x86: tmpBuf[0] = 0xce; tmpBuf[1] = 0xac; replacementLen = 2; break;
            case 0x88: tmpBuf[0] = 0xce; tmpBuf[1] = 0xad; replacementLen = 2; break;
            case 0x89: tmpBuf[0] = 0xce; tmpBuf[1] = 0xae; replacementLen = 2; break;
            case 0x8a: tmpBuf[0] = 0xce; tmpBuf[1] = 0xaf; replacementLen = 2; break;
            case 0x8c: tmpBuf[0] = 0xcf; tmpBuf[1] = 0x8c; replacementLen = 2; break;
            case 0x8e: tmpBuf[0] = 0xcf; tmpBuf[1] = 0x8d; replacementLen = 2; break;
            case 0x8f: tmpBuf[0] = 0xcf; tmpBuf[1] = 0x8e; replacementLen = 2; break;
            case 0x91: tmpBuf[0] = 0xce; tmpBuf[1] = 0xb1; replacementLen = 2; break;
            case 0x92: tmpBuf[0] = 0xce; tmpBuf[1] = 0xb2; replacementLen = 2; break;
            case 0x93: tmpBuf[0] = 0xce; tmpBuf[1] = 0xb3; replacementLen = 2; break;
            case 0x94: tmpBuf[0] = 0xce; tmpBuf[1] = 0xb4; replacementLen = 2; break;
            case 0x95: tmpBuf[0] = 0xce; tmpBuf[1] = 0xb5; replacementLen = 2; break;
            case 0x96: tmpBuf[0] = 0xce; tmpBuf[1] = 0xb6; replacementLen = 2; break;
            case 0x97: tmpBuf[0] = 0xce; tmpBuf[1] = 0xb7; replacementLen = 2; break;
            case 0x98: tmpBuf[0] = 0xce; tmpBuf[1] = 0xb8; replacementLen = 2; break;
            case 0x99: tmpBuf[0] = 0xce; tmpBuf[1] = 0xb9; replacementLen = 2; break;
            case 0x9a: tmpBuf[0] = 0xce; tmpBuf[1] = 0xba; replacementLen = 2; break;
            case 0x9b: tmpBuf[0] = 0xce; tmpBuf[1] = 0xbb; replacementLen = 2; break;
            case 0x9c: tmpBuf[0] = 0xce; tmpBuf[1] = 0xbc; replacementLen = 2; break;
            case 0x9d: tmpBuf[0] = 0xce; tmpBuf[1] = 0xbd; replacementLen = 2; break;
            case 0x9e: tmpBuf[0] = 0xce; tmpBuf[1] = 0xbe; replacementLen = 2; break;
            case 0x9f: tmpBuf[0] = 0xce; tmpBuf[1] = 0xbf; replacementLen = 2; break;
            case 0xa0: tmpBuf[0] = 0xcf; tmpBuf[1] = 0x80; replacementLen = 2; break;
            case 0xa1: tmpBuf[0] = 0xcf; tmpBuf[1] = 0x81; replacementLen = 2; break;
            case 0xa3: tmpBuf[0] = 0xcf; tmpBuf[1] = 0x83; replacementLen = 2; break;
            case 0xa4: tmpBuf[0] = 0xcf; tmpBuf[1] = 0x84; replacementLen = 2; break;
            case 0xa5: tmpBuf[0] = 0xcf; tmpBuf[1] = 0x85; replacementLen = 2; break;
            case 0xa6: tmpBuf[0] = 0xcf; tmpBuf[1] = 0x86; replacementLen = 2; break;
            case 0xa7: tmpBuf[0] = 0xcf; tmpBuf[1] = 0x87; replacementLen = 2; break;
            case 0xa8: tmpBuf[0] = 0xcf; tmpBuf[1] = 0x88; replacementLen = 2; break;
            case 0xa9: tmpBuf[0] = 0xcf; tmpBuf[1] = 0x89; replacementLen = 2; break;
            case 0xaa: tmpBuf[0] = 0xcf; tmpBuf[1] = 0x8a; replacementLen = 2; break;
            case 0xab: tmpBuf[0] = 0xcf; tmpBuf[1] = 0x8b; replacementLen = 2; break;
            default:
              break;
          }
          break;
        case 0xcf:
          switch(*(p+1))
          {
            case 0x8f: tmpBuf[0] = 0xcf; tmpBuf[1] = 0x97; replacementLen = 2; break;
            case 0x98: tmpBuf[0] = 0xcf; tmpBuf[1] = 0x99; replacementLen = 2; break;
            case 0x9a: tmpBuf[0] = 0xcf; tmpBuf[1] = 0x9b; replacementLen = 2; break;
            case 0x9c: tmpBuf[0] = 0xcf; tmpBuf[1] = 0x9d; replacementLen = 2; break;
            case 0x9e: tmpBuf[0] = 0xcf; tmpBuf[1] = 0x9f; replacementLen = 2; break;
            case 0xa0: tmpBuf[0] = 0xcf; tmpBuf[1] = 0xa1; replacementLen = 2; break;
            case 0xa2: tmpBuf[0] = 0xcf; tmpBuf[1] = 0xa3; replacementLen = 2; break;
            case 0xa4: tmpBuf[0] = 0xcf; tmpBuf[1] = 0xa5; replacementLen = 2; break;
            case 0xa6: tmpBuf[0] = 0xcf; tmpBuf[1] = 0xa7; replacementLen = 2; break;
            case 0xa8: tmpBuf[0] = 0xcf; tmpBuf[1] = 0xa9; replacementLen = 2; break;
            case 0xaa: tmpBuf[0] = 0xcf; tmpBuf[1] = 0xab; replacementLen = 2; break;
            case 0xac: tmpBuf[0] = 0xcf; tmpBuf[1] = 0xad; replacementLen = 2; break;
            case 0xae: tmpBuf[0] = 0xcf; tmpBuf[1] = 0xaf; replacementLen = 2; break;
            case 0xb4: tmpBuf[0] = 0xce; tmpBuf[1] = 0xb8; replacementLen = 2; break;
            case 0xb7: tmpBuf[0] = 0xcf; tmpBuf[1] = 0xb8; replacementLen = 2; break;
            case 0xb9: tmpBuf[0] = 0xcf; tmpBuf[1] = 0xb2; replacementLen = 2; break;
            case 0xba: tmpBuf[0] = 0xcf; tmpBuf[1] = 0xbb; replacementLen = 2; break;
            case 0xbd: tmpBuf[0] = 0xcd; tmpBuf[1] = 0xbb; replacementLen = 2; break;
            case 0xbe: tmpBuf[0] = 0xcd; tmpBuf[1] = 0xbc; replacementLen = 2; break;
            case 0xbf: tmpBuf[0] = 0xcd; tmpBuf[1] = 0xbd; replacementLen = 2; break;
            default:
              break;
          }
          break;
        case 0xd0:
          switch(*(p+1))
          {
            case 0x80: tmpBuf[0] = 0xd1; tmpBuf[1] = 0x90; replacementLen = 2; break;
            case 0x81: tmpBuf[0] = 0xd1; tmpBuf[1] = 0x91; replacementLen = 2; break;
            case 0x82: tmpBuf[0] = 0xd1; tmpBuf[1] = 0x92; replacementLen = 2; break;
            case 0x83: tmpBuf[0] = 0xd1; tmpBuf[1] = 0x93; replacementLen = 2; break;
            case 0x84: tmpBuf[0] = 0xd1; tmpBuf[1] = 0x94; replacementLen = 2; break;
            case 0x85: tmpBuf[0] = 0xd1; tmpBuf[1] = 0x95; replacementLen = 2; break;
            case 0x86: tmpBuf[0] = 0xd1; tmpBuf[1] = 0x96; replacementLen = 2; break;
            case 0x87: tmpBuf[0] = 0xd1; tmpBuf[1] = 0x97; replacementLen = 2; break;
            case 0x88: tmpBuf[0] = 0xd1; tmpBuf[1] = 0x98; replacementLen = 2; break;
            case 0x89: tmpBuf[0] = 0xd1; tmpBuf[1] = 0x99; replacementLen = 2; break;
            case 0x8a: tmpBuf[0] = 0xd1; tmpBuf[1] = 0x9a; replacementLen = 2; break;
            case 0x8b: tmpBuf[0] = 0xd1; tmpBuf[1] = 0x9b; replacementLen = 2; break;
            case 0x8c: tmpBuf[0] = 0xd1; tmpBuf[1] = 0x9c; replacementLen = 2; break;
            case 0x8d: tmpBuf[0] = 0xd1; tmpBuf[1] = 0x9d; replacementLen = 2; break;
            case 0x8e: tmpBuf[0] = 0xd1; tmpBuf[1] = 0x9e; replacementLen = 2; break;
            case 0x8f: tmpBuf[0] = 0xd1; tmpBuf[1] = 0x9f; replacementLen = 2; break;
            case 0x90: tmpBuf[0] = 0xd0; tmpBuf[1] = 0xb0; replacementLen = 2; break;
            case 0x91: tmpBuf[0] = 0xd0; tmpBuf[1] = 0xb1; replacementLen = 2; break;
            case 0x92: tmpBuf[0] = 0xd0; tmpBuf[1] = 0xb2; replacementLen = 2; break;
            case 0x93: tmpBuf[0] = 0xd0; tmpBuf[1] = 0xb3; replacementLen = 2; break;
            case 0x94: tmpBuf[0] = 0xd0; tmpBuf[1] = 0xb4; replacementLen = 2; break;
            case 0x95: tmpBuf[0] = 0xd0; tmpBuf[1] = 0xb5; replacementLen = 2; break;
            case 0x96: tmpBuf[0] = 0xd0; tmpBuf[1] = 0xb6; replacementLen = 2; break;
            case 0x97: tmpBuf[0] = 0xd0; tmpBuf[1] = 0xb7; replacementLen = 2; break;
            case 0x98: tmpBuf[0] = 0xd0; tmpBuf[1] = 0xb8; replacementLen = 2; break;
            case 0x99: tmpBuf[0] = 0xd0; tmpBuf[1] = 0xb9; replacementLen = 2; break;
            case 0x9a: tmpBuf[0] = 0xd0; tmpBuf[1] = 0xba; replacementLen = 2; break;
            case 0x9b: tmpBuf[0] = 0xd0; tmpBuf[1] = 0xbb; replacementLen = 2; break;
            case 0x9c: tmpBuf[0] = 0xd0; tmpBuf[1] = 0xbc; replacementLen = 2; break;
            case 0x9d: tmpBuf[0] = 0xd0; tmpBuf[1] = 0xbd; replacementLen = 2; break;
            case 0x9e: tmpBuf[0] = 0xd0; tmpBuf[1] = 0xbe; replacementLen = 2; break;
            case 0x9f: tmpBuf[0] = 0xd0; tmpBuf[1] = 0xbf; replacementLen = 2; break;
            case 0xa0: tmpBuf[0] = 0xd1; tmpBuf[1] = 0x80; replacementLen = 2; break;
            case 0xa1: tmpBuf[0] = 0xd1; tmpBuf[1] = 0x81; replacementLen = 2; break;
            case 0xa2: tmpBuf[0] = 0xd1; tmpBuf[1] = 0x82; replacementLen = 2; break;
            case 0xa3: tmpBuf[0] = 0xd1; tmpBuf[1] = 0x83; replacementLen = 2; break;
            case 0xa4: tmpBuf[0] = 0xd1; tmpBuf[1] = 0x84; replacementLen = 2; break;
            case 0xa5: tmpBuf[0] = 0xd1; tmpBuf[1] = 0x85; replacementLen = 2; break;
            case 0xa6: tmpBuf[0] = 0xd1; tmpBuf[1] = 0x86; replacementLen = 2; break;
            case 0xa7: tmpBuf[0] = 0xd1; tmpBuf[1] = 0x87; replacementLen = 2; break;
            case 0xa8: tmpBuf[0] = 0xd1; tmpBuf[1] = 0x88; replacementLen = 2; break;
            case 0xa9: tmpBuf[0] = 0xd1; tmpBuf[1] = 0x89; replacementLen = 2; break;
            case 0xaa: tmpBuf[0] = 0xd1; tmpBuf[1] = 0x8a; replacementLen = 2; break;
            case 0xab: tmpBuf[0] = 0xd1; tmpBuf[1] = 0x8b; replacementLen = 2; break;
            case 0xac: tmpBuf[0] = 0xd1; tmpBuf[1] = 0x8c; replacementLen = 2; break;
            case 0xad: tmpBuf[0] = 0xd1; tmpBuf[1] = 0x8d; replacementLen = 2; break;
            case 0xae: tmpBuf[0] = 0xd1; tmpBuf[1] = 0x8e; replacementLen = 2; break;
            case 0xaf: tmpBuf[0] = 0xd1; tmpBuf[1] = 0x8f; replacementLen = 2; break;
            default:
              break;
          }
          break;
        case 0xd1:
          switch(*(p+1))
          {
            case 0xa0: tmpBuf[0] = 0xd1; tmpBuf[1] = 0xa1; replacementLen = 2; break;
            case 0xa2: tmpBuf[0] = 0xd1; tmpBuf[1] = 0xa3; replacementLen = 2; break;
            case 0xa4: tmpBuf[0] = 0xd1; tmpBuf[1] = 0xa5; replacementLen = 2; break;
            case 0xa6: tmpBuf[0] = 0xd1; tmpBuf[1] = 0xa7; replacementLen = 2; break;
            case 0xa8: tmpBuf[0] = 0xd1; tmpBuf[1] = 0xa9; replacementLen = 2; break;
            case 0xaa: tmpBuf[0] = 0xd1; tmpBuf[1] = 0xab; replacementLen = 2; break;
            case 0xac: tmpBuf[0] = 0xd1; tmpBuf[1] = 0xad; replacementLen = 2; break;
            case 0xae: tmpBuf[0] = 0xd1; tmpBuf[1] = 0xaf; replacementLen = 2; break;
            case 0xb0: tmpBuf[0] = 0xd1; tmpBuf[1] = 0xb1; replacementLen = 2; break;
            case 0xb2: tmpBuf[0] = 0xd1; tmpBuf[1] = 0xb3; replacementLen = 2; break;
            case 0xb4: tmpBuf[0] = 0xd1; tmpBuf[1] = 0xb5; replacementLen = 2; break;
            case 0xb6: tmpBuf[0] = 0xd1; tmpBuf[1] = 0xb7; replacementLen = 2; break;
            case 0xb8: tmpBuf[0] = 0xd1; tmpBuf[1] = 0xb9; replacementLen = 2; break;
            case 0xba: tmpBuf[0] = 0xd1; tmpBuf[1] = 0xbb; replacementLen = 2; break;
            case 0xbc: tmpBuf[0] = 0xd1; tmpBuf[1] = 0xbd; replacementLen = 2; break;
            case 0xbe: tmpBuf[0] = 0xd1; tmpBuf[1] = 0xbf; replacementLen = 2; break;
            default:
              break;
          }
          break;
        case 0xd2:
          switch(*(p+1))
          {
            case 0x80: tmpBuf[0] = 0xd2; tmpBuf[1] = 0x81; replacementLen = 2; break;
            case 0x8a: tmpBuf[0] = 0xd2; tmpBuf[1] = 0x8b; replacementLen = 2; break;
            case 0x8c: tmpBuf[0] = 0xd2; tmpBuf[1] = 0x8d; replacementLen = 2; break;
            case 0x8e: tmpBuf[0] = 0xd2; tmpBuf[1] = 0x8f; replacementLen = 2; break;
            case 0x90: tmpBuf[0] = 0xd2; tmpBuf[1] = 0x91; replacementLen = 2; break;
            case 0x92: tmpBuf[0] = 0xd2; tmpBuf[1] = 0x93; replacementLen = 2; break;
            case 0x94: tmpBuf[0] = 0xd2; tmpBuf[1] = 0x95; replacementLen = 2; break;
            case 0x96: tmpBuf[0] = 0xd2; tmpBuf[1] = 0x97; replacementLen = 2; break;
            case 0x98: tmpBuf[0] = 0xd2; tmpBuf[1] = 0x99; replacementLen = 2; break;
            case 0x9a: tmpBuf[0] = 0xd2; tmpBuf[1] = 0x9b; replacementLen = 2; break;
            case 0x9c: tmpBuf[0] = 0xd2; tmpBuf[1] = 0x9d; replacementLen = 2; break;
            case 0x9e: tmpBuf[0] = 0xd2; tmpBuf[1] = 0x9f; replacementLen = 2; break;
            case 0xa0: tmpBuf[0] = 0xd2; tmpBuf[1] = 0xa1; replacementLen = 2; break;
            case 0xa2: tmpBuf[0] = 0xd2; tmpBuf[1] = 0xa3; replacementLen = 2; break;
            case 0xa4: tmpBuf[0] = 0xd2; tmpBuf[1] = 0xa5; replacementLen = 2; break;
            case 0xa6: tmpBuf[0] = 0xd2; tmpBuf[1] = 0xa7; replacementLen = 2; break;
            case 0xa8: tmpBuf[0] = 0xd2; tmpBuf[1] = 0xa9; replacementLen = 2; break;
            case 0xaa: tmpBuf[0] = 0xd2; tmpBuf[1] = 0xab; replacementLen = 2; break;
            case 0xac: tmpBuf[0] = 0xd2; tmpBuf[1] = 0xad; replacementLen = 2; break;
            case 0xae: tmpBuf[0] = 0xd2; tmpBuf[1] = 0xaf; replacementLen = 2; break;
            case 0xb0: tmpBuf[0] = 0xd2; tmpBuf[1] = 0xb1; replacementLen = 2; break;
            case 0xb2: tmpBuf[0] = 0xd2; tmpBuf[1] = 0xb3; replacementLen = 2; break;
            case 0xb4: tmpBuf[0] = 0xd2; tmpBuf[1] = 0xb5; replacementLen = 2; break;
            case 0xb6: tmpBuf[0] = 0xd2; tmpBuf[1] = 0xb7; replacementLen = 2; break;
            case 0xb8: tmpBuf[0] = 0xd2; tmpBuf[1] = 0xb9; replacementLen = 2; break;
            case 0xba: tmpBuf[0] = 0xd2; tmpBuf[1] = 0xbb; replacementLen = 2; break;
            case 0xbc: tmpBuf[0] = 0xd2; tmpBuf[1] = 0xbd; replacementLen = 2; break;
            case 0xbe: tmpBuf[0] = 0xd2; tmpBuf[1] = 0xbf; replacementLen = 2; break;
            default:
              break;
          }
          break;
        case 0xd3:
          switch(*(p+1))
          {
            case 0x80: tmpBuf[0] = 0xd3; tmpBuf[1] = 0x8f; replacementLen = 2; break;
            case 0x81: tmpBuf[0] = 0xd3; tmpBuf[1] = 0x82; replacementLen = 2; break;
            case 0x83: tmpBuf[0] = 0xd3; tmpBuf[1] = 0x84; replacementLen = 2; break;
            case 0x85: tmpBuf[0] = 0xd3; tmpBuf[1] = 0x86; replacementLen = 2; break;
            case 0x87: tmpBuf[0] = 0xd3; tmpBuf[1] = 0x88; replacementLen = 2; break;
            case 0x89: tmpBuf[0] = 0xd3; tmpBuf[1] = 0x8a; replacementLen = 2; break;
            case 0x8b: tmpBuf[0] = 0xd3; tmpBuf[1] = 0x8c; replacementLen = 2; break;
            case 0x8d: tmpBuf[0] = 0xd3; tmpBuf[1] = 0x8e; replacementLen = 2; break;
            case 0x90: tmpBuf[0] = 0xd3; tmpBuf[1] = 0x91; replacementLen = 2; break;
            case 0x92: tmpBuf[0] = 0xd3; tmpBuf[1] = 0x93; replacementLen = 2; break;
            case 0x94: tmpBuf[0] = 0xd3; tmpBuf[1] = 0x95; replacementLen = 2; break;
            case 0x96: tmpBuf[0] = 0xd3; tmpBuf[1] = 0x97; replacementLen = 2; break;
            case 0x98: tmpBuf[0] = 0xd3; tmpBuf[1] = 0x99; replacementLen = 2; break;
            case 0x9a: tmpBuf[0] = 0xd3; tmpBuf[1] = 0x9b; replacementLen = 2; break;
            case 0x9c: tmpBuf[0] = 0xd3; tmpBuf[1] = 0x9d; replacementLen = 2; break;
            case 0x9e: tmpBuf[0] = 0xd3; tmpBuf[1] = 0x9f; replacementLen = 2; break;
            case 0xa0: tmpBuf[0] = 0xd3; tmpBuf[1] = 0xa1; replacementLen = 2; break;
            case 0xa2: tmpBuf[0] = 0xd3; tmpBuf[1] = 0xa3; replacementLen = 2; break;
            case 0xa4: tmpBuf[0] = 0xd3; tmpBuf[1] = 0xa5; replacementLen = 2; break;
            case 0xa6: tmpBuf[0] = 0xd3; tmpBuf[1] = 0xa7; replacementLen = 2; break;
            case 0xa8: tmpBuf[0] = 0xd3; tmpBuf[1] = 0xa9; replacementLen = 2; break;
            case 0xaa: tmpBuf[0] = 0xd3; tmpBuf[1] = 0xab; replacementLen = 2; break;
            case 0xac: tmpBuf[0] = 0xd3; tmpBuf[1] = 0xad; replacementLen = 2; break;
            case 0xae: tmpBuf[0] = 0xd3; tmpBuf[1] = 0xaf; replacementLen = 2; break;
            case 0xb0: tmpBuf[0] = 0xd3; tmpBuf[1] = 0xb1; replacementLen = 2; break;
            case 0xb2: tmpBuf[0] = 0xd3; tmpBuf[1] = 0xb3; replacementLen = 2; break;
            case 0xb4: tmpBuf[0] = 0xd3; tmpBuf[1] = 0xb5; replacementLen = 2; break;
            case 0xb6: tmpBuf[0] = 0xd3; tmpBuf[1] = 0xb7; replacementLen = 2; break;
            case 0xb8: tmpBuf[0] = 0xd3; tmpBuf[1] = 0xb9; replacementLen = 2; break;
            case 0xba: tmpBuf[0] = 0xd3; tmpBuf[1] = 0xbb; replacementLen = 2; break;
            case 0xbc: tmpBuf[0] = 0xd3; tmpBuf[1] = 0xbd; replacementLen = 2; break;
            case 0xbe: tmpBuf[0] = 0xd3; tmpBuf[1] = 0xbf; replacementLen = 2; break;
            default:
              break;
          }
          break;
        case 0xd4:
          switch(*(p+1))
          {
            case 0x80: tmpBuf[0] = 0xd4; tmpBuf[1] = 0x81; replacementLen = 2; break;
            case 0x82: tmpBuf[0] = 0xd4; tmpBuf[1] = 0x83; replacementLen = 2; break;
            case 0x84: tmpBuf[0] = 0xd4; tmpBuf[1] = 0x85; replacementLen = 2; break;
            case 0x86: tmpBuf[0] = 0xd4; tmpBuf[1] = 0x87; replacementLen = 2; break;
            case 0x88: tmpBuf[0] = 0xd4; tmpBuf[1] = 0x89; replacementLen = 2; break;
            case 0x8a: tmpBuf[0] = 0xd4; tmpBuf[1] = 0x8b; replacementLen = 2; break;
            case 0x8c: tmpBuf[0] = 0xd4; tmpBuf[1] = 0x8d; replacementLen = 2; break;
            case 0x8e: tmpBuf[0] = 0xd4; tmpBuf[1] = 0x8f; replacementLen = 2; break;
            case 0x90: tmpBuf[0] = 0xd4; tmpBuf[1] = 0x91; replacementLen = 2; break;
            case 0x92: tmpBuf[0] = 0xd4; tmpBuf[1] = 0x93; replacementLen = 2; break;
            case 0x94: tmpBuf[0] = 0xd4; tmpBuf[1] = 0x95; replacementLen = 2; break;
            case 0x96: tmpBuf[0] = 0xd4; tmpBuf[1] = 0x97; replacementLen = 2; break;
            case 0x98: tmpBuf[0] = 0xd4; tmpBuf[1] = 0x99; replacementLen = 2; break;
            case 0x9a: tmpBuf[0] = 0xd4; tmpBuf[1] = 0x9b; replacementLen = 2; break;
            case 0x9c: tmpBuf[0] = 0xd4; tmpBuf[1] = 0x9d; replacementLen = 2; break;
            case 0x9e: tmpBuf[0] = 0xd4; tmpBuf[1] = 0x9f; replacementLen = 2; break;
            case 0xa0: tmpBuf[0] = 0xd4; tmpBuf[1] = 0xa1; replacementLen = 2; break;
            case 0xa2: tmpBuf[0] = 0xd4; tmpBuf[1] = 0xa3; replacementLen = 2; break;
            case 0xa4: tmpBuf[0] = 0xd4; tmpBuf[1] = 0xa5; replacementLen = 2; break;
            case 0xa6: tmpBuf[0] = 0xd4; tmpBuf[1] = 0xa7; replacementLen = 2; break;
            case 0xa8: tmpBuf[0] = 0xd4; tmpBuf[1] = 0xa9; replacementLen = 2; break;
            case 0xaa: tmpBuf[0] = 0xd4; tmpBuf[1] = 0xab; replacementLen = 2; break;
            case 0xac: tmpBuf[0] = 0xd4; tmpBuf[1] = 0xad; replacementLen = 2; break;
            case 0xae: tmpBuf[0] = 0xd4; tmpBuf[1] = 0xaf; replacementLen = 2; break;
            case 0xb1: tmpBuf[0] = 0xd5; tmpBuf[1] = 0xa1; replacementLen = 2; break;
            case 0xb2: tmpBuf[0] = 0xd5; tmpBuf[1] = 0xa2; replacementLen = 2; break;
            case 0xb3: tmpBuf[0] = 0xd5; tmpBuf[1] = 0xa3; replacementLen = 2; break;
            case 0xb4: tmpBuf[0] = 0xd5; tmpBuf[1] = 0xa4; replacementLen = 2; break;
            case 0xb5: tmpBuf[0] = 0xd5; tmpBuf[1] = 0xa5; replacementLen = 2; break;
            case 0xb6: tmpBuf[0] = 0xd5; tmpBuf[1] = 0xa6; replacementLen = 2; break;
            case 0xb7: tmpBuf[0] = 0xd5; tmpBuf[1] = 0xa7; replacementLen = 2; break;
            case 0xb8: tmpBuf[0] = 0xd5; tmpBuf[1] = 0xa8; replacementLen = 2; break;
            case 0xb9: tmpBuf[0] = 0xd5; tmpBuf[1] = 0xa9; replacementLen = 2; break;
            case 0xba: tmpBuf[0] = 0xd5; tmpBuf[1] = 0xaa; replacementLen = 2; break;
            case 0xbb: tmpBuf[0] = 0xd5; tmpBuf[1] = 0xab; replacementLen = 2; break;
            case 0xbc: tmpBuf[0] = 0xd5; tmpBuf[1] = 0xac; replacementLen = 2; break;
            case 0xbd: tmpBuf[0] = 0xd5; tmpBuf[1] = 0xad; replacementLen = 2; break;
            case 0xbe: tmpBuf[0] = 0xd5; tmpBuf[1] = 0xae; replacementLen = 2; break;
            case 0xbf: tmpBuf[0] = 0xd5; tmpBuf[1] = 0xaf; replacementLen = 2; break;
            default:
              break;
          }
          break;
        case 0xd5:
          switch(*(p+1))
          {
            case 0x80: tmpBuf[0] = 0xd5; tmpBuf[1] = 0xb0; replacementLen = 2; break;
            case 0x81: tmpBuf[0] = 0xd5; tmpBuf[1] = 0xb1; replacementLen = 2; break;
            case 0x82: tmpBuf[0] = 0xd5; tmpBuf[1] = 0xb2; replacementLen = 2; break;
            case 0x83: tmpBuf[0] = 0xd5; tmpBuf[1] = 0xb3; replacementLen = 2; break;
            case 0x84: tmpBuf[0] = 0xd5; tmpBuf[1] = 0xb4; replacementLen = 2; break;
            case 0x85: tmpBuf[0] = 0xd5; tmpBuf[1] = 0xb5; replacementLen = 2; break;
            case 0x86: tmpBuf[0] = 0xd5; tmpBuf[1] = 0xb6; replacementLen = 2; break;
            case 0x87: tmpBuf[0] = 0xd5; tmpBuf[1] = 0xb7; replacementLen = 2; break;
            case 0x88: tmpBuf[0] = 0xd5; tmpBuf[1] = 0xb8; replacementLen = 2; break;
            case 0x89: tmpBuf[0] = 0xd5; tmpBuf[1] = 0xb9; replacementLen = 2; break;
            case 0x8a: tmpBuf[0] = 0xd5; tmpBuf[1] = 0xba; replacementLen = 2; break;
            case 0x8b: tmpBuf[0] = 0xd5; tmpBuf[1] = 0xbb; replacementLen = 2; break;
            case 0x8c: tmpBuf[0] = 0xd5; tmpBuf[1] = 0xbc; replacementLen = 2; break;
            case 0x8d: tmpBuf[0] = 0xd5; tmpBuf[1] = 0xbd; replacementLen = 2; break;
            case 0x8e: tmpBuf[0] = 0xd5; tmpBuf[1] = 0xbe; replacementLen = 2; break;
            case 0x8f: tmpBuf[0] = 0xd5; tmpBuf[1] = 0xbf; replacementLen = 2; break;
            case 0x90: tmpBuf[0] = 0xd6; tmpBuf[1] = 0x80; replacementLen = 2; break;
            case 0x91: tmpBuf[0] = 0xd6; tmpBuf[1] = 0x81; replacementLen = 2; break;
            case 0x92: tmpBuf[0] = 0xd6; tmpBuf[1] = 0x82; replacementLen = 2; break;
            case 0x93: tmpBuf[0] = 0xd6; tmpBuf[1] = 0x83; replacementLen = 2; break;
            case 0x94: tmpBuf[0] = 0xd6; tmpBuf[1] = 0x84; replacementLen = 2; break;
            case 0x95: tmpBuf[0] = 0xd6; tmpBuf[1] = 0x85; replacementLen = 2; break;
            case 0x96: tmpBuf[0] = 0xd6; tmpBuf[1] = 0x86; replacementLen = 2; break;
            default:
              break;
          }
          break;
        case 0xe1:
          switch(*(p+1))
          {
            case 0x82:
              switch(*(p+2))
              {
                case 0xa0: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb4; tmpBuf[2] = 0x80; replacementLen = 3; break;
                case 0xa1: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb4; tmpBuf[2] = 0x81; replacementLen = 3; break;
                case 0xa2: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb4; tmpBuf[2] = 0x82; replacementLen = 3; break;
                case 0xa3: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb4; tmpBuf[2] = 0x83; replacementLen = 3; break;
                case 0xa4: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb4; tmpBuf[2] = 0x84; replacementLen = 3; break;
                case 0xa5: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb4; tmpBuf[2] = 0x85; replacementLen = 3; break;
                case 0xa6: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb4; tmpBuf[2] = 0x86; replacementLen = 3; break;
                case 0xa7: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb4; tmpBuf[2] = 0x87; replacementLen = 3; break;
                case 0xa8: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb4; tmpBuf[2] = 0x88; replacementLen = 3; break;
                case 0xa9: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb4; tmpBuf[2] = 0x89; replacementLen = 3; break;
                case 0xaa: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb4; tmpBuf[2] = 0x8a; replacementLen = 3; break;
                case 0xab: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb4; tmpBuf[2] = 0x8b; replacementLen = 3; break;
                case 0xac: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb4; tmpBuf[2] = 0x8c; replacementLen = 3; break;
                case 0xad: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb4; tmpBuf[2] = 0x8d; replacementLen = 3; break;
                case 0xae: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb4; tmpBuf[2] = 0x8e; replacementLen = 3; break;
                case 0xaf: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb4; tmpBuf[2] = 0x8f; replacementLen = 3; break;
                case 0xb0: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb4; tmpBuf[2] = 0x90; replacementLen = 3; break;
                case 0xb1: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb4; tmpBuf[2] = 0x91; replacementLen = 3; break;
                case 0xb2: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb4; tmpBuf[2] = 0x92; replacementLen = 3; break;
                case 0xb3: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb4; tmpBuf[2] = 0x93; replacementLen = 3; break;
                case 0xb4: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb4; tmpBuf[2] = 0x94; replacementLen = 3; break;
                case 0xb5: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb4; tmpBuf[2] = 0x95; replacementLen = 3; break;
                case 0xb6: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb4; tmpBuf[2] = 0x96; replacementLen = 3; break;
                case 0xb7: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb4; tmpBuf[2] = 0x97; replacementLen = 3; break;
                case 0xb8: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb4; tmpBuf[2] = 0x98; replacementLen = 3; break;
                case 0xb9: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb4; tmpBuf[2] = 0x99; replacementLen = 3; break;
                case 0xba: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb4; tmpBuf[2] = 0x9a; replacementLen = 3; break;
                case 0xbb: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb4; tmpBuf[2] = 0x9b; replacementLen = 3; break;
                case 0xbc: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb4; tmpBuf[2] = 0x9c; replacementLen = 3; break;
                case 0xbd: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb4; tmpBuf[2] = 0x9d; replacementLen = 3; break;
                case 0xbe: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb4; tmpBuf[2] = 0x9e; replacementLen = 3; break;
                case 0xbf: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb4; tmpBuf[2] = 0x9f; replacementLen = 3; break;
                default:
                  break;
              }
              break;
            case 0x83:
              switch(*(p+2))
              {
                case 0x80: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb4; tmpBuf[2] = 0xa0; replacementLen = 3; break;
                case 0x81: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb4; tmpBuf[2] = 0xa1; replacementLen = 3; break;
                case 0x82: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb4; tmpBuf[2] = 0xa2; replacementLen = 3; break;
                case 0x83: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb4; tmpBuf[2] = 0xa3; replacementLen = 3; break;
                case 0x84: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb4; tmpBuf[2] = 0xa4; replacementLen = 3; break;
                case 0x85: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb4; tmpBuf[2] = 0xa5; replacementLen = 3; break;
                case 0x87: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb4; tmpBuf[2] = 0xa7; replacementLen = 3; break;
                case 0x8d: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb4; tmpBuf[2] = 0xad; replacementLen = 3; break;
                default:
                  break;
              }
              break;
            case 0x8e:
              switch(*(p+2))
              {
                case 0xa0: tmpBuf[0] = 0xea; tmpBuf[1] = 0xad; tmpBuf[2] = 0xb0; replacementLen = 3; break;
                case 0xa1: tmpBuf[0] = 0xea; tmpBuf[1] = 0xad; tmpBuf[2] = 0xb1; replacementLen = 3; break;
                case 0xa2: tmpBuf[0] = 0xea; tmpBuf[1] = 0xad; tmpBuf[2] = 0xb2; replacementLen = 3; break;
                case 0xa3: tmpBuf[0] = 0xea; tmpBuf[1] = 0xad; tmpBuf[2] = 0xb3; replacementLen = 3; break;
                case 0xa4: tmpBuf[0] = 0xea; tmpBuf[1] = 0xad; tmpBuf[2] = 0xb4; replacementLen = 3; break;
                case 0xa5: tmpBuf[0] = 0xea; tmpBuf[1] = 0xad; tmpBuf[2] = 0xb5; replacementLen = 3; break;
                case 0xa6: tmpBuf[0] = 0xea; tmpBuf[1] = 0xad; tmpBuf[2] = 0xb6; replacementLen = 3; break;
                case 0xa7: tmpBuf[0] = 0xea; tmpBuf[1] = 0xad; tmpBuf[2] = 0xb7; replacementLen = 3; break;
                case 0xa8: tmpBuf[0] = 0xea; tmpBuf[1] = 0xad; tmpBuf[2] = 0xb8; replacementLen = 3; break;
                case 0xa9: tmpBuf[0] = 0xea; tmpBuf[1] = 0xad; tmpBuf[2] = 0xb9; replacementLen = 3; break;
                case 0xaa: tmpBuf[0] = 0xea; tmpBuf[1] = 0xad; tmpBuf[2] = 0xba; replacementLen = 3; break;
                case 0xab: tmpBuf[0] = 0xea; tmpBuf[1] = 0xad; tmpBuf[2] = 0xbb; replacementLen = 3; break;
                case 0xac: tmpBuf[0] = 0xea; tmpBuf[1] = 0xad; tmpBuf[2] = 0xbc; replacementLen = 3; break;
                case 0xad: tmpBuf[0] = 0xea; tmpBuf[1] = 0xad; tmpBuf[2] = 0xbd; replacementLen = 3; break;
                case 0xae: tmpBuf[0] = 0xea; tmpBuf[1] = 0xad; tmpBuf[2] = 0xbe; replacementLen = 3; break;
                case 0xaf: tmpBuf[0] = 0xea; tmpBuf[1] = 0xad; tmpBuf[2] = 0xbf; replacementLen = 3; break;
                case 0xb0: tmpBuf[0] = 0xea; tmpBuf[1] = 0xae; tmpBuf[2] = 0x80; replacementLen = 3; break;
                case 0xb1: tmpBuf[0] = 0xea; tmpBuf[1] = 0xae; tmpBuf[2] = 0x81; replacementLen = 3; break;
                case 0xb2: tmpBuf[0] = 0xea; tmpBuf[1] = 0xae; tmpBuf[2] = 0x82; replacementLen = 3; break;
                case 0xb3: tmpBuf[0] = 0xea; tmpBuf[1] = 0xae; tmpBuf[2] = 0x83; replacementLen = 3; break;
                case 0xb4: tmpBuf[0] = 0xea; tmpBuf[1] = 0xae; tmpBuf[2] = 0x84; replacementLen = 3; break;
                case 0xb5: tmpBuf[0] = 0xea; tmpBuf[1] = 0xae; tmpBuf[2] = 0x85; replacementLen = 3; break;
                case 0xb6: tmpBuf[0] = 0xea; tmpBuf[1] = 0xae; tmpBuf[2] = 0x86; replacementLen = 3; break;
                case 0xb7: tmpBuf[0] = 0xea; tmpBuf[1] = 0xae; tmpBuf[2] = 0x87; replacementLen = 3; break;
                case 0xb8: tmpBuf[0] = 0xea; tmpBuf[1] = 0xae; tmpBuf[2] = 0x88; replacementLen = 3; break;
                case 0xb9: tmpBuf[0] = 0xea; tmpBuf[1] = 0xae; tmpBuf[2] = 0x89; replacementLen = 3; break;
                case 0xba: tmpBuf[0] = 0xea; tmpBuf[1] = 0xae; tmpBuf[2] = 0x8a; replacementLen = 3; break;
                case 0xbb: tmpBuf[0] = 0xea; tmpBuf[1] = 0xae; tmpBuf[2] = 0x8b; replacementLen = 3; break;
                case 0xbc: tmpBuf[0] = 0xea; tmpBuf[1] = 0xae; tmpBuf[2] = 0x8c; replacementLen = 3; break;
                case 0xbd: tmpBuf[0] = 0xea; tmpBuf[1] = 0xae; tmpBuf[2] = 0x8d; replacementLen = 3; break;
                case 0xbe: tmpBuf[0] = 0xea; tmpBuf[1] = 0xae; tmpBuf[2] = 0x8e; replacementLen = 3; break;
                case 0xbf: tmpBuf[0] = 0xea; tmpBuf[1] = 0xae; tmpBuf[2] = 0x8f; replacementLen = 3; break;
                default:
                  break;
              }
              break;
            case 0x8f:
              switch(*(p+2))
              {
                case 0x80: tmpBuf[0] = 0xea; tmpBuf[1] = 0xae; tmpBuf[2] = 0x90; replacementLen = 3; break;
                case 0x81: tmpBuf[0] = 0xea; tmpBuf[1] = 0xae; tmpBuf[2] = 0x91; replacementLen = 3; break;
                case 0x82: tmpBuf[0] = 0xea; tmpBuf[1] = 0xae; tmpBuf[2] = 0x92; replacementLen = 3; break;
                case 0x83: tmpBuf[0] = 0xea; tmpBuf[1] = 0xae; tmpBuf[2] = 0x93; replacementLen = 3; break;
                case 0x84: tmpBuf[0] = 0xea; tmpBuf[1] = 0xae; tmpBuf[2] = 0x94; replacementLen = 3; break;
                case 0x85: tmpBuf[0] = 0xea; tmpBuf[1] = 0xae; tmpBuf[2] = 0x95; replacementLen = 3; break;
                case 0x86: tmpBuf[0] = 0xea; tmpBuf[1] = 0xae; tmpBuf[2] = 0x96; replacementLen = 3; break;
                case 0x87: tmpBuf[0] = 0xea; tmpBuf[1] = 0xae; tmpBuf[2] = 0x97; replacementLen = 3; break;
                case 0x88: tmpBuf[0] = 0xea; tmpBuf[1] = 0xae; tmpBuf[2] = 0x98; replacementLen = 3; break;
                case 0x89: tmpBuf[0] = 0xea; tmpBuf[1] = 0xae; tmpBuf[2] = 0x99; replacementLen = 3; break;
                case 0x8a: tmpBuf[0] = 0xea; tmpBuf[1] = 0xae; tmpBuf[2] = 0x9a; replacementLen = 3; break;
                case 0x8b: tmpBuf[0] = 0xea; tmpBuf[1] = 0xae; tmpBuf[2] = 0x9b; replacementLen = 3; break;
                case 0x8c: tmpBuf[0] = 0xea; tmpBuf[1] = 0xae; tmpBuf[2] = 0x9c; replacementLen = 3; break;
                case 0x8d: tmpBuf[0] = 0xea; tmpBuf[1] = 0xae; tmpBuf[2] = 0x9d; replacementLen = 3; break;
                case 0x8e: tmpBuf[0] = 0xea; tmpBuf[1] = 0xae; tmpBuf[2] = 0x9e; replacementLen = 3; break;
                case 0x8f: tmpBuf[0] = 0xea; tmpBuf[1] = 0xae; tmpBuf[2] = 0x9f; replacementLen = 3; break;
                case 0x90: tmpBuf[0] = 0xea; tmpBuf[1] = 0xae; tmpBuf[2] = 0xa0; replacementLen = 3; break;
                case 0x91: tmpBuf[0] = 0xea; tmpBuf[1] = 0xae; tmpBuf[2] = 0xa1; replacementLen = 3; break;
                case 0x92: tmpBuf[0] = 0xea; tmpBuf[1] = 0xae; tmpBuf[2] = 0xa2; replacementLen = 3; break;
                case 0x93: tmpBuf[0] = 0xea; tmpBuf[1] = 0xae; tmpBuf[2] = 0xa3; replacementLen = 3; break;
                case 0x94: tmpBuf[0] = 0xea; tmpBuf[1] = 0xae; tmpBuf[2] = 0xa4; replacementLen = 3; break;
                case 0x95: tmpBuf[0] = 0xea; tmpBuf[1] = 0xae; tmpBuf[2] = 0xa5; replacementLen = 3; break;
                case 0x96: tmpBuf[0] = 0xea; tmpBuf[1] = 0xae; tmpBuf[2] = 0xa6; replacementLen = 3; break;
                case 0x97: tmpBuf[0] = 0xea; tmpBuf[1] = 0xae; tmpBuf[2] = 0xa7; replacementLen = 3; break;
                case 0x98: tmpBuf[0] = 0xea; tmpBuf[1] = 0xae; tmpBuf[2] = 0xa8; replacementLen = 3; break;
                case 0x99: tmpBuf[0] = 0xea; tmpBuf[1] = 0xae; tmpBuf[2] = 0xa9; replacementLen = 3; break;
                case 0x9a: tmpBuf[0] = 0xea; tmpBuf[1] = 0xae; tmpBuf[2] = 0xaa; replacementLen = 3; break;
                case 0x9b: tmpBuf[0] = 0xea; tmpBuf[1] = 0xae; tmpBuf[2] = 0xab; replacementLen = 3; break;
                case 0x9c: tmpBuf[0] = 0xea; tmpBuf[1] = 0xae; tmpBuf[2] = 0xac; replacementLen = 3; break;
                case 0x9d: tmpBuf[0] = 0xea; tmpBuf[1] = 0xae; tmpBuf[2] = 0xad; replacementLen = 3; break;
                case 0x9e: tmpBuf[0] = 0xea; tmpBuf[1] = 0xae; tmpBuf[2] = 0xae; replacementLen = 3; break;
                case 0x9f: tmpBuf[0] = 0xea; tmpBuf[1] = 0xae; tmpBuf[2] = 0xaf; replacementLen = 3; break;
                case 0xa0: tmpBuf[0] = 0xea; tmpBuf[1] = 0xae; tmpBuf[2] = 0xb0; replacementLen = 3; break;
                case 0xa1: tmpBuf[0] = 0xea; tmpBuf[1] = 0xae; tmpBuf[2] = 0xb1; replacementLen = 3; break;
                case 0xa2: tmpBuf[0] = 0xea; tmpBuf[1] = 0xae; tmpBuf[2] = 0xb2; replacementLen = 3; break;
                case 0xa3: tmpBuf[0] = 0xea; tmpBuf[1] = 0xae; tmpBuf[2] = 0xb3; replacementLen = 3; break;
                case 0xa4: tmpBuf[0] = 0xea; tmpBuf[1] = 0xae; tmpBuf[2] = 0xb4; replacementLen = 3; break;
                case 0xa5: tmpBuf[0] = 0xea; tmpBuf[1] = 0xae; tmpBuf[2] = 0xb5; replacementLen = 3; break;
                case 0xa6: tmpBuf[0] = 0xea; tmpBuf[1] = 0xae; tmpBuf[2] = 0xb6; replacementLen = 3; break;
                case 0xa7: tmpBuf[0] = 0xea; tmpBuf[1] = 0xae; tmpBuf[2] = 0xb7; replacementLen = 3; break;
                case 0xa8: tmpBuf[0] = 0xea; tmpBuf[1] = 0xae; tmpBuf[2] = 0xb8; replacementLen = 3; break;
                case 0xa9: tmpBuf[0] = 0xea; tmpBuf[1] = 0xae; tmpBuf[2] = 0xb9; replacementLen = 3; break;
                case 0xaa: tmpBuf[0] = 0xea; tmpBuf[1] = 0xae; tmpBuf[2] = 0xba; replacementLen = 3; break;
                case 0xab: tmpBuf[0] = 0xea; tmpBuf[1] = 0xae; tmpBuf[2] = 0xbb; replacementLen = 3; break;
                case 0xac: tmpBuf[0] = 0xea; tmpBuf[1] = 0xae; tmpBuf[2] = 0xbc; replacementLen = 3; break;
                case 0xad: tmpBuf[0] = 0xea; tmpBuf[1] = 0xae; tmpBuf[2] = 0xbd; replacementLen = 3; break;
                case 0xae: tmpBuf[0] = 0xea; tmpBuf[1] = 0xae; tmpBuf[2] = 0xbe; replacementLen = 3; break;
                case 0xaf: tmpBuf[0] = 0xea; tmpBuf[1] = 0xae; tmpBuf[2] = 0xbf; replacementLen = 3; break;
                case 0xb0: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8f; tmpBuf[2] = 0xb8; replacementLen = 3; break;
                case 0xb1: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8f; tmpBuf[2] = 0xb9; replacementLen = 3; break;
                case 0xb2: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8f; tmpBuf[2] = 0xba; replacementLen = 3; break;
                case 0xb3: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8f; tmpBuf[2] = 0xbb; replacementLen = 3; break;
                case 0xb4: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8f; tmpBuf[2] = 0xbc; replacementLen = 3; break;
                case 0xb5: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x8f; tmpBuf[2] = 0xbd; replacementLen = 3; break;
                default:
                  break;
              }
              break;
            case 0xb2:
              switch(*(p+2))
              {
                case 0x90: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x83; tmpBuf[2] = 0x90; replacementLen = 3; break;
                case 0x91: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x83; tmpBuf[2] = 0x91; replacementLen = 3; break;
                case 0x92: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x83; tmpBuf[2] = 0x92; replacementLen = 3; break;
                case 0x93: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x83; tmpBuf[2] = 0x93; replacementLen = 3; break;
                case 0x94: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x83; tmpBuf[2] = 0x94; replacementLen = 3; break;
                case 0x95: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x83; tmpBuf[2] = 0x95; replacementLen = 3; break;
                case 0x96: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x83; tmpBuf[2] = 0x96; replacementLen = 3; break;
                case 0x97: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x83; tmpBuf[2] = 0x97; replacementLen = 3; break;
                case 0x98: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x83; tmpBuf[2] = 0x98; replacementLen = 3; break;
                case 0x99: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x83; tmpBuf[2] = 0x99; replacementLen = 3; break;
                case 0x9a: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x83; tmpBuf[2] = 0x9a; replacementLen = 3; break;
                case 0x9b: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x83; tmpBuf[2] = 0x9b; replacementLen = 3; break;
                case 0x9c: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x83; tmpBuf[2] = 0x9c; replacementLen = 3; break;
                case 0x9d: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x83; tmpBuf[2] = 0x9d; replacementLen = 3; break;
                case 0x9e: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x83; tmpBuf[2] = 0x9e; replacementLen = 3; break;
                case 0x9f: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x83; tmpBuf[2] = 0x9f; replacementLen = 3; break;
                case 0xa0: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x83; tmpBuf[2] = 0xa0; replacementLen = 3; break;
                case 0xa1: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x83; tmpBuf[2] = 0xa1; replacementLen = 3; break;
                case 0xa2: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x83; tmpBuf[2] = 0xa2; replacementLen = 3; break;
                case 0xa3: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x83; tmpBuf[2] = 0xa3; replacementLen = 3; break;
                case 0xa4: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x83; tmpBuf[2] = 0xa4; replacementLen = 3; break;
                case 0xa5: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x83; tmpBuf[2] = 0xa5; replacementLen = 3; break;
                case 0xa6: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x83; tmpBuf[2] = 0xa6; replacementLen = 3; break;
                case 0xa7: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x83; tmpBuf[2] = 0xa7; replacementLen = 3; break;
                case 0xa8: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x83; tmpBuf[2] = 0xa8; replacementLen = 3; break;
                case 0xa9: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x83; tmpBuf[2] = 0xa9; replacementLen = 3; break;
                case 0xaa: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x83; tmpBuf[2] = 0xaa; replacementLen = 3; break;
                case 0xab: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x83; tmpBuf[2] = 0xab; replacementLen = 3; break;
                case 0xac: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x83; tmpBuf[2] = 0xac; replacementLen = 3; break;
                case 0xad: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x83; tmpBuf[2] = 0xad; replacementLen = 3; break;
                case 0xae: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x83; tmpBuf[2] = 0xae; replacementLen = 3; break;
                case 0xaf: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x83; tmpBuf[2] = 0xaf; replacementLen = 3; break;
                case 0xb0: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x83; tmpBuf[2] = 0xb0; replacementLen = 3; break;
                case 0xb1: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x83; tmpBuf[2] = 0xb1; replacementLen = 3; break;
                case 0xb2: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x83; tmpBuf[2] = 0xb2; replacementLen = 3; break;
                case 0xb3: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x83; tmpBuf[2] = 0xb3; replacementLen = 3; break;
                case 0xb4: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x83; tmpBuf[2] = 0xb4; replacementLen = 3; break;
                case 0xb5: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x83; tmpBuf[2] = 0xb5; replacementLen = 3; break;
                case 0xb6: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x83; tmpBuf[2] = 0xb6; replacementLen = 3; break;
                case 0xb7: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x83; tmpBuf[2] = 0xb7; replacementLen = 3; break;
                case 0xb8: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x83; tmpBuf[2] = 0xb8; replacementLen = 3; break;
                case 0xb9: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x83; tmpBuf[2] = 0xb9; replacementLen = 3; break;
                case 0xba: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x83; tmpBuf[2] = 0xba; replacementLen = 3; break;
                case 0xbd: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x83; tmpBuf[2] = 0xbd; replacementLen = 3; break;
                case 0xbe: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x83; tmpBuf[2] = 0xbe; replacementLen = 3; break;
                case 0xbf: tmpBuf[0] = 0xe1; tmpBuf[1] = 0x83; tmpBuf[2] = 0xbf; replacementLen = 3; break;
                default:
                  break;
              }
              break;
            case 0xb8:
              switch(*(p+2))
              {
                case 0x80: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb8; tmpBuf[2] = 0x81; replacementLen = 3; break;
                case 0x82: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb8; tmpBuf[2] = 0x83; replacementLen = 3; break;
                case 0x84: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb8; tmpBuf[2] = 0x85; replacementLen = 3; break;
                case 0x86: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb8; tmpBuf[2] = 0x87; replacementLen = 3; break;
                case 0x88: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb8; tmpBuf[2] = 0x89; replacementLen = 3; break;
                case 0x8a: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb8; tmpBuf[2] = 0x8b; replacementLen = 3; break;
                case 0x8c: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb8; tmpBuf[2] = 0x8d; replacementLen = 3; break;
                case 0x8e: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb8; tmpBuf[2] = 0x8f; replacementLen = 3; break;
                case 0x90: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb8; tmpBuf[2] = 0x91; replacementLen = 3; break;
                case 0x92: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb8; tmpBuf[2] = 0x93; replacementLen = 3; break;
                case 0x94: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb8; tmpBuf[2] = 0x95; replacementLen = 3; break;
                case 0x96: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb8; tmpBuf[2] = 0x97; replacementLen = 3; break;
                case 0x98: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb8; tmpBuf[2] = 0x99; replacementLen = 3; break;
                case 0x9a: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb8; tmpBuf[2] = 0x9b; replacementLen = 3; break;
                case 0x9c: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb8; tmpBuf[2] = 0x9d; replacementLen = 3; break;
                case 0x9e: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb8; tmpBuf[2] = 0x9f; replacementLen = 3; break;
                case 0xa0: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb8; tmpBuf[2] = 0xa1; replacementLen = 3; break;
                case 0xa2: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb8; tmpBuf[2] = 0xa3; replacementLen = 3; break;
                case 0xa4: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb8; tmpBuf[2] = 0xa5; replacementLen = 3; break;
                case 0xa6: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb8; tmpBuf[2] = 0xa7; replacementLen = 3; break;
                case 0xa8: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb8; tmpBuf[2] = 0xa9; replacementLen = 3; break;
                case 0xaa: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb8; tmpBuf[2] = 0xab; replacementLen = 3; break;
                case 0xac: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb8; tmpBuf[2] = 0xad; replacementLen = 3; break;
                case 0xae: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb8; tmpBuf[2] = 0xaf; replacementLen = 3; break;
                case 0xb0: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb8; tmpBuf[2] = 0xb1; replacementLen = 3; break;
                case 0xb2: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb8; tmpBuf[2] = 0xb3; replacementLen = 3; break;
                case 0xb4: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb8; tmpBuf[2] = 0xb5; replacementLen = 3; break;
                case 0xb6: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb8; tmpBuf[2] = 0xb7; replacementLen = 3; break;
                case 0xb8: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb8; tmpBuf[2] = 0xb9; replacementLen = 3; break;
                case 0xba: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb8; tmpBuf[2] = 0xbb; replacementLen = 3; break;
                case 0xbc: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb8; tmpBuf[2] = 0xbd; replacementLen = 3; break;
                case 0xbe: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb8; tmpBuf[2] = 0xbf; replacementLen = 3; break;
                default:
                  break;
              }
              break;
            case 0xb9:
              switch(*(p+2))
              {
                case 0x80: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb9; tmpBuf[2] = 0x81; replacementLen = 3; break;
                case 0x82: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb9; tmpBuf[2] = 0x83; replacementLen = 3; break;
                case 0x84: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb9; tmpBuf[2] = 0x85; replacementLen = 3; break;
                case 0x86: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb9; tmpBuf[2] = 0x87; replacementLen = 3; break;
                case 0x88: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb9; tmpBuf[2] = 0x89; replacementLen = 3; break;
                case 0x8a: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb9; tmpBuf[2] = 0x8b; replacementLen = 3; break;
                case 0x8c: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb9; tmpBuf[2] = 0x8d; replacementLen = 3; break;
                case 0x8e: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb9; tmpBuf[2] = 0x8f; replacementLen = 3; break;
                case 0x90: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb9; tmpBuf[2] = 0x91; replacementLen = 3; break;
                case 0x92: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb9; tmpBuf[2] = 0x93; replacementLen = 3; break;
                case 0x94: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb9; tmpBuf[2] = 0x95; replacementLen = 3; break;
                case 0x96: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb9; tmpBuf[2] = 0x97; replacementLen = 3; break;
                case 0x98: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb9; tmpBuf[2] = 0x99; replacementLen = 3; break;
                case 0x9a: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb9; tmpBuf[2] = 0x9b; replacementLen = 3; break;
                case 0x9c: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb9; tmpBuf[2] = 0x9d; replacementLen = 3; break;
                case 0x9e: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb9; tmpBuf[2] = 0x9f; replacementLen = 3; break;
                case 0xa0: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb9; tmpBuf[2] = 0xa1; replacementLen = 3; break;
                case 0xa2: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb9; tmpBuf[2] = 0xa3; replacementLen = 3; break;
                case 0xa4: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb9; tmpBuf[2] = 0xa5; replacementLen = 3; break;
                case 0xa6: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb9; tmpBuf[2] = 0xa7; replacementLen = 3; break;
                case 0xa8: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb9; tmpBuf[2] = 0xa9; replacementLen = 3; break;
                case 0xaa: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb9; tmpBuf[2] = 0xab; replacementLen = 3; break;
                case 0xac: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb9; tmpBuf[2] = 0xad; replacementLen = 3; break;
                case 0xae: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb9; tmpBuf[2] = 0xaf; replacementLen = 3; break;
                case 0xb0: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb9; tmpBuf[2] = 0xb1; replacementLen = 3; break;
                case 0xb2: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb9; tmpBuf[2] = 0xb3; replacementLen = 3; break;
                case 0xb4: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb9; tmpBuf[2] = 0xb5; replacementLen = 3; break;
                case 0xb6: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb9; tmpBuf[2] = 0xb7; replacementLen = 3; break;
                case 0xb8: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb9; tmpBuf[2] = 0xb9; replacementLen = 3; break;
                case 0xba: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb9; tmpBuf[2] = 0xbb; replacementLen = 3; break;
                case 0xbc: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb9; tmpBuf[2] = 0xbd; replacementLen = 3; break;
                case 0xbe: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb9; tmpBuf[2] = 0xbf; replacementLen = 3; break;
                default:
                  break;
              }
              break;
            case 0xba:
              switch(*(p+2))
              {
                case 0x80: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xba; tmpBuf[2] = 0x81; replacementLen = 3; break;
                case 0x82: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xba; tmpBuf[2] = 0x83; replacementLen = 3; break;
                case 0x84: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xba; tmpBuf[2] = 0x85; replacementLen = 3; break;
                case 0x86: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xba; tmpBuf[2] = 0x87; replacementLen = 3; break;
                case 0x88: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xba; tmpBuf[2] = 0x89; replacementLen = 3; break;
                case 0x8a: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xba; tmpBuf[2] = 0x8b; replacementLen = 3; break;
                case 0x8c: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xba; tmpBuf[2] = 0x8d; replacementLen = 3; break;
                case 0x8e: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xba; tmpBuf[2] = 0x8f; replacementLen = 3; break;
                case 0x90: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xba; tmpBuf[2] = 0x91; replacementLen = 3; break;
                case 0x92: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xba; tmpBuf[2] = 0x93; replacementLen = 3; break;
                case 0x94: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xba; tmpBuf[2] = 0x95; replacementLen = 3; break;
                case 0x9e: tmpBuf[0] = 0xc3; tmpBuf[1] = 0x9f; replacementLen = 2; break;
                case 0xa0: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xba; tmpBuf[2] = 0xa1; replacementLen = 3; break;
                case 0xa2: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xba; tmpBuf[2] = 0xa3; replacementLen = 3; break;
                case 0xa4: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xba; tmpBuf[2] = 0xa5; replacementLen = 3; break;
                case 0xa6: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xba; tmpBuf[2] = 0xa7; replacementLen = 3; break;
                case 0xa8: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xba; tmpBuf[2] = 0xa9; replacementLen = 3; break;
                case 0xaa: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xba; tmpBuf[2] = 0xab; replacementLen = 3; break;
                case 0xac: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xba; tmpBuf[2] = 0xad; replacementLen = 3; break;
                case 0xae: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xba; tmpBuf[2] = 0xaf; replacementLen = 3; break;
                case 0xb0: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xba; tmpBuf[2] = 0xb1; replacementLen = 3; break;
                case 0xb2: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xba; tmpBuf[2] = 0xb3; replacementLen = 3; break;
                case 0xb4: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xba; tmpBuf[2] = 0xb5; replacementLen = 3; break;
                case 0xb6: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xba; tmpBuf[2] = 0xb7; replacementLen = 3; break;
                case 0xb8: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xba; tmpBuf[2] = 0xb9; replacementLen = 3; break;
                case 0xba: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xba; tmpBuf[2] = 0xbb; replacementLen = 3; break;
                case 0xbc: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xba; tmpBuf[2] = 0xbd; replacementLen = 3; break;
                case 0xbe: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xba; tmpBuf[2] = 0xbf; replacementLen = 3; break;
                default:
                  break;
              }
              break;
            case 0xbb:
              switch(*(p+2))
              {
                case 0x80: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbb; tmpBuf[2] = 0x81; replacementLen = 3; break;
                case 0x82: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbb; tmpBuf[2] = 0x83; replacementLen = 3; break;
                case 0x84: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbb; tmpBuf[2] = 0x85; replacementLen = 3; break;
                case 0x86: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbb; tmpBuf[2] = 0x87; replacementLen = 3; break;
                case 0x88: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbb; tmpBuf[2] = 0x89; replacementLen = 3; break;
                case 0x8a: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbb; tmpBuf[2] = 0x8b; replacementLen = 3; break;
                case 0x8c: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbb; tmpBuf[2] = 0x8d; replacementLen = 3; break;
                case 0x8e: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbb; tmpBuf[2] = 0x8f; replacementLen = 3; break;
                case 0x90: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbb; tmpBuf[2] = 0x91; replacementLen = 3; break;
                case 0x92: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbb; tmpBuf[2] = 0x93; replacementLen = 3; break;
                case 0x94: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbb; tmpBuf[2] = 0x95; replacementLen = 3; break;
                case 0x96: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbb; tmpBuf[2] = 0x97; replacementLen = 3; break;
                case 0x98: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbb; tmpBuf[2] = 0x99; replacementLen = 3; break;
                case 0x9a: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbb; tmpBuf[2] = 0x9b; replacementLen = 3; break;
                case 0x9c: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbb; tmpBuf[2] = 0x9d; replacementLen = 3; break;
                case 0x9e: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbb; tmpBuf[2] = 0x9f; replacementLen = 3; break;
                case 0xa0: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbb; tmpBuf[2] = 0xa1; replacementLen = 3; break;
                case 0xa2: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbb; tmpBuf[2] = 0xa3; replacementLen = 3; break;
                case 0xa4: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbb; tmpBuf[2] = 0xa5; replacementLen = 3; break;
                case 0xa6: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbb; tmpBuf[2] = 0xa7; replacementLen = 3; break;
                case 0xa8: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbb; tmpBuf[2] = 0xa9; replacementLen = 3; break;
                case 0xaa: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbb; tmpBuf[2] = 0xab; replacementLen = 3; break;
                case 0xac: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbb; tmpBuf[2] = 0xad; replacementLen = 3; break;
                case 0xae: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbb; tmpBuf[2] = 0xaf; replacementLen = 3; break;
                case 0xb0: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbb; tmpBuf[2] = 0xb1; replacementLen = 3; break;
                case 0xb2: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbb; tmpBuf[2] = 0xb3; replacementLen = 3; break;
                case 0xb4: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbb; tmpBuf[2] = 0xb5; replacementLen = 3; break;
                case 0xb6: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbb; tmpBuf[2] = 0xb7; replacementLen = 3; break;
                case 0xb8: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbb; tmpBuf[2] = 0xb9; replacementLen = 3; break;
                case 0xba: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbb; tmpBuf[2] = 0xbb; replacementLen = 3; break;
                case 0xbc: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbb; tmpBuf[2] = 0xbd; replacementLen = 3; break;
                case 0xbe: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbb; tmpBuf[2] = 0xbf; replacementLen = 3; break;
                default:
                  break;
              }
              break;
            case 0xbc:
              switch(*(p+2))
              {
                case 0x88: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbc; tmpBuf[2] = 0x80; replacementLen = 3; break;
                case 0x89: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbc; tmpBuf[2] = 0x81; replacementLen = 3; break;
                case 0x8a: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbc; tmpBuf[2] = 0x82; replacementLen = 3; break;
                case 0x8b: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbc; tmpBuf[2] = 0x83; replacementLen = 3; break;
                case 0x8c: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbc; tmpBuf[2] = 0x84; replacementLen = 3; break;
                case 0x8d: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbc; tmpBuf[2] = 0x85; replacementLen = 3; break;
                case 0x8e: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbc; tmpBuf[2] = 0x86; replacementLen = 3; break;
                case 0x8f: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbc; tmpBuf[2] = 0x87; replacementLen = 3; break;
                case 0x98: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbc; tmpBuf[2] = 0x90; replacementLen = 3; break;
                case 0x99: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbc; tmpBuf[2] = 0x91; replacementLen = 3; break;
                case 0x9a: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbc; tmpBuf[2] = 0x92; replacementLen = 3; break;
                case 0x9b: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbc; tmpBuf[2] = 0x93; replacementLen = 3; break;
                case 0x9c: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbc; tmpBuf[2] = 0x94; replacementLen = 3; break;
                case 0x9d: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbc; tmpBuf[2] = 0x95; replacementLen = 3; break;
                case 0xa8: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbc; tmpBuf[2] = 0xa0; replacementLen = 3; break;
                case 0xa9: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbc; tmpBuf[2] = 0xa1; replacementLen = 3; break;
                case 0xaa: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbc; tmpBuf[2] = 0xa2; replacementLen = 3; break;
                case 0xab: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbc; tmpBuf[2] = 0xa3; replacementLen = 3; break;
                case 0xac: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbc; tmpBuf[2] = 0xa4; replacementLen = 3; break;
                case 0xad: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbc; tmpBuf[2] = 0xa5; replacementLen = 3; break;
                case 0xae: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbc; tmpBuf[2] = 0xa6; replacementLen = 3; break;
                case 0xaf: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbc; tmpBuf[2] = 0xa7; replacementLen = 3; break;
                case 0xb8: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbc; tmpBuf[2] = 0xb0; replacementLen = 3; break;
                case 0xb9: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbc; tmpBuf[2] = 0xb1; replacementLen = 3; break;
                case 0xba: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbc; tmpBuf[2] = 0xb2; replacementLen = 3; break;
                case 0xbb: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbc; tmpBuf[2] = 0xb3; replacementLen = 3; break;
                case 0xbc: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbc; tmpBuf[2] = 0xb4; replacementLen = 3; break;
                case 0xbd: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbc; tmpBuf[2] = 0xb5; replacementLen = 3; break;
                case 0xbe: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbc; tmpBuf[2] = 0xb6; replacementLen = 3; break;
                case 0xbf: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbc; tmpBuf[2] = 0xb7; replacementLen = 3; break;
                default:
                  break;
              }
              break;
            case 0xbd:
              switch(*(p+2))
              {
                case 0x88: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbd; tmpBuf[2] = 0x80; replacementLen = 3; break;
                case 0x89: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbd; tmpBuf[2] = 0x81; replacementLen = 3; break;
                case 0x8a: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbd; tmpBuf[2] = 0x82; replacementLen = 3; break;
                case 0x8b: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbd; tmpBuf[2] = 0x83; replacementLen = 3; break;
                case 0x8c: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbd; tmpBuf[2] = 0x84; replacementLen = 3; break;
                case 0x8d: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbd; tmpBuf[2] = 0x85; replacementLen = 3; break;
                case 0x99: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbd; tmpBuf[2] = 0x91; replacementLen = 3; break;
                case 0x9b: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbd; tmpBuf[2] = 0x93; replacementLen = 3; break;
                case 0x9d: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbd; tmpBuf[2] = 0x95; replacementLen = 3; break;
                case 0x9f: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbd; tmpBuf[2] = 0x97; replacementLen = 3; break;
                case 0xa8: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbd; tmpBuf[2] = 0xa0; replacementLen = 3; break;
                case 0xa9: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbd; tmpBuf[2] = 0xa1; replacementLen = 3; break;
                case 0xaa: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbd; tmpBuf[2] = 0xa2; replacementLen = 3; break;
                case 0xab: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbd; tmpBuf[2] = 0xa3; replacementLen = 3; break;
                case 0xac: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbd; tmpBuf[2] = 0xa4; replacementLen = 3; break;
                case 0xad: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbd; tmpBuf[2] = 0xa5; replacementLen = 3; break;
                case 0xae: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbd; tmpBuf[2] = 0xa6; replacementLen = 3; break;
                case 0xaf: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbd; tmpBuf[2] = 0xa7; replacementLen = 3; break;
                default:
                  break;
              }
              break;
            case 0xbe:
              switch(*(p+2))
              {
                case 0x88: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbe; tmpBuf[2] = 0x80; replacementLen = 3; break;
                case 0x89: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbe; tmpBuf[2] = 0x81; replacementLen = 3; break;
                case 0x8a: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbe; tmpBuf[2] = 0x82; replacementLen = 3; break;
                case 0x8b: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbe; tmpBuf[2] = 0x83; replacementLen = 3; break;
                case 0x8c: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbe; tmpBuf[2] = 0x84; replacementLen = 3; break;
                case 0x8d: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbe; tmpBuf[2] = 0x85; replacementLen = 3; break;
                case 0x8e: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbe; tmpBuf[2] = 0x86; replacementLen = 3; break;
                case 0x8f: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbe; tmpBuf[2] = 0x87; replacementLen = 3; break;
                case 0x98: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbe; tmpBuf[2] = 0x90; replacementLen = 3; break;
                case 0x99: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbe; tmpBuf[2] = 0x91; replacementLen = 3; break;
                case 0x9a: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbe; tmpBuf[2] = 0x92; replacementLen = 3; break;
                case 0x9b: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbe; tmpBuf[2] = 0x93; replacementLen = 3; break;
                case 0x9c: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbe; tmpBuf[2] = 0x94; replacementLen = 3; break;
                case 0x9d: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbe; tmpBuf[2] = 0x95; replacementLen = 3; break;
                case 0x9e: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbe; tmpBuf[2] = 0x96; replacementLen = 3; break;
                case 0x9f: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbe; tmpBuf[2] = 0x97; replacementLen = 3; break;
                case 0xa8: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbe; tmpBuf[2] = 0xa0; replacementLen = 3; break;
                case 0xa9: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbe; tmpBuf[2] = 0xa1; replacementLen = 3; break;
                case 0xaa: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbe; tmpBuf[2] = 0xa2; replacementLen = 3; break;
                case 0xab: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbe; tmpBuf[2] = 0xa3; replacementLen = 3; break;
                case 0xac: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbe; tmpBuf[2] = 0xa4; replacementLen = 3; break;
                case 0xad: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbe; tmpBuf[2] = 0xa5; replacementLen = 3; break;
                case 0xae: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbe; tmpBuf[2] = 0xa6; replacementLen = 3; break;
                case 0xaf: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbe; tmpBuf[2] = 0xa7; replacementLen = 3; break;
                case 0xb8: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbe; tmpBuf[2] = 0xb0; replacementLen = 3; break;
                case 0xb9: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbe; tmpBuf[2] = 0xb1; replacementLen = 3; break;
                case 0xba: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbd; tmpBuf[2] = 0xb0; replacementLen = 3; break;
                case 0xbb: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbd; tmpBuf[2] = 0xb1; replacementLen = 3; break;
                case 0xbc: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbe; tmpBuf[2] = 0xb3; replacementLen = 3; break;
                default:
                  break;
              }
              break;
            case 0xbf:
              switch(*(p+2))
              {
                case 0x88: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbd; tmpBuf[2] = 0xb2; replacementLen = 3; break;
                case 0x89: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbd; tmpBuf[2] = 0xb3; replacementLen = 3; break;
                case 0x8a: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbd; tmpBuf[2] = 0xb4; replacementLen = 3; break;
                case 0x8b: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbd; tmpBuf[2] = 0xb5; replacementLen = 3; break;
                case 0x8c: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbf; tmpBuf[2] = 0x83; replacementLen = 3; break;
                case 0x98: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbf; tmpBuf[2] = 0x90; replacementLen = 3; break;
                case 0x99: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbf; tmpBuf[2] = 0x91; replacementLen = 3; break;
                case 0x9a: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbd; tmpBuf[2] = 0xb6; replacementLen = 3; break;
                case 0x9b: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbd; tmpBuf[2] = 0xb7; replacementLen = 3; break;
                case 0xa8: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbf; tmpBuf[2] = 0xa0; replacementLen = 3; break;
                case 0xa9: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbf; tmpBuf[2] = 0xa1; replacementLen = 3; break;
                case 0xaa: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbd; tmpBuf[2] = 0xba; replacementLen = 3; break;
                case 0xab: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbd; tmpBuf[2] = 0xbb; replacementLen = 3; break;
                case 0xac: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbf; tmpBuf[2] = 0xa5; replacementLen = 3; break;
                case 0xb8: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbd; tmpBuf[2] = 0xb8; replacementLen = 3; break;
                case 0xb9: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbd; tmpBuf[2] = 0xb9; replacementLen = 3; break;
                case 0xba: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbd; tmpBuf[2] = 0xbc; replacementLen = 3; break;
                case 0xbb: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbd; tmpBuf[2] = 0xbd; replacementLen = 3; break;
                case 0xbc: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xbf; tmpBuf[2] = 0xb3; replacementLen = 3; break;
                default:
                  break;
              }
              break;
            default:
              break;
          }
          break;
        case 0xe2:
          switch(*(p+1))
          {
            case 0x84:
              switch(*(p+2))
              {
                case 0xb2: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x85; tmpBuf[2] = 0x8e; replacementLen = 3; break;
                default:
                  break;
              }
              break;
            case 0x85:
              switch(*(p+2))
              {
                case 0xa0: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x85; tmpBuf[2] = 0xb0; replacementLen = 3; break;
                case 0xa1: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x85; tmpBuf[2] = 0xb1; replacementLen = 3; break;
                case 0xa2: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x85; tmpBuf[2] = 0xb2; replacementLen = 3; break;
                case 0xa3: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x85; tmpBuf[2] = 0xb3; replacementLen = 3; break;
                case 0xa4: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x85; tmpBuf[2] = 0xb4; replacementLen = 3; break;
                case 0xa5: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x85; tmpBuf[2] = 0xb5; replacementLen = 3; break;
                case 0xa6: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x85; tmpBuf[2] = 0xb6; replacementLen = 3; break;
                case 0xa7: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x85; tmpBuf[2] = 0xb7; replacementLen = 3; break;
                case 0xa8: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x85; tmpBuf[2] = 0xb8; replacementLen = 3; break;
                case 0xa9: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x85; tmpBuf[2] = 0xb9; replacementLen = 3; break;
                case 0xaa: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x85; tmpBuf[2] = 0xba; replacementLen = 3; break;
                case 0xab: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x85; tmpBuf[2] = 0xbb; replacementLen = 3; break;
                case 0xac: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x85; tmpBuf[2] = 0xbc; replacementLen = 3; break;
                case 0xad: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x85; tmpBuf[2] = 0xbd; replacementLen = 3; break;
                case 0xae: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x85; tmpBuf[2] = 0xbe; replacementLen = 3; break;
                case 0xaf: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x85; tmpBuf[2] = 0xbf; replacementLen = 3; break;
                default:
                  break;
              }
              break;
            case 0x86:
              switch(*(p+2))
              {
                case 0x83: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x86; tmpBuf[2] = 0x84; replacementLen = 3; break;
                default:
                  break;
              }
              break;
            case 0x92:
              switch(*(p+2))
              {
                case 0xb6: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x93; tmpBuf[2] = 0x90; replacementLen = 3; break;
                case 0xb7: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x93; tmpBuf[2] = 0x91; replacementLen = 3; break;
                case 0xb8: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x93; tmpBuf[2] = 0x92; replacementLen = 3; break;
                case 0xb9: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x93; tmpBuf[2] = 0x93; replacementLen = 3; break;
                case 0xba: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x93; tmpBuf[2] = 0x94; replacementLen = 3; break;
                case 0xbb: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x93; tmpBuf[2] = 0x95; replacementLen = 3; break;
                case 0xbc: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x93; tmpBuf[2] = 0x96; replacementLen = 3; break;
                case 0xbd: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x93; tmpBuf[2] = 0x97; replacementLen = 3; break;
                case 0xbe: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x93; tmpBuf[2] = 0x98; replacementLen = 3; break;
                case 0xbf: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x93; tmpBuf[2] = 0x99; replacementLen = 3; break;
                default:
                  break;
              }
              break;
            case 0x93:
              switch(*(p+2))
              {
                case 0x80: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x93; tmpBuf[2] = 0x9a; replacementLen = 3; break;
                case 0x81: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x93; tmpBuf[2] = 0x9b; replacementLen = 3; break;
                case 0x82: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x93; tmpBuf[2] = 0x9c; replacementLen = 3; break;
                case 0x83: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x93; tmpBuf[2] = 0x9d; replacementLen = 3; break;
                case 0x84: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x93; tmpBuf[2] = 0x9e; replacementLen = 3; break;
                case 0x85: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x93; tmpBuf[2] = 0x9f; replacementLen = 3; break;
                case 0x86: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x93; tmpBuf[2] = 0xa0; replacementLen = 3; break;
                case 0x87: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x93; tmpBuf[2] = 0xa1; replacementLen = 3; break;
                case 0x88: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x93; tmpBuf[2] = 0xa2; replacementLen = 3; break;
                case 0x89: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x93; tmpBuf[2] = 0xa3; replacementLen = 3; break;
                case 0x8a: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x93; tmpBuf[2] = 0xa4; replacementLen = 3; break;
                case 0x8b: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x93; tmpBuf[2] = 0xa5; replacementLen = 3; break;
                case 0x8c: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x93; tmpBuf[2] = 0xa6; replacementLen = 3; break;
                case 0x8d: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x93; tmpBuf[2] = 0xa7; replacementLen = 3; break;
                case 0x8e: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x93; tmpBuf[2] = 0xa8; replacementLen = 3; break;
                case 0x8f: tmpBuf[0] = 0xe2; tmpBuf[1] = 0x93; tmpBuf[2] = 0xa9; replacementLen = 3; break;
                default:
                  break;
              }
              break;
            case 0xb0:
              switch(*(p+2))
              {
                case 0x80: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb0; tmpBuf[2] = 0xb0; replacementLen = 3; break;
                case 0x81: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb0; tmpBuf[2] = 0xb1; replacementLen = 3; break;
                case 0x82: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb0; tmpBuf[2] = 0xb2; replacementLen = 3; break;
                case 0x83: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb0; tmpBuf[2] = 0xb3; replacementLen = 3; break;
                case 0x84: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb0; tmpBuf[2] = 0xb4; replacementLen = 3; break;
                case 0x85: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb0; tmpBuf[2] = 0xb5; replacementLen = 3; break;
                case 0x86: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb0; tmpBuf[2] = 0xb6; replacementLen = 3; break;
                case 0x87: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb0; tmpBuf[2] = 0xb7; replacementLen = 3; break;
                case 0x88: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb0; tmpBuf[2] = 0xb8; replacementLen = 3; break;
                case 0x89: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb0; tmpBuf[2] = 0xb9; replacementLen = 3; break;
                case 0x8a: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb0; tmpBuf[2] = 0xba; replacementLen = 3; break;
                case 0x8b: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb0; tmpBuf[2] = 0xbb; replacementLen = 3; break;
                case 0x8c: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb0; tmpBuf[2] = 0xbc; replacementLen = 3; break;
                case 0x8d: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb0; tmpBuf[2] = 0xbd; replacementLen = 3; break;
                case 0x8e: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb0; tmpBuf[2] = 0xbe; replacementLen = 3; break;
                case 0x8f: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb0; tmpBuf[2] = 0xbf; replacementLen = 3; break;
                case 0x90: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb1; tmpBuf[2] = 0x80; replacementLen = 3; break;
                case 0x91: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb1; tmpBuf[2] = 0x81; replacementLen = 3; break;
                case 0x92: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb1; tmpBuf[2] = 0x82; replacementLen = 3; break;
                case 0x93: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb1; tmpBuf[2] = 0x83; replacementLen = 3; break;
                case 0x94: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb1; tmpBuf[2] = 0x84; replacementLen = 3; break;
                case 0x95: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb1; tmpBuf[2] = 0x85; replacementLen = 3; break;
                case 0x96: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb1; tmpBuf[2] = 0x86; replacementLen = 3; break;
                case 0x97: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb1; tmpBuf[2] = 0x87; replacementLen = 3; break;
                case 0x98: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb1; tmpBuf[2] = 0x88; replacementLen = 3; break;
                case 0x99: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb1; tmpBuf[2] = 0x89; replacementLen = 3; break;
                case 0x9a: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb1; tmpBuf[2] = 0x8a; replacementLen = 3; break;
                case 0x9b: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb1; tmpBuf[2] = 0x8b; replacementLen = 3; break;
                case 0x9c: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb1; tmpBuf[2] = 0x8c; replacementLen = 3; break;
                case 0x9d: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb1; tmpBuf[2] = 0x8d; replacementLen = 3; break;
                case 0x9e: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb1; tmpBuf[2] = 0x8e; replacementLen = 3; break;
                case 0x9f: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb1; tmpBuf[2] = 0x8f; replacementLen = 3; break;
                case 0xa0: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb1; tmpBuf[2] = 0x90; replacementLen = 3; break;
                case 0xa1: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb1; tmpBuf[2] = 0x91; replacementLen = 3; break;
                case 0xa2: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb1; tmpBuf[2] = 0x92; replacementLen = 3; break;
                case 0xa3: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb1; tmpBuf[2] = 0x93; replacementLen = 3; break;
                case 0xa4: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb1; tmpBuf[2] = 0x94; replacementLen = 3; break;
                case 0xa5: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb1; tmpBuf[2] = 0x95; replacementLen = 3; break;
                case 0xa6: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb1; tmpBuf[2] = 0x96; replacementLen = 3; break;
                case 0xa7: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb1; tmpBuf[2] = 0x97; replacementLen = 3; break;
                case 0xa8: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb1; tmpBuf[2] = 0x98; replacementLen = 3; break;
                case 0xa9: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb1; tmpBuf[2] = 0x99; replacementLen = 3; break;
                case 0xaa: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb1; tmpBuf[2] = 0x9a; replacementLen = 3; break;
                case 0xab: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb1; tmpBuf[2] = 0x9b; replacementLen = 3; break;
                case 0xac: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb1; tmpBuf[2] = 0x9c; replacementLen = 3; break;
                case 0xad: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb1; tmpBuf[2] = 0x9d; replacementLen = 3; break;
                case 0xae: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb1; tmpBuf[2] = 0x9e; replacementLen = 3; break;
                default:
                  break;
              }
              break;
            case 0xb1:
              switch(*(p+2))
              {
                case 0xa0: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb1; tmpBuf[2] = 0xa1; replacementLen = 3; break;
                case 0xa2: tmpBuf[0] = 0xc9; tmpBuf[1] = 0xab; replacementLen = 2; break;
                case 0xa3: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb5; tmpBuf[2] = 0xbd; replacementLen = 3; break;
                case 0xa4: tmpBuf[0] = 0xc9; tmpBuf[1] = 0xbd; replacementLen = 2; break;
                case 0xa7: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb1; tmpBuf[2] = 0xa8; replacementLen = 3; break;
                case 0xa9: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb1; tmpBuf[2] = 0xaa; replacementLen = 3; break;
                case 0xab: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb1; tmpBuf[2] = 0xac; replacementLen = 3; break;
                case 0xad: tmpBuf[0] = 0xc9; tmpBuf[1] = 0x91; replacementLen = 2; break;
                case 0xae: tmpBuf[0] = 0xc9; tmpBuf[1] = 0xb1; replacementLen = 2; break;
                case 0xaf: tmpBuf[0] = 0xc9; tmpBuf[1] = 0x90; replacementLen = 2; break;
                case 0xb0: tmpBuf[0] = 0xc9; tmpBuf[1] = 0x92; replacementLen = 2; break;
                case 0xb2: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb1; tmpBuf[2] = 0xb3; replacementLen = 3; break;
                case 0xb5: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb1; tmpBuf[2] = 0xb6; replacementLen = 3; break;
                case 0xbe: tmpBuf[0] = 0xc8; tmpBuf[1] = 0xbf; replacementLen = 2; break;
                case 0xbf: tmpBuf[0] = 0xc9; tmpBuf[1] = 0x80; replacementLen = 2; break;
                default:
                  break;
              }
              break;
            case 0xb2:
              switch(*(p+2))
              {
                case 0x80: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb2; tmpBuf[2] = 0x81; replacementLen = 3; break;
                case 0x82: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb2; tmpBuf[2] = 0x83; replacementLen = 3; break;
                case 0x84: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb2; tmpBuf[2] = 0x85; replacementLen = 3; break;
                case 0x86: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb2; tmpBuf[2] = 0x87; replacementLen = 3; break;
                case 0x88: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb2; tmpBuf[2] = 0x89; replacementLen = 3; break;
                case 0x8a: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb2; tmpBuf[2] = 0x8b; replacementLen = 3; break;
                case 0x8c: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb2; tmpBuf[2] = 0x8d; replacementLen = 3; break;
                case 0x8e: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb2; tmpBuf[2] = 0x8f; replacementLen = 3; break;
                case 0x90: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb2; tmpBuf[2] = 0x91; replacementLen = 3; break;
                case 0x92: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb2; tmpBuf[2] = 0x93; replacementLen = 3; break;
                case 0x94: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb2; tmpBuf[2] = 0x95; replacementLen = 3; break;
                case 0x96: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb2; tmpBuf[2] = 0x97; replacementLen = 3; break;
                case 0x98: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb2; tmpBuf[2] = 0x99; replacementLen = 3; break;
                case 0x9a: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb2; tmpBuf[2] = 0x9b; replacementLen = 3; break;
                case 0x9c: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb2; tmpBuf[2] = 0x9d; replacementLen = 3; break;
                case 0x9e: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb2; tmpBuf[2] = 0x9f; replacementLen = 3; break;
                case 0xa0: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb2; tmpBuf[2] = 0xa1; replacementLen = 3; break;
                case 0xa2: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb2; tmpBuf[2] = 0xa3; replacementLen = 3; break;
                case 0xa4: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb2; tmpBuf[2] = 0xa5; replacementLen = 3; break;
                case 0xa6: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb2; tmpBuf[2] = 0xa7; replacementLen = 3; break;
                case 0xa8: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb2; tmpBuf[2] = 0xa9; replacementLen = 3; break;
                case 0xaa: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb2; tmpBuf[2] = 0xab; replacementLen = 3; break;
                case 0xac: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb2; tmpBuf[2] = 0xad; replacementLen = 3; break;
                case 0xae: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb2; tmpBuf[2] = 0xaf; replacementLen = 3; break;
                case 0xb0: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb2; tmpBuf[2] = 0xb1; replacementLen = 3; break;
                case 0xb2: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb2; tmpBuf[2] = 0xb3; replacementLen = 3; break;
                case 0xb4: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb2; tmpBuf[2] = 0xb5; replacementLen = 3; break;
                case 0xb6: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb2; tmpBuf[2] = 0xb7; replacementLen = 3; break;
                case 0xb8: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb2; tmpBuf[2] = 0xb9; replacementLen = 3; break;
                case 0xba: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb2; tmpBuf[2] = 0xbb; replacementLen = 3; break;
                case 0xbc: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb2; tmpBuf[2] = 0xbd; replacementLen = 3; break;
                case 0xbe: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb2; tmpBuf[2] = 0xbf; replacementLen = 3; break;
                default:
                  break;
              }
              break;
            case 0xb3:
              switch(*(p+2))
              {
                case 0x80: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb3; tmpBuf[2] = 0x81; replacementLen = 3; break;
                case 0x82: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb3; tmpBuf[2] = 0x83; replacementLen = 3; break;
                case 0x84: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb3; tmpBuf[2] = 0x85; replacementLen = 3; break;
                case 0x86: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb3; tmpBuf[2] = 0x87; replacementLen = 3; break;
                case 0x88: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb3; tmpBuf[2] = 0x89; replacementLen = 3; break;
                case 0x8a: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb3; tmpBuf[2] = 0x8b; replacementLen = 3; break;
                case 0x8c: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb3; tmpBuf[2] = 0x8d; replacementLen = 3; break;
                case 0x8e: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb3; tmpBuf[2] = 0x8f; replacementLen = 3; break;
                case 0x90: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb3; tmpBuf[2] = 0x91; replacementLen = 3; break;
                case 0x92: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb3; tmpBuf[2] = 0x93; replacementLen = 3; break;
                case 0x94: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb3; tmpBuf[2] = 0x95; replacementLen = 3; break;
                case 0x96: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb3; tmpBuf[2] = 0x97; replacementLen = 3; break;
                case 0x98: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb3; tmpBuf[2] = 0x99; replacementLen = 3; break;
                case 0x9a: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb3; tmpBuf[2] = 0x9b; replacementLen = 3; break;
                case 0x9c: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb3; tmpBuf[2] = 0x9d; replacementLen = 3; break;
                case 0x9e: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb3; tmpBuf[2] = 0x9f; replacementLen = 3; break;
                case 0xa0: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb3; tmpBuf[2] = 0xa1; replacementLen = 3; break;
                case 0xa2: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb3; tmpBuf[2] = 0xa3; replacementLen = 3; break;
                case 0xab: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb3; tmpBuf[2] = 0xac; replacementLen = 3; break;
                case 0xad: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb3; tmpBuf[2] = 0xae; replacementLen = 3; break;
                case 0xb2: tmpBuf[0] = 0xe2; tmpBuf[1] = 0xb3; tmpBuf[2] = 0xb3; replacementLen = 3; break;
                default:
                  break;
              }
              break;
            default:
              break;
          }
          break;
        case 0xea:
          switch(*(p+1))
          {
            case 0x99:
              switch(*(p+2))
              {
                case 0x80: tmpBuf[0] = 0xea; tmpBuf[1] = 0x99; tmpBuf[2] = 0x81; replacementLen = 3; break;
                case 0x82: tmpBuf[0] = 0xea; tmpBuf[1] = 0x99; tmpBuf[2] = 0x83; replacementLen = 3; break;
                case 0x84: tmpBuf[0] = 0xea; tmpBuf[1] = 0x99; tmpBuf[2] = 0x85; replacementLen = 3; break;
                case 0x86: tmpBuf[0] = 0xea; tmpBuf[1] = 0x99; tmpBuf[2] = 0x87; replacementLen = 3; break;
                case 0x88: tmpBuf[0] = 0xea; tmpBuf[1] = 0x99; tmpBuf[2] = 0x89; replacementLen = 3; break;
                case 0x8a: tmpBuf[0] = 0xea; tmpBuf[1] = 0x99; tmpBuf[2] = 0x8b; replacementLen = 3; break;
                case 0x8c: tmpBuf[0] = 0xea; tmpBuf[1] = 0x99; tmpBuf[2] = 0x8d; replacementLen = 3; break;
                case 0x8e: tmpBuf[0] = 0xea; tmpBuf[1] = 0x99; tmpBuf[2] = 0x8f; replacementLen = 3; break;
                case 0x90: tmpBuf[0] = 0xea; tmpBuf[1] = 0x99; tmpBuf[2] = 0x91; replacementLen = 3; break;
                case 0x92: tmpBuf[0] = 0xea; tmpBuf[1] = 0x99; tmpBuf[2] = 0x93; replacementLen = 3; break;
                case 0x94: tmpBuf[0] = 0xea; tmpBuf[1] = 0x99; tmpBuf[2] = 0x95; replacementLen = 3; break;
                case 0x96: tmpBuf[0] = 0xea; tmpBuf[1] = 0x99; tmpBuf[2] = 0x97; replacementLen = 3; break;
                case 0x98: tmpBuf[0] = 0xea; tmpBuf[1] = 0x99; tmpBuf[2] = 0x99; replacementLen = 3; break;
                case 0x9a: tmpBuf[0] = 0xea; tmpBuf[1] = 0x99; tmpBuf[2] = 0x9b; replacementLen = 3; break;
                case 0x9c: tmpBuf[0] = 0xea; tmpBuf[1] = 0x99; tmpBuf[2] = 0x9d; replacementLen = 3; break;
                case 0x9e: tmpBuf[0] = 0xea; tmpBuf[1] = 0x99; tmpBuf[2] = 0x9f; replacementLen = 3; break;
                case 0xa0: tmpBuf[0] = 0xea; tmpBuf[1] = 0x99; tmpBuf[2] = 0xa1; replacementLen = 3; break;
                case 0xa2: tmpBuf[0] = 0xea; tmpBuf[1] = 0x99; tmpBuf[2] = 0xa3; replacementLen = 3; break;
                case 0xa4: tmpBuf[0] = 0xea; tmpBuf[1] = 0x99; tmpBuf[2] = 0xa5; replacementLen = 3; break;
                case 0xa6: tmpBuf[0] = 0xea; tmpBuf[1] = 0x99; tmpBuf[2] = 0xa7; replacementLen = 3; break;
                case 0xa8: tmpBuf[0] = 0xea; tmpBuf[1] = 0x99; tmpBuf[2] = 0xa9; replacementLen = 3; break;
                case 0xaa: tmpBuf[0] = 0xea; tmpBuf[1] = 0x99; tmpBuf[2] = 0xab; replacementLen = 3; break;
                case 0xac: tmpBuf[0] = 0xea; tmpBuf[1] = 0x99; tmpBuf[2] = 0xad; replacementLen = 3; break;
                default:
                  break;
              }
              break;
            case 0x9a:
              switch(*(p+2))
              {
                case 0x80: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9a; tmpBuf[2] = 0x81; replacementLen = 3; break;
                case 0x82: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9a; tmpBuf[2] = 0x83; replacementLen = 3; break;
                case 0x84: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9a; tmpBuf[2] = 0x85; replacementLen = 3; break;
                case 0x86: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9a; tmpBuf[2] = 0x87; replacementLen = 3; break;
                case 0x88: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9a; tmpBuf[2] = 0x89; replacementLen = 3; break;
                case 0x8a: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9a; tmpBuf[2] = 0x8b; replacementLen = 3; break;
                case 0x8c: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9a; tmpBuf[2] = 0x8d; replacementLen = 3; break;
                case 0x8e: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9a; tmpBuf[2] = 0x8f; replacementLen = 3; break;
                case 0x90: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9a; tmpBuf[2] = 0x91; replacementLen = 3; break;
                case 0x92: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9a; tmpBuf[2] = 0x93; replacementLen = 3; break;
                case 0x94: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9a; tmpBuf[2] = 0x95; replacementLen = 3; break;
                case 0x96: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9a; tmpBuf[2] = 0x97; replacementLen = 3; break;
                case 0x98: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9a; tmpBuf[2] = 0x99; replacementLen = 3; break;
                case 0x9a: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9a; tmpBuf[2] = 0x9b; replacementLen = 3; break;
                default:
                  break;
              }
              break;
            case 0x9c:
              switch(*(p+2))
              {
                case 0xa2: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9c; tmpBuf[2] = 0xa3; replacementLen = 3; break;
                case 0xa4: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9c; tmpBuf[2] = 0xa5; replacementLen = 3; break;
                case 0xa6: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9c; tmpBuf[2] = 0xa7; replacementLen = 3; break;
                case 0xa8: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9c; tmpBuf[2] = 0xa9; replacementLen = 3; break;
                case 0xaa: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9c; tmpBuf[2] = 0xab; replacementLen = 3; break;
                case 0xac: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9c; tmpBuf[2] = 0xad; replacementLen = 3; break;
                case 0xae: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9c; tmpBuf[2] = 0xaf; replacementLen = 3; break;
                case 0xb2: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9c; tmpBuf[2] = 0xb3; replacementLen = 3; break;
                case 0xb4: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9c; tmpBuf[2] = 0xb5; replacementLen = 3; break;
                case 0xb6: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9c; tmpBuf[2] = 0xb7; replacementLen = 3; break;
                case 0xb8: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9c; tmpBuf[2] = 0xb9; replacementLen = 3; break;
                case 0xba: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9c; tmpBuf[2] = 0xbb; replacementLen = 3; break;
                case 0xbc: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9c; tmpBuf[2] = 0xbd; replacementLen = 3; break;
                case 0xbe: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9c; tmpBuf[2] = 0xbf; replacementLen = 3; break;
                default:
                  break;
              }
              break;
            case 0x9d:
              switch(*(p+2))
              {
                case 0x80: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9d; tmpBuf[2] = 0x81; replacementLen = 3; break;
                case 0x82: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9d; tmpBuf[2] = 0x83; replacementLen = 3; break;
                case 0x84: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9d; tmpBuf[2] = 0x85; replacementLen = 3; break;
                case 0x86: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9d; tmpBuf[2] = 0x87; replacementLen = 3; break;
                case 0x88: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9d; tmpBuf[2] = 0x89; replacementLen = 3; break;
                case 0x8a: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9d; tmpBuf[2] = 0x8b; replacementLen = 3; break;
                case 0x8c: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9d; tmpBuf[2] = 0x8d; replacementLen = 3; break;
                case 0x8e: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9d; tmpBuf[2] = 0x8f; replacementLen = 3; break;
                case 0x90: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9d; tmpBuf[2] = 0x91; replacementLen = 3; break;
                case 0x92: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9d; tmpBuf[2] = 0x93; replacementLen = 3; break;
                case 0x94: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9d; tmpBuf[2] = 0x95; replacementLen = 3; break;
                case 0x96: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9d; tmpBuf[2] = 0x97; replacementLen = 3; break;
                case 0x98: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9d; tmpBuf[2] = 0x99; replacementLen = 3; break;
                case 0x9a: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9d; tmpBuf[2] = 0x9b; replacementLen = 3; break;
                case 0x9c: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9d; tmpBuf[2] = 0x9d; replacementLen = 3; break;
                case 0x9e: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9d; tmpBuf[2] = 0x9f; replacementLen = 3; break;
                case 0xa0: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9d; tmpBuf[2] = 0xa1; replacementLen = 3; break;
                case 0xa2: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9d; tmpBuf[2] = 0xa3; replacementLen = 3; break;
                case 0xa4: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9d; tmpBuf[2] = 0xa5; replacementLen = 3; break;
                case 0xa6: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9d; tmpBuf[2] = 0xa7; replacementLen = 3; break;
                case 0xa8: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9d; tmpBuf[2] = 0xa9; replacementLen = 3; break;
                case 0xaa: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9d; tmpBuf[2] = 0xab; replacementLen = 3; break;
                case 0xac: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9d; tmpBuf[2] = 0xad; replacementLen = 3; break;
                case 0xae: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9d; tmpBuf[2] = 0xaf; replacementLen = 3; break;
                case 0xb9: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9d; tmpBuf[2] = 0xba; replacementLen = 3; break;
                case 0xbb: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9d; tmpBuf[2] = 0xbc; replacementLen = 3; break;
                case 0xbd: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb5; tmpBuf[2] = 0xb9; replacementLen = 3; break;
                case 0xbe: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9d; tmpBuf[2] = 0xbf; replacementLen = 3; break;
                default:
                  break;
              }
              break;
            case 0x9e:
              switch(*(p+2))
              {
                case 0x80: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9e; tmpBuf[2] = 0x81; replacementLen = 3; break;
                case 0x82: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9e; tmpBuf[2] = 0x83; replacementLen = 3; break;
                case 0x84: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9e; tmpBuf[2] = 0x85; replacementLen = 3; break;
                case 0x86: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9e; tmpBuf[2] = 0x87; replacementLen = 3; break;
                case 0x8b: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9e; tmpBuf[2] = 0x8c; replacementLen = 3; break;
                case 0x8d: tmpBuf[0] = 0xc9; tmpBuf[1] = 0xa5; replacementLen = 2; break;
                case 0x90: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9e; tmpBuf[2] = 0x91; replacementLen = 3; break;
                case 0x92: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9e; tmpBuf[2] = 0x93; replacementLen = 3; break;
                case 0x96: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9e; tmpBuf[2] = 0x97; replacementLen = 3; break;
                case 0x98: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9e; tmpBuf[2] = 0x99; replacementLen = 3; break;
                case 0x9a: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9e; tmpBuf[2] = 0x9b; replacementLen = 3; break;
                case 0x9c: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9e; tmpBuf[2] = 0x9d; replacementLen = 3; break;
                case 0x9e: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9e; tmpBuf[2] = 0x9f; replacementLen = 3; break;
                case 0xa0: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa1; replacementLen = 3; break;
                case 0xa2: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa3; replacementLen = 3; break;
                case 0xa4: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa5; replacementLen = 3; break;
                case 0xa6: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa7; replacementLen = 3; break;
                case 0xa8: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa9; replacementLen = 3; break;
                case 0xaa: tmpBuf[0] = 0xc9; tmpBuf[1] = 0xa6; replacementLen = 2; break;
                case 0xab: tmpBuf[0] = 0xc9; tmpBuf[1] = 0x9c; replacementLen = 2; break;
                case 0xac: tmpBuf[0] = 0xc9; tmpBuf[1] = 0xa1; replacementLen = 2; break;
                case 0xad: tmpBuf[0] = 0xc9; tmpBuf[1] = 0xac; replacementLen = 2; break;
                case 0xae: tmpBuf[0] = 0xc9; tmpBuf[1] = 0xaa; replacementLen = 2; break;
                case 0xb0: tmpBuf[0] = 0xca; tmpBuf[1] = 0x9e; replacementLen = 2; break;
                case 0xb1: tmpBuf[0] = 0xca; tmpBuf[1] = 0x87; replacementLen = 2; break;
                case 0xb2: tmpBuf[0] = 0xca; tmpBuf[1] = 0x9d; replacementLen = 2; break;
                case 0xb3: tmpBuf[0] = 0xea; tmpBuf[1] = 0xad; tmpBuf[2] = 0x93; replacementLen = 3; break;
                case 0xb4: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xb5; replacementLen = 3; break;
                case 0xb6: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xb7; replacementLen = 3; break;
                case 0xb8: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xb9; replacementLen = 3; break;
                case 0xba: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xbb; replacementLen = 3; break;
                case 0xbc: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xbd; replacementLen = 3; break;
                case 0xbe: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xbf; replacementLen = 3; break;
                default:
                  break;
              }
              break;
            case 0x9f:
              switch(*(p+2))
              {
                case 0x82: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9f; tmpBuf[2] = 0x83; replacementLen = 3; break;
                case 0x84: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9e; tmpBuf[2] = 0x94; replacementLen = 3; break;
                case 0x85: tmpBuf[0] = 0xca; tmpBuf[1] = 0x82; replacementLen = 2; break;
                case 0x86: tmpBuf[0] = 0xe1; tmpBuf[1] = 0xb6; tmpBuf[2] = 0x8e; replacementLen = 3; break;
                case 0x87: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9f; tmpBuf[2] = 0x88; replacementLen = 3; break;
                case 0x89: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9f; tmpBuf[2] = 0x8a; replacementLen = 3; break;
                case 0xb5: tmpBuf[0] = 0xea; tmpBuf[1] = 0x9f; tmpBuf[2] = 0xb6; replacementLen = 3; break;
                default:
                  break;
              }
              break;
            default:
              break;
          }
          break;
        case 0xef:
          switch(*(p+1))
          {
            case 0xbc:
              switch(*(p+2))
              {
                case 0xa1: tmpBuf[0] = 0xef; tmpBuf[1] = 0xbd; tmpBuf[2] = 0x81; replacementLen = 3; break;
                case 0xa2: tmpBuf[0] = 0xef; tmpBuf[1] = 0xbd; tmpBuf[2] = 0x82; replacementLen = 3; break;
                case 0xa3: tmpBuf[0] = 0xef; tmpBuf[1] = 0xbd; tmpBuf[2] = 0x83; replacementLen = 3; break;
                case 0xa4: tmpBuf[0] = 0xef; tmpBuf[1] = 0xbd; tmpBuf[2] = 0x84; replacementLen = 3; break;
                case 0xa5: tmpBuf[0] = 0xef; tmpBuf[1] = 0xbd; tmpBuf[2] = 0x85; replacementLen = 3; break;
                case 0xa6: tmpBuf[0] = 0xef; tmpBuf[1] = 0xbd; tmpBuf[2] = 0x86; replacementLen = 3; break;
                case 0xa7: tmpBuf[0] = 0xef; tmpBuf[1] = 0xbd; tmpBuf[2] = 0x87; replacementLen = 3; break;
                case 0xa8: tmpBuf[0] = 0xef; tmpBuf[1] = 0xbd; tmpBuf[2] = 0x88; replacementLen = 3; break;
                case 0xa9: tmpBuf[0] = 0xef; tmpBuf[1] = 0xbd; tmpBuf[2] = 0x89; replacementLen = 3; break;
                case 0xaa: tmpBuf[0] = 0xef; tmpBuf[1] = 0xbd; tmpBuf[2] = 0x8a; replacementLen = 3; break;
                case 0xab: tmpBuf[0] = 0xef; tmpBuf[1] = 0xbd; tmpBuf[2] = 0x8b; replacementLen = 3; break;
                case 0xac: tmpBuf[0] = 0xef; tmpBuf[1] = 0xbd; tmpBuf[2] = 0x8c; replacementLen = 3; break;
                case 0xad: tmpBuf[0] = 0xef; tmpBuf[1] = 0xbd; tmpBuf[2] = 0x8d; replacementLen = 3; break;
                case 0xae: tmpBuf[0] = 0xef; tmpBuf[1] = 0xbd; tmpBuf[2] = 0x8e; replacementLen = 3; break;
                case 0xaf: tmpBuf[0] = 0xef; tmpBuf[1] = 0xbd; tmpBuf[2] = 0x8f; replacementLen = 3; break;
                case 0xb0: tmpBuf[0] = 0xef; tmpBuf[1] = 0xbd; tmpBuf[2] = 0x90; replacementLen = 3; break;
                case 0xb1: tmpBuf[0] = 0xef; tmpBuf[1] = 0xbd; tmpBuf[2] = 0x91; replacementLen = 3; break;
                case 0xb2: tmpBuf[0] = 0xef; tmpBuf[1] = 0xbd; tmpBuf[2] = 0x92; replacementLen = 3; break;
                case 0xb3: tmpBuf[0] = 0xef; tmpBuf[1] = 0xbd; tmpBuf[2] = 0x93; replacementLen = 3; break;
                case 0xb4: tmpBuf[0] = 0xef; tmpBuf[1] = 0xbd; tmpBuf[2] = 0x94; replacementLen = 3; break;
                case 0xb5: tmpBuf[0] = 0xef; tmpBuf[1] = 0xbd; tmpBuf[2] = 0x95; replacementLen = 3; break;
                case 0xb6: tmpBuf[0] = 0xef; tmpBuf[1] = 0xbd; tmpBuf[2] = 0x96; replacementLen = 3; break;
                case 0xb7: tmpBuf[0] = 0xef; tmpBuf[1] = 0xbd; tmpBuf[2] = 0x97; replacementLen = 3; break;
                case 0xb8: tmpBuf[0] = 0xef; tmpBuf[1] = 0xbd; tmpBuf[2] = 0x98; replacementLen = 3; break;
                case 0xb9: tmpBuf[0] = 0xef; tmpBuf[1] = 0xbd; tmpBuf[2] = 0x99; replacementLen = 3; break;
                case 0xba: tmpBuf[0] = 0xef; tmpBuf[1] = 0xbd; tmpBuf[2] = 0x9a; replacementLen = 3; break;
                default:
                  break;
              }
              break;
            default:
              break;
          }
          break;
        case 0xf0:
          switch(*(p+1))
          {
            case 0x90:
              switch(*(p+2))
              {
                case 0x90:
                  switch(*(p+3))
                  {
                    case 0x80: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x90; tmpBuf[3] = 0xa8; replacementLen = 4; break;
                    case 0x81: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x90; tmpBuf[3] = 0xa9; replacementLen = 4; break;
                    case 0x82: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x90; tmpBuf[3] = 0xaa; replacementLen = 4; break;
                    case 0x83: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x90; tmpBuf[3] = 0xab; replacementLen = 4; break;
                    case 0x84: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x90; tmpBuf[3] = 0xac; replacementLen = 4; break;
                    case 0x85: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x90; tmpBuf[3] = 0xad; replacementLen = 4; break;
                    case 0x86: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x90; tmpBuf[3] = 0xae; replacementLen = 4; break;
                    case 0x87: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x90; tmpBuf[3] = 0xaf; replacementLen = 4; break;
                    case 0x88: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x90; tmpBuf[3] = 0xb0; replacementLen = 4; break;
                    case 0x89: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x90; tmpBuf[3] = 0xb1; replacementLen = 4; break;
                    case 0x8a: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x90; tmpBuf[3] = 0xb2; replacementLen = 4; break;
                    case 0x8b: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x90; tmpBuf[3] = 0xb3; replacementLen = 4; break;
                    case 0x8c: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x90; tmpBuf[3] = 0xb4; replacementLen = 4; break;
                    case 0x8d: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x90; tmpBuf[3] = 0xb5; replacementLen = 4; break;
                    case 0x8e: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x90; tmpBuf[3] = 0xb6; replacementLen = 4; break;
                    case 0x8f: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x90; tmpBuf[3] = 0xb7; replacementLen = 4; break;
                    case 0x90: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x90; tmpBuf[3] = 0xb8; replacementLen = 4; break;
                    case 0x91: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x90; tmpBuf[3] = 0xb9; replacementLen = 4; break;
                    case 0x92: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x90; tmpBuf[3] = 0xba; replacementLen = 4; break;
                    case 0x93: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x90; tmpBuf[3] = 0xbb; replacementLen = 4; break;
                    case 0x94: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x90; tmpBuf[3] = 0xbc; replacementLen = 4; break;
                    case 0x95: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x90; tmpBuf[3] = 0xbd; replacementLen = 4; break;
                    case 0x96: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x90; tmpBuf[3] = 0xbe; replacementLen = 4; break;
                    case 0x97: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x90; tmpBuf[3] = 0xbf; replacementLen = 4; break;
                    case 0x98: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x91; tmpBuf[3] = 0x80; replacementLen = 4; break;
                    case 0x99: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x91; tmpBuf[3] = 0x81; replacementLen = 4; break;
                    case 0x9a: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x91; tmpBuf[3] = 0x82; replacementLen = 4; break;
                    case 0x9b: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x91; tmpBuf[3] = 0x83; replacementLen = 4; break;
                    case 0x9c: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x91; tmpBuf[3] = 0x84; replacementLen = 4; break;
                    case 0x9d: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x91; tmpBuf[3] = 0x85; replacementLen = 4; break;
                    case 0x9e: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x91; tmpBuf[3] = 0x86; replacementLen = 4; break;
                    case 0x9f: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x91; tmpBuf[3] = 0x87; replacementLen = 4; break;
                    case 0xa0: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x91; tmpBuf[3] = 0x88; replacementLen = 4; break;
                    case 0xa1: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x91; tmpBuf[3] = 0x89; replacementLen = 4; break;
                    case 0xa2: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x91; tmpBuf[3] = 0x8a; replacementLen = 4; break;
                    case 0xa3: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x91; tmpBuf[3] = 0x8b; replacementLen = 4; break;
                    case 0xa4: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x91; tmpBuf[3] = 0x8c; replacementLen = 4; break;
                    case 0xa5: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x91; tmpBuf[3] = 0x8d; replacementLen = 4; break;
                    case 0xa6: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x91; tmpBuf[3] = 0x8e; replacementLen = 4; break;
                    case 0xa7: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x91; tmpBuf[3] = 0x8f; replacementLen = 4; break;
                    default:
                      break;
                  }
                  break;
                case 0x92:
                  switch(*(p+3))
                  {
                    case 0xb0: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x93; tmpBuf[3] = 0x98; replacementLen = 4; break;
                    case 0xb1: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x93; tmpBuf[3] = 0x99; replacementLen = 4; break;
                    case 0xb2: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x93; tmpBuf[3] = 0x9a; replacementLen = 4; break;
                    case 0xb3: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x93; tmpBuf[3] = 0x9b; replacementLen = 4; break;
                    case 0xb4: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x93; tmpBuf[3] = 0x9c; replacementLen = 4; break;
                    case 0xb5: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x93; tmpBuf[3] = 0x9d; replacementLen = 4; break;
                    case 0xb6: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x93; tmpBuf[3] = 0x9e; replacementLen = 4; break;
                    case 0xb7: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x93; tmpBuf[3] = 0x9f; replacementLen = 4; break;
                    case 0xb8: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x93; tmpBuf[3] = 0xa0; replacementLen = 4; break;
                    case 0xb9: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x93; tmpBuf[3] = 0xa1; replacementLen = 4; break;
                    case 0xba: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x93; tmpBuf[3] = 0xa2; replacementLen = 4; break;
                    case 0xbb: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x93; tmpBuf[3] = 0xa3; replacementLen = 4; break;
                    case 0xbc: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x93; tmpBuf[3] = 0xa4; replacementLen = 4; break;
                    case 0xbd: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x93; tmpBuf[3] = 0xa5; replacementLen = 4; break;
                    case 0xbe: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x93; tmpBuf[3] = 0xa6; replacementLen = 4; break;
                    case 0xbf: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x93; tmpBuf[3] = 0xa7; replacementLen = 4; break;
                    default:
                      break;
                  }
                  break;
                case 0x93:
                  switch(*(p+3))
                  {
                    case 0x80: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x93; tmpBuf[3] = 0xa8; replacementLen = 4; break;
                    case 0x81: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x93; tmpBuf[3] = 0xa9; replacementLen = 4; break;
                    case 0x82: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x93; tmpBuf[3] = 0xaa; replacementLen = 4; break;
                    case 0x83: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x93; tmpBuf[3] = 0xab; replacementLen = 4; break;
                    case 0x84: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x93; tmpBuf[3] = 0xac; replacementLen = 4; break;
                    case 0x85: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x93; tmpBuf[3] = 0xad; replacementLen = 4; break;
                    case 0x86: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x93; tmpBuf[3] = 0xae; replacementLen = 4; break;
                    case 0x87: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x93; tmpBuf[3] = 0xaf; replacementLen = 4; break;
                    case 0x88: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x93; tmpBuf[3] = 0xb0; replacementLen = 4; break;
                    case 0x89: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x93; tmpBuf[3] = 0xb1; replacementLen = 4; break;
                    case 0x8a: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x93; tmpBuf[3] = 0xb2; replacementLen = 4; break;
                    case 0x8b: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x93; tmpBuf[3] = 0xb3; replacementLen = 4; break;
                    case 0x8c: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x93; tmpBuf[3] = 0xb4; replacementLen = 4; break;
                    case 0x8d: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x93; tmpBuf[3] = 0xb5; replacementLen = 4; break;
                    case 0x8e: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x93; tmpBuf[3] = 0xb6; replacementLen = 4; break;
                    case 0x8f: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x93; tmpBuf[3] = 0xb7; replacementLen = 4; break;
                    case 0x90: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x93; tmpBuf[3] = 0xb8; replacementLen = 4; break;
                    case 0x91: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x93; tmpBuf[3] = 0xb9; replacementLen = 4; break;
                    case 0x92: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x93; tmpBuf[3] = 0xba; replacementLen = 4; break;
                    case 0x93: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0x93; tmpBuf[3] = 0xbb; replacementLen = 4; break;
                    default:
                      break;
                  }
                  break;
                case 0xb2:
                  switch(*(p+3))
                  {
                    case 0x80: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb3; tmpBuf[3] = 0x80; replacementLen = 4; break;
                    case 0x81: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb3; tmpBuf[3] = 0x81; replacementLen = 4; break;
                    case 0x82: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb3; tmpBuf[3] = 0x82; replacementLen = 4; break;
                    case 0x83: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb3; tmpBuf[3] = 0x83; replacementLen = 4; break;
                    case 0x84: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb3; tmpBuf[3] = 0x84; replacementLen = 4; break;
                    case 0x85: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb3; tmpBuf[3] = 0x85; replacementLen = 4; break;
                    case 0x86: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb3; tmpBuf[3] = 0x86; replacementLen = 4; break;
                    case 0x87: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb3; tmpBuf[3] = 0x87; replacementLen = 4; break;
                    case 0x88: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb3; tmpBuf[3] = 0x88; replacementLen = 4; break;
                    case 0x89: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb3; tmpBuf[3] = 0x89; replacementLen = 4; break;
                    case 0x8a: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb3; tmpBuf[3] = 0x8a; replacementLen = 4; break;
                    case 0x8b: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb3; tmpBuf[3] = 0x8b; replacementLen = 4; break;
                    case 0x8c: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb3; tmpBuf[3] = 0x8c; replacementLen = 4; break;
                    case 0x8d: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb3; tmpBuf[3] = 0x8d; replacementLen = 4; break;
                    case 0x8e: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb3; tmpBuf[3] = 0x8e; replacementLen = 4; break;
                    case 0x8f: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb3; tmpBuf[3] = 0x8f; replacementLen = 4; break;
                    case 0x90: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb3; tmpBuf[3] = 0x90; replacementLen = 4; break;
                    case 0x91: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb3; tmpBuf[3] = 0x91; replacementLen = 4; break;
                    case 0x92: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb3; tmpBuf[3] = 0x92; replacementLen = 4; break;
                    case 0x93: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb3; tmpBuf[3] = 0x93; replacementLen = 4; break;
                    case 0x94: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb3; tmpBuf[3] = 0x94; replacementLen = 4; break;
                    case 0x95: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb3; tmpBuf[3] = 0x95; replacementLen = 4; break;
                    case 0x96: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb3; tmpBuf[3] = 0x96; replacementLen = 4; break;
                    case 0x97: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb3; tmpBuf[3] = 0x97; replacementLen = 4; break;
                    case 0x98: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb3; tmpBuf[3] = 0x98; replacementLen = 4; break;
                    case 0x99: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb3; tmpBuf[3] = 0x99; replacementLen = 4; break;
                    case 0x9a: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb3; tmpBuf[3] = 0x9a; replacementLen = 4; break;
                    case 0x9b: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb3; tmpBuf[3] = 0x9b; replacementLen = 4; break;
                    case 0x9c: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb3; tmpBuf[3] = 0x9c; replacementLen = 4; break;
                    case 0x9d: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb3; tmpBuf[3] = 0x9d; replacementLen = 4; break;
                    case 0x9e: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb3; tmpBuf[3] = 0x9e; replacementLen = 4; break;
                    case 0x9f: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb3; tmpBuf[3] = 0x9f; replacementLen = 4; break;
                    case 0xa0: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb3; tmpBuf[3] = 0xa0; replacementLen = 4; break;
                    case 0xa1: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb3; tmpBuf[3] = 0xa1; replacementLen = 4; break;
                    case 0xa2: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb3; tmpBuf[3] = 0xa2; replacementLen = 4; break;
                    case 0xa3: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb3; tmpBuf[3] = 0xa3; replacementLen = 4; break;
                    case 0xa4: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb3; tmpBuf[3] = 0xa4; replacementLen = 4; break;
                    case 0xa5: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb3; tmpBuf[3] = 0xa5; replacementLen = 4; break;
                    case 0xa6: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb3; tmpBuf[3] = 0xa6; replacementLen = 4; break;
                    case 0xa7: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb3; tmpBuf[3] = 0xa7; replacementLen = 4; break;
                    case 0xa8: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb3; tmpBuf[3] = 0xa8; replacementLen = 4; break;
                    case 0xa9: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb3; tmpBuf[3] = 0xa9; replacementLen = 4; break;
                    case 0xaa: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb3; tmpBuf[3] = 0xaa; replacementLen = 4; break;
                    case 0xab: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb3; tmpBuf[3] = 0xab; replacementLen = 4; break;
                    case 0xac: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb3; tmpBuf[3] = 0xac; replacementLen = 4; break;
                    case 0xad: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb3; tmpBuf[3] = 0xad; replacementLen = 4; break;
                    case 0xae: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb3; tmpBuf[3] = 0xae; replacementLen = 4; break;
                    case 0xaf: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb3; tmpBuf[3] = 0xaf; replacementLen = 4; break;
                    case 0xb0: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb3; tmpBuf[3] = 0xb0; replacementLen = 4; break;
                    case 0xb1: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb3; tmpBuf[3] = 0xb1; replacementLen = 4; break;
                    case 0xb2: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x90; tmpBuf[2] = 0xb3; tmpBuf[3] = 0xb2; replacementLen = 4; break;
                    default:
                      break;
                  }
                  break;
                default:
                  break;
              }
              break;
            case 0x91:
              switch(*(p+2))
              {
                case 0xa2:
                  switch(*(p+3))
                  {
                    case 0xa0: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x91; tmpBuf[2] = 0xa3; tmpBuf[3] = 0x80; replacementLen = 4; break;
                    case 0xa1: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x91; tmpBuf[2] = 0xa3; tmpBuf[3] = 0x81; replacementLen = 4; break;
                    case 0xa2: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x91; tmpBuf[2] = 0xa3; tmpBuf[3] = 0x82; replacementLen = 4; break;
                    case 0xa3: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x91; tmpBuf[2] = 0xa3; tmpBuf[3] = 0x83; replacementLen = 4; break;
                    case 0xa4: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x91; tmpBuf[2] = 0xa3; tmpBuf[3] = 0x84; replacementLen = 4; break;
                    case 0xa5: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x91; tmpBuf[2] = 0xa3; tmpBuf[3] = 0x85; replacementLen = 4; break;
                    case 0xa6: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x91; tmpBuf[2] = 0xa3; tmpBuf[3] = 0x86; replacementLen = 4; break;
                    case 0xa7: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x91; tmpBuf[2] = 0xa3; tmpBuf[3] = 0x87; replacementLen = 4; break;
                    case 0xa8: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x91; tmpBuf[2] = 0xa3; tmpBuf[3] = 0x88; replacementLen = 4; break;
                    case 0xa9: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x91; tmpBuf[2] = 0xa3; tmpBuf[3] = 0x89; replacementLen = 4; break;
                    case 0xaa: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x91; tmpBuf[2] = 0xa3; tmpBuf[3] = 0x8a; replacementLen = 4; break;
                    case 0xab: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x91; tmpBuf[2] = 0xa3; tmpBuf[3] = 0x8b; replacementLen = 4; break;
                    case 0xac: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x91; tmpBuf[2] = 0xa3; tmpBuf[3] = 0x8c; replacementLen = 4; break;
                    case 0xad: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x91; tmpBuf[2] = 0xa3; tmpBuf[3] = 0x8d; replacementLen = 4; break;
                    case 0xae: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x91; tmpBuf[2] = 0xa3; tmpBuf[3] = 0x8e; replacementLen = 4; break;
                    case 0xaf: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x91; tmpBuf[2] = 0xa3; tmpBuf[3] = 0x8f; replacementLen = 4; break;
                    case 0xb0: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x91; tmpBuf[2] = 0xa3; tmpBuf[3] = 0x90; replacementLen = 4; break;
                    case 0xb1: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x91; tmpBuf[2] = 0xa3; tmpBuf[3] = 0x91; replacementLen = 4; break;
                    case 0xb2: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x91; tmpBuf[2] = 0xa3; tmpBuf[3] = 0x92; replacementLen = 4; break;
                    case 0xb3: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x91; tmpBuf[2] = 0xa3; tmpBuf[3] = 0x93; replacementLen = 4; break;
                    case 0xb4: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x91; tmpBuf[2] = 0xa3; tmpBuf[3] = 0x94; replacementLen = 4; break;
                    case 0xb5: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x91; tmpBuf[2] = 0xa3; tmpBuf[3] = 0x95; replacementLen = 4; break;
                    case 0xb6: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x91; tmpBuf[2] = 0xa3; tmpBuf[3] = 0x96; replacementLen = 4; break;
                    case 0xb7: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x91; tmpBuf[2] = 0xa3; tmpBuf[3] = 0x97; replacementLen = 4; break;
                    case 0xb8: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x91; tmpBuf[2] = 0xa3; tmpBuf[3] = 0x98; replacementLen = 4; break;
                    case 0xb9: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x91; tmpBuf[2] = 0xa3; tmpBuf[3] = 0x99; replacementLen = 4; break;
                    case 0xba: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x91; tmpBuf[2] = 0xa3; tmpBuf[3] = 0x9a; replacementLen = 4; break;
                    case 0xbb: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x91; tmpBuf[2] = 0xa3; tmpBuf[3] = 0x9b; replacementLen = 4; break;
                    case 0xbc: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x91; tmpBuf[2] = 0xa3; tmpBuf[3] = 0x9c; replacementLen = 4; break;
                    case 0xbd: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x91; tmpBuf[2] = 0xa3; tmpBuf[3] = 0x9d; replacementLen = 4; break;
                    case 0xbe: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x91; tmpBuf[2] = 0xa3; tmpBuf[3] = 0x9e; replacementLen = 4; break;
                    case 0xbf: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x91; tmpBuf[2] = 0xa3; tmpBuf[3] = 0x9f; replacementLen = 4; break;
                    default:
                      break;
                  }
                  break;
                default:
                  break;
              }
              break;
            case 0x96:
              switch(*(p+2))
              {
                case 0xb9:
                  switch(*(p+3))
                  {
                    case 0x80: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x96; tmpBuf[2] = 0xb9; tmpBuf[3] = 0xa0; replacementLen = 4; break;
                    case 0x81: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x96; tmpBuf[2] = 0xb9; tmpBuf[3] = 0xa1; replacementLen = 4; break;
                    case 0x82: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x96; tmpBuf[2] = 0xb9; tmpBuf[3] = 0xa2; replacementLen = 4; break;
                    case 0x83: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x96; tmpBuf[2] = 0xb9; tmpBuf[3] = 0xa3; replacementLen = 4; break;
                    case 0x84: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x96; tmpBuf[2] = 0xb9; tmpBuf[3] = 0xa4; replacementLen = 4; break;
                    case 0x85: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x96; tmpBuf[2] = 0xb9; tmpBuf[3] = 0xa5; replacementLen = 4; break;
                    case 0x86: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x96; tmpBuf[2] = 0xb9; tmpBuf[3] = 0xa6; replacementLen = 4; break;
                    case 0x87: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x96; tmpBuf[2] = 0xb9; tmpBuf[3] = 0xa7; replacementLen = 4; break;
                    case 0x88: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x96; tmpBuf[2] = 0xb9; tmpBuf[3] = 0xa8; replacementLen = 4; break;
                    case 0x89: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x96; tmpBuf[2] = 0xb9; tmpBuf[3] = 0xa9; replacementLen = 4; break;
                    case 0x8a: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x96; tmpBuf[2] = 0xb9; tmpBuf[3] = 0xaa; replacementLen = 4; break;
                    case 0x8b: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x96; tmpBuf[2] = 0xb9; tmpBuf[3] = 0xab; replacementLen = 4; break;
                    case 0x8c: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x96; tmpBuf[2] = 0xb9; tmpBuf[3] = 0xac; replacementLen = 4; break;
                    case 0x8d: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x96; tmpBuf[2] = 0xb9; tmpBuf[3] = 0xad; replacementLen = 4; break;
                    case 0x8e: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x96; tmpBuf[2] = 0xb9; tmpBuf[3] = 0xae; replacementLen = 4; break;
                    case 0x8f: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x96; tmpBuf[2] = 0xb9; tmpBuf[3] = 0xaf; replacementLen = 4; break;
                    case 0x90: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x96; tmpBuf[2] = 0xb9; tmpBuf[3] = 0xb0; replacementLen = 4; break;
                    case 0x91: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x96; tmpBuf[2] = 0xb9; tmpBuf[3] = 0xb1; replacementLen = 4; break;
                    case 0x92: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x96; tmpBuf[2] = 0xb9; tmpBuf[3] = 0xb2; replacementLen = 4; break;
                    case 0x93: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x96; tmpBuf[2] = 0xb9; tmpBuf[3] = 0xb3; replacementLen = 4; break;
                    case 0x94: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x96; tmpBuf[2] = 0xb9; tmpBuf[3] = 0xb4; replacementLen = 4; break;
                    case 0x95: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x96; tmpBuf[2] = 0xb9; tmpBuf[3] = 0xb5; replacementLen = 4; break;
                    case 0x96: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x96; tmpBuf[2] = 0xb9; tmpBuf[3] = 0xb6; replacementLen = 4; break;
                    case 0x97: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x96; tmpBuf[2] = 0xb9; tmpBuf[3] = 0xb7; replacementLen = 4; break;
                    case 0x98: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x96; tmpBuf[2] = 0xb9; tmpBuf[3] = 0xb8; replacementLen = 4; break;
                    case 0x99: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x96; tmpBuf[2] = 0xb9; tmpBuf[3] = 0xb9; replacementLen = 4; break;
                    case 0x9a: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x96; tmpBuf[2] = 0xb9; tmpBuf[3] = 0xba; replacementLen = 4; break;
                    case 0x9b: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x96; tmpBuf[2] = 0xb9; tmpBuf[3] = 0xbb; replacementLen = 4; break;
                    case 0x9c: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x96; tmpBuf[2] = 0xb9; tmpBuf[3] = 0xbc; replacementLen = 4; break;
                    case 0x9d: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x96; tmpBuf[2] = 0xb9; tmpBuf[3] = 0xbd; replacementLen = 4; break;
                    case 0x9e: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x96; tmpBuf[2] = 0xb9; tmpBuf[3] = 0xbe; replacementLen = 4; break;
                    case 0x9f: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x96; tmpBuf[2] = 0xb9; tmpBuf[3] = 0xbf; replacementLen = 4; break;
                    default:
                      break;
                  }
                  break;
                default:
                  break;
              }
              break;
            case 0x9e:
              switch(*(p+2))
              {
                case 0xa4:
                  switch(*(p+3))
                  {
                    case 0x80: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa4; tmpBuf[3] = 0xa2; replacementLen = 4; break;
                    case 0x81: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa4; tmpBuf[3] = 0xa3; replacementLen = 4; break;
                    case 0x82: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa4; tmpBuf[3] = 0xa4; replacementLen = 4; break;
                    case 0x83: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa4; tmpBuf[3] = 0xa5; replacementLen = 4; break;
                    case 0x84: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa4; tmpBuf[3] = 0xa6; replacementLen = 4; break;
                    case 0x85: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa4; tmpBuf[3] = 0xa7; replacementLen = 4; break;
                    case 0x86: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa4; tmpBuf[3] = 0xa8; replacementLen = 4; break;
                    case 0x87: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa4; tmpBuf[3] = 0xa9; replacementLen = 4; break;
                    case 0x88: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa4; tmpBuf[3] = 0xaa; replacementLen = 4; break;
                    case 0x89: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa4; tmpBuf[3] = 0xab; replacementLen = 4; break;
                    case 0x8a: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa4; tmpBuf[3] = 0xac; replacementLen = 4; break;
                    case 0x8b: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa4; tmpBuf[3] = 0xad; replacementLen = 4; break;
                    case 0x8c: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa4; tmpBuf[3] = 0xae; replacementLen = 4; break;
                    case 0x8d: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa4; tmpBuf[3] = 0xaf; replacementLen = 4; break;
                    case 0x8e: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa4; tmpBuf[3] = 0xb0; replacementLen = 4; break;
                    case 0x8f: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa4; tmpBuf[3] = 0xb1; replacementLen = 4; break;
                    case 0x90: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa4; tmpBuf[3] = 0xb2; replacementLen = 4; break;
                    case 0x91: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa4; tmpBuf[3] = 0xb3; replacementLen = 4; break;
                    case 0x92: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa4; tmpBuf[3] = 0xb4; replacementLen = 4; break;
                    case 0x93: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa4; tmpBuf[3] = 0xb5; replacementLen = 4; break;
                    case 0x94: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa4; tmpBuf[3] = 0xb6; replacementLen = 4; break;
                    case 0x95: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa4; tmpBuf[3] = 0xb7; replacementLen = 4; break;
                    case 0x96: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa4; tmpBuf[3] = 0xb8; replacementLen = 4; break;
                    case 0x97: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa4; tmpBuf[3] = 0xb9; replacementLen = 4; break;
                    case 0x98: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa4; tmpBuf[3] = 0xba; replacementLen = 4; break;
                    case 0x99: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa4; tmpBuf[3] = 0xbb; replacementLen = 4; break;
                    case 0x9a: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa4; tmpBuf[3] = 0xbc; replacementLen = 4; break;
                    case 0x9b: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa4; tmpBuf[3] = 0xbd; replacementLen = 4; break;
                    case 0x9c: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa4; tmpBuf[3] = 0xbe; replacementLen = 4; break;
                    case 0x9d: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa4; tmpBuf[3] = 0xbf; replacementLen = 4; break;
                    case 0x9e: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa5; tmpBuf[3] = 0x80; replacementLen = 4; break;
                    case 0x9f: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa5; tmpBuf[3] = 0x81; replacementLen = 4; break;
                    case 0xa0: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa5; tmpBuf[3] = 0x82; replacementLen = 4; break;
                    case 0xa1: tmpBuf[0] = 0xf0; tmpBuf[1] = 0x9e; tmpBuf[2] = 0xa5; tmpBuf[3] = 0x83; replacementLen = 4; break;
                    default:
                      break;
                  }
                  break;
                default:
                  break;
              }
              break;
            default:
              break;
          }
          break;
        default:
          break;
      }
      int incr = 1;
      if ((*p&0xE0)==0xC0)
      {
        incr=2; // 110x.xxxx: >=2 byte character
      }
      if ((*p&0xF0)==0xE0)
      {
        incr=3; // 1110.xxxx: >=3 byte character
      }
      if ((*p&0xF8)==0xF0)
      {
        incr=4; // 1111.0xxx: >=4 byte character
      }
      if (replacementLen == 0)
      {
        for (int i = 0; i < incr; i++)
        {
          tmpBuf[i] = *(p+i);
        }
        replacementLen = incr;
      }
      p += incr;
      if (cb->curBufLen < curLen + replacementLen)
      {
        cb->outBuf = (unsigned char *)realloc(cb->outBuf,cb->curBufLen + 41);
        cb->curBufLen = cb->curBufLen + 40;
      }
      for (size_t i = 0; i < replacementLen; i++)
      {
        cb->outBuf[curLen + i] = tmpBuf[i];
      }
      curLen += replacementLen;
      cb->outBuf[curLen] = 0;
    }
  }
  return curLen;
}
