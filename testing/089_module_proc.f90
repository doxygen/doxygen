!// objective: test visibility of variables in modules
!// check: namespacem1.xml
!// check: namespacem2.xml
!// config: OPTIMIZE_FOR_FORTRAN=YES

module M1
  implicit none
  private

  public :: f1
  public :: f2, f3

contains

  subroutine f1
  end subroutine

  subroutine f2
  end subroutine

  subroutine f3
  end subroutine

end module M1

module M2
  implicit none

  private :: f1
  public :: f2
  public :: f3

contains

  subroutine f1
  end subroutine

  function f2
  integer :: f2
  end subroutine

  subroutine f3
  end subroutine

end module M2
