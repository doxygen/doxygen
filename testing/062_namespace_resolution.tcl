#// objective: tests correct namespace resolution, only references/referencedby relations are relevant
#// check: namespacen1.xml
#// check: namespacen2.xml
#// check: namespacen3.xml
#// config: REFERENCED_BY_RELATION = yes
#// config: REFERENCES_RELATION = yes
#// config: EXTRACT_ALL = yes
#// config: INLINE_SOURCES = yes

# now: combine namespace eval and qualified names
namespace eval n1 {
    proc p1 args {
        array set info [info frame 0]; puts -nonewline ->$info(proc)
        p2
        return
    }
    proc p2 args {
        array set info [info frame 0]; puts -nonewline ->$info(proc)
        return
    }
    namespace eval n1 {
        proc p1 args {
            array set info [info frame 0]; puts -nonewline ->$info(proc)
            return
        }
    }
}
# same thing, but fully qualified proc names
namespace eval ::n2 {}
namespace eval ::n2::n2 {}
proc ::n2::p1 args {
    array set info [info frame 0]; puts -nonewline ->$info(proc)
    p2
    return
}
proc ::n2::p2 args {
    array set info [info frame 0]; puts -nonewline ->$info(proc)
    return
}
proc ::n2::n2::p2 args {
    array set info [info frame 0]; puts -nonewline ->$info(proc)
    return
}
# same thing, without leading ::
namespace eval n3 {}
namespace eval n3::n3 {}
proc n3::p1 args {
    array set info [info frame 0]; puts -nonewline ->$info(proc)
    p2
    return
}
proc n3::p2 args {
    array set info [info frame 0]; puts -nonewline ->$info(proc)
    return
}
proc n3::n3::p2 args {
    array set info [info frame 0]; puts -nonewline ->$info(proc)
    return
}
# now, check with tcl what is called
n1::p1
puts ""
n2::p1
puts ""
n3::p1
puts ""
exit

