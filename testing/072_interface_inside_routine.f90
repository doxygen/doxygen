!// objective: don't show interfaces inside routines except for callgraphs
!// check: 072__interface__inside__routine_8f90.xml
!// config: OPTIMIZE_FOR_FORTRAN=YES
!// config: EXTRACT_ALL=YES

!> Test for interface
SUBROUTINE use_tst()

INTERFACE
  SUBROUTINE routine_interface()
  END SUBROUTINE routine_interface
END INTERFACE
END SUBROUTINE use_tst
