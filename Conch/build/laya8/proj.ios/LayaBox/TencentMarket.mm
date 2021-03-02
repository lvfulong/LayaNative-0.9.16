//
//  TencentMarket.m
//  Laya8
//
//  Created by LayaBox on 11/17/16.
//  Copyright © 2016 LayaBox. All rights reserved.
//
#import "TencentMarket.h"
#import "wx/WXApiObject.h"
#import "NavigationViewController.h"
#import "Laya8ViewController.h"
#import "layaWebview.h"
#import "conchConfig.h"
#import "AppDelegate.h"

static TencentMarket* g_pTencentMarket = nil;
@implementation TencentMarket

-(instancetype) init
{
    self = [super init];
     if (self != nil){
         m_bIsRegQQ=m_bIsRegWX=false;
         m_bIsRefresh=false;
         g_pTencentMarket=self;
         m_bneedLoginCallback=false;
         m_bneedShareCallback=false;
         //[WXApi registerApp:m_sAppid withDescription:@"LayaBox"];
         [[IAPManager getInstance] initWithGameID:[conchConfig GetInstance]->m_sGameID andDelegate:self];
         return self;
     }
    return nil;
}
-(void)runJSCallBack:(NSString *)methodName jsonStr:(NSString *)jsonStr
{
    jsonStr=[jsonStr stringByAddingPercentEncodingWithAllowedCharacters:[NSCharacterSet characterSetWithCharactersInString:@"!*'();:@&=+$,/?%#[]"]];
    NSString* script = [[NSString alloc] initWithFormat:@"window.native%@(\"%@\")",methodName,jsonStr];
    [[NavigationViewController GetInstance].browserViewController.browser runJS:script];

}

-(void)login:(NSString *)jsonParam
{
    m_bIsRefresh=false;
    m_bneedLoginCallback=true;
    
    //构造SendAuthReq结构体
    NSData* pJsonData = [jsonParam dataUsingEncoding:NSUTF8StringEncoding];
    NSError* pError;
    NSDictionary* pJson = [NSJSONSerialization JSONObjectWithData:pJsonData options:kNilOptions error:&pError];
    if( !pError )
    {
        NSString* type = [pJson objectForKey:@"type"];
        if([[type lowercaseString] isEqualToString:@"wx"])
        {
            [self checkQQorWX:@"wx"];
            SendAuthReq* req =[[SendAuthReq alloc ] init];
            req.scope = @"snsapi_userinfo,snsapi_base" ;
            req.state = @"123" ;
            //第三方向微信终端发送一个SendAuthReq消息结构
            AppDelegate * appDelegate = (AppDelegate*)[UIApplication sharedApplication].delegate;
            [WXApi sendAuthReq:req viewController:[NavigationViewController GetInstance].browserViewController delegate:appDelegate];
            //[WXApi sendReq:req];
        }
        else if([[type lowercaseString] isEqualToString:@"qq"])
        {
            [self checkQQorWX:@"qq"];
            
            NSArray* permissions = [NSArray arrayWithObjects:
                                    kOPEN_PERMISSION_GET_USER_INFO,
                                    kOPEN_PERMISSION_GET_SIMPLE_USER_INFO,
                                    kOPEN_PERMISSION_ADD_ALBUM,
                                    kOPEN_PERMISSION_ADD_ONE_BLOG,
                                    kOPEN_PERMISSION_ADD_SHARE,
                                    kOPEN_PERMISSION_ADD_TOPIC,
                                    kOPEN_PERMISSION_CHECK_PAGE_FANS,
                                    kOPEN_PERMISSION_GET_INFO,
                                    kOPEN_PERMISSION_GET_OTHER_INFO,
                                    kOPEN_PERMISSION_LIST_ALBUM,
                                    kOPEN_PERMISSION_UPLOAD_PIC,
                                    kOPEN_PERMISSION_GET_VIP_INFO,
                                    kOPEN_PERMISSION_GET_VIP_RICH_INFO,
                                    nil];
            [m_pTencentOAuth authorize:permissions inSafari:NO];
        }
        else
        {
            [self onloginCallBack:@"this logintype not support" result:CallBackNotSupport userId:nil headimg:nil nickname:nil data:nil];
        }
    }
    else{
        [self onloginCallBack:@"json error" result:CallBackJSONError userId:nil headimg:nil nickname:nil data:nil];
    }
}

-(void) recharge:(NSString *)jsonParam
{
    [[IAPManager getInstance] recharge:jsonParam];
}
-(void) getImageFromURL:(NSString *)fileURL completionHandler:(void (^)(UIImage* img))completionHandler
{
    NSURLSession* session = [NSURLSession sessionWithConfiguration:[NSURLSessionConfiguration defaultSessionConfiguration] delegate:nil delegateQueue:nil];
    NSURL* url = [[NSURL alloc] initWithString:fileURL];

    NSURLSessionDataTask* downloadTask = [session dataTaskWithURL:url completionHandler:^(NSData *data, NSURLResponse *response, NSError *error){
        if (error){
            completionHandler(nil);
        }
        else{
            completionHandler([UIImage imageWithData:data]);
        }
    }];
    [downloadTask resume];
}

-(void)share:(NSString *)jsonParam
{
    NSData* pJsonData = [jsonParam dataUsingEncoding:NSUTF8StringEncoding];
    NSError* pError;
    NSDictionary* pJson = [NSJSONSerialization JSONObjectWithData:pJsonData options:kNilOptions error:&pError];
    if( !pError )
    {
        NSString* nsTitle = [pJson objectForKey:@"title"];
        NSString* nsLink = [pJson objectForKey:@"link"];
        NSString* nsDesc = [pJson objectForKey:@"desc"];
        NSString* nsImgsrc = [pJson objectForKey:@"imgsrc"];
        NSNumber* type=[pJson objectForKey:@"shareType"];
       // NSString* nsImgTitle = [pJson objectForKey:@"imgtitle"];
        //NSString* nsImgCustxt = [pJson objectForKey:@"custxt"];
        
        if(type.intValue==1||type.intValue==2)
        {
            [self checkQQorWX:@"wx"];
            if(![WXApi isWXAppInstalled])
            {
                [self onshareCallBack:@"wechat not install" result:CallBackNotSupport];
                return;
            }
            m_bneedShareCallback=true;
            [self getImageFromURL:nsImgsrc completionHandler:^(UIImage* img){
                if (img != nil){
                    WXMediaMessage* message = [WXMediaMessage message];
                    [message setThumbImage: img];
                    message.title = nsTitle;
                    message.description = nsDesc;
                    WXWebpageObject* webpageObject = [WXWebpageObject object];
                    webpageObject.webpageUrl = nsLink;
                    message.mediaObject = webpageObject;
                    SendMessageToWXReq* pReq = [[SendMessageToWXReq alloc]init];
                    pReq.bText = NO;
                    pReq.message = message;
                    if(type.intValue==1)
                    {
                        pReq.scene = WXSceneTimeline;//;//;//朋友圈
                    }
                    else
                    {
                        pReq.scene = WXSceneSession;
                    }
                    bool bRet = [WXApi sendReq:pReq];
                    if( bRet == false )
                    {
                        //NSString* pJsonString = @"{ \"code\" : \"-10\",  \"desc\" : \"error\",  \"app\" : \"8\"}";
                        //[[LayaPlatformCallback GetInstance] LP_ShareAndFeedCallback:pJsonString];
                    }
                }

            }];
        }
        else if(type.intValue==0||type.intValue==3)
        {
            [self checkQQorWX:@"qq"];
            m_bneedShareCallback=true;
            if(type.intValue==3)
            {
                QQApiNewsObject* pNewsObj = [QQApiNewsObject objectWithURL:[NSURL URLWithString:nsLink] title:nsTitle description:nsDesc previewImageURL:[NSURL URLWithString:nsImgsrc]];
                SendMessageToQQReq* req = [SendMessageToQQReq reqWithContent:pNewsObj];
                QQApiSendResultCode sent = [QQApiInterface sendReq:req];
            }
            else
            {
                QQApiNewsObject* pNewsObj = [QQApiNewsObject objectWithURL:[NSURL URLWithString:nsLink] title:nsTitle description:nsDesc previewImageURL:[NSURL URLWithString:nsImgsrc]];
                SendMessageToQQReq* req = [SendMessageToQQReq reqWithContent:pNewsObj];
                QQApiSendResultCode sent = [QQApiInterface SendReqToQZone:req];
            }
        }
    }
}

-(void) refreshToken:(NSString *)jsonParam
{
    m_bIsRefresh=true;
    NSData* pJsonData = [jsonParam dataUsingEncoding:NSUTF8StringEncoding];
    NSError* pError;
    NSDictionary* pJson = [NSJSONSerialization JSONObjectWithData:pJsonData options:kNilOptions error:&pError];
    if( !pError )
    {
        NSString* type=[pJson objectForKey:@"type"];
        m_sRefreshToken = [pJson objectForKey:@"refreshToken"];
        if([[type lowercaseString] isEqualToString:@"wx"])
        {
            //m_sAppid = [pJson objectForKey:@"appid"];
            [self checkQQorWX:@"wx"];
            NSString *url =[NSString stringWithFormat:@"https://api.weixin.qq.com/sns/oauth2/refresh_token?appid=%@&grant_type=refresh_token&refresh_token=%@",m_sAppid,m_sRefreshToken];
            dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
                NSURL* zoneUrl = [NSURL URLWithString:url];
                NSString* zoneStr = [NSString stringWithContentsOfURL:zoneUrl encoding:NSUTF8StringEncoding error:nil];
                NSData* data = [zoneStr dataUsingEncoding:NSUTF8StringEncoding];
                dispatch_async(dispatch_get_main_queue(), ^
                   {
                       if (data)
                       {
                           NSDictionary* dic = [NSJSONSerialization JSONObjectWithData:data options:NSJSONReadingMutableContainers error:nil];
                           NSString* nsToken = [dic objectForKey:@"access_token"];
                           m_sRefreshToken = [dic objectForKey:@"refresh_token"];
                           NSString* nsOpenID = [dic objectForKey:@"openid"];
                           [self getWXUserInfo:nsToken openID:nsOpenID refreshToken:m_sRefreshToken isRefreshToken:true];
                       }
                       else
                       {
                           [self onRefreshTokenCallBack:@"refreshToken failed" result:-2 userId:nil headimg:nil nickname:nil data:nil];
                       }
                   });
            });
        }
        else if([[type lowercaseString] isEqualToString:@"qq"])
        {
            [self checkQQorWX:@"qq"];
            [m_pTencentOAuth setAccessToken:m_sRefreshToken];
            NSString* time = [pJson objectForKey:@"time"];
            NSString* openid=[pJson objectForKey:@"openid"];
            [m_pTencentOAuth setOpenId:openid];
            long long ltime=[time longLongValue];
            long long currentTime=  [[NSDate dateWithTimeIntervalSinceNow:0] timeIntervalSince1970]*1000;
            if(ltime<currentTime)
            {
                 [self runJSCallBack:@"refreshToken" jsonStr:@"{\"result\":-2,\"desc\":\"expirationDate comming\"}"];
            }
            else{
                NSDate* date = [NSDate dateWithTimeIntervalSince1970:currentTime / 1000+7776000];
                [m_pTencentOAuth setExpirationDate:date];
                if( m_pTencentOAuth.accessToken && [m_pTencentOAuth.accessToken length] !=0 )
                {
                    [m_pTencentOAuth getUserInfo];
                }
            }

        }
        else
        {
            [self onRefreshTokenCallBack:@"this logintype not support" result:CallBackNotSupport userId:nil headimg:nil nickname:nil data:nil];
        }
    }
    else
    {
        [self onRefreshTokenCallBack:@"json error" result:CallBackJSONError userId:nil headimg:nil nickname:nil data:nil];
    }

}


-(void)onloginCallBack:(NSString *)desc result:(int)result userId:(NSString *)userId headimg:(NSString *)headimg nickname:(NSString *)nickname data:(NSDictionary*)data
{
    NSString* pJsonString=nil;
    NSNumber* sResult = [[NSNumber alloc]initWithInt:result];
    //NSString* sTime = [NSString stringWithFormat:@"%lld",p_nTime];
    NSString* sTime=nil;
    if(m_pTencentOAuth!=nil&&m_pTencentOAuth.expirationDate!=nil)
    {
        sTime= [NSString stringWithFormat:@"%lld",(long long)[m_pTencentOAuth.expirationDate timeIntervalSince1970]*1000];
    }

    NSDictionary* pDictionary = [NSDictionary dictionaryWithObjectsAndKeys:
                                 sResult,@"result",
                                 desc,@"desc",
                                 userId,@"userId",
                                 nickname,@"nickname",
                                 headimg,@"headimg",
                                 @"",@"spUserName",
                                 m_sRefreshToken,@"token",
                                 @"",@"stoken",
                                 userId,@"spUserId",
                                 data,@"channelExt",
                                 m_sLoginType,@"channel",
                                 m_sRefreshToken,@"refreshToken",
                                 sTime,@"time",
                                 nil];
    NSError* pError = nil;
    NSData* pJsonData = [NSJSONSerialization dataWithJSONObject:pDictionary options:NSJSONWritingPrettyPrinted error:&pError];
    if( !pError )
    {
        pJsonString = [[NSString alloc] initWithData:pJsonData encoding:NSUTF8StringEncoding];
        [self runJSCallBack:@"login" jsonStr:pJsonString];
    }
    else
    {
        NSLog(@"JSON error");
        [self runJSCallBack:@"login" jsonStr:@"{\"desc\":\"login jsonEror\",\"result\":-4}"];
    }
    m_bneedLoginCallback=false;
}

-(void)onRefreshTokenCallBack:(NSString *)desc result:(int)result userId:(NSString *)userId headimg:(NSString *)headimg nickname:(NSString *)nickname data:(NSDictionary*) data
{
    NSString* pJsonString=nil;
    NSNumber* sResult = [[NSNumber alloc]initWithInt:result];
    //NSString* sTime = [NSString stringWithFormat:@"%lld",p_nTime];
    
    NSDictionary* pDictionary = [NSDictionary dictionaryWithObjectsAndKeys:
                                 sResult,@"result",
                                 desc,@"desc",
                                 userId,@"userId",
                                 nickname,@"nickname",
                                 headimg,@"headimg",
                                 @"",@"spUserName",
                                 m_sRefreshToken,@"token",
                                 @"",@"stoken",
                                 userId,@"spUserId",
                                  @"wx",@"channel",
                                 data,@"channelExt",
                                 m_sRefreshToken,@"refreshToken",
                                 nil];
    NSError* pError = nil;
    NSData* pJsonData = [NSJSONSerialization dataWithJSONObject:pDictionary options:NSJSONWritingPrettyPrinted error:&pError];
    if( !pError )
    {
        pJsonString = [[NSString alloc] initWithData:pJsonData encoding:NSUTF8StringEncoding];
        [self runJSCallBack:@"refreshToken" jsonStr:pJsonString];
    }

}

-(void)onshareCallBack:(NSString *)desc result:(int)result
{
    NSString* pJsonString=nil;
    NSNumber* sResult = [[NSNumber alloc]initWithInt:result];
    
    NSDictionary* pDictionary = [NSDictionary dictionaryWithObjectsAndKeys:
                                 sResult,@"result",
                                 desc,@"desc",
                                 nil];
    NSError* pError = nil;
    NSData* pJsonData = [NSJSONSerialization dataWithJSONObject:pDictionary options:NSJSONWritingPrettyPrinted error:&pError];
    if( !pError )
    {
        pJsonString = [[NSString alloc] initWithData:pJsonData encoding:NSUTF8StringEncoding];
        [self runJSCallBack:@"share" jsonStr:pJsonString];
    }
    m_bneedShareCallback=false;
}

- (void)onRespWX:(BaseReq *)resp
{
    NSString * desc=nil;
    int result;
    NSString *code=nil;
    if([resp isKindOfClass:[SendAuthResp class]]) {
        SendAuthResp *authResp = (SendAuthResp *)resp;
        //WXSuccess           = 0,    /**< 成功    */
        //WXErrCodeCommon     = -1,   /**< 普通错误类型    */
        //WXErrCodeUserCancel = -2,   /**< 用户点击取消并返回    */
        //WXErrCodeSentFail   = -3,   /**< 发送失败    */
        //WXErrCodeAuthDeny   = -4,   /**< 授权失败    */
        //WXErrCodeUnsupport  = -5,   /**< 微信不支持    */
        switch (authResp.errCode)
        {
            case WXSuccess://ERR_OK
            {
                result=CallBackSuccess;
                desc=@"login success";
                NSLog(@">>>>>>授权成功");
                code = authResp.code;
                //微信平台因为获得openid需要appkey,所以为了安全这些在服务器层做了
                //[self getWXAccessToken:m_sWXAppID appKey:m_sWXAppKey wxcode:nsCode];
                //TODO
            }
                [self getWXAccessToken:m_sAppid wxcode:code];
                return;
            case WXErrCodeCommon:
                result=CallBackError;
                desc=@"login faild";
                NSLog(@">>>>>>普通错误类型");
                break;
            case WXErrCodeUserCancel:
                result=CallBackCancel;
                desc=@"login cancel";
                NSLog(@">>>>>>用户点击取消并返回");
                break;
            case WXErrCodeAuthDeny://ERR_USER_CANCEL
                result=CallBackError;
                desc=@"login error";
                NSLog(@">>>>>>授权失败");
                break;
            case WXErrCodeSentFail://not support
                NSLog(@">>>>>>发送失败");
                break;
            case WXErrCodeUnsupport://not support
                NSLog(@">>>>>>微信不支持");
                break;
            default:
                NSLog(@">>>>>>WXOnResp返回值无效");
                break;
        }
        [self onloginCallBack:desc result:result userId:nil headimg:nil nickname:nil data:nil];
    }
    else if([resp isKindOfClass:[SendMessageToWXResp class]])
    {
        SendMessageToWXResp *sendMessageResp = (SendMessageToWXResp *)resp;
        switch (sendMessageResp.errCode) {
            case WXSuccess:
                desc=@"share success";
                NSLog(@">>>>>>>分享成功");
                result=CallBackSuccess;
                break;
            case WXErrCodeUserCancel:
                desc=@"share cancel";
                result=CallBackCancel;
                NSLog(@">>>>>>>分享取消");
                break;
            default:
                result=CallBackError;
                desc=@"share failed";
                NSLog(@">>>>>>>分享失败>%@",sendMessageResp.errStr);
                break;
        }
        [self onshareCallBack:desc result:result];
        
    }
}


-(void)getWXAccessToken:(NSString*)p_sAppid wxcode:(NSString*)p_sWxCode
{
    NSString *url =[NSString stringWithFormat:@"http://api.masteropen.layabox.com/third/wx_access_token?code=%@&account=xqlaya8&appid=%@&grant_type=authorization_code",p_sWxCode,m_sAppid];

    /*NSString *url =[NSString stringWithFormat:@"https://api.weixin.qq.com/sns/oauth2/access_token?appid=%@&secret=%@&code=%@&grant_type=authorization_code",p_sAppid,p_sAppKey,p_sWxCode];*/
    /*NSString *url =@"http://api.masteropen.layabox.com/third/wx_access_token?code=weixin&account=xqlaya8&appid=wx8817a03c142c6795&grant_type=authorization_code";*/
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        NSURL* zoneUrl = [NSURL URLWithString:url];
        NSString* zoneStr = [NSString stringWithContentsOfURL:zoneUrl encoding:NSUTF8StringEncoding error:nil];
        NSData* data = [zoneStr dataUsingEncoding:NSUTF8StringEncoding];
        dispatch_async(dispatch_get_main_queue(), ^{
            if (data)
            {
                NSDictionary* dic = [NSJSONSerialization JSONObjectWithData:data options:NSJSONReadingMutableContainers error:nil];
                NSDictionary *dicdata=[dic objectForKey:@"data"];
                NSString* nsToken = [dicdata objectForKey:@"access_token"];
                m_sRefreshToken = [dicdata objectForKey:@"refresh_token"];
                NSString* nsOpenID = [dicdata objectForKey:@"unionid"];
                [self getWXUserInfo:nsToken openID:nsOpenID refreshToken:m_sRefreshToken isRefreshToken:false];
            }
            else
            {
                 m_bneedLoginCallback=false;
                [self onloginCallBack:@"login failed getWXAccessToken" result:CallBackError userId:nil headimg:nil nickname:nil data:nil];
            }
        });
    });
}



-(void)getWXUserInfo:(NSString*)p_sTokenID openID:(NSString*)p_sOpenID refreshToken:(NSString*)p_sRefreshToken isRefreshToken:(bool)isRefreshToken
{
    NSString *url =[NSString stringWithFormat:@"https://api.weixin.qq.com/sns/userinfo?access_token=%@&openid=%@",p_sTokenID,p_sOpenID];
    
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        NSURL* zoneUrl = [NSURL URLWithString:url];
        NSString* zoneStr = [NSString stringWithContentsOfURL:zoneUrl encoding:NSUTF8StringEncoding error:nil];
        NSData* data = [zoneStr dataUsingEncoding:NSUTF8StringEncoding];
        dispatch_async(dispatch_get_main_queue(), ^
               {
                   if (data)
                   {
                       NSDictionary* dic = [NSJSONSerialization JSONObjectWithData:data options:NSJSONReadingMutableContainers error:nil];
                       NSString* nsNickName = [dic objectForKey:@"nickname"];
                       NSString* nsHeadimgurl = [dic objectForKey:@"headimgurl"];
                       NSString* userId = [dic objectForKey:@"openid"];
                       if(isRefreshToken)
                       {
                           [self onRefreshTokenCallBack:@"refreshtoken sussecc" result:CallBackSuccess userId:userId headimg:nsHeadimgurl nickname:nsNickName data:dic];
                       }
                       else
                       {
                           m_bneedLoginCallback=false;
                           [self onloginCallBack:@"login sussecc" result:CallBackSuccess userId:userId headimg:nsHeadimgurl nickname:nsNickName data:dic];
                       }
                       
                   }
                   else
                   {
                       if(isRefreshToken)
                       {
                           [self onRefreshTokenCallBack:@"refreshtoken failed getwxuserInfo" result:CallBackError userId:nil headimg:nil nickname:nil data:nil];
                       }
                       else{
                           m_bneedLoginCallback=false;
                           [self onloginCallBack:@"login failed getwxuserInfo" result:CallBackError userId:nil headimg:nil nickname:nil data:nil];
                           
                       }
                   }
               });
    });
}

-(void) dispathByMethodName:(NSString *)methodName param:(NSString *)paramJSON
{
    NSMutableString* name = [[NSMutableString alloc] initWithString:methodName];
    [name appendString:@":"];
    SEL sel=NSSelectorFromString(name);
    if([self respondsToSelector:sel])
    {
        [self performSelector:sel withObject: paramJSON];
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:sel object:nil];
    }
}

+(TencentMarket*) GetTencentMarket
{
   if(g_pTencentMarket==nil)
   {
        g_pTencentMarket=[[TencentMarket alloc] init];
   }
   return g_pTencentMarket;
}


- (void)onSuccessPurchase:(NSString *)p_pProductionIdentifier quantity:(NSInteger)p_iNumber orderID:(NSString *)p_sOrderID
{
    NSString* pJsonString = @"{  \"order_id\" : \"error\",  \"amount\" : \"0\",  \"result\" : -2, \"desc\" : \"error\",  \"product_id\" : \"error\"}";
    NSNumber* sNum = [[NSNumber alloc] initWithInt:p_iNumber];
    NSDictionary* pDictionary = [NSDictionary dictionaryWithObjectsAndKeys:@"0",@"code",p_pProductionIdentifier?p_pProductionIdentifier:@"error",@"product_id",sNum,@"amount",p_sOrderID?p_sOrderID:@"error", @"order_id",@"Success",@"desc",nil];
    NSError* pError = nil;
    NSData* pJsonData = [NSJSONSerialization dataWithJSONObject:pDictionary options:NSJSONWritingPrettyPrinted error:&pError];
    if( !pError )
    {
        pJsonString = [[NSString alloc] initWithData:pJsonData encoding:NSUTF8StringEncoding];
    }
    //TODO Callback
    [self runJSCallBack:@"recharge" jsonStr:pJsonString];
}

- (void)onFailedPurchase:(NSString *)p_pProductionIdentifier quantity:(NSInteger)p_iNumber orderID:(NSString *)p_sOrderID result:(int)result
{
    NSString* pJsonString = @"{  \"order_id\" : \"error\",  \"amount\" : \"0\",  \"result\" : -2, \"desc\" : \"error\",  \"product_id\" : \"error\"}";
    NSNumber*code=[[NSNumber alloc] initWithInt:result];
    NSNumber* sNum = [[NSNumber alloc] initWithInt:p_iNumber];
    NSDictionary* pDictionary = [NSDictionary dictionaryWithObjectsAndKeys:code,@"result",p_pProductionIdentifier?p_pProductionIdentifier:@"error",@"product_id",sNum,@"amount",p_sOrderID?p_sOrderID:@"error", @"order_id",@"error",@"desc",nil];
    NSError* pError = nil;
    NSData* pJsonData = [NSJSONSerialization dataWithJSONObject:pDictionary options:NSJSONWritingPrettyPrinted error:&pError];
    if( !pError )
    {
        pJsonString = [[NSString alloc] initWithData:pJsonData encoding:NSUTF8StringEncoding];
    }
    //TODO Callback
    [self runJSCallBack:@"recharge" jsonStr:pJsonString];
}


/****************************QQ相关*************************************/
- (void)tencentDidLogin
{
    if( m_pTencentOAuth.accessToken && [m_pTencentOAuth.accessToken length] !=0 )
    {
       BOOL a= [m_pTencentOAuth getUserInfo];
        m_bneedLoginCallback=false;
    }
    else
    {
        [self onloginCallBack:@"login failed asscessToken" result:CallBackError userId:nil headimg:nil nickname:nil data:nil];
    }
}

- (void)tencentDidUpdate:(TencentOAuth *)tencentOAuth
{
    if( m_pTencentOAuth.accessToken && [m_pTencentOAuth.accessToken length] !=0 )
    {
        [m_pTencentOAuth getUserInfo];
        m_bneedLoginCallback=false;
    }
    else
    {
        [self onloginCallBack:@"login failed asscessToken" result:CallBackError userId:nil headimg:nil nickname:nil data:nil];
    }
}

/**
 * 登录失败后的回调
 * \param cancelled 代表用户是否主动退出登录
 */
- (void)tencentDidNotLogin:(BOOL)cancelled
{
    if(cancelled)
    {
        [self onloginCallBack:@"login cancel" result:CallBackCancel userId:nil headimg: nil nickname:nil data:nil];
    }
    else{
        [self onloginCallBack:@"login failed" result:CallBackError userId:nil headimg: nil nickname:nil data:nil];
    }
}

/**
 * 登录时网络有问题的回调
 */
- (void)tencentDidNotNetWork
{
    [self onloginCallBack:@"login net error" result:CallBackNetError userId:nil headimg: nil nickname:nil data:nil];
}

/**
 *QQ登录成功回调
 */
-(void) getUserInfoResponse:(APIResponse *)response
{
    if(response.retCode!=0)
    {
        if(!m_bIsRefresh)
            [self onloginCallBack:@"login failed" result:CallBackError userId:nil headimg:nil nickname:nil data:nil];
        else
            [self onRefreshTokenCallBack:@"refreshToken failed" result:CallBackError userId:nil headimg:nil nickname:nil data:nil];
    }
    NSLog(@"token=%@,openID=%@",m_pTencentOAuth.accessToken,m_pTencentOAuth.openId);
    NSLog(@"%@",response.jsonResponse);//打印用户信息
    NSLog(@"%@",[response.jsonResponse objectForKey:@"nickname"]);//打印用户昵称
    NSString* nsNickName = [response.jsonResponse objectForKey:@"nickname"];
    NSString* nsHeadimgurl = [NSString stringWithFormat:@"%@",[response.jsonResponse objectForKey:@"figureurl_qq_2"]];//用户头像
    m_sRefreshToken=m_pTencentOAuth.accessToken;
    if(!m_bIsRefresh)
       [self onloginCallBack:@"login success" result:CallBackSuccess userId:m_pTencentOAuth.openId headimg:nsHeadimgurl nickname:nsNickName data:response.jsonResponse];
    else
        [self onRefreshTokenCallBack:@"refreshToken success" result:CallBackSuccess userId:m_pTencentOAuth.openId headimg:nsHeadimgurl nickname:nsNickName data:response.jsonResponse];
}

- (void) checkQQorWX:(NSString*)type
{
    m_sLoginType=type;
    if([type isEqualToString:@"wx"])
    {
        if(!m_bIsRegWX)
        {
            m_sAppid=@"wx8817a03c142c6795";//@"wx8817a03c142c6795";
            [WXApi registerApp:m_sAppid withDescription:@"LayaBox"];
            m_bIsRegWX=true;
        }
    }
    else{
        if(!m_bIsRegQQ)
        {
            m_sAppid=@"1105844826";
            m_pTencentOAuth = [[TencentOAuth alloc]initWithAppId:m_sAppid andDelegate:self];
            m_bIsRegQQ=true;
        }
    }
}
//QQ分享回调
- (void)onResp:(QQBaseReq *)resp
{
    if([resp isKindOfClass:[SendMessageToQQResp class]]) {
        SendMessageToQQResp* shareResp=(SendMessageToQQResp*)resp;
        NSString* desc=nil;
        int result;
        if([shareResp.result isEqualToString:@"0"])
        {
            result=CallBackSuccess;
            desc=@"qq share success";
        }
        else if([shareResp.result isEqualToString:@"-4"])
        {
            result=CallBackCancel;
            desc=@"qq share cancel";
        }
        else{
            result=CallBackError;
            desc=@"qq share failed";
        }
        [self onshareCallBack:desc result:result];
        
    }
}
/**
 *onResume的时候check回调
 */
-(void) checkCallBack
{
    if(m_bneedLoginCallback)
    {
        [self onloginCallBack:@"login cancel" result:CallBackCancel userId:nil headimg: nil nickname:nil data:nil];
    }
    if(m_bneedShareCallback)
    {
        [self onshareCallBack:@"share cancel" result:CallBackCancel];
    }
}


@end
