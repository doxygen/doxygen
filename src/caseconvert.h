struct CaseBuffer
{
  size_t curBufLen = 0;
  unsigned char *outBuf = NULL;
};


static CaseBuffer caseBuffer;

int StrToUprExt(unsigned char* pString, CaseBuffer *cb);
int StrToLwrExt(unsigned char* pString, CaseBuffer *cb);
