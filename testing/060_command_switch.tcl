#// objective: tests processing of switch, only references/referencedby relations are relevant
#// check: 060__command__switch_8tcl.xml
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
# switch command
# switch ?options? string pattern body ?pattern body ...? 
proc b args {
    switch value NotInvoked {
        } NotInvoked {
        } default {
            Invoked
        }
    return
}
proc c args {
    switch value NotInvoked {
        } [Invoked] {
        } default {
        }
    return
}
proc d args {
    switch NotInvoked pattern {
        } [Invoked] {
        } default {
        }
    return
}
proc e args {
    switch [Invoked] pattern {
        } NotInvoked {
        } default {
        }
    return
}
proc f args {
    switch -exact value pattern {
        } NotInvoked {
        } default {
            Invoked
        }
    return
}
proc g args {
    switch -exact -- value pattern {
        } NotInvoked {
        } default {
            Invoked
        }
    return
}
proc h args {
    switch -exact -- -value pattern {
        } NotInvoked {
        } default {
            Invoked
        }
    return
}
# switch ?options? string {pattern body ?pattern body ...?} 
proc i args {
    switch value {
        NotInvoked {
        }
        NotInvoked {
        }
        default {
            Invoked
        }
    }
    return
}
proc j args {
    switch vale {
        NotInvoked {
        }
        [NotInvoked] {
        }
        default {
            Invoked
        }
    }
    return
}
proc k args {
    switch NotInvoked {
        [NotInvoked] {
        }
        NotInvoked {
            Invoked
        }
        default {
        }
    }
    return
}
proc l args {
    switch [Invoked] {
        pattern {
        }
        NotInvoked {
        }
        default {
        }
    }
    return
}
proc m args {
    switch -exact value {
        pattern {
        }
        NotInvoked {
        }
        default {
            Invoked
        }
    }
    return
}
proc n args {
    switch -exact -- value {
        pattern {
        }
        NotInvoked {
        }
        default {
            Invoked
        }
    }
    return
}
proc o args {
    switch -exact -- -value {
        pattern {
        }
        NotInvoked {
        }
        default {
            Invoked
        }
    }
    return
}
proc p args {
    switch -exact -- inquotes {
        "inquotes" {
            Invoked
        }
        default {
        }
    }
    return
}
proc q args {
    switch -exact -- "in quotes" {
        "in quotes" {
            Invoked
        }
        default {
        }
    }
    return
}
proc r args {
    switch -exact -- inbraces {
        {inbraces} {
            Invoked
        }
        default {
        }
    }
    return
}
proc s args {
    switch -exact -- {in braces} {
        {in braces} {
            Invoked
        }
        default {
        }
    }
    return
}
# wrong syntax
#proc x args {
#    catch {switch -exact -- [Invoked] pattern1 NotInvoked pattern2}
#    return
#}
# The current version does not check the last argument beforehand.
# Therefore, all script elements are evaluated as scripts before
# the parser detects the dangling pattern. It throws a warning, at the very least.
# Anyway, for working code the documentation will be correct.
#proc y args {
#    catch {switch -exact -- [Invoked] {
#        pattern {
#            NotInvoked
#        }
#        NotInvoked {
#            NotInvoked
#        }
#        default {
#            NotInvoked
#        }
#        pattern
#    }}
#    return
#}
#
# call all single letter procs
# let tcl check what is called and what is not called
foreach p [info procs ?] {
    puts "Check procedure \"$p\""
    $p
}
exit

