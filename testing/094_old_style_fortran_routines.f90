!// objective: test old style fortran without modules
!// check: 094__old__style__fortran__routines_8f90.xml
!// config: OPTIMIZE_FOR_FORTRAN=YES
!// config: EXTRACT_ALL=YES

SUBROUTINE use_tst2
  write(*,*) "I'm use_tst2"
  call inner2
END SUBROUTINE use_tst2

SUBROUTINE use_tst
  write(*,*) "I'm use_tst"
  call inner
END SUBROUTINE use_tst

SUBROUTINE use_tst1(routine_interface)
  external routine_interface
  call routine_interface(7)
END SUBROUTINE use_tst1

SUBROUTINE A(i)
  integer i
    write(*,*) "I'm A"
END SUBROUTINE

SUBROUTINE B(i)
  integer i
    write(*,*) "I'm B"
END SUBROUTINE

program tst
  call use_tst
  call use_tst2
  call use_tst1(A)
  call use_tst1(B)
end program
