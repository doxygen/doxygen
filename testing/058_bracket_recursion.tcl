#// objective: tests processing of commands inside brackets [], only references/referencedby relations are relevant
#// check: 058__bracket__recursion_8tcl.xml
#// config: REFERENCED_BY_RELATION = yes
#// config: REFERENCES_RELATION = yes
#// config: EXTRACT_ALL = yes
#// config: INLINE_SOURCES = yes

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
# check brackets with various quoting, bracing
proc b args {
    set r [Invoked]
    set r [list \[NotInvoked \]]
    return
}
proc c args {
    set r \{[Invoked]\}
    set r {[NotInvoked]}
    return
}
proc d args {
    set r "[Invoked]"
    set r "\[NotInvoked \]"
    return
}
proc e args {
    set r [list \[NotInvoked [Invoked]\]]
    return
}
proc f args {
    set r [list [Invoked \[NotInvoked \]]]
    return
}
proc g args {
    set r "{[Invoked]}"
    set r "{\[NotInvoked \]}"
    return
}
proc h args {
    [Invoked set] r {[NotInvoked]}
    return
}
# check brackets in tcl commands containing script arguments
#
# example generated according to
# https://groups.google.com/d/msg/comp.lang.tcl/G5-mc3GiIyY/e-AVD9t7xMkJ
proc i args {
    foreach item [Invoked] {
    return
    }
}
proc j args {
    foreach [Invoked item] [list one two three] {
    }
    return
}
proc k args {
    while {[Invoked 0]} {
    }
}
proc l args {
    for {} {[Invoked 0]} {} {
    }
}
proc m args {
    if {[Invoked 1]} {
    }
}
proc n args {
    if [Invoked 1] {
    }
}
proc o args {
    if {0} {
    } elseif {[Invoked 0]} {
    }
}
# these are really nasty examples
# they shows, that the condition argument may not be parsed as a script
set NotInvoked \$NotInvoked
proc $NotInvoked args {
    puts "Procedure \"\$NotInvoked\" is invoked. Not Ok!"
    return $args
}
proc p args {
    set NotInvoked \$NotInvoked
    if {$NotInvoked eq [Invoked 1]} {
    }
    return
}
proc q args {
    set NotInvoked \$NotInvoked
    if {0} {
    } elseif {$NotInvoked eq [Invoked 1]} {
    }
    return
}
proc r args {
    set NotInvoked \$NotInvoked
    while {$NotInvoked eq [Invoked 1]} {
    }
    return
}
proc s args {
    set NotInvoked \$NotInvoked
    for {} {$NotInvoked eq [Invoked 1]} {} {
    }
    return
}
# dangling open brackets should not confuse the scanner
proc t args {
    set foo ]]]][Invoked]
    return
}
# Example according to
# https://bugzilla.gnome.org/show_bug.cgi?id=729135
#                                       |
# Note the subtle difference in this    | whitespace
#                                       V
proc y {} {
    set classifier_state {{bphy} }
    if { ($classifier_state == {{bphy} }) } {
        Invoked
    }
}
proc z {} {
    set classifier_state {{bphy} }
    if { ($classifier_state == {{bphy} } ) } {
        Invoked
    }
}
#
# call all single letter procs
# let tcl check what is called and what is not called
foreach p [info procs ?] {
    puts "Check procedure \"$p\""
    $p
}
exit

