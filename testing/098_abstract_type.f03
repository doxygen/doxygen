!// objective: test abstract types
!// check: structmymodule_1_1t1.xml
!// config: OPTIMIZE_FOR_FORTRAN=YES
!// config: WARN_IF_UNDOCUMENTED=NO

module myModule
  implicit none
  private

  public :: T1

  type, abstract :: T1
  contains
    !> brief doc
    procedure(AbstractProc), deferred :: deferredProc
  end type T1

  abstract interface
    subroutine AbstractProc(this, i)
      import T1
      class(T1), intent(inout) :: this
      integer, intent(in) :: i
    end subroutine
  end interface

end module myModule
