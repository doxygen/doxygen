// objective: test the \parblock command
// check: 054__parblock_8cpp.xml

/** @file */

/**
 call by target-specific code to manage resources required by the client.

 @param[in]  client    ID of client requesting resource.
 @param[out] resource  Requested resource
 @param[in]  parblock  @parblock This is a test for the \@parblock
                       command.

                       A list if values for the parblock param:
                       - Item 1. This is short one-line description.
                       - Item 2. This is a long bullet item;
                         sometimes they wrap on multiple lines like this
                         one.

                       This is the second paragraph description for the
                       \@parblock parameter. Always end the text inside
                       the \@parblock command with an \@endparblock
                       command.
                       @endparblock
 @param[out] test      This is a test parameter for this function to see if
                       it is included in the parameter table
 @param[in]  p
         @parblock First paragraph of the param description.

         Second paragraph of the param description.
         @endparblock
 */
void function(int client,int *resource,int parblock,int *test,int p);
/**
 call by target-specific code to manage resources required by the client.

 @param[in]  client    ID of client requesting resource.
 @param[out] resource  Requested resource
 @param[in]  parblock  @parblock This is a test for the \@parblock
                       command.

                       A list if values for the parblock param:
                       - Item 1. This is short one-line description.
                       - Item 2. This is a long bullet item;
                         sometimes they wrap on multiple lines like this
                         one.

                       This is the second paragraph description for the
                       \@parblock parameter. Always end the text inside
                       the \@parblock command with an \@endparblock
                       command.
                       @endparblock
 @param[out] test      This is a test parameter for this function to see if
                       it is included in the parameter table
 @param[in]  p         @parblock
                       First paragraph of the param description.

                       Second paragraph of the param description.
 @endparblock
 */
void function_2(int client,int *resource,int parblock,int *test,int p);
