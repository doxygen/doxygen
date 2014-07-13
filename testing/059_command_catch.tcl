#// objective: tests processing of catch/eval, only references/referencedby relations are relevant
#// check: 059__command__catch_8tcl.xml
#// config: REFERENCED_BY_RELATION = yes
#// config: REFERENCES_RELATION = yes
#// config: EXTRACT_ALL = yes
#// config: INLINE_SOURCES = no

##
# \brief should be reference by every proc below
proc Invoked args {
    puts "Procedure \"Invoked\" is invoked indeed. Ok."
    return $args
}
##
# \brief must not be reference by every proc below
proc NotInvoked args {
    puts "Procedure \"NotInvoked\" is invoked. Not Ok!"
    return $args
}
#
# check if call references work at all
proc a args {
    Invoked NotInvoked
    return
}
#
# catch command
# Tcl8.5: catch script ?resultVarName? ?optionsVarName?
proc b args {
    catch Invoked
    return
}
proc c args {
    catch Invoked NotInvoked
    return
}
proc d args {
    catch Invoked NotInvoked NotInvoked
    return
}
proc e args {
    set r [catch Invoked NotInvoked NotInvoked]
    return
}
proc f args {
    set r [catch {Invoked} NotInvoked NotInvoked]
    return
}
proc g args {
    set r [catch {
        set x [Invoked]
        } NotInvoked NotInvoked]
    return
}
# eval arg ?arg ...?
proc h args {
    eval Invoked NotInvoked
    return
}
proc i args {
    eval set NotInvoked [Invoked NotInvoked]
    return
}
# This is a striped down example. Original:
#
# jpeg.tcl --
#
#       Querying and modifying JPEG image files.
#
# Copyright (c) 2004    Aaron Faupell <afaupell@users.sourceforge.net>
#
# ...
# eval [list addComment $file] [lreplace $com 0 0 $comment]
# ...
proc j args {
    eval [list set] [list NotInvoked] [Invoked NotInvoked]
    return
}
#
# call all single letter procs
# let tcl check what is called and what is not called
foreach p [info procs ?] {
    puts "Check procedure \"$p\""
    $p
}
exit

