find_program(CLANG clang REQUIRED)
find_program(CLANGXX clang++ REQUIRED)
set(CMAKE_C_COMPILER ${CLANG})
set(CMAKE_CXX_COMPILER ${CLANGXX})
