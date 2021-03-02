//
//  LayaWKWebview.m
//  LayaWKWebview
//
//  Created by helloworldlv on 2018/3/23.
//  Copyright © 2018年 render. All rights reserved.
//

#import "LayaWKWebview.h"
#import "GCDWebServer.h"
#import "GCDWebServerDataResponse.h"
#import "GCDWebServerFunctions.h"
#include <string>
#include "LayaCache/LayaCacheManager.h"
extern std::string gRedistPath;
extern std::string gAssetRootPath;
@implementation LayaWKWebview
{
    NSString* _url;
    int _hostPort;
    GCDWebServer* _GCDWebServer;
}
- (instancetype)initWithWebview:(WKWebView*)webview url:(NSString*)url
{
    if ((self = [super init]))
    {
        _webview = webview;
        _url = url;
        _hostPort = 0;
        NSURL* pUrl = [NSURL URLWithString:url];
        NSURLRequest* pUrlRequest = [NSURLRequest requestWithURL:pUrl];
        [_webview loadRequest:pUrlRequest];
    }
    return self;
}
- (instancetype)initWithWebview:(WKWebView*)webview url:(NSString*)url hostPort:(int)port;
{
    if ((self = [super init]))
    {
        _webview = webview;
        _url = url;
        _hostPort = port;
         [self updateDcc:_url];
    }
    return self;
}
-(NSString *)convertDataToHexStr:(NSData *)data
{
    if (!data || [data length] == 0)
    {
        return @"";
    }
    NSMutableString *string = [[NSMutableString alloc] initWithCapacity:[data length]];
    [data enumerateByteRangesUsingBlock:^(const void *bytes, NSRange byteRange, BOOL *stop)
    {
        unsigned char *dataBytes = (unsigned char*)bytes;
        for (NSInteger i = byteRange.length - 1; i >=0 ; i--)
        {
            NSString *hexStr = [NSString stringWithFormat:@"%x", (dataBytes[i]) & 0xff];
            if ([hexStr length] == 2)
            {
                [string appendString:hexStr];
            } else
            {
                [string appendFormat:@"0%@", hexStr];
            }
        }
    }];
    return string;
}
-(NSString*)getLaunchUrlDomain:(NSString*)url
{
    NSString* domain = [[NSString alloc] initWithString:url];
    domain = [domain stringByReplacingOccurrencesOfString:@"?" withString:@""];
    domain = [[NSURL alloc] initWithString:domain].URLByDeletingLastPathComponent.absoluteString;
    return domain;
}
-(NSString*)getLayaBoxUrl
{
    return [NSString stringWithFormat:@"http://127.0.0.1:%lu/s/%@", (unsigned long)(_GCDWebServer.port),_url];
}
-(NSString*) getResourcePath
{
    return [[NSBundle mainBundle] resourcePath];
}
-(NSString*) getRootCachePath
{
    NSString* sAppDirctory = NSHomeDirectory();
    NSString* sDownloadRootPath = [ NSString stringWithFormat: @"%@/Library/Caches/", sAppDirctory ];
    return sDownloadRootPath;
}
- (void)updateDcc:(NSString*)url
{
    gRedistPath = [[self getRootCachePath] cStringUsingEncoding:NSUTF8StringEncoding];
    gAssetRootPath= [[self getResourcePath] cStringUsingEncoding:NSUTF8StringEncoding];
    gAssetRootPath += "/cache/";
    NSLog(@"[dcc] update dcc start [%@] ...",url);
    NSString* urlpath = [self getLaunchUrlDomain:url];
    /*NSString* urlpath = [[NSString alloc] initWithString:url];
    urlpath = [urlpath stringByReplacingOccurrencesOfString:@"?" withString:@""];
    urlpath = [[NSURL alloc] initWithString:urlpath].URLByDeletingLastPathComponent.absoluteString;*/
    NSLog(@"[dcc] [%@]",url);
    std::string curassets = laya::LayaCacheManager::getInstance()->preUpdateDcc(gRedistPath,urlpath.UTF8String);
    NSURLSession *session = [NSURLSession sessionWithConfiguration:[NSURLSessionConfiguration defaultSessionConfiguration] delegate:nil delegateQueue:[NSOperationQueue mainQueue]];
    NSString* asidUrl = [[NSString alloc] initWithFormat:@"%@update/assetsid.txt?rand=%f",urlpath,(rand()/(float)RAND_MAX) * [NSDate date].timeIntervalSince1970];
    NSLog(@"[dcc] %@",asidUrl);
            
    NSURLSessionDataTask *task = [session dataTaskWithURL:[NSURL URLWithString:asidUrl] completionHandler:^(NSData *  data, NSURLResponse * response, NSError * error)
    {
        NSLog(@"[dcc] assetsid.txt download complete");
        NSString* assetsidStr = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
        if (error || (assetsidStr == nil) || [assetsidStr isEqualToString:@""])
        {
            if (error)
            {
                NSLog(@"[dcc] assetsid.txt download error [%@] [%@] [%@]",error.localizedFailureReason,error.localizedDescription,error.localizedRecoverySuggestion);
            }
            laya::LayaCacheManager::getInstance()->updateDccClearAssetsid(gRedistPath,urlpath.UTF8String);
            [self requestGame:url];
        }
        else
        {
            if ([assetsidStr UTF8String] != curassets)
            {
                __block NSString* txtdcc = @"";
                NSURLSession *session = [NSURLSession sessionWithConfiguration:[NSURLSessionConfiguration defaultSessionConfiguration] delegate:nil delegateQueue:[NSOperationQueue mainQueue]];
                NSString* asidUrl = [[NSString alloc] initWithFormat:@"%@update/filetable.bin",urlpath];
                NSURLSessionDataTask *task = [session dataTaskWithURL:[NSURL URLWithString:asidUrl] completionHandler:^(NSData *  data, NSURLResponse * response, NSError * error)
                {
                                                          
                    NSLog(@"[dcc] filetable.bin download complete");
                    if (error)
                    {
                        NSLog(@"[dcc] filetable.bin download error");
                    }
                    if (data.length % 8 != 0)
                    {
                        NSLog(@"[dcc] filetable.bin length error");
                    }
                    else
                    {
                        int32_t* v = (int32_t*)data.bytes;
                        if (v[0] != 0xffeeddcc || v[1] != 1)
                        {
                            NSLog(@"dcc.bin file err!");
                        }
                        else
                        {
                            if (v[2] == 0x00ffffff)
                            {
                                int stp = (4 + 8) / 2;
                                NSData* md5 = [data subdataWithRange:NSMakeRange(4*4, 8*4)];
                                NSString* so = [[NSString alloc] initWithData:md5 encoding:NSUTF8StringEncoding];
                                NSLog(@"--------------------------------------------");
                                NSLog(@"so=%@",so);
                                NSLog(@"netid=%@",assetsidStr);
                                if ([so isEqualToString:assetsidStr])
                                {
                                    for (NSUInteger ii = stp, isz = data.length / (2*4); ii < isz; ii++)
                                    {
                                        NSData* data1 = [data subdataWithRange:NSMakeRange(ii * 2 * 4, 4)];
                                        NSData* data2 = [data subdataWithRange:NSMakeRange(ii * 2 * 4 + 4, 4)];
                                        NSString* dccstr = [[NSString alloc] initWithFormat:@"%@ %@\n",[self convertDataToHexStr:data1],[self convertDataToHexStr:data2]];
                                        txtdcc = [txtdcc stringByAppendingString:dccstr];
                                     }
                                 }
                            }
                            else
                            {
                               NSLog(@"error old format unsupport");
                            }
                        }
                    }
                    if ([txtdcc length] > 0)
                    {
                        laya::LayaCacheManager::getInstance()->doUpdateDcc(gRedistPath,urlpath.UTF8String,txtdcc.UTF8String,assetsidStr.UTF8String);
                    }
                    else
                    {
                    }
                    [self requestGame:url];
                }];
                [task resume];
            }
            else
            {
                [self requestGame:url];
            }
        }
    }];
    [task resume];
}
-(void)requestGame:(NSString*)url
{
    // Create server
    _GCDWebServer = [[GCDWebServer alloc] init];
    
    // Add a handler to respond to GET requests on any URL
    [_GCDWebServer addDefaultHandlerForMethod:@"GET"
                                 requestClass:[GCDWebServerRequest class]
                            asyncProcessBlock:^(GCDWebServerRequest* request, GCDWebServerCompletionBlock completionBlock) {
                                NSString* urlOCStr = [[request.path stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceCharacterSet]]substringFromIndex:3];
                                const char* pURL = [urlOCStr stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding].UTF8String;
                                laya::LayaCacheManager::getInstance()->handleRequest(pURL, [completionBlock,request](void* pData, int length) {
                                    NSData *data = [[NSData alloc] initWithBytes:pData length:length];
                                    NSString* contentType = GCDWebServerGetMimeTypeForExtension([[request path] pathExtension], nil);
                                    GCDWebServerDataResponse* response = [GCDWebServerDataResponse responseWithData:data contentType:contentType];
                                    completionBlock(response);
                                    
                                },[completionBlock](int errCode){
                                      //NSLog(@"dowlonad error [%d]",errCode);
                                      completionBlock(nil);
                                  });
                              }];
    NSError *error = nil;
    int httpPort = (_hostPort != 0) ? _hostPort :12344;
    
    NSMutableDictionary *options = [NSMutableDictionary dictionary];
    [options setObject:[NSNumber numberWithBool:YES] forKey:GCDWebServerOption_BindToLocalhost];
    [options setObject:@"GCD Web Server" forKey:GCDWebServerOption_ServerName];
    
    do {
         NSLog(@"Starting http daemon port %d",httpPort);
        [options setObject:[NSNumber numberWithInteger:httpPort++] forKey:GCDWebServerOption_Port];
    } while(![_GCDWebServer startWithOptions:options error:&error]);
    
    if (error) {
        NSLog(@"Error starting http daemon: %@", error);
    } else {
        //[_GCDWebServer setLogLevel:kGCDWebServerLoggingLevel_Warning];
        NSLog(@"Started http daemon port %d",_GCDWebServer.port);
    }
    [self loadUrl:[self getLayaBoxUrl]];
}
- (void)loadUrl:(NSString*)url
{
    NSURL* pUrl = [NSURL URLWithString:url ];
    NSURLRequest* pUrlRequest = [NSURLRequest requestWithURL:pUrl];
    [_webview loadRequest:pUrlRequest];
}
@end
