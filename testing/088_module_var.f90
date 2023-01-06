!// objective: test visibility of variables in modules
!// check: namespacem1.xml
!// check: namespacem2.xml
!// config: OPTIMIZE_FOR_FORTRAN=YES

module M1
  implicit none
  private

  public :: V3

  integer :: V1
  public :: V1

  integer, public :: V2
  integer :: V3

end module M1

module M2
  implicit none

  private :: V1
  integer :: V1

  integer :: V2

  integer, private :: V3

  integer :: V4
  public :: V4

end module M2
