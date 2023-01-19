!// objective: test type bound procedures
!// check: structmymodule_1_1t1.xml
!// config: OPTIMIZE_FOR_FORTRAN=YES
!// config: WARN_IF_UNDOCUMENTED=NO

module myModule
  implicit none
  private

  public :: T1

  type T1
    integer :: publicVariable
  contains
    !> brief doc
    procedure :: firstProc
    procedure, non_overridable :: secondProc => secondProc_impl
    procedure, nopass :: staticProc
    procedure, pass(this) :: redundantPass => firstProc !< another brief doc
    generic :: overloadedProc => firstProc, secondProc
    final :: destructor
  end type T1

contains

  !> short doc
  subroutine firstProc(this, i)
    class(T1) :: this !< this pointer
    integer, intent(in) :: i !< integer argument
  end subroutine

  subroutine secondProc_impl(this, r)
    class(T1) :: this
    real, intent(out) :: r
  end subroutine

  subroutine staticProc(someArg)
    integer :: someArg
  end subroutine

  elemental subroutine destructor(this)
    type(T1), intent(inout) :: this
  end subroutine

end module myModule
