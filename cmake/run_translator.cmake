include(${TOP}/cmake/version.cmake)
find_program(PYTHON NAMES python)
execute_process(
    COMMAND ${PYTHON} ${CMAKE_SOURCE_DIR}/translator.py
)
