!// objective: test visibility of types in modules
!// check: structmymodule_1_1t1.xml
!// check: structmymodule_1_1t2.xml
!// check: structmymodule_1_1t3.xml
!// check: structmymodule_1_1t4.xml
!// config: OPTIMIZE_FOR_FORTRAN=YES
!// config: WARN_IF_UNDOCUMENTED=NO

module myModule
  implicit none
  private

  public :: T3

  type T1
    integer :: publicVariable
  end type T1

  public :: T1

  type, public :: T2
    integer :: publicVariable
  contains
  end type

  type T3
    private
    integer :: privateVariable
  end type

  type, private :: T4
    integer :: publicVariable
  end type

end module myModule
