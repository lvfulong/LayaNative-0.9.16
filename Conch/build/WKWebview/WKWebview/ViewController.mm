#import "ViewController.h"
#import "conchConfig.h"
#import "LayaWKWebview.h"
#import "PlatformInterface/LayaPlatformFactory.h"
#import "PlatformInterface/LayaPlatformCallback.h"
#import "PlatformInterface/MarketAppStore.h"

@implementation ViewController
{
     LayaWKWebview*          _layaWKWebview;
     NSString* _scriptsPath;
     id<LayaPlatformInterface> _market;
}
-(id)init
{
    self = [super init];
    if( self != nil )
    {
        return self;
    }
    return nil;
}
- (void)viewDidLoad
{
    [super viewDidLoad];
    [conchConfig GetInstance];
    [self initPlatform];
    WKWebViewConfiguration* config = [[WKWebViewConfiguration alloc]init];
    config.preferences.javaScriptEnabled=true;
    config.preferences.javaScriptCanOpenWindowsAutomatically = true;
    config.userContentController = [[WKUserContentController alloc] init];
    
    NSString* scriptsPath = [[[NSBundle mainBundle] resourcePath] stringByAppendingString:@"/scripts/init.js"];
    NSString* iniScript = [NSString stringWithContentsOfFile:scriptsPath encoding:NSUTF8StringEncoding error:nil];
    WKUserScript *script = [[WKUserScript alloc] initWithSource:iniScript injectionTime:WKUserScriptInjectionTimeAtDocumentStart forMainFrameOnly:YES];
    [config.userContentController addUserScript:script];
    
    WKWebView* webview = [[WKWebView alloc] initWithFrame:self.view.bounds configuration:config];
    webview.allowsBackForwardNavigationGestures = YES;
    webview.UIDelegate = self;
    webview.navigationDelegate = self;
    
    webview.scrollView.bounces = false;
    webview.scrollView.bouncesZoom = false;
    LayaPlatformCallback* layaPlatformCallback = [LayaPlatformCallback GetInstance];
    layaPlatformCallback.webview = webview;
    
    [self setNeedsStatusBarAppearanceUpdate];
    [self prefersStatusBarHidden];
    
    _layaWKWebview = [[LayaWKWebview alloc] initWithWebview:webview url:[conchConfig GetInstance]->m_sUrl hostPort:[conchConfig GetInstance]->m_nHostPort];
    [self.view addSubview:_layaWKWebview.webview];
}
- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
}
//-------------------------------------------------------------------------------
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
//-------------------------------------------------------------------------------
- (BOOL)prefersStatusBarHidden
{
    return YES;
}
- (BOOL)shouldAutorotate
{
    return YES;//支持转屏
}
- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    [_layaWKWebview.webview.configuration.userContentController addScriptMessageHandler:self name:@"init"];
    [_layaWKWebview.webview.configuration.userContentController addScriptMessageHandler:self name:@"login"];
    [_layaWKWebview.webview.configuration.userContentController addScriptMessageHandler:self name:@"logout"];
    [_layaWKWebview.webview.configuration.userContentController addScriptMessageHandler:self name:@"switchUser"];
    [_layaWKWebview.webview.configuration.userContentController addScriptMessageHandler:self name:@"enterPlatform"];
    [_layaWKWebview.webview.configuration.userContentController addScriptMessageHandler:self name:@"enterBBS"];
    [_layaWKWebview.webview.configuration.userContentController addScriptMessageHandler:self name:@"enterFeedback"];
    [_layaWKWebview.webview.configuration.userContentController addScriptMessageHandler:self name:@"enterAccountMgr"];
    [_layaWKWebview.webview.configuration.userContentController addScriptMessageHandler:self name:@"authorize"];
    [_layaWKWebview.webview.configuration.userContentController addScriptMessageHandler:self name:@"refreshToken"];
    [_layaWKWebview.webview.configuration.userContentController addScriptMessageHandler:self name:@"recharge"];
    [_layaWKWebview.webview.configuration.userContentController addScriptMessageHandler:self name:@"buyProps"];
    [_layaWKWebview.webview.configuration.userContentController addScriptMessageHandler:self name:@"setRechargeInfo"];
    [_layaWKWebview.webview.configuration.userContentController addScriptMessageHandler:self name:@"enterShareAndFeed"];
    [_layaWKWebview.webview.configuration.userContentController addScriptMessageHandler:self name:@"enterInvite"];
    [_layaWKWebview.webview.configuration.userContentController addScriptMessageHandler:self name:@"getGameFriends"];
    [_layaWKWebview.webview.configuration.userContentController addScriptMessageHandler:self name:@"sendToDesktop"];
    [_layaWKWebview.webview.configuration.userContentController addScriptMessageHandler:self name:@"sendMessageToPlatform"];
    [_layaWKWebview.webview.configuration.userContentController addScriptMessageHandler:self name:@"canSendToDesktop"];
    [_layaWKWebview.webview.configuration.userContentController addScriptMessageHandler:self name:@"openTopicCircle"];
    [_layaWKWebview.webview.configuration.userContentController addScriptMessageHandler:self name:@"getUserInfo"];
    [_layaWKWebview.webview.configuration.userContentController addScriptMessageHandler:self name:@"getAvailableLoginType"];
}
- (void)viewWillDisppear:(BOOL)animated
{
    [super viewWillDisappear:animated];
    [_layaWKWebview.webview.configuration.userContentController removeScriptMessageHandlerForName:@"init"];
    [_layaWKWebview.webview.configuration.userContentController removeScriptMessageHandlerForName:@"login"];
    [_layaWKWebview.webview.configuration.userContentController removeScriptMessageHandlerForName:@"logout"];
    [_layaWKWebview.webview.configuration.userContentController removeScriptMessageHandlerForName:@"switchUser"];
    [_layaWKWebview.webview.configuration.userContentController removeScriptMessageHandlerForName:@"enterPlatform"];
    [_layaWKWebview.webview.configuration.userContentController removeScriptMessageHandlerForName:@"enterBBS"];
    [_layaWKWebview.webview.configuration.userContentController removeScriptMessageHandlerForName:@"enterFeedback"];
    [_layaWKWebview.webview.configuration.userContentController removeScriptMessageHandlerForName:@"enterAccountMgr"];
    [_layaWKWebview.webview.configuration.userContentController removeScriptMessageHandlerForName:@"authorize"];
    [_layaWKWebview.webview.configuration.userContentController removeScriptMessageHandlerForName:@"refreshToken"];
    [_layaWKWebview.webview.configuration.userContentController removeScriptMessageHandlerForName:@"recharge"];
    [_layaWKWebview.webview.configuration.userContentController removeScriptMessageHandlerForName:@"buyProps"];
    [_layaWKWebview.webview.configuration.userContentController removeScriptMessageHandlerForName:@"setRechargeInfo"];
    [_layaWKWebview.webview.configuration.userContentController removeScriptMessageHandlerForName:@"enterShareAndFeed"];
    [_layaWKWebview.webview.configuration.userContentController removeScriptMessageHandlerForName:@"enterInvite"];
    [_layaWKWebview.webview.configuration.userContentController removeScriptMessageHandlerForName:@"getGameFriends"];
    [_layaWKWebview.webview.configuration.userContentController removeScriptMessageHandlerForName:@"sendToDesktop"];
    [_layaWKWebview.webview.configuration.userContentController removeScriptMessageHandlerForName:@"sendMessageToPlatform"];
    [_layaWKWebview.webview.configuration.userContentController removeScriptMessageHandlerForName:@"canSendToDesktop"];
    [_layaWKWebview.webview.configuration.userContentController removeScriptMessageHandlerForName:@"openTopicCircle"];
    [_layaWKWebview.webview.configuration.userContentController removeScriptMessageHandlerForName:@"getUserInfo"];
    [_layaWKWebview.webview.configuration.userContentController removeScriptMessageHandlerForName:@"getAvailableLoginType"];
}
- (void)userContentController:(WKUserContentController *)userContentController didReceiveScriptMessage:(WKScriptMessage *)message
{
    if (![message.body isKindOfClass:NSString.class])
    {
        return;
    }
    NSString* data = message.body;
    if (data == nil)
    {
        return;
    }
    
    NSLog(@"JS-->OC  %@",message.body);
    
    if ([message.name isEqualToString:@"init"])
    {
        [_market LP_Init:data];
    }
    else if ([message.name isEqualToString:@"login"])
    {
        [_market LP_Login:data];
    }
    else if ([message.name isEqualToString:@"logout"])
    {
        [_market LP_Logout:data];
    }
    else if ([message.name isEqualToString:@"switchUser"])
    {
        [_market LP_SwitchUser:data];
    }
    else if ([message.name isEqualToString:@"enterPlatform"])
    {
        [_market LP_EnterPlatform:data];
    }
    else if ([message.name isEqualToString:@"enterBBS"])
    {
        [_market LP_enterBBS:data];
    }
    else if ([message.name isEqualToString:@"enterFeedback"])
    {
        [_market LP_enterFeedback:data];
    }
    else if ([message.name isEqualToString:@"enterAccountMgr"])
    {
        [_market LP_enterAccountMgr:data];
    }
    else if ([message.name isEqualToString:@"authorize"])
    {
        [_market LP_authorize:data];
    }
    else if ([message.name isEqualToString:@"refreshToken"])
    {
        [_market LP_RefreshToken:data];
    }
    else if ([message.name isEqualToString:@"recharge"])
    {
        [_market LP_Recharge:data];
    }
    else if ([message.name isEqualToString:@"buyProps"])
    {
        [_market LP_buyProps:data];
    }
    else if ([message.name isEqualToString:@"setRechargeInfo"])
    {
        [_market LP_setRechargeInfo:data];
    }
    else if ([message.name isEqualToString:@"enterInvite"])
    {
        [_market LP_enterInvite:data];
    }
    else if ([message.name isEqualToString:@"enterShareAndFeed"])
    {
        [_market LP_enterShareAndFeed:data];
    }
    else if ([message.name isEqualToString:@"getGameFriends"])
    {
        [_market LP_getGameFriends:data];
    }
    else if ([message.name isEqualToString:@"sendToDesktop"])
    {
        [_market LP_sendToDesktop:data];
    }
    else if ([message.name isEqualToString:@"canSendToDesktop"])
    {
        [_market LP_canSendToDesktop:data];
    }
    else if ([message.name isEqualToString:@"openTopicCircle"])
    {
        [_market LP_openTopicCircle:data];
    }
    else if ([message.name isEqualToString:@"getUserInfo"])
    {
        //TODO
    }
    else if ([message.name isEqualToString:@"getAvailableLoginType"])
    {
        //TODO
    }
    else if ([message.name isEqualToString:@"openTopicCircle"])
    {
        [_market LP_openTopicCircle:data];
    }
}
-(void) initPlatform
{
    NSString* nsPlatformClassName = [conchConfig GetInstance]->m_sPlatformClassName;
    if(  nsPlatformClassName != nil )
    {
        _market = [[LayaPlatformFactory GetInstance] CreateInterface:nsPlatformClassName];
        if( _market != nil )
        {
            [_market LP_Init];
        }
        else
        {
            NSLog(@"严重错误 new Market 的时候错误了");
        }
    }
}
- (void)webView:(WKWebView *)webView decidePolicyForNavigationAction:(WKNavigationAction *)navigationAction decisionHandler:(void (^)(WKNavigationActionPolicy))decisionHandler
{
    NSDictionary* pDictionary =
    [NSDictionary dictionaryWithObjectsAndKeys:
    [conchConfig GetInstance]->m_sPlatformClassName,@"market_name",
    [NSNumber numberWithInt:[conchConfig GetInstance]->m_nChargeType],@"charge_type",
    [NSNumber numberWithInt:[conchConfig GetInstance]->m_nPayType],@"pay_type",
    [NSNumber numberWithInt:[conchConfig GetInstance]->m_nEnterPlatformType],@"enter_platform_type",
    [NSNumber numberWithInt:[conchConfig GetInstance]->m_nLoginType],@"login_type",nil];
    NSError* pError = nil;
    NSString* pJsonString = nil;
    NSData* pJsonData = [NSJSONSerialization dataWithJSONObject:pDictionary options:0 error:&pError];
    if( !pError )
    {
        pJsonString = [[NSString alloc] initWithData:pJsonData encoding:NSUTF8StringEncoding];
    }

    NSString* js = [NSString stringWithFormat:@"if(!window.conchMarketData){window.conchMarketData=JSON.parse('%@')}",pJsonString];
    [_layaWKWebview.webview evaluateJavaScript:js completionHandler:^(id _Nullable response, NSError * _Nullable error) {
        if (response || error)
        {
            NSLog(@"value: %@ error: %@;", response, error);
        }
    }];
    
    decisionHandler(WKNavigationActionPolicyAllow);
}
- (void)webView:(WKWebView *)webView runJavaScriptAlertPanelWithMessage:(NSString *)message initiatedByFrame:(WKFrameInfo *)frame completionHandler:(void (^)(void))completionHandler {
    // 确定按钮
    UIAlertAction *alertAction = [UIAlertAction actionWithTitle:@"OK" style:UIAlertActionStyleCancel handler:^(UIAlertAction * _Nonnull action) {
        completionHandler();
    }];
    // alert弹出框
    UIAlertController *alertController = [UIAlertController alertControllerWithTitle:message message:nil preferredStyle:UIAlertControllerStyleAlert];
    [alertController addAction:alertAction];
    [self presentViewController:alertController animated:YES completion:nil];
}
@end
