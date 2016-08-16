!// objective: test different variants of interfaces in Fortran
!// check: namespacemymodule.xml
!// check: interfacemymodule_1_1genericproc.xml
!// check: interfacemymodule_1_1externalproc.xml
!// check: interfacemymodule_1_1abstractproc.xml
!// config: OPTIMIZE_FOR_FORTRAN=YES

module myModule
  implicit none
  private

  public :: genericProc
  public :: externalProc
  public :: abstractProc

  ! meaning 1: interface declaration for external proc, e.g. function declaration
  ! (the target is obviosly not in this module, so somewhere outside a module
  !  there is a function we want to use)
  interface
    real function externalProc(r)
      real, intent(in) :: r
    end function
  end interface
  
  ! meaning 2: generic interface, e.g. function overloading
  ! (can be mixed with 1)
  interface genericProc
    module procedure firstProc
    module procedure otherProc
    subroutine externalProc2(r)
      real, intent(out) :: r
    end subroutine
  end interface

  ! meaning 3: abstract interface, e.g. for function pointers, deferred methods
  ! (this is more like a typedef, but needs to be handled similarly to a function definition)
  abstract interface
    subroutine abstractProc(logicalArg)
      logical, intent(out) :: logicalArg
    end subroutine
  end interface

contains

  !> short doc
  subroutine firstProc(i)
    integer, intent(in) :: i !< integer argument
  end subroutine

  subroutine secondProc(r)
    real, intent(out) :: r
  end subroutine

  subroutine otherProc(someArg)
    character(len=*), intent(in) :: someArg
  end subroutine

end module myModule
