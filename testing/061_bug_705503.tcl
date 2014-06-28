#// objective: test for bug 705503 - TCL: Documentation of oo::define is not working
#// check: class_test.xml
#// config: EXTRACT_ALL = yes

# taken from
# https://bugzilla.gnome.org/show_bug.cgi?id=705503

## @class Test
# @brief Testclass
oo::class create Test {
	## @brief Test method 1.
	method testmethod_one args {}
}

## @brief Construction of class
oo::define Test constructor args {}

## @brief Test method 2
oo::define Test method testmethod_two args {}
