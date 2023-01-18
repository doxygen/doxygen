!// objective: test different variants of interfaces in Fortran
!// check: namespacemymodule.xml
!// check: interfacemymodule_1_1genericproc.xml
!// check: interfacemymodule_1_1externalproc.xml
!// check: interfacemymodule_1_1abstractproc.xml
!// check: interfacemymodule_1_1operator_07_8cross_8_08.xml
!// check: interfacemymodule_1_1operator_07_2_2_08.xml
!// config: OPTIMIZE_FOR_FORTRAN=YES
!// config: WARN_IF_UNDOCUMENTED=NO
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

  ! meaning 2 again, this time as user defined operator
  interface operator(.cross.)
    module procedure cross_product
  end interface

  ! meaning 2 again, this time for overloading an existing operator
  interface operator(//)
    module procedure cross_product
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

  pure function cross_product(v, w)
    real, intent(in) :: v(3), w(3)
    real :: cross_product(3)

    !cross_product = ...
  end function

end module myModule
