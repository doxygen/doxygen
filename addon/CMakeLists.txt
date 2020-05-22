if (build_xmlparser)
    add_subdirectory(doxmlparser)
endif ()

if (build_app)
    add_subdirectory(doxyapp)
endif ()

if (build_parse)
    add_subdirectory(doxyparse)
endif ()

if (build_search)
    add_subdirectory(doxysearch)
endif ()

if (build_wizard)
    add_subdirectory(doxywizard)
endif ()
