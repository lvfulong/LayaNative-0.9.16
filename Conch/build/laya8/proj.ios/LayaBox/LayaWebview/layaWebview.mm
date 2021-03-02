//
//  layaWebview.m
//  layaWebview
//
//  Created by layabox_mac on 16/10/24.
//  Copyright © 2016年 layabox_mac. All rights reserved.
//

#import "layaWebview.h"
#import "URLSessionProtocol.h"
#include <string>
#include "layaWebviewCacheManager.h"
#import "Reflection/refection.h"
#import "conchConfig.h"
#import "ViewController.h"
#import "NavigationViewController.h"
#import "TencentMarket.h"

enum {
    GAME_NO,
    GAME_HTML5,
    GAME_CONCH,
};
@interface layaWebview ()<NSURLSessionDataDelegate>
@property (nonatomic, assign) id<IWebViewListener> webViewListener;
@property (nonatomic,strong) JSContext* context;
@property (nonatomic,strong) Reflection* reflection;
@property (nonatomic, assign) int gameType;
@property (nonatomic,strong) NSDictionary* orientationDic;
@property (nonatomic,assign) int orientation;
@property (nonatomic,assign) int subOrientation;
@property (nonatomic,strong) NSString* hallUrl;
@end

extern std::string gRedistPath;
//extern std::string gResourcePath;
extern std::string gAssetRootPath;

@implementation layaWebview

-(id)initWithView:(UIView*)view url:(NSString*)url orientation:(int)orientation delegate:(id <IWebViewListener>)delegate
{
    self = [super init];
    if( self != nil )
    {
        self.scrollView.bounces = NO;
        [self setScreenOrientation:orientation];//!!!
        self.orientation = orientation;
        self.orientationDic = [NSDictionary dictionaryWithObjectsAndKeys:@0,@"landscape",@1,@"portrait",@2,@"user",@3,@"behind",@4,@"sensor",@5,@"nosensor",@6,@"sensor_landscape",@7,@"sensor_portrait",@8,@"reverse_landscape",@9,@"reverse_portrait",@10,@"full_sensor",nil];
        self.subOrientation = -1;
        NSNotificationCenter * center = [NSNotificationCenter defaultCenter];
        [center addObserver:self selector:@selector(notice:) name:@"conchCallLayaBroswerJS" object:nil];
        
        self.gameType = GAME_NO;
        self.reflection = [[Reflection alloc] init];
        self.webViewListener = delegate;
        [NSURLProtocol registerClass:[URLSessionProtocol class]];
        //gRedistPath = [[self getRootCachePath] cStringUsingEncoding:NSUTF8StringEncoding];
        
        //const char* sResourcePath = [[[NSBundle mainBundle] resourcePath] cStringUsingEncoding:NSUTF8StringEncoding];
        //std::string gResourcePath = sResourcePath;
        //gResourcePath += "/";
        //gAssetRootPath= gResourcePath+"/cache/";
        
        self.delegate = self;
        [view addSubview:self];
        self.frame = self.superview.bounds;
        self.hallUrl = url;
        //layaWebviewCacheManager::getInstance()->init(self.hallUrl.UTF8String);
        [self updateDcc:url];
        return self;
    }
    return self;
}

- (void)dealloc
{
    //layaWebviewCacheManager::delInstance();
    [[NSNotificationCenter defaultCenter] removeObserver:self name:@"conchCallLayaBroswerJS" object:nil];
}

-(NSString*)getLaunchUrlDomain:(NSString*)url
{
    NSString* domain = [[NSString alloc] initWithString:url];
    domain = [domain stringByReplacingOccurrencesOfString:@"?" withString:@""];
    domain = [[NSURL alloc] initWithString:domain].URLByDeletingLastPathComponent.absoluteString;
    return domain;
}

-(void)notice:(NSNotification*)notification
{
   NSString* js = notification.object;
   [self runJS:js];
}

/*-(NSString*) getRootCachePath
{
    NSString* sAppDirctory = NSHomeDirectory();
    NSString* sDownloadRootPath = [ NSString stringWithFormat: @"%@/Library/Caches/", sAppDirctory ];
    return sDownloadRootPath;
}*/
- (bool)isHall:(NSString*)url
{
    return [self.hallUrl isEqualToString:url];
}
- (void)updateDcc:(NSString*)url
{
    NSLog(@"[cache] update dcc start [%@] ...",url);
    NSString* urlpath = [self getLaunchUrlDomain:url];
    /*NSString* urlpath = [[NSString alloc] initWithString:url];
    urlpath = [urlpath stringByReplacingOccurrencesOfString:@"?" withString:@""];
    urlpath = [[NSURL alloc] initWithString:urlpath].URLByDeletingLastPathComponent.absoluteString;*/
    NSLog(@"[cache] [%@]",url);
    
    laya::Laya8Cache* pCache = nullptr;
    if ([self isHall:url]){
        pCache = layaWebviewCacheManager::getInstance()->getHallCache().get();
    }
    else{
        pCache = layaWebviewCacheManager::getInstance()->getGameCache().get();
    }
    
    std::string curassets = laya::Laya8CacheManager::preUpdateDcc(pCache,gRedistPath,urlpath.UTF8String);
    
    NSURLSession *session = [NSURLSession sessionWithConfiguration:[NSURLSessionConfiguration defaultSessionConfiguration] delegate:nil delegateQueue:[NSOperationQueue mainQueue]];
    
    NSString* asidUrl = [[NSString alloc] initWithFormat:@"%@update/assetsid.txt?rand=%f",urlpath,(rand()/(float)RAND_MAX) * [NSDate date].timeIntervalSince1970];
    NSLog(@"[cache] %@",asidUrl);
    
    NSURLSessionDataTask *task = [session dataTaskWithURL:[NSURL URLWithString:asidUrl] completionHandler:^(NSData *  data, NSURLResponse * response, NSError * error) {
        NSLog(@"[cache] assetsid.txt download complete");
        NSString* assetsidStr = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
        if (error || (assetsidStr == nil) || [assetsidStr isEqualToString:@""]){
            if (error){
                NSLog(@"[cache] assetsid.txt download error [%@] [%@] [%@]",error.localizedFailureReason,error.localizedDescription,error.localizedRecoverySuggestion);
            }
            
            laya::Laya8CacheManager::updateDccClearAssetsid(pCache,gRedistPath,urlpath.UTF8String);
            
            [self requestGame:url];
        }
        else{
            if ([assetsidStr UTF8String] != curassets)
            {
                __block NSString* txtdcc = @"";
                
                NSURLSession *session = [NSURLSession sessionWithConfiguration:[NSURLSessionConfiguration defaultSessionConfiguration] delegate:nil delegateQueue:[NSOperationQueue mainQueue]];
                
                NSString* asidUrl = [[NSString alloc] initWithFormat:@"%@update/filetable.bin",urlpath];
                NSURLSessionDataTask *task = [session dataTaskWithURL:[NSURL URLWithString:asidUrl] completionHandler:^(NSData *  data, NSURLResponse * response, NSError * error)
                                              {
                                                  
                                                  NSLog(@"[cache] filetable.bin download complete");
                                                  
                                                  if (error)
                                                  {
                                                      NSLog(@"[cache] filetable.bin download error");
                                                  }
                                                  if (data.length % 8 != 0)
                                                  {
                                                      NSLog(@"[cache] filetable.bin length error");
                                                  }
                                                  else
                                                  {
                                                      int32_t* v = (int32_t*)data.bytes;
                                                      if (v[0] != 0xffeeddcc || v[1] != 1)
                                                      {
                                                          NSLog(@"dcc.bin file err!");
                                                      }
                                                      else {
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
                                                                      NSString* dccstr = [[NSString alloc] initWithFormat:@"%@ %@\n",[layaWebview convertDataToHexStr:data1],[layaWebview convertDataToHexStr:data2]];
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
                          laya::Laya8CacheManager::doUpdateDcc(pCache,gRedistPath,urlpath.UTF8String,txtdcc.UTF8String,assetsidStr.UTF8String);
                                                      }
                                                  else
                                                  {
                                                  }
                                                  [self requestGame:url];
                                              }];
                [task resume];
            }
            else{
                [self requestGame:url];
            }
        }
    }];
    [task resume];
}

-(void)requestGame:(NSString*)url
{
    if ([self isHall:url]){
        NSURLRequest* request = [NSURLRequest requestWithURL:[NSURL URLWithString:url]];
        [self loadRequest:request];
    }
    else{
        
        if(self.subOrientation != -1 && self.webViewListener){
            [self setScreenOrientation:self.subOrientation];
        }
        layaWebviewCacheManager::getInstance()->openGame(std::string(url.UTF8String));
        NSMutableString *js = [[NSMutableString alloc] init];
        [js appendFormat:@"var a=window.LayaGameIFrame;a.setAttribute('src','%@');a.style.setProperty('display','block');if(!a.parentNode)document.body.appendChild(a);",url];
        [self runJS:js];
    }
}

-(void) setScreenOrientation:(int)p_nType
{
    switch(p_nType)
    {
        case 0://landscape 16
        {
            [conchConfig GetInstance]->m_nOrientationType = UIInterfaceOrientationMaskLandscapeRight;
            NSNumber *value=[NSNumber numberWithInt:UIInterfaceOrientationLandscapeRight];
            [[UIDevice currentDevice] setValue:value forKey:@"orientation"];
            
        }
            break;
        case 1://portrait 2
        {
            [conchConfig GetInstance]->m_nOrientationType = UIInterfaceOrientationMaskPortrait;
            NSNumber *value=[NSNumber numberWithInt:UIInterfaceOrientationPortrait];
            [[UIDevice currentDevice] setValue:value forKey:@"orientation"];
        }
            break;
        case 8://reverse_landscape 8
        {
            [conchConfig GetInstance]->m_nOrientationType = UIInterfaceOrientationMaskLandscapeLeft;
            NSNumber *value=[NSNumber numberWithInt:UIInterfaceOrientationLandscapeLeft];
            [[UIDevice currentDevice] setValue:value forKey:@"orientation"];
        }
            break;
        case 9://reverse_portrait 4
        {
            [conchConfig GetInstance]->m_nOrientationType = UIInterfaceOrientationMaskPortraitUpsideDown;
            NSNumber *value=[NSNumber numberWithInt:UIInterfaceOrientationPortraitUpsideDown];
            [[UIDevice currentDevice] setValue:value forKey:@"orientation"];
        }
            break;
        case 4://sensor all
        {
            [conchConfig GetInstance]->m_nOrientationType = UIInterfaceOrientationMaskAll;
        }
            break;
        case 10://full sensor all
        {
            [conchConfig GetInstance]->m_nOrientationType = UIInterfaceOrientationMaskAll;
        }
            break;
        case 2://user full
        {
            [conchConfig GetInstance]->m_nOrientationType = UIInterfaceOrientationMaskAll;
        }
            break;
        case 6://sensor_landscape 16+8
        {
            [conchConfig GetInstance]->m_nOrientationType = UIInterfaceOrientationMaskLandscapeRight | UIInterfaceOrientationMaskLandscapeLeft;
            NSNumber *value=[NSNumber numberWithInt:UIInterfaceOrientationLandscapeRight];
            [[UIDevice currentDevice] setValue:value forKey:@"orientation"];
        }
            break;
        case 7://reverse_portrait 2+4
        {
            [conchConfig GetInstance]->m_nOrientationType = UIInterfaceOrientationMaskPortrait | UIInterfaceOrientationMaskPortraitUpsideDown;
            NSNumber *value=[NSNumber numberWithInt:UIInterfaceOrientationPortrait];
            [[UIDevice currentDevice] setValue:value forKey:@"orientation"];
        }
            break;
            
        default://behind nosensor
            break;
    }
}

+ (NSString *)convertDataToHexStr:(NSData *)data {
    if (!data || [data length] == 0) {
        return @"";
    }
    NSMutableString *string = [[NSMutableString alloc] initWithCapacity:[data length]];
    [data enumerateByteRangesUsingBlock:^(const void *bytes, NSRange byteRange, BOOL *stop) {
        unsigned char *dataBytes = (unsigned char*)bytes;
        for (NSInteger i = byteRange.length - 1; i >=0 ; i--) {
            NSString *hexStr = [NSString stringWithFormat:@"%x", (dataBytes[i]) & 0xff];
            if ([hexStr length] == 2) {
                [string appendString:hexStr];
            } else {
                [string appendFormat:@"0%@", hexStr];
            }
        }
    }];
    return string;
}

-(NSString*)encodeString:(NSString*)nsParam
{
    
    return [nsParam stringByAddingPercentEncodingWithAllowedCharacters:[NSCharacterSet characterSetWithCharactersInString:@"!*'();:@&=+$,/?%#[]"]];
    /*return (__bridge_transfer NSString *)CFURLCreateStringByAddingPercentEscapes(kCFAllocatorDefault,
     (__bridge CFStringRef)nsParam,
     NULL,
     CFSTR("!*'();:@&=+$,/?%#[]"),
     kCFStringEncodingUTF8);*/
}

-(NSString*)decodeString:(NSString*)nsParam
{
    return [nsParam stringByRemovingPercentEncoding];
    /*return (__bridge_transfer NSString *)CFURLCreateStringByReplacingPercentEscapesUsingEncoding(kCFAllocatorDefault,
     (__bridge CFStringRef)nsParam,
     CFSTR(""),
     kCFStringEncodingUTF8);*/
}

#pragma mark - UIWebViewDelegate

- (void)webViewDidStartLoad:(UIWebView *)webView
{
    self.context = [webView valueForKeyPath:@"documentView.webView.mainFrame.javaScriptContext"];
    self.context.exceptionHandler =
    ^(JSContext *context, JSValue *exceptionValue)
    {
        context.exception = exceptionValue;
        NSLog(@"%@", exceptionValue);
    };
    self.context[@"runtime"] = self;
    
    
    NSError* error = nil;
    NSString* path = [[NSBundle mainBundle].resourcePath stringByAppendingPathComponent:@"/scripts/initWebView.js"];
    NSString* js = [NSString stringWithContentsOfFile:path encoding:NSUTF8StringEncoding error:&error];
    if (error != nil || js == nil){
        NSLog(@"[webview] read /scripts/initWebView.js error [%@] [%@] [%@]",error.localizedDescription,error.localizedFailureReason,error.localizedRecoverySuggestion);
    }
    [self stringByEvaluatingJavaScriptFromString:js];
}

#pragma mark - IWebView

-(void)runJS:(NSString*)js
{
    [self stringByEvaluatingJavaScriptFromString:js];
}

- (void)openGame:(NSString *)jsonParam
{
    NSData* pJsonData = [jsonParam dataUsingEncoding:NSUTF8StringEncoding];
    NSError* pError;
    NSDictionary* pJson = [NSJSONSerialization JSONObjectWithData:pJsonData options:kNilOptions error:&pError];
    if(pError ){
        NSLog(@"opengame paramter json serialization error");
        return;
    }
    NSString* engineName = [pJson objectForKey:@"engineName"];
        NSString* orientation = [pJson objectForKey:@"orientation"];
        NSNumber* oren = [self.orientationDic objectForKey:orientation];
        self.subOrientation = oren.intValue;
    if ([engineName isEqualToString:@"webview"])
    {
        NSString* gameUrl = [pJson objectForKey:@"gameUrl"];
        [self updateDcc:gameUrl];
        self.gameType=GAME_HTML5;
    }
    else{
        if (self.webViewListener != nil){
            [self.webViewListener openGame:jsonParam];
            //NSString* orientation = [pJson objectForKey:@"orientation"];
            //NSNumber* oren = [self.orientationDic objectForKey:orientation];
            /*if(oren && self.webViewListener){
                [self setScreenOrientation:oren.intValue];
            }*/
        }
        self.gameType=GAME_CONCH;
    }
}

-(void)postMessage:(NSString*)jsonMessage targetOrigin:(NSString*)target
{
    if(self.gameType==GAME_HTML5)
    {
        NSMutableString *js = [[NSMutableString alloc] init];
        [js appendFormat:@"window.frames[0].postMessage(JSON.parse(decodeURIComponent(\"%@\")),\"%@\");",jsonMessage, target];
        [self runJS:js];
    }
    else if(self.gameType==GAME_CONCH)
    {
        NSMutableString *js = [[NSMutableString alloc] init];
        [js appendFormat:@"window.postRuntimeMessage(\"%@\",\"%@\");",jsonMessage, target];
        if(self.webViewListener!=nil)
            [self.webViewListener runJS:js];
    }
    
}

-(void)closeGame
{
    self.subOrientation = -1;
    if(self.gameType==GAME_HTML5) {
        [self runJS:@"var a=window.LayaGameIFrame;a.src='about:blank';try{a.contentWindow.document.write(''); a.contentWindow.document.close(); }catch(e){};document.body.removeChild(a);"];
        self.gameType=GAME_NO;
        if( self.webViewListener){
            [self setScreenOrientation:self.orientation];
        }
        layaWebviewCacheManager::getInstance()->closeGame();
    }
}

-(void)orientChange
{
    self.frame = [[UIScreen mainScreen] bounds];
}

#pragma mark - IJSExport

- (void)hideWebviewJS:(bool)bHide
{
    dispatch_async(dispatch_get_main_queue(), ^{
        if (bHide){
            [self setScreenOrientation:self.subOrientation];
            [[NavigationViewController GetInstance] showConch];
        }
        else{
            [self setScreenOrientation:self.orientation];
            [[NavigationViewController GetInstance] showBrowser];
        }
    });
}

-(void)openGameJS:(NSString*) jsonParam
{
    dispatch_async(dispatch_get_main_queue(), ^{
        [self openGame:jsonParam];
    });
}

-(void)postMessageJS:(NSString*)jsonMessage targetOrigin:(NSString*)target
{
    dispatch_async(dispatch_get_main_queue(), ^{
        [self postMessage:jsonMessage targetOrigin:target];
    });
}

-(void)callMethodJS:(int)objid isSync:(BOOL)issync className:(NSString*)cls methodName:(NSString*)method param:(NSString*)param
{
    dispatch_async(dispatch_get_main_queue(), ^{
        [self.reflection callMethod:objid className:cls methodName:method param:param];
    });
}

-(void)logJS:(NSString*)log
{
    NSLog(@"%@",log);
}
-(void)dispatchFunctionWithCallBackJS:(NSString*)methodName param:(NSString*)parmJSON
{
    dispatch_async(dispatch_get_main_queue(), ^{
         [[TencentMarket GetTencentMarket] dispathByMethodName:methodName param:parmJSON];
    });
}
-(void)reloadJS:(bool)force
{
    [self removeFromSuperview];
    [self.webViewListener reloadWebView];
}
@end
