//
//  LayaWKWebview.h
//  LayaWKWebview
//
//  Created by helloworldlv on 2018/3/23.
//  Copyright © 2018年 render. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <WebKit/WebKit.h>
#import <WebKit/WKWebview.h>
#import <WebKit/WKUserContentController.h>
#import <WebKit/WKUserScript.h>

@interface LayaWKWebview : NSObject
@property(nonatomic, strong) WKWebView* webview;
- (instancetype)initWithWebview:(WKWebView*)webview url:(NSString*)url hostPort:(int)port;
- (instancetype)initWithWebview:(WKWebView*)webview url:(NSString*)url;
- (void)loadUrl:(NSString*)url;
@end
