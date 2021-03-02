//
//  TencentMarket.h
//  Laya8
//
//  Created by LayaBox on 11/17/16.
//  Copyright © 2016 LayaBox. All rights reserved.
//
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import "wx/WXApi.h"
#import "iap/IAPManager.h"
#import <TencentOpenAPI/TencentOAuth.h>
#import <TencentOpenAPI/QQApiInterface.h>
enum  CallBackErrCode {
    CallBackSuccess           = 0,    /**< 成功    */
    CallBackCancel            = -1,   /**< 取消   */
    CallBackError             = -2,   /**< 错误    */
    CallBackNetError        =-3, /**< 网络错误 */
    CallBackJSONError   =      -4,   /**< 发送失败    */
    CallBackNotSupport   =     -50,   /**< 不支持    */
};
@interface TencentMarket : NSObject<JCIapProcessCtrlDelegate,TencentSessionDelegate,QQApiInterfaceDelegate>
{
    NSString*       m_sAuthorizeType;
    NSString*       m_sAppid;
    NSString*       m_sRefreshToken;
    TencentOAuth*   m_pTencentOAuth;
    NSString*       m_sLoginType;
    bool            m_bIsRegQQ;
    bool            m_bIsRegWX;
    bool            m_bIsRefresh;
    bool            m_bneedLoginCallback;
    bool            m_bneedShareCallback;
}
+ (TencentMarket*)GetTencentMarket;
- (void)login:(NSString*)jsonParam;
- (void)share:(NSString*)jsonParam;
- (void)recharge:(NSString*)jsonParam;
- (void)refreshToken:(NSString*)jsonParam;
- (void)getWXAccessToken:(NSString*)p_sAppid wxcode:(NSString*)p_sWxCode;
- (void)getWXUserInfo:(NSString*)p_sTokenID openID:(NSString*)p_sOpenID refreshToken:(NSString*)p_sRefreshToken isRefreshToken:(bool)isRefreshToken;
- (void)onRespWX:(BaseReq *)resp;
- (void)onResp:(QQBaseReq *)resp;
- (void)dispathByMethodName:(NSString*)methodName param:(NSString*)paramJSON;
- (void)onloginCallBack:(NSString*)desc result:(int)result userId:(NSString*) userId headimg:(NSString*)headimg nickname:(NSString*) nickname data:(NSDictionary *)data;
- (void)onshareCallBack:(NSString*)desc result:(int)result;
- (void)onRefreshTokenCallBack:(NSString*)desc result:(int)result userId:(NSString*) userId headimg:(NSString*)headimg nickname:(NSString*) nickname  data:(NSDictionary *)data;
- (void)onRecharge:(NSString*)desc result:(int)result;
- (UIImage *) getImageFromURL:(NSString *)fileURL;
- (void) runJSCallBack:(NSString*)methodName jsonStr:(NSString*)jsonStr;
- (void) checkQQorWX:(NSString*)type;
- (void) checkCallBack;
- (instancetype)init;
@end
