!// objective: test different variants of interfaces in Fortran
!// check: namespacestrings.xml
!// check: interfacestrings_1_1append.xml
!// check: interfacestrings_1_1operator_07_0a_0a_08.xml
!// config: OPTIMIZE_FOR_FORTRAN=YES
!// config: WARN_IF_UNDOCUMENTED=NO

! $Id: Strings.f90 2203 2016-06-09 06:25:07Z ian $
! ff08 source code copyright 2012 M.E.G.M.S.  See LICENCE.txt for licence.
!> @file
!! Defines the Strings module.


!*******************************************************************************
!!
!> Routines for managing character strings, particularly one dimensional 
!! arrays (vectors) of strings.
!!
!! The String type and associated procedures exposed by this module conform 
!! to the typical interface for vectors:
!!
!! - The item component obtains the item for a particular element in the 
!!   vector.
!! - Append: add an object to the vector.
!! - Grow: create space for new objects at the end of the vector, but do not 
!!   define those objects.
!! - Shrink: remove objects from the end of the vector.

MODULE Strings
  
  IMPLICIT NONE
  
  PRIVATE
  
  !-----------------------------------------------------------------------------
  ! Expose module procedures and interfaces
  
  PUBLIC :: Append
  PUBLIC :: AppendIfUnique
  PUBLIC :: Find
  PUBLIC :: Join
  PUBLIC :: Grow
  PUBLIC :: Replace
  PUBLIC :: Len
  PUBLIC :: Shrink
  
  PUBLIC :: Pack
  
  PUBLIC :: OPERATOR(==)
  
  !-----------------------------------------------------------------------------
  ! The String derived type.
  
  !> Strings store default kind characters.
  INTEGER, PARAMETER :: ck = KIND('a')
  
  !> Element in an array of strings of varying length.
  TYPE, PUBLIC :: String
    !> The item for a particular element.
    CHARACTER(:,KIND=ck), ALLOCATABLE :: item
  END TYPE String
  
  !-----------------------------------------------------------------------------
  ! Interfaces
  
  !> Append a string or vector of strings to another vector of strings.
  INTERFACE Append
    MODULE PROCEDURE Append_
    MODULE PROCEDURE Append_scalar
    MODULE PROCEDURE Append_vector
  END INTERFACE Append
  
  !> Append a string or vector of strings to another vector of strings 
  !! if the items are not already in that vector.
  INTERFACE AppendIfUnique
    MODULE PROCEDURE AppendIfUnique_
    MODULE PROCEDURE AppendIfUnique_scalar
    MODULE PROCEDURE AppendIfUnique_vector
  END INTERFACE AppendIfUnique
  
  !> Function to find a string in a vector of strings.
  INTERFACE Find
    MODULE PROCEDURE Find_
  END INTERFACE Find
  
  !> Elemental comparison of two strings or of a string and a character 
  !! scalar.
  INTERFACE OPERATOR(==)
    MODULE PROCEDURE compare
    MODULE PROCEDURE compare_str
  END INTERFACE OPERATOR(==)
  
  !> Add undefined items to the end of the vector.
  INTERFACE Grow
    MODULE PROCEDURE Grow_
  END INTERFACE Grow
  
  !> Join an array of strings into one character string.
  INTERFACE Join
    MODULE PROCEDURE Join_
  END INTERFACE Join
  
  !> Function to pack an array into a vector.
  INTERFACE Pack
    MODULE PROCEDURE Pack_chvector
  END INTERFACE Pack
  
  !> Replace an item with other items
  INTERFACE Replace
    MODULE PROCEDURE Replace_
  END INTERFACE Replace
  
  !> Remove items from the end of the vector.
  INTERFACE Shrink
    MODULE PROCEDURE Shrink_
  END INTERFACE Shrink
  
  !> Add a single item to the end of the vector.
  INTERFACE Push
    MODULE PROCEDURE Append_
  END INTERFACE Push
  
  !> Get the length of a string.
  !
  ! It is not permitted to reference the generic name in specification 
  ! expressions prior to the LEN_ subprogram.  Be careful with the 
  ! LEN intrinsic!
  INTERFACE Len
    MODULE PROCEDURE Len_
  END INTERFACE Len
  
CONTAINS
  
  !*****************************************************************************
  !!
  !> Expands a vector of Strings with undefined items.
  !!
  !! @param[in]     vector            The vector to expand.
  !!
  !! @param[in]     count             Optional number of items to expand by.  
  !! If not present, the vector is expanded by one item.  If negative, the 
  !! vector will shrink.
  
  SUBROUTINE Grow_(vector, count)
    
    USE CharUtils
    
    !---------------------------------------------------------------------------
    ! Arguments
    
    TYPE(String), INTENT(INOUT), ALLOCATABLE :: vector(:)
    INTEGER, INTENT(IN), OPTIONAL :: count
    
    !---------------------------------------------------------------------------
    ! Local variables
    
    INTEGER :: local_count    ! Local copy of count
    INTEGER :: i              ! Vector index.
    
    ! Temporary for size change.
    TYPE(String), ALLOCATABLE :: tmp(:)
    
    !***************************************************************************
    
    IF (PRESENT(count)) THEN
      local_count = count
    ELSE
      local_count = 1
    END IF
    
    !---------------------------------------------------------------------------
    
    IF (local_count < 0) THEN
      CALL Shrink(vector, -local_count)
      RETURN
    END IF
    
    IF (.NOT. ALLOCATED(vector)) THEN
      ALLOCATE(tmp(local_count))
    ELSE
      ! Copy across existing items
      ALLOCATE(tmp(SIZE(vector)+local_count))
      DO i = 1, SIZE(vector)
        CALL MOVE_ALLOC(vector(i)%item, tmp(i)%item)
      END DO
    END IF
    
    CALL MOVE_ALLOC(tmp, vector)
    
  END SUBROUTINE Grow_
  
  
  !*****************************************************************************
  !!
  !> Shrinks a vector of Strings by removing items from the end.
  !!
  !! @param[in]     vector            The vector to expand.
  !!
  !! @param[in]     count             Optional number of items to shrink by.  
  !! If not present, the vector is shrunk by one item.  If negative, the 
  !! vector will grow.
  !!
  !! This is perhaps a more performance way of going:
  !! @code
  !! vector = vector(:SIZE(vector)-count)
  !! @endcode
  !!
  !! though it handles some edge cases nicely too.
  
  SUBROUTINE Shrink_(vector, count)
    
    !---------------------------------------------------------------------------
    ! Arguments
    
    TYPE(String), INTENT(INOUT), ALLOCATABLE :: vector(:)
    INTEGER, INTENT(IN), OPTIONAL :: count
    
    !---------------------------------------------------------------------------
    ! Local variables
    
    INTEGER :: local_count    ! Local copy of count
    INTEGER :: i              ! Vector index.
    
    ! Temporary for size change.
    TYPE(String), ALLOCATABLE :: tmp(:)
    
    !***************************************************************************
    
    IF (PRESENT(count)) THEN
      local_count = count
    ELSE
      local_count = 1
    END IF
    
    !---------------------------------------------------------------------------
    
    IF (local_count < 0) THEN
      CALL Grow(vector, local_count)
      RETURN
    END IF
    
    IF (.NOT. ALLOCATED(vector)) THEN
      ! Vector is already empty - leave it that way.
      RETURN
    ELSE
      ! Copy across existing items
      ALLOCATE(tmp(MAX(0,SIZE(vector)-local_count)))
      DO i = 1, SIZE(tmp)
        CALL MOVE_ALLOC(vector(i)%item, tmp(i)%item)
      END DO
    END IF
    
    CALL MOVE_ALLOC(tmp, vector)
    
  END SUBROUTINE Shrink_
  
  
  !*****************************************************************************
  !!
  !> Adds a single item to the end of the vector of Strings.
  !!
  !! @param[in]     vector            The vector of strings.
  !!
  !! @param[in]     item              The CHARACTER object to append as a 
  !! new item.
  
  SUBROUTINE Append_(vector, item)
    
    USE CharUtils
    
    !---------------------------------------------------------------------------
    ! Arguments
    
    TYPE(String), INTENT(INOUT), ALLOCATABLE :: vector(:)
    CHARACTER(*,KIND=ck), INTENT(IN) :: item
    
    !---------------------------------------------------------------------------
    ! Local variables
    
    INTEGER :: i              ! Vector index.
    
    ! Temporary for size change.
    TYPE(String), ALLOCATABLE :: tmp(:)
    
    !***************************************************************************
    
    IF (.NOT. ALLOCATED(vector)) THEN
      ALLOCATE(tmp(1))
    ELSE
      ALLOCATE(tmp(SIZE(vector)+1))
      DO i = 1, SIZE(vector)
        CALL MOVE_ALLOC(vector(i)%item, tmp(i)%item)
      END DO
    END IF
    tmp(SIZE(tmp))%item = item
    
    CALL MOVE_ALLOC(tmp, vector)
    
  END SUBROUTINE Append_
  
  
  !*****************************************************************************
  !!
  !> Adds a single item to the end of the vector of Strings.
  !!
  !! @param[in]     vector            The vector of strings.
  !!
  !! @param[in]     item              The scalar String object to append as a 
  !! new item.
  
  SUBROUTINE Append_scalar(vector, item)
    
    USE CharUtils
    
    !---------------------------------------------------------------------------
    ! Arguments
    
    TYPE(String), INTENT(INOUT), ALLOCATABLE :: vector(:)
    TYPE(String), INTENT(IN) :: item
    
    !---------------------------------------------------------------------------
    ! Local variables
    
    INTEGER :: i              ! Vector index.
    
    ! Temporary for size change.
    TYPE(String), ALLOCATABLE :: tmp(:)
    
    !***************************************************************************
    
    IF (.NOT. ALLOCATED(vector)) THEN
      ALLOCATE(tmp(1))
    ELSE
      ALLOCATE(tmp(SIZE(vector)+1))
      DO i = 1, SIZE(vector)
        CALL MOVE_ALLOC(vector(i)%item, tmp(i)%item)
      END DO
    END IF
    tmp(SIZE(tmp)) = item
    
    CALL MOVE_ALLOC(tmp, vector)
    
  END SUBROUTINE Append_scalar
  
  
  !*****************************************************************************
  !!
  !> Appends a vector of Strings to another vector of Strings.
  !!
  !! @param[in]     vector            The vector of Strings to be appended 
  !! to.
  !!
  !! @param[in]     items             The potential selection of Strings 
  !! to be appended.
  !!
  !! @param[in]     mask              A mask that designates which elements 
  !! from @a items will be appended.  If not present then all elements from 
  !! @a items will be appended.  If present must be the same size as @a items.
  !!
  !! Note that @a items is left unchanged.
  
  SUBROUTINE Append_vector(vector, items, mask)
    
    !---------------------------------------------------------------------------
    ! Arguments
    
    TYPE(String), INTENT(INOUT), ALLOCATABLE :: vector(:)
    TYPE(String), INTENT(IN) :: items(:)
    LOGICAL, INTENT(IN), OPTIONAL :: mask(:)
    
    !---------------------------------------------------------------------------
    ! Local variables
    
    INTEGER :: i              ! Vector index.
    INTEGER :: count          ! Number of elements to be appended.
    
    ! Temporary for size change.
    TYPE(String), ALLOCATABLE :: tmp(:)
    
    !***************************************************************************
    
    count = 0
    DO i = 1, SIZE(items)
      IF (PRESENT(mask)) THEN
        IF (.NOT. mask(i)) CYCLE
      END IF
      count = count + 1
    END DO
    
    IF (.NOT. ALLOCATED(vector)) THEN
      ALLOCATE(tmp(count))
    ELSE
      ALLOCATE(tmp(SIZE(vector) + count))
      DO i = 1, SIZE(vector)
        CALL MOVE_ALLOC(vector(i)%item, tmp(i)%item)
      END DO
    END IF
    
    count = SIZE(vector)
    DO i = 1, SIZE(items)
      IF (PRESENT(mask)) THEN
        IF (.NOT. mask(i)) CYCLE
      END IF
      count = count + 1
      tmp(count)%item = items(i)%item
    END DO
    
    CALL MOVE_ALLOC(tmp, vector)
    
  END SUBROUTINE Append_vector
  
  
  !*****************************************************************************
  !!
  !> Append an item to a vector of strings if the item is not already in the 
  !! vector.
  !!
  !! @param[in]     vector            The vector of Strings to be appended to.
  !!
  !! @param[in]     item              The item to be appended, if it is unique.
  !!
  !! Appends @a item to @a vector if @a item is not already in @a vector.
  
  SUBROUTINE AppendIfUnique_(vector, item)
    
    USE CharUtils
    
    !---------------------------------------------------------------------------
    ! Arguments
    
    TYPE(String), INTENT(INOUT), ALLOCATABLE :: vector(:)
    CHARACTER(*,KIND=ck), INTENT(IN) :: item
    
    !---------------------------------------------------------------------------
    ! Local variables
    
    INTEGER :: i              ! Vector index.
    
    !***************************************************************************
    
    DO i = 1, SIZE(vector)
      IF (vector(i)%item == item) RETURN ! already in vector.
    END DO
    
    ! If we get to here then item isn't in vector.
    CALL Append(vector, item)
    
  END SUBROUTINE AppendIfUnique_
  
  
  !*****************************************************************************
  !!
  !> Append an item to a vector of strings if the item is not already in the 
  !! vector.
  !!
  !! @param[in]     vector            The vector of Strings to be appended to.
  !!
  !! @param[in]     item              The scalar string to be appended, if it 
  !! is unique.
  !!
  !! Appends @a item to @a vector if @a item is not already in @a vector.
  
  SUBROUTINE AppendIfUnique_scalar(vector, item)
    
    USE CharUtils
    
    !---------------------------------------------------------------------------
    ! Arguments
    
    TYPE(String), INTENT(INOUT), ALLOCATABLE :: vector(:)
    TYPE(String), INTENT(IN) :: item
    
    !---------------------------------------------------------------------------
    ! Local variables
    
    INTEGER :: i              ! Vector index.
    
    !***************************************************************************
    
    DO i = 1, SIZE(vector)
      IF (vector(i) == item) RETURN ! already in vector.
    END DO
    
    ! If we get to here then item isn't in vector.
    CALL Append(vector, item)
    
  END SUBROUTINE AppendIfUnique_scalar
  
  
  !*****************************************************************************
  !!
  !> Append items from a vector of strings to another vector if those items 
  !! are not already in the vector to be appended to.
  !!
  !! @param[in]     vector            The vector of Strings to be appended 
  !! to.
  !!
  !! @param[in]     items             The vector of items to be appended 
  !! if they are not already in @a vector.
  
  SUBROUTINE AppendIfUnique_vector(vector, items)
    
    !---------------------------------------------------------------------------
    ! Arguments
    
    TYPE(String), INTENT(INOUT), ALLOCATABLE :: vector(:)
    TYPE(String), INTENT(IN) :: items(:)
    
    !---------------------------------------------------------------------------
    ! Local variables
    
    INTEGER :: i1             ! Index into items.
    INTEGER :: i2             ! Index into vector.
    
    ! Mask of the items that will be appended.
    LOGICAL :: mask(SIZE(items))
    
    !***************************************************************************
    
    outer_loop: DO i1 = 1, SIZE(items)
      DO i2 = 1, SIZE(vector)
        IF (items(i1)%item == vector(i2)%item) THEN
          mask(i1) = .TRUE.
          CYCLE outer_loop
        END IF
      END DO
      mask(i1) = .FALSE.
    END DO outer_loop
    
    CALL Append(vector, items, mask)
    
  END SUBROUTINE AppendIfUnique_vector
  
  
  !*****************************************************************************
  !!
  !> Find the index in a vector of Strings of a particular character string.
  !!
  !! @param[in]     vector            The vector of Strings to search.
  !!
  !! @param[in]     item              The string to search for.
  !!
  !> @returns the element of @a vector that corresponds to @a item.
  
  FUNCTION Find_(vector, item) RESULT(i)
    
    !---------------------------------------------------------------------------
    ! Characteristics
    
    TYPE(String), INTENT(IN) :: vector(:)
    CHARACTER(*,KIND=ck), INTENT(IN) :: item
    
    ! Function result
    INTEGER :: i
    
    !***************************************************************************
    
    DO i = 1, SIZE(vector)
      IF (vector(i)%item == item) RETURN
    END DO
    i = 0
    
  END FUNCTION Find_
  
  
  !*****************************************************************************
  !!
  !> Implementation of OPERATOR(==) - compare two Strings.
  !!
  !! @param[in]     lhs               The left hand string.
  !!
  !! @param[in]     rhs               The right hand string.
  !!
  !! @return .TRUE. if the strings compare equal (Fortran rules), .FALSE. 
  !! otherwise.
  
  ELEMENTAL FUNCTION compare(lhs, rhs) RESULT(b)
    
    !---------------------------------------------------------------------------
    ! Characteristics
    
    TYPE(String), INTENT(IN) :: lhs
    TYPE(String), INTENT(IN) :: rhs
    
    ! Function result
    LOGICAL :: b
    
    !***************************************************************************
    
    b = lhs%item == rhs%item
    
  END FUNCTION compare
  
  
  !*****************************************************************************
  !!
  !> Implementation of OPERATOR(==) - compare a String with a character 
  !! scalar.
  !!
  !! @param[in]     lhs               The string to compare.
  !!
  !! @param[in]     rhs               The character scalar to compare.
  !!
  !! @returns .TRUE. if string and the character variable compare equal 
  !! (Fortran rules), .FALSE. otherwise.
  
  ELEMENTAL FUNCTION compare_str(lhs, rhs) RESULT(b)
    
    !---------------------------------------------------------------------------
    ! Characteristics
    
    TYPE(String), INTENT(IN) :: lhs
    CHARACTER(*,KIND=ck), INTENT(IN) :: rhs
    
    ! Function result
    LOGICAL :: b
    
    !***************************************************************************
    
    b = lhs%item == rhs
    
  END FUNCTION compare_str
  
  
  !*****************************************************************************
  !!
  !> Replace an item in a vector by one or more items.
  !!
  !! @param[in,out] vector            The vector of Strings to operate on.
  !!
  !! @param[in]     idx               The index of the item to replace.
  !!
  !! @param[in]     insert_items      The vector of items to insert.
  
  SUBROUTINE Replace_(vector, idx, insert_items)
    
    !---------------------------------------------------------------------------
    ! Arguments
    
    TYPE(String), INTENT(INOUT), ALLOCATABLE :: vector(:)
    INTEGER, INTENT(IN) :: idx
    TYPE(String), INTENT(IN) :: insert_items(:)
    
    !---------------------------------------------------------------------------!
    ! Local variables
    
    ! Temporary for changing the size of vector.
    TYPE(String), ALLOCATABLE :: tmp(:)
    
    !***************************************************************************
    
    ALLOCATE(tmp(SIZE(vector) - 1 + SIZE(insert_items)))
    
    tmp(:idx-1) = vector(:idx-1)
    tmp(idx:idx+SIZE(insert_items)-1) = insert_items
    tmp(idx+SIZE(insert_items):) = vector(idx:)
    
  END SUBROUTINE Replace_
  
  
  !*****************************************************************************
  !!
  !> Pack function specialized for TYPE(String) as a replacement for 
  !! the intrinsic.
  !!
  !! @param[in]     array             The array of things to pack.
  !!
  !! @param[in]     mask              Shall be the same size as @a array.  The 
  !! intrinsic form simply requires conformability, but that's a trivial case 
  !! to implement in client code.
  !!
  !! @param[in]     vector            Optional.  Shall have at least as many 
  !! elements as there are .TRUE. elements in @a mask.
  !!
  !! @returns If @a vector is present, the result size is that of @a vector, 
  !! otherwise the result size is the number <i>t</i> of true elements in 
  !! @a mask.
  !!
  !! Element <i>i</i> of the result is the element of @a array that 
  !! corresponds to the <i>i</i>th true element of @a mask, taking elements in 
  !! array element order, for <i>i</i> = 1, 2, ..., <i>t</i>.  If @a vector is 
  !! present and has size <i>n</i> > <i>t</i>, element <i>i</i> of the result 
  !! has the value @a vector(<i>i</i>), for <i>i</i> = <i>t</i> + 1, ..., 
  !! <i>n</i>.
  !!
  !! Delete and revert to the intrinsic once the ifort 12.1.0 bug reported at 
  !!   http://software.intel.com/en-us/forums/showthread.php?t=85834
  !! has been fixed.
  
  FUNCTION Pack_chvector(array, mask, vector) RESULT(res)
    
    !---------------------------------------------------------------------------
    ! Characteristics
    
    TYPE(String), INTENT(IN) :: array(:)
    LOGICAL, INTENT(IN) :: mask(:)
    TYPE(String), INTENT(IN), OPTIONAL :: vector(:)
    
    ! Function result
    TYPE(String), ALLOCATABLE :: res(:)
    
    !---------------------------------------------------------------------------
    ! Local variables
    
    INTEGER :: ia             ! Array index
    INTEGER :: im             ! Mask index
    
    !***************************************************************************
    
    IF (PRESENT(vector)) THEN
      ! There is a requirement that SIZE(vector) be greater than or equal to 
      ! COUNT(mask).
      ALLOCATE(res(SIZE(vector)))
    ELSE
      ALLOCATE(res(COUNT(mask)))
    END IF
    
    ! Copy the items across to the result under the control of the mask.
    ia = 1
    DO im = 1, SIZE(mask)
      IF (mask(im)) THEN
        res(ia) = array(im)
        ia = ia + 1
      END IF
    END DO
    
    ! ia will come out of the above loop pointing to the next empty space 
    ! in res, if it exists.  SIZE(vector) == SIZE(res), so if ia > SIZE(res) 
    ! then the sequence is empty on both sides of the assignment.
    IF (PRESENT(vector)) res(ia:) = vector(ia:)
    
  END FUNCTION Pack_chvector
  
  
  !*****************************************************************************
  !!
  !> Get the length of a string.
  !!
  !! @param[in]     str
  !!
  !! This is just a forwarding function to the LEN intrinsic.
  
  ELEMENTAL FUNCTION Len_(str)
    
    !---------------------------------------------------------------------------
    ! Characteristics
    
    TYPE(String), INTENT(IN) :: str
    
    ! Function result.
    INTEGER :: Len_
    
    !***************************************************************************
    
    Len_ = LEN(str%item)
    
  END FUNCTION Len_
  
  
  !*****************************************************************************
  !!
  !> Join a vector of strings into a single character variable.
  !!
  !! @param[in]     vector            The vector of strings.
  !!
  !! @param[in]     separator         Optional separator to appear between 
  !! each element in the string.  If not present then ', ' is used.
  !!
  !! @returns The joined string.
  
  FUNCTION Join_(vector, separator) RESULT(str)
    
    !---------------------------------------------------------------------------
    ! Characteristics
    
    TYPE(String), INTENT(IN) :: vector(:)
    CHARACTER(*), INTENT(IN), OPTIONAL :: separator
    
    ! Function result.
    CHARACTER(:), ALLOCATABLE :: str
    
    !---------------------------------------------------------------------------
    ! Locals
    
    INTEGER :: i              ! vector index.
    
    !***************************************************************************
    
    IF (SIZE(vector) == 0) THEN
      str = ''
      RETURN
    END IF
    
    str = vector(1)%item
    DO i = 2, SIZE(vector)
      IF (PRESENT(separator)) THEN
        str = str // separator // vector(i)%item
      ELSE
        str = str // ', ' // vector(i)%item
      END IF
    END DO
    
  END FUNCTION Join_
  
END MODULE Strings
