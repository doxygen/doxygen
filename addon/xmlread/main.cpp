// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include <util/PlatformUtils.hpp>
#include <parsers/SAXParser.hpp>
#include "strx.h"
#include "saxhandlers.h"
#include "compounddef.h"

// ---------------------------------------------------------------------------
//  Local data
//
//  xmlFile
//      The path to the file to parser. Set via command line.
//
// ---------------------------------------------------------------------------
static char*    xmlFile         = 0;

// ---------------------------------------------------------------------------
//  Local helper methods
// ---------------------------------------------------------------------------
static void usage()
{
    cout <<  "\nUsage: xmlread file.xml\n"
         <<  "   Extracts the class hierarchy from a doxygen generated XML file.\n"
         <<  endl;
}

// recursively display non-root compounds
static void showDerivedCompounds(QList<CompoundDef> *compoundList,int level)
{
  if (compoundList->count()>0)
  {
    QListIterator<CompoundDef> cli(*compoundList);
    CompoundDef *cd;
    for (cli.toFirst();(cd=cli.current());++cli)
    {
      int i;
      for (i=0;i<level;i++) cout << "  ";
      cout << cd->name() << endl;
      showDerivedCompounds(cd->derivedCompounds(),level+1);
    }
  }
}

// show compound hierarchy
static void showCompoundHierarchy(CompoundSDict *compounds)
{
  CompoundSDictIterator cdi(*compounds);
  CompoundDef *cd;
  for (cdi.toFirst();(cd=cdi.current());++cdi)
  {
    if (cd->type()==CompoundDef::Class_t && 
        cd->baseCompounds()->count()==0) // root compound
    {
      cout << cd->name() << endl;
      showDerivedCompounds(cd->derivedCompounds(),1);
    }
  }
}

// ---------------------------------------------------------------------------
//  Program entry point
// ---------------------------------------------------------------------------
int main(int argC, char* argV[])
{
    // Initialize the XML4C2 system
    try
    {
         XMLPlatformUtils::Initialize();
    }
    catch (const XMLException& toCatch)
    {
         cerr << "Error during initialization! :\n"
              << StrX(toCatch.getMessage()) << endl;
         return 1;
    }

    // Check command line and extract arguments.
    if (argC!=2)
    {
        usage();
        return 1;
    }

    // Watch for special case help request
    if (strcmp(argV[1], "-?") == 0)
    {
        usage();
        return 0;
    }

    // Assume the argument is a file name
    xmlFile = argV[1];

    //
    //  Create a SAX validating parser object. 
    //
    SAXParser parser;
    parser.setDoValidation(TRUE);

    //  Create a compound dictionary, where the results will be stored.
    //  The size of the dictionary is 1009 entries (should be a prime number)
    CompoundSDict compounds(1009);

    //
    //  Create the handler object and install it as the document and error
    //  handler for the parser.
    //
    SaxPass1Handlers handler(&compounds);
    parser.setDocumentHandler(&handler);
    parser.setErrorHandler(&handler);

    // Parse the file and catch any exceptions that propogate out
    try
    {
        parser.parse(xmlFile);
    }
    catch (const XMLException& toCatch)
    {
        cerr << "\nFile not found: '" << xmlFile << "'\n"
             << "Exception message is: \n"
             << StrX(toCatch.getMessage())
             << "\n" << endl;
        return -1;
    }

    // Show the class hierarchy that we extracted
    showCompoundHierarchy(&compounds);

    return 0;
}

