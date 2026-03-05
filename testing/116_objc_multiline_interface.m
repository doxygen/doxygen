// objective: test Objective-C interface with generics and base class on separate line
// check: interface_foo.xml
// config: EXTENSION_MAPPING = h=Objective-C

/**
 * A generic container.
 */
@interface Foo<T>
    : NSObject

/** add an item */
- (void)addItem:(T)item;
/** remove an item */
- (void)removeItem:(T)item;

@end
