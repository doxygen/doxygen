# run all tests sequentially (keep for backward compatibilty)
add_custom_target(tests
         COMMENT "Running doxygen tests..."
	 COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/testing/runtests.py --doxygen ${PROJECT_BINARY_DIR}/bin/doxygen --inputdir ${CMAKE_SOURCE_DIR}/testing --outputdir ${PROJECT_BINARY_DIR}/testing
         DEPENDS doxygen
)

# to update this list run `ls [0-9][0-9][0-9]_*.*` in the testing directory
set(TEST_FILES
001_a.dox
002_addindex.dox
003_anchor.dox
004_arg.dox
005_attention.dox
006_author.dox
007_b.dox
008_brief.c
009_bug.cpp
010_c.dox
011_category.m
012_cite.dox
013_class.h
014_code.dox
015_cond.c
016_copydoc.c
017_copyright.dox
018_def.c
019_defgroup.c
020_only.dox
021_dontinclude.cpp
022_dot.cpp
023_e.dox
024_if.dox
025_example.cpp
026_exception.cpp
027_extends.c
028_formula.c
029_hideinit.c
030_htmlinclude.dox
031_image.dox
032_include.cpp
033_internal.dox
034_internal.dox
035_invariant.c
036_link.c
037_msc.cpp
038_n.dox
039_name.cpp
040_namespace.cpp
041_overload.cpp
042_package.java
043_page.dox
044_section.h
045_refitem.dox
046_related.cpp
047_return.cpp
048_showinit.c
049_snippet.cpp
050_verbatim.dox
051_escape.dox
052_tilde.dox
053_tilde.dox
054_parblock.cpp
055_markdown.md
056_latexinclude.dox
064_castoperator.cpp
065_tilde.dox
066_property_initializer.cs
067_link_varargs.cpp
068_ref_varargs.cpp
069_link_variadic_template.cpp
070_ref_variadic_template.cpp
071_enum_in_anon_ns.cpp
072_using.cpp
073_typed_enum.cpp
074_ref.cpp
075_argmatch.cpp
076_emojis.cpp
077_no_xml_namespace_members_in_file_scope.h
078_xml_namespace_members_in_file_scope.h
079_tableofcontents.dox
080_extract_private_virtual.cpp
081_brief_lists.h
082_decl_def.cpp
083_decl_def.cpp
084_markdown_pre.f90
085_tooltip.cpp
086_style_tags.h
)

foreach(TEST_FILE ${TEST_FILES})
	# extract the test name from the file name
	string(REGEX REPLACE "^([0-9][0-9][0-9]*.*)\\.[^.]*$" "\\1" TEST_NAME "${TEST_FILE}")
	# extract the test id from the file name
	string(REGEX REPLACE "^([0-9][0-9][0-9]*).*$" "\\1" TEST_ID "${TEST_FILE}")
	# add a test target for each test
	add_test(NAME ${TEST_NAME}
		 COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/testing/runtests.py --id ${TEST_ID} --doxygen $<TARGET_FILE:doxygen> --inputdir ${CMAKE_SOURCE_DIR}/testing --outputdir ${PROJECT_BINARY_DIR}/testing
	)
endforeach()
