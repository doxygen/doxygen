# adapted from
# https://github.com/lefticus/cppbestpractices/blob/master/02-Use_the_Tools_Available.md

function(set_project_warnings project_name)

  set(MSVC_WARNINGS
      #/W4 # Baseline reasonable warnings
      /w14242 # 'identfier': conversion from 'type1' to 'type1', possible loss
              # of data
      /w14254 # 'operator': conversion from 'type1:field_bits' to
              # 'type2:field_bits', possible loss of data
      /w14263 # 'function': member function does not override any base class
              # virtual member function
      /w14265 # 'classname': class has virtual functions, but destructor is not
              # virtual instances of this class may not be destructed correctly
      /w14287 # 'operator': unsigned/negative constant mismatch
      /we4289 # nonstandard extension used: 'variable': loop control variable
              # declared in the for-loop is used outside the for-loop scope
      /w14296 # 'operator': expression is always 'boolean_value'
      /w14311 # 'variable': pointer truncation from 'type1' to 'type2'
      /w14456 # declaration of 'name' hides previous local declaration
      /w14457 # declaration of 'name' hides function parameter
      /w14458 # declaration of 'name' hides class member
      /w14459 # declaration of 'name' hides global declaration
      /w14545 # expression before comma evaluates to a function which is missing
              # an argument list
      /w14546 # function call before comma missing argument list
      /w14547 # 'operator': operator before comma has no effect; expected
              # operator with side-effect
      /w14549 # 'operator': operator before comma has no effect; did you intend
              # 'operator'?
      /w14555 # expression has no effect; expected expression with side- effect
      /w14619 # pragma warning: there is no warning number 'number'
      /w14640 # Enable warning on thread un-safe static member initialization
      /w14826 # Conversion from 'type1' to 'type_2' is sign-extended. This may
              # cause unexpected runtime behavior.
      /w14905 # wide string literal cast to 'LPSTR'
      /w14906 # string literal cast to 'LPWSTR'
      /w14928 # illegal copy-initialization; more than one user-defined
              # conversion has been implicitly applied
  )

  set(CLANG_WARNINGS
      -Wall
      -Wextra # reasonable and standard
      -Wshadow # warn the user if a variable declaration shadows one from a
               # parent context
      $<$<COMPILE_LANGUAGE:CXX>:-Wnon-virtual-dtor>
               # warn the user if a class with virtual functions has a
               # non-virtual destructor. This helps catch hard to
               # track down memory errors
#      -Wold-style-cast # warn for c-style casts
      -Wcast-align # warn for potential performance problem casts
      -Wunused # warn on anything being unused
      $<$<COMPILE_LANGUAGE:CXX>:-Woverloaded-virtual>
               # warn if you overload (not override) a virtual function
      -Wpedantic # warn if non-standard C++ is used
      -Wconversion # warn on type conversions that may lose data
      -Wnull-dereference # warn if a null dereference is detected
      -Wdouble-promotion # warn if float is implicit promoted to double
      -Wformat=2 # warn on security issues around functions that format output
                 # (ie printf)

      # turn off warning caused by generated code (flex)
      -Wno-unused-parameter
      -Wno-implicit-int-conversion
      -Wno-sign-conversion
      -Wno-format-nonliteral
      -Wno-shorten-64-to-32

      # enable to turn warnings into errors
      #-Werror
  )

  set(GCC_WARNINGS
      -Wall
      -Wextra # reasonable and standard
      -Wshadow # warn the user if a variable declaration shadows one from a
               # parent context
      $<$<COMPILE_LANGUAGE:CXX>:-Wnon-virtual-dtor>
               # warn the user if a class with virtual functions has a
               # non-virtual destructor. This helps catch hard to
               # track down memory errors
#      -Wold-style-cast # warn for c-style casts
      -Wcast-align # warn for potential performance problem casts
      -Wunused # warn on anything being unused
      $<$<COMPILE_LANGUAGE:CXX>:-Woverloaded-virtual>
               # warn if you overload (not override) a virtual function
      -Wpedantic # warn if non-standard C++ is used
      #-Wconversion # warn on type conversions that may lose data
      #-Wnull-dereference # warn if a null dereference is detected
      -Wdouble-promotion # warn if float is implicit promoted to double
      -Wformat=2 # warn on security issues around functions that format output
                 # (ie printf)

      # turn off warning caused by generated code (flex)
      -Wno-unused-parameter
      -Wno-sign-conversion
      -Wno-format-nonliteral

      # enable to turn warnings into errors
      #-Werror
  )

  if(MSVC)
    set(PROJECT_WARNINGS ${MSVC_WARNINGS})
  elseif(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang") # e.g. Clang or AppleClang
    set(PROJECT_WARNINGS ${CLANG_WARNINGS})
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER "7.0.0")
      set(GCC_EXTRA_WARNINGS
        -Wno-implicit-fallthrough
      )
    else()
      set(GCC_EXTRA_WARNINGS
      )
    endif()
    set(PROJECT_WARNINGS ${GCC_WARNINGS} ${GCC_EXTRA_WARNINGS})
  endif()

  target_compile_options(${project_name} PRIVATE ${PROJECT_WARNINGS})

endfunction()
