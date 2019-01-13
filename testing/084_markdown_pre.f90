! // objective: test <pre> in Fortran, no translation of markdown
! // check: 084__markdown__pre_8f90.xml
!> \file

!> subr1
!><pre>
!>                     ___________________________
!></pre>
subroutine subr1()
end subroutine

!> subr2
!><pre>
!>                     ___________________________
!></pre>
!>
subroutine subr2()
end subroutine
