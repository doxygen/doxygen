// objective: test if A<B>::C within backticks is properly handled
// check: struct_n_1_1allocator__traits.xml
namespace N
{
  /**
   * @brief  Uniform interface to all allocator types.
   */
  template<typename _Alloc> struct allocator_traits
  {
    /**
     * @brief   The allocator's const pointer type.
     *
     *
     * `Alloc::const_pointer` if that type exists, otherwise
     * `pointer_traits<pointer>::rebind<const value_type>`.
     *
    */
    using const_pointer = typename _Ptr<__c_pointer, const value_type>::type;
  };
}
