//
//  IWebView.h
//  LayaBox
//
//  Created by layabox_mac on 16/10/18.
//  Copyright © 2016年 LayaBox. All rights reserved.
//



@protocol IWebView
-(void)runJS:(NSString*)js;
-(void)openGame:(NSString*) jsonParam;
-(void)postMessage:(NSString*)jsonMessage targetOrigin:(NSString*)target;
-(void)closeGame;
-(void)orientChange;
@end

@protocol IWebViewListener
-(void)openGame:(NSString*) jsonParam;
-(void)runJS:(NSString*)script;
-(void)reloadWebView;
@end


@protocol IJSExport
-(void)logJS:(NSString*)log;
-(void)hideWebviewJS:(bool)bHide;
-(void)openGameJS:(NSString*) jsonParam;
-(void)postMessageJS:(NSString*)jsonMessage targetOrigin:(NSString*)target;
-(void)callMethodJS:(int)objid isSync:(BOOL)issync className:(NSString*)cls methodName:(NSString*)method param:(NSString*)param;
-(void)dispatchFunctionWithCallBackJS:(NSString*)methName param:(NSString*)parmJSON;
-(void)reloadJS:(bool)force;
@end
