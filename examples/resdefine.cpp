#define ExportedName InternalName

class InternalName
{
  public:
    InternalName() {}
   ~InternalName() {}
};

/*! \class ExportedName
 *  This class's real name is InternalName but everyone should use 
 *  ExportedName.
 */

/*! \fn ExportedName::ExportedName()
 *  The constructor
 */

/*! \fn ExportedName::~ExportedName()
 *  The destructor
 */
