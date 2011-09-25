//
// File: main.cpp
//
// A small Qt example application written by Troll Tech.
//
// It displays a text in a window and quits when you click
// the mouse in the window.
//

#include "hello.h"
#include <qapp.h>


/*
  The program starts here. It parses the command line and build a message
  string to be displayed by the Hello widget.
*/

int main( int argc, char **argv )
{
    QApplication a(argc,argv);
    QString s;
    for ( int i=1; i<argc; i++ ) {
	s += argv[i];
	if ( i<argc-1 )
	    s += " ";
    }
    if ( s.isEmpty() )
	s = "Hello, World";
    Hello h( s );
    h.setCaption( "Qt says hello" );
    QObject::connect( &h, SIGNAL(clicked()), &a, SLOT(quit()) );
    h.setFont( QFont("times",32,QFont::Bold) );		// default font
    h.setBackgroundColor( white );			// default bg color
    a.setMainWidget( &h );
    h.show();
    return a.exec();
}
