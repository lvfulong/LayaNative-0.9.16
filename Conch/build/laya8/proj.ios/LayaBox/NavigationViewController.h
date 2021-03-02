
//
//  NavigationViewController.h
//  LayaBox
//
//  Created by layabox_mac on 16/10/19.
//  Copyright © 2016年 LayaBox. All rights reserved.
//

#import <UIKit/UIKit.h>
@class Laya8ViewController;
@class ViewController;
@interface NavigationViewController : UIViewController
@property (nonatomic,strong) Laya8ViewController* browserViewController;
@property (nonatomic,strong) ViewController* conchViewController;
+(instancetype)GetInstance;
-(void)loadConch:(NSString*)json;
-(void)showConch;
-(void)exitConch;
-(void)showBrowser;
-(void) setScreenOrientation:(int)p_nType;
@end
