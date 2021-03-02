//
//  URLSessionProtocol.h
//  CachedWebView
//
//  Created by Mark on 16/5/19.
//
//

#import <Foundation/Foundation.h>

extern NSString *const KProtocolHttpHeadKey;

@interface URLSessionProtocol : NSURLProtocol
@property (nonatomic, assign) void* laya8CacheData;
-(void)load;
-(void)onCacheHit:(const void*)buffer length:(int)len;
@end
