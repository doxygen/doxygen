!// objective: test inheritance in Fortran
!// check: structm1_1_1t1.xml
!// check: structm1_1_1t2.xml
!// check: structm1_1_1t3.xml
!// check: structm2_1_1t3.xml
!// check: structm3_1_1t4.xml
!// check: namespacem1.xml
!// check: namespacem2.xml
!// check: namespacem3.xml
!// config: OPTIMIZE_FOR_FORTRAN=YES

module M1
  implicit none
  public

  type T1
  end type T1

  type, extends(T1) :: T2
  contains
  end type

  type, extends(T2) :: T3
  end type

end module M1

module M2
  use M1
  implicit none

  type, extends(T2) :: T3
  end type

end module M2

module M3
  use M2, only: T1
  implicit none

  type, extends(T1) :: T4
  end type

end module M3
