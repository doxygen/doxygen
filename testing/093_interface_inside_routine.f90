!// objective: don't show interfaces inside routines except for arguments
!// check: 093__interface__inside__routine_8f90.xml
!// config: OPTIMIZE_FOR_FORTRAN=YES
!// config: EXTRACT_ALL=YES

!> Test for interface
SUBROUTINE use_tst(interfArg)
INTERFACE
  SUBROUTINE routine_interface()
  END SUBROUTINE routine_interface
  INTEGER FUNCTION interfArg(str)
    CHARACTER(LEN=*), INTENT(IN) :: str
  END FUNCTION
END INTERFACE
CONTAINS
  SUBROUTINE inner_routine
  END SUBROUTINE
END SUBROUTINE use_tst
