//
//  ViewController.m
//  proj.ios
//
//  Created by layabox_mac on 16/10/10.
//  Copyright © 2016年 layabox_mac. All rights reserved.
//

#import "Laya8ViewController.h"
#import "NavigationViewController.h"
#import "LayaWebview/layaWebview.h"
#import "ViewController.h"
#import <conchRuntime.h>

@interface Laya8ViewController ()
@property (nonatomic,assign) int orientation;
@end
@implementation Laya8ViewController

- (instancetype)init:(int)orientation
{
    self = [super init];
    if( self != nil )
    {
        self.orientation = orientation;
        return self;
    }
    return nil;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    if([conchConfig GetInstance]->m_sLaya8Url)
    {
        self.browser = [[layaWebview alloc] initWithView:self.view url:[conchConfig GetInstance]->m_sLaya8Url orientation:self.orientation delegate:self];

        /*[[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(runtimeCallWebView:)
                                                     name:@"JSCallLaya8WebView"
                                                   object:nil];*/
    }
}

/*- (void) runtimeCallWebView:(NSNotification*)aNotification
{
    NSString* js = aNotification.object;
    [self.browser runJS:js];
}*/

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

#pragma mark - IWebViewListener

-(void)openGame:(NSString*) jsonParam
{
    [[NavigationViewController GetInstance] loadConch:jsonParam];
}

-(void)runJS:(NSString*)script
{
    if ([NavigationViewController GetInstance].conchViewController){
        [[NavigationViewController GetInstance].conchViewController->m_pConchRuntime runJS:script];
    }
}
-(void)reloadWebView
{
    self.browser = [[layaWebview alloc] initWithView:self.view url:[conchConfig GetInstance]->m_sLaya8Url orientation:self.orientation delegate:self];
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

@end
