Issue 6880 (https://github.com/doxygen/doxygen/issues/6880) required fixes to the mscgen tool.
Since this tool is no longer maintained, it was decided to build mscgen as part of doxygen.
This directory contains the mscgen code.

Since mscgen depends on libgd for PNG output, a part of the gd library is included as well.
Instead of using libpng as PNG generator, the lodepng library is used. This PNG library was
already part of doxygen. Module gd_lodepng.c was added to make libgd use lodepng.

Original copyright statement follows:

LICENCE
=======

Mscgen, Copyright (C) 2010 Michael C McTernan,
                      Michael.McTernan.2001@cs.bris.ac.uk
Mscgen comes with ABSOLUTELY NO WARRANTY.  Mscgen is free software, and you
are welcome to redistribute it under certain conditions; see the COPYING
file for details.


