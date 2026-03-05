// objective: test Objective-C interface with generics and base class on separate line
// check: interface_r_c_t_generic_delegate_splitter.xml
// config: EXTENSION_MAPPING = h=Objective-C

/**
 * A generic delegate splitter.
 */
@interface RCTGenericDelegateSplitter<DelegateT>
    : NSObject

/** add a delegate */
- (void)addDelegate:(DelegateT)delegate;
/** remove a delegate */
- (void)removeDelegate:(DelegateT)delegate;

@end
