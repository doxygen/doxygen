#ifndef MARSHAL_H
#define MARSHAL_H

#include <qlist.h>
#include <qfile.h>
#include "sortdict.h"
#include "store.h"

class ArgumentList;
struct BaseInfo;
struct Grouping;
struct SectionInfo;
struct ListItemInfo;
class QCString;
class QGString;
class SectionDict;
class MemberSDict;
class GroupList;
struct BodyInfo;
struct DocInfo;
struct BriefInfo;
class MemberList;
class ExampleSDict;
class Entry;

#define NULL_LIST 0xffffffff

class FileStorage : public QFile, public StorageIntf
{
  public:
    FileStorage() : QFile() {}
    FileStorage( const QString &name) : QFile(name) {}
    int read(char *buf,uint size)        { return QFile::readBlock(buf,size); }
    int write(const char *buf,uint size) { return QFile::writeBlock(buf,size); }
};

class StreamStorage : public StorageIntf
{
  public:
    StreamStorage()
    {
      m_data=0;
      m_offset=0;
      m_len=0;
    }
   ~StreamStorage()
    {
      delete m_data;
    }
    StreamStorage(char *data,uint len)
    {
      m_data=data;
      m_offset=0;
      m_len=len;
    }
    int read(char *buf,uint size)
    {
      int bytesLeft = QMIN((int)size,m_len-m_offset);
      if (bytesLeft>0) memcpy(buf,m_data,bytesLeft);
      m_offset+=bytesLeft;
      return bytesLeft;
    }
    int write(const char *buf,uint size)
    {
      m_data=(char *)realloc(m_data,m_offset+size);
      memcpy(m_data+m_offset,buf,size);
      m_offset+=size;
      m_len+=size;
      return size;
    }
    void rewind() { m_offset=0; }
    char *data() const { return m_data; }
    int size() { return m_len; }
  private:
    char *m_data;
    int m_offset;
    int m_len;
};

//----- marshaling function: datatype -> byte stream --------------------

void marshalInt(StorageIntf *s,int v);
void marshalUInt(StorageIntf *s,uint v);
void marshalBool(StorageIntf *s,bool b);
void marshalQCString(StorageIntf *s,const QCString &str);
void marshalQGString(StorageIntf *s,const QGString &str);
void marshalArgumentList(StorageIntf *s,ArgumentList *argList);
void marshalArgumentLists(StorageIntf *s,QList<ArgumentList> *argLists);
void marshalBaseInfoList(StorageIntf *s, QList<BaseInfo> *baseList);
void marshalGroupingList(StorageIntf *s, QList<Grouping> *groups);
void marshalSectionInfoList(StorageIntf *s, QList<SectionInfo> *anchors);
void marshalItemInfoList(StorageIntf *s, QList<ListItemInfo> *sli);
void marshalObjPointer(StorageIntf *s,void *obj);
void marshalSectionDict(StorageIntf *s,SectionDict *sections);
void marshalMemberSDict(StorageIntf *s,MemberSDict *memberSDict);
void marshalDocInfo(StorageIntf *s,DocInfo *docInfo);
void marshalBriefInfo(StorageIntf *s,BriefInfo *briefInfo);
void marshalBodyInfo(StorageIntf *s,BodyInfo *bodyInfo);
void marshalGroupList(StorageIntf *s,GroupList *groupList);
void marshalMemberList(StorageIntf *s,MemberList *ml);
void marshalExampleSDict(StorageIntf *s,ExampleSDict *ed);
void marshalMemberLists(StorageIntf *s,SDict<MemberList> *mls);
void marshalEntry(StorageIntf *s,Entry *e);
void marshalEntryTree(StorageIntf *s,Entry *e);

//----- unmarshaling function: byte stream -> datatype ------------------

int                  unmarshalInt(StorageIntf *s);
uint                 unmarshalUInt(StorageIntf *s);
bool                 unmarshalBool(StorageIntf *s);
QCString             unmarshalQCString(StorageIntf *s);
QGString             unmarshalQGString(StorageIntf *s);
ArgumentList *       unmarshalArgumentList(StorageIntf *s);
QList<ArgumentList> *unmarshalArgumentLists(StorageIntf *s);
QList<BaseInfo> *    unmarshalBaseInfoList(StorageIntf *s);
QList<Grouping> *    unmarshalGroupingList(StorageIntf *s);
QList<SectionInfo> * unmarshalSectionInfoList(StorageIntf *s);
QList<ListItemInfo> *unmarshalItemInfoList(StorageIntf *s);
void *               unmarshalObjPointer(StorageIntf *s);
SectionDict *        unmarshalSectionDict(StorageIntf *s);
MemberSDict *        unmarshalMemberSDict(StorageIntf *s);
DocInfo *            unmarshalDocInfo(StorageIntf *s);
BriefInfo *          unmarshalBriefInfo(StorageIntf *s);
BodyInfo *           unmarshalBodyInfo(StorageIntf *s);
GroupList *          unmarshalGroupList(StorageIntf *s);
MemberList *         unmarshalMemberList(StorageIntf *s);
ExampleSDict *       unmarshalExampleSDict(StorageIntf *s);
SDict<MemberList> *  unmarshalMemberLists(StorageIntf *s);
Entry *              unmarshalEntry(StorageIntf *s);
Entry *              unmarshalEntryTree(StorageIntf *s);

#endif
