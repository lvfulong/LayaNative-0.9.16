//
//  ViewController.h
//  proj.ios
//
//  Created by layabox_mac on 16/10/10.
//  Copyright © 2016年 layabox_mac. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "LayaWebview/layaWebviewInterface.h"
@class layaWebview;
@interface Laya8ViewController : UIViewController<IWebViewListener>
@property (nonatomic,strong) layaWebview* browser;
- (instancetype)init:(int)orientation;
@end

