// objective: test <summay> against @brief
// check: 080__xml__summary_8cs.xml
// check: class_my_name_space_doesnt_1_1_my_class_which_doesnt.xml
// check: class_my_name_space_works_1_1_my_class_which_works.xml
// check: namespace_my_name_space_doesnt.xml
// check: namespace_my_name_space_works.xml
// check: namespace_system.xml
// check: namespace_system_1_1_runtime_1_1_interop_services.xml
// check: namespace_system_1_1_text.xml
// config: REPEAT_BRIEF=NO
using System;
using System.Text;
using System.Runtime.InteropServices;

/// \brief The workspace which works
namespace MyNameSpaceWorks
{
    /// \brief The class which works
    public class MyClassWhichWorks
    {

        public const int MyConstant = 1; ///< This is the brief description. Nothing else
    }
}
	
/// <summary>The namespace which doesn't</summary>
namespace MyNameSpaceDoesnt
{
    /// <summary>The class which doesn't</summary>
    public class MyClassWhichDoesnt
    {

        /// <summary>
        /// The idea is that this is the brief description without a detail section
        /// </summary>
        public const int MyOtherConstant = 1;
    }
}
