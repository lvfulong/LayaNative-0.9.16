//
//  NavigationViewController.m
//  LayaBox
//
//  Created by layabox_mac on 16/10/19.
//  Copyright © 2016年 LayaBox. All rights reserved.
//

#import "NavigationViewController.h"
#import "ViewController.h"
#import "Laya8ViewController.h"
#import "LayaWebview/layaWebview.h"
#include "layaWebviewCacheManager.h"

@interface NavigationViewController()
@property (nonatomic,strong) NSDictionary* orientationDic;
@property (nonatomic,assign) int orientation;
@end
static NavigationViewController* g_pNavigationViewController;

extern std::string gRedistPath;
extern std::string gAssetRootPath;


@implementation NavigationViewController

- (instancetype)init
{
    
    self = [super init];
   
    if( self != nil )
    {
        
        gRedistPath = [[self getRootCachePath] cStringUsingEncoding:NSUTF8StringEncoding];
        
        const char* sResourcePath = [[[NSBundle mainBundle] resourcePath] cStringUsingEncoding:NSUTF8StringEncoding];
        std::string gResourcePath = sResourcePath;
        gResourcePath += "/";
        gAssetRootPath= gResourcePath+"/cache/";
        
        layaWebviewCacheManager::getInstance()->init([conchConfig GetInstance]->m_sLaya8Url.UTF8String);

        self.orientation = 10;
        self.orientationDic = [NSDictionary dictionaryWithObjectsAndKeys:@0,@"landscape",@1,@"portrait",@2,@"user",@3,@"behind",@4,@"sensor",@5,@"nosensor",@6,@"sensor_landscape",@7,@"sensor_portrait",@8,@"reverse_landscape",@9,@"reverse_portrait",@10,@"full_sensor",nil];
        g_pNavigationViewController = self;
        self.browserViewController = [[Laya8ViewController alloc] init:0];//LVTODO
        [self.view addSubview:self.browserViewController.view];
        // 注册屏幕方向改变的事件
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(orientChange:)
                                                     name:UIDeviceOrientationDidChangeNotification
                                                   object:nil];
        if ([self respondsToSelector:@selector(setNeedsStatusBarAppearanceUpdate)])
        {
            [self prefersStatusBarHidden];
            [self performSelector:@selector(setNeedsStatusBarAppearanceUpdate)];
        }
        return self;
    }
    return nil;
}
-(NSString*) getRootCachePath
{
    NSString* sAppDirctory = NSHomeDirectory();
    NSString* sDownloadRootPath = [ NSString stringWithFormat: @"%@/Library/Caches/", sAppDirctory ];
    return sDownloadRootPath;
}
- (void)dealloc
{
    layaWebviewCacheManager::delInstance();
    [[NSNotificationCenter defaultCenter] removeObserver:self name:UIDeviceOrientationDidChangeNotification object:nil];

}
-(void)buttonClickBack:(id)sender
{
    /*if ([self.topViewController class] == [ViewController class])
    {
        [self popViewControllerAnimated:YES];
    }
    else if ([self.topViewController class] == [Laya8ViewController class])
    {
        [self.browserViewController.browser closeGame];
    }*///LVTODO
}

-(void)orientChange:(NSNotification *)aNotification
{
    [self.browserViewController.browser orientChange];
}

-(NSUInteger)supportedInterfaceOrientations
{
    /*
     UIInterfaceOrientationMaskPortrait,             ===2
     UIInterfaceOrientationMaskPortraitUpsideDown,   ===4
     UIInterfaceOrientationMaskLandscapeLeft,        ===8
     UIInterfaceOrientationMaskLandscapeRight,       ===16
     */
    return [conchConfig GetInstance]->m_nOrientationType;
}

- (BOOL)shouldAutorotate
{
    return YES;//支持转屏
}
+(instancetype)GetInstance
{
    return g_pNavigationViewController;
}
-(void)loadConch:(NSString*)json
{
    //if (!self.conchViewController)
    {
        NSData* pJsonData = [json dataUsingEncoding:NSUTF8StringEncoding];
        NSError* pError;
        NSDictionary* pJson = [NSJSONSerialization JSONObjectWithData:pJsonData options:kNilOptions error:&pError];
        if(pError ){
            NSLog(@"Laya8ViewController openGame paramter json serialization error");
            return;
        }
        
        NSString* url = [pJson objectForKey:@"gameUrl"];
        if (url && ![url isEqualToString:@""]){
            NSString* orientation = [pJson objectForKey:@"orientation"];
            NSNumber* oren = [self.orientationDic objectForKey:orientation];
            if(self.conchViewController && self.conchViewController->m_pConchRuntime){
                [self.conchViewController->m_pConchRuntime destory];
            }
            self.conchViewController = [[ViewController alloc] init:url];
            [self.view insertSubview:self.conchViewController.view belowSubview:self.browserViewController.view];
            self.orientation = oren.intValue;
            
            self.conchViewController.view.hidden = YES;
            self.browserViewController.view.hidden = NO;
        }
    }
}
-(void)showConch
{
    if (self.conchViewController && self.conchViewController.view.superview && self.browserViewController.view.superview)
    {
        [self setScreenOrientation:self.orientation];
        [self.view bringSubviewToFront:self.conchViewController.view];
        
        self.conchViewController.view.hidden = NO;
        self.browserViewController.view.hidden = YES;
    }
}
-(void)exitConch
{
    if (self.conchViewController && self.conchViewController.view.superview)
    {
        [self.conchViewController.view removeFromSuperview];
        self.conchViewController = nil;
    }
    if (!self.browserViewController.view.superview)
    {
        [self.view addSubview:self.browserViewController.view];
    }
    
    self.conchViewController.view.hidden = YES;
    self.browserViewController.view.hidden = NO;
}
-(void)showBrowser
{
    if (self.browserViewController.view.superview)
    {
        [self.view bringSubviewToFront:self.browserViewController.view];
        
        self.conchViewController.view.hidden = YES;
        self.browserViewController.view.hidden = NO;
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
- (BOOL)prefersStatusBarHidden
{
    return YES;//隐藏为YES，显示为NO
}
@end
