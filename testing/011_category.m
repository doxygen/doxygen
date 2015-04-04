// objective: test the \interface and \category command
// check: category_integer_07_arithmetic_08.xml
// check: interface_integer.xml
# import <objc/Object.h>
 
@interface Integer : Object {
 /** data member */
 int integer;
}
 
/** getter */
- (int) integer;
/** setter */
- (id) integer: (int) _integer;
@end

@interface Integer (Arithmetic)
/** add operation */
- (id) add: (Integer *) addend;
/** subtract operation */
- (id) sub: (Integer *) subtrahend;
@end

/** \interface Integer
 *  An interface 
 */

/** \category Integer(Arithmetic)
 *  A category
 */
