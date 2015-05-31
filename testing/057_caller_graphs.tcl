#// objective: test for completeness and correctness of references/referencedby relations 
#// check: 057__caller__graphs_8tcl.xml
#// check: __057__caller__graphs_8tcl.xml
#// check: namespacebar.xml
#// check: namespacefoo.xml
#// check: namespace1.xml
#// check: namespace1_1_11.xml
#// check: namespace1_1_11_1_11.xml
#// check: namespace2.xml
#// check: namespace2_1_12.xml
#// check: namespace2_1_12_1_12.xml
#// check: namespace2_1_12_1_12_1_12.xml
#// check: namespace2_1_12_1_12_1_12_1_12.xml
#// config: EXTRACT_ALL = yes
#// config: INLINE_SOURCES = no
#// config: REFERENCED_BY_RELATION = yes
#// config: REFERENCES_RELATION = yes
#// config: INPUT = $INPUTDIR/057_caller_graphs.tcl $INPUTDIR/_057_caller_graphs.tcl
# config: HAVE_DOT = yes
# config: CALLER_GRAPH = yes
# config: CALL_GRAPH = yes
# config: GENERATE_HTML = yes

# This is a stripped down example from my code.
# Doxygen 1.8.7 generates the correct relations (xml)
# but caller graphs will be incomplete.
# It does not generate any relations at all if INLINE_SOURCES = no.
namespace eval bar {}
proc bar::slave { } {
    array set info [info frame 0]; puts -nonewline ->$info(proc)
    if {1} then {
        bar::baz
    }
    return 
}
proc bar::baz {} {
    array set info [info frame 0]; puts -nonewline ->$info(proc)
    bar::bazbaz
}
proc bar::bazbaz {} {
    array set info [info frame 0]; puts -nonewline ->$info(proc)
}
namespace eval foo {}
proc foo::master {  } {
    array set info [info frame 0]; puts -nonewline $info(proc)
    bar::slave
    return
}
#
# now we check tcl's rules: from the help
# NAME RESOLUTION
#... Command names are also always resolved by looking in the current
#namespace first. If not found there, they are searched for in every namespace on
#the current namespace's command path (which is empty by default). If not found
#there, command names are looked up in the global namespace (or, failing that,
#are processed by the unknown command.) ...
#
namespace eval ::1::1::1 {}
proc ::baz args {
    array set info [info frame 0]; puts -nonewline ->$info(proc)
}
proc ::1::baz args {
    array set info [info frame 0]; puts -nonewline ->$info(proc)
}
proc ::bar args {
    array set info [info frame 0]; puts -nonewline ->$info(proc)
}
proc ::1::bar args {
    array set info [info frame 0]; puts -nonewline ->$info(proc)
}
proc ::1::1::bar args {
    array set info [info frame 0]; puts -nonewline ->$info(proc)
}
proc ::1::1::1::bar args {
    array set info [info frame 0]; puts -nonewline ->$info(proc)
}
proc ::1::test1 args {
    array set info [info frame 0]; puts -nonewline $info(proc)
    baz
}
proc ::1::test2 args {
    array set info [info frame 0]; puts -nonewline $info(proc)
    bar
}
proc ::1::test3 args {
    array set info [info frame 0]; puts -nonewline $info(proc)
    ::bar
}
proc ::1::test4 args {
    array set info [info frame 0]; puts -nonewline $info(proc)
    1::bar
}
proc ::1::test5 args {
    array set info [info frame 0]; puts -nonewline $info(proc)
    1::baz
}
#
# funny example, do you see the infinite loop?
# we stop before the interpreter crashes
set ::countdown 10
namespace eval ::2::2::2::2::2 {}
proc ::next args {
    array set info [info frame 0]; puts $info(proc)
    2::next
}
proc ::2::next args {
    array set info [info frame 0]; puts $info(proc)
    incr ::countdown -1
    if {$::countdown>0} then {
        2::next
    } else {
        puts "stop after 10 rounds."
    }
}
proc ::2::2::next args {
    array set info [info frame 0]; puts $info(proc)
    2::next
}
proc ::2::2::2::next args {
    array set info [info frame 0]; puts $info(proc)
    2::next
}
proc ::2::2::2::2::next args {
    array set info [info frame 0]; puts $info(proc)
    2::next
}
proc ::2::2::2::2::2::next args {
    array set info [info frame 0]; puts $info(proc)
    2::next
}
#
# cross check with two files
# If doxygen did not do two passes, then xrefs would depend on file order
# and would be incomplete.
source _057_caller_graphs.tcl
proc master args {
    array set info [info frame 0]; puts -nonewline ->$info(proc)
    inFileB
    return
}
proc inFileA args {
    array set info [info frame 0]; puts -nonewline ->$info(proc)
    return
}
# now, check with tcl what is called
foo::master
puts ""
foreach proc [lsort [info procs ::1::test?]] {
    $proc
    puts ""
}
::next
master
exit

