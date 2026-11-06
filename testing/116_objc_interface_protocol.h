// objective: test ObjC interface->protocol base parsing does not leak previous base class name
// check: interface_myinterface.xml
// check: protocol_myprotocol-p.xml
// config: EXTENSION_MAPPING = h=Objective-C

#import <Foundation/Foundation.h>

@interface Myinterface : NSObject
@end

@protocol Myprotocol <NSCopying>
@end
