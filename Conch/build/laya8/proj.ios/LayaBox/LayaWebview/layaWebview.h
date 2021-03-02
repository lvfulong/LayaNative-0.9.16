//
//  layaWebview.h
//  layaWebview
//
//  Created by layabox_mac on 16/10/24.
//  Copyright © 2016年 layabox_mac. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import "layaWebviewInterface.h"
#import <JavaScriptCore/JavaScriptCore.h>

@protocol OCJSExport <IJSExport,JSExport>
JSExportAs(log,-(void)logJS:(NSString*)log);
JSExportAs(hideWebview,-(void)hideWebviewJS:(bool)bHide);
JSExportAs(openGame,-(void)openGameJS:(NSString*) jsonParam);
JSExportAs(postMessage,-(void)postMessageJS:(NSString*)jsonMessage targetOrigin:(NSString*)target);
JSExportAs(callMethod,-(void)callMethodJS:(int)objid isSync:(BOOL)issync className:(NSString*)cls methodName:(NSString*)method param:(NSString*)param);
JSExportAs(dispatchFunctionWithCallBack,-(void)dispatchFunctionWithCallBackJS:(NSString*)methodName param:(NSString*)parmJSON);
JSExportAs(reload,-(void)reloadJS:(bool)force);
@end

@interface layaWebview : UIWebView<IWebView,OCJSExport,UIWebViewDelegate>
-(id)initWithView:(UIView*)view url:(NSString*)url orientation:(int)orientation delegate:(id <IWebViewListener>)delegate;
-(void)orientChange;
@end

