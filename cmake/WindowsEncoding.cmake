if(CMAKE_C_COMPILER_ID STREQUAL "MSVC" OR CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  include(FindPythonInterp)
  execute_process(
    COMMAND ${Python_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/QueryCodePage.py"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    RESULT_VARIABLE ReturnCode
    OUTPUT_VARIABLE CodePage
  )
  message(STATUS "CodePage is ${CodePage}")
  if("${CodePage}" STREQUAL "936")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /source-charset:utf-8 /execution-charset:gbk")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /source-charset:utf-8 /execution-charset:gbk")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /utf-8")
  endif()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj") # /bigobj needed for language.cpp on 64bit
endif()
