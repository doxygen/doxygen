#ifndef TAGREADER_H
#define TAGREADER_H

#include "qtbc.h"

class Entry;

void parseTagFile(Entry *root,const char *fullPathName,const char *fileName);

#endif
