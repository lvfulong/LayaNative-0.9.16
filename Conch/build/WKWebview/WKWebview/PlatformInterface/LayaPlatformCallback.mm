#import "LayaPlatformCallback.h"

//-------------------------------------------------------------------------------
static LayaPlatformCallback* g_pPlatformCallback = nil;
//-------------------------------------------------------------------------------
@implementation LayaPlatformCallback
//-------------------------------------------------------------------------------
-(id) init
{
    self = [super init];
    return self;
}
//-------------------------------------------------------------------------------
+(id) GetInstance
{
    if( g_pPlatformCallback == nil )
    {
        g_pPlatformCallback = [[LayaPlatformCallback alloc] init];
    }
    return g_pPlatformCallback;
}
-(void) callbackToJS:(NSString*)name data:(NSString*)jsonParam
{
    NSString* js = [NSString stringWithFormat:@"window.wkbridge.callback('%@','%@');",name, jsonParam];
    [self.webview evaluateJavaScript:js completionHandler:^(id _Nullable response, NSError * _Nullable error) {
        if (response || error)
        {
            NSLog(@"value: %@ error: %@", response, error);
        }
    }];
}
//-------------------------------------------------------------------------------
-(void) LP_onMarketInitCallback:(NSString*)jsonParam tokenDir:(NSString*)p_sTokenDir;
{
    /*const char* sValue = [p_sTokenDir cStringUsingEncoding:NSUTF8StringEncoding];
    laya::JSMarket::onMarketInit( sValue );
    if( jsonParam )
    {
        std::string sBuffer = [jsonParam cStringUsingEncoding:NSUTF8StringEncoding];
        laya::JCScriptRuntime::s_JSRT->m_pMarket->onInitCallback( sBuffer.c_str() );
    }*/
    [self callbackToJS:@"init" data:jsonParam];
}
-(void) LP_LoginCallback:(NSString*) jsonParam
{
    [self callbackToJS:@"login" data:jsonParam];
}
-(void) LP_onRechargeCallback:(NSString*) jsonParam
{
}
-(void) LP_ShareAndFeedCallback:(NSString*) jsonParam
{
}
-(void) LP_onAuthorizeCallback:(NSString*) jsonParam
{
}
-(void) LP_onRefreshTokenCallback:(NSString*) jsonParam
{
}
-(void) LP_onGetGameFriendsCallback:(NSString*) jsonParam
{
}
-(void) LP_onSendToDesktopCallback:(NSString*) jsonParam
{
}
-(void) LP_onLogoutCallback:(NSString*) jsonParam
{
}
-(void) LP_onTopicCircleCallback:(NSString*) jsonParam
{
}
-(void) LP_InviteCallback:(NSString*) jsonParam
{
}
-(void) LP_SwitchUserCallback:(NSString*) jsonParam
{
}
-(void) LP_EnterPlatformCallback:(NSString*) jsonParam
{
}
-(void) LP_EnterBBSCallback:(NSString*) jsonParam
{
}
-(void) LP_EnterFeedbackCallback:(NSString*) jsonParam
{
}
-(void) LP_EnterAccountMgrCallback:(NSString*) jsonParam
{
}
-(void) LP_BuyPropsCallback:(NSString*) jsonParam
{
}
-(void) LP_SetRechargeInfoCallback:(NSString*) jsonParam
{
}
-(void) LP_SendMessageToPlatformCallback:(NSString*) jsonParam
{
}

@end
