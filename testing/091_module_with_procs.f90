!// objective: test module procedures with arguments
!// check: namespacemymodule.xml
!// config: OPTIMIZE_FOR_FORTRAN=YES

module myModule
  implicit none
  public

contains

  !> comment on firstProc
  subroutine firstProc(i)
    integer, intent(in) :: i !< comment on i of firstProc
  end subroutine

  !> comment on secondProc_fun
  function secondProc_fun(r) result(res)
    real, intent(out) :: r !< comment on r of secondProc_fun
    integer :: Res         !< comment on result Res of secondProc_fun
  end function secondProc_fun

  !> comment on anotherProc
  subroutine anotherProc(someArg)
    character(len=*), intent(in) :: someArg !< comment on someArg of anotherProc
  end subroutine

end module myModule

!> comment on hello
subroutine hello(world)
  character(len=*), intent(in) :: world !< comment on world of hello
end subroutine
