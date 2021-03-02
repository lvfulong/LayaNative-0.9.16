//
//  URLSessionProtocol.m
//  CachedWebView
//
//  Created by Mark on 16/5/19.
//
//

#import "URLSessionProtocol.h"
#import <objc/runtime.h>
#import <CommonCrypto/CommonDigest.h>
#include "downloadCache/JCServerFileCache.h"
#include "resource/JCFileResManager.h"
#include "layaWebviewCacheManager.h"
#import <MobileCoreServices/MobileCoreServices.h>

NSString *const KProtocolHttpHeadKey = @"KProtocolHttpHeadKey";


@interface NSURLRequest(MutableCopyWorkaround)
- (id)mutableCopyWorkaround;
@end


@interface URLSessionProtocol ()<NSURLSessionDataDelegate>
@property (nonatomic, strong) NSURLSession *session;
@property (nonatomic, strong) NSURLSessionDataTask *downloadTask;
@property (nonatomic, strong) NSURLResponse *response;
@property (nonatomic, strong) NSMutableData *cacheData;
@end

@implementation NSURLRequest (MutableCopyWorkaround)

-(id)mutableCopyWorkaround {
    
    NSMutableURLRequest *mutableURLRequest = [[NSMutableURLRequest alloc] initWithURL:[self URL]
                                                                          cachePolicy:[self cachePolicy]
                                                                      timeoutInterval:[self timeoutInterval]];
    [mutableURLRequest setAllHTTPHeaderFields:[self allHTTPHeaderFields]];
    if ([self HTTPBodyStream]) {
        [mutableURLRequest setHTTPBodyStream:[self HTTPBodyStream]];
    } else {
        [mutableURLRequest setHTTPBody:[self HTTPBody]];
    }
    [mutableURLRequest setHTTPMethod:[self HTTPMethod]];
    
    return mutableURLRequest;
}

@end

@implementation URLSessionProtocol

+ (void)initialize
{
    if (self == [URLSessionProtocol class])
    {
    }
}
- (NSURLSession *)session {
    
    if (!_session) {
        _session = [NSURLSession sessionWithConfiguration:[NSURLSessionConfiguration defaultSessionConfiguration] delegate:self delegateQueue:[NSOperationQueue mainQueue]];
    }
    return _session;
}

#pragma mark - override
- (instancetype)initWithRequest:(NSURLRequest *)request cachedResponse:( NSCachedURLResponse *)cachedResponse client:( id <NSURLProtocolClient>)client
{
    self = [super initWithRequest:request cachedResponse:cachedResponse client:client];
    if( self != nil )
    {
        self.laya8CacheData = new laya::Laya8CacheData;
        
        std::shared_ptr<laya::Laya8Cache> pGameCache = layaWebviewCacheManager::getInstance()->getGameCache();
        std::shared_ptr<laya::Laya8Cache> pHallCache = layaWebviewCacheManager::getInstance()->getHallCache();
        
        if (pHallCache->isMyRequest(request.URL.absoluteString.UTF8String)){
            ((laya::Laya8CacheData*)self.laya8CacheData)->laya8Cache = pHallCache;
        }
        else if(pGameCache->isMyRequest(request.URL.absoluteString.UTF8String)){
            ((laya::Laya8CacheData*)self.laya8CacheData)->laya8Cache = pGameCache;
        }
        
        return self;
    }
    return nil;
}
+(BOOL)canInitWithRequest:(NSURLRequest *)request {
    
    if ([request valueForHTTPHeaderField:KProtocolHttpHeadKey] == nil) {
        //拦截请求头中包含KProtocolHttpHeadKey的请求
        return YES;
    }
    NSLog(@"NO request url = %@://%@:%@%@",request.URL.scheme,request.URL.host,request.URL.port,request.URL.path);
    return NO;
}

+ (NSURLRequest *)canonicalRequestForRequest:(NSURLRequest *)request {
    
    return request;
}

+ (BOOL)requestIsCacheEquivalent:(NSURLRequest *)a toRequest:(NSURLRequest *)b {
    
    return [super requestIsCacheEquivalent:a toRequest:b];
}

- (void)startLoading {
    NSLog(@"startLoading url = %@://%@:%@%@",self.request.URL.scheme,self.request.URL.host,self.request.URL.port,self.request.URL.path);

    if (auto pCache = ((laya::Laya8CacheData*)self.laya8CacheData)->laya8Cache.lock()){
        layaWebviewCacheManager::getInstance()->setCurrentProtocol(self);
        laya::JCFileRes* pRes = pCache->getFileResManager()->getRes(self.request.URL.absoluteString.UTF8String);
        layaWebviewCacheManager::getInstance()->setOnHitCacheCallBack(pRes);
    }
    else{
        [self load];
    }
}

- (void)stopLoading {
    [self.downloadTask cancel];
    self.cacheData = nil;
    self.downloadTask = nil;
    self.response = nil;
    if (self.laya8CacheData){
        delete (laya::Laya8CacheData*)self.laya8CacheData;
        self.laya8CacheData = nil;
    }
    
}

-(void)dealloc
{
    if (self.laya8CacheData){
        delete (laya::Laya8CacheData*)self.laya8CacheData;
        self.laya8CacheData = nil;
    }
}
#pragma mark - session delegate

/*- (void)URLSession:(NSURLSession *)session task:(NSURLSessionTask *)task willPerformHTTPRedirection:(NSHTTPURLResponse *)response newRequest:(NSURLRequest *)request completionHandler:(void (^)(NSURLRequest * _Nullable))completionHandler {
    
    //处理重定向问题
    if (response != nil) {
        NSMutableURLRequest *redirectableRequest = [request mutableCopyWorkaround];
        TURLProtocolCacheData *cacheData = [[TURLProtocolCacheData alloc] init];
        cacheData.data = self.cacheData;
        cacheData.response = response;
        cacheData.redirectRequest = redirectableRequest;
        [NSKeyedArchiver archiveRootObject:cacheData toFile:[self p_filePathWithUrlString:request.URL.absoluteString]];
        
        [self.client URLProtocol:self wasRedirectedToRequest:redirectableRequest redirectResponse:response];
        completionHandler(request);
        
    } else {
        
        completionHandler(request);
    }
}*/

- (void)URLSession:(NSURLSession *)session dataTask:(NSURLSessionDataTask *)dataTask didReceiveResponse:(NSURLResponse *)response completionHandler:(void (^)(NSURLSessionResponseDisposition))completionHandler {
    
    [self.client URLProtocol:self didReceiveResponse:response cacheStoragePolicy:NSURLCacheStorageNotAllowed];
    // 允许处理服务器的响应，才会继续接收服务器返回的数据
    completionHandler(NSURLSessionResponseAllow);
    self.cacheData = [NSMutableData data];
    self.response = response;
}

-  (void)URLSession:(NSURLSession *)session dataTask:(NSURLSessionDataTask *)dataTask didReceiveData:(NSData *)data {
    //下载过程中
    [self.client URLProtocol:self didLoadData:data];
    [self.cacheData appendData:data];
    
}

- (void)URLSession:(NSURLSession *)session task:(NSURLSessionTask *)task didCompleteWithError:(NSError *)error {
    //下载完成之后的处理
    if (error) {
        NSLog(@"error url = %@ reason [%@]",task.currentRequest.URL.absoluteString,error.localizedFailureReason);
        [self.client URLProtocol:self didFailWithError:error];
    } else {
        //将数据的缓存归档存入到本地文件中
        NSLog(@"ok url = %@://%@:%@%@",task.currentRequest.URL.scheme,task.currentRequest.URL.host,task.currentRequest.URL.port,task.currentRequest.URL.path);
        if (auto p = ((laya::Laya8CacheData*)self.laya8CacheData)->laya8Cache.lock()){
            ((laya::Laya8CacheData*)self.laya8CacheData)->call((const char*)self.cacheData.mutableBytes,(int)self.cacheData.length,200); //todo
        }
        [self.client URLProtocolDidFinishLoading:self];
    }
}
-(void)load
{
    NSLog(@"from download url = %@://%@:%@%@",self.request.URL.scheme,self.request.URL.host,self.request.URL.port,self.request.URL.path);
    NSMutableURLRequest *request = [self.request mutableCopyWorkaround];
    request.cachePolicy = NSURLRequestReloadIgnoringLocalCacheData;
    [request setValue:@"test" forHTTPHeaderField:KProtocolHttpHeadKey];
    self.downloadTask = [self.session dataTaskWithRequest:request];
    [self.downloadTask resume];

}

-(NSString *)getMIMETypeWithCAPIAtUrl:(NSString *)url
{
    CFStringRef UTI = UTTypeCreatePreferredIdentifierForTag(kUTTagClassFilenameExtension, (__bridge CFStringRef)[url pathExtension], NULL);
    CFStringRef MIMEType = UTTypeCopyPreferredTagWithClass (UTI, kUTTagClassMIMEType);
    CFRelease(UTI);
    if (!MIMEType) {
        return @"application/octet-stream";
    }
    return (__bridge NSString *)(MIMEType);
}

-(void)onCacheHit:(const void*)buffer length:(int)len
{
    NSLog(@"from cache url = %@//:%@:%@%@",self.request.URL.scheme,self.request.URL.host,self.request.URL.port,self.request.URL.path);
    NSData* data = [[NSData alloc] initWithBytes:buffer length:len];
    NSString *url = self.request.URL.absoluteString;//请求的链接
    

    NSURLResponse *response = [[NSURLResponse alloc] initWithURL:[[self request] URL]
                                                        MIMEType:[self getMIMETypeWithCAPIAtUrl:url]
                                            expectedContentLength:-1
                                                textEncodingName:nil];
        
    [self.client URLProtocol:self didReceiveResponse:response cacheStoragePolicy:NSURLCacheStorageNotAllowed];
    [self.client URLProtocol:self didLoadData:data];
    [self.client URLProtocolDidFinishLoading:self];
}
@end
