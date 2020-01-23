

# Generate doxygen config for given version and profile

    export EWIDL_PREPROCESSOR=/path/to/ewidl_parser/main.py script
    cat ref_config_mobile | sed "s|{EWIDL_PREPROCESSOR}|${EWIDL_PREPROCESSOR} -p mobile -v 3.0|g"


