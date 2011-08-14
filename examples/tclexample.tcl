## \file tclexample.tcl
# File documentation.
#\verbatim

# Startup code:\
exec tclsh "$0" "$@"
#\endverbatim
## Documented namespace \c ns .
# The code is inserted here:
#\code
namespace eval ns {
  ## Documented proc \c ns_proc .
  # param[in] arg some argument
  proc ns_proc {arg} {}
  ## Documented var \c ns_var .
  # Some documentation.
  variable ns_var
  ## Documented itcl class \c itcl_class .
  itcl::class itcl_class {
    ## Create object.
    constructor {args} {eval $args}
    ## Destroy object.
    destructor {exit}
    ## Documented itcl method \c itcl_method_x .
    # param[in] arg Argument
    private method itcl_method_x {arg}
    ## Documented itcl method \c itcl_method_y .
    # param[in] arg Argument
    protected method itcl_method_y {arg} {}
    ## Documented itcl method \c itcl_method_z .
    # param[in] arg Argument
    public method itcl_method_z {arg} {}
    ## Documented common itcl var \c itcl_Var .
    common itcl_Var
    ## \protectedsection
    
    variable itcl_var1;#< Documented itcl var \c itcl_var1 .
    variable itcl_var2;#< Documented itcl var \c itcl_var2 .
  }
  ## Documented oo class \c oo_class .
  oo::class create oo_class {
    ## Create object.
    # Configure with args
    constructor {args} {eval $args}
    ## Destroy object.
    # Exit.
    destructor {exit}
    ## Documented oo var \c oo_var .
    # Defined inside class
    variable oo_var
    ## \private Documented oo method \c oo_method_x .
    # param[in] arg Argument
    method oo_method_x {arg} {}
    ## \protected Documented oo method \c oo_method_y .
    # param[in] arg Argument
    method oo_method_y {arg} {}
    ## \public Documented oo method \c oo_method_z .
    # param[in] arg Argument
    method oo_method_z {arg} {}
  }
}
#\endcode

itcl::body ::ns::itcl_class::itcl_method_x {argx} {
  puts "$argx OK"
}

oo::define ns::oo_class {
  ## \public Outside defined variable \c oo_var_out .
  # Inside oo_class
  variable oo_var_out
}

## Documented global proc \c glob_proc .
# param[in] arg Argument
proc glob_proc {arg} {puts $arg}

variable glob_var;#< Documented global var \c glob_var\
  with newline
#< and continued line

# end of file
