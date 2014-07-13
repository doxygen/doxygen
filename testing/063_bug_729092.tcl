#// objective: test for bug 729092 - TCL: Full documentation not shown for procs in namespaces.
#// check: namespaceoo.xml
#// check: namespaceoo_1_1_helpers.xml
#// check: namespaceoo_1_1define.xml
#// config: EXTRACT_ALL = yes
#// config: GENERATE_HTML = yes

# taken from
# https://bugzilla.gnome.org/show_bug.cgi?id=729092

##
# Extension to TclOO to add static methods.
# Defines the method on the class instead of on the object. Can be used for
# the creation of megawidgets using TclOO by overriding the unknown method to
# detect if the user is trying to instantiate a widget (because the method
# will be unknown and start with a dot).
#
proc ::oo::define::classmethod {name {args ""} {body ""}} {
    # Create the method on the class if the caller gave arguments and body.
    if {[llength [info level 0]] == 4} {
        uplevel 1 [list self method $name $args $body]
    }
    # Get the name of the class being defined.
    set cls [lindex [info level -1] 1]
    # Make connection to private class "my" command by forwarding.
    uplevel forward $name [info object namespace $cls]::my $name
}

##
# Extension to TclOO to add static variables.
# Defines variables on the class instead of on the object. Can be used to
# enforce a limited number of instantiations.
#
proc ::oo::Helpers::classvar {args} {
    # Get reference to class's namespace.
    set nsCl [info object namespace [uplevel 1 {self class}]]
    set nsObj [uplevel 1 {namespace current}]
    # Link variables into local (caller's) scope.
    foreach v $args {
        uplevel "my variable $v"
        upvar #0 ${nsCl}::$v ${nsObj}::$v
    }
}
