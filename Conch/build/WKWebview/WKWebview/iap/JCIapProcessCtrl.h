#import <Foundation/Foundation.h>
#import <StoreKit/StoreKit.h>

@protocol JCIapProcessCtrlDelegate <NSObject>
@required

/*
 *  购买成功
 */
- (void)onSuccessPurchase:(NSString *)p_pProductionIdentifier quantity:(NSInteger)p_iNumber orderID:(NSString *)p_pszOrderID;

/*
 *  购买恢复
 */
- (void)onRestoredPurchase:(NSString *)p_pProductionIdentifier quantity:(NSInteger)p_iNumber orderID:(NSString *)p_pszOrderID;

/*
 *  购买失败
 */
- (void)onFailedPurchase:(NSString *)p_pProductionIdentifier quantity:(NSInteger)p_iNumber orderID:(NSString *)p_pszOrderID;

@optional
/*
 *  购买请求已经加入服务器处理队列
 */
- (void)onPendingPurchase:(NSString *)p_pProductionIdentifier;

@end

//购买的数据
@interface PurchaseData : NSObject
{
@public
    int nCount;
    NSString* strOrderId;
    NSString* strBackUrl;
}
-(id)init;
@end

////////////////////
// IapProcessCtrl //
////////////////////
@interface JCIapProcessCtrl : NSObject<SKProductsRequestDelegate,SKPaymentTransactionObserver>

// 获取产品的本地化的价格描述字符串，例如：￥6.00
+ (NSString *)GetLocalizedPriceString:(SKProduct *)p_pProduction;

// 判断设备是否可以支付
+ (BOOL)CanMakePayment;

+(JCIapProcessCtrl*)getInstance;

/*
 *  初始化部分
 */
// 设置代理
// 设置获取 Product Identifiers 列表的 URL
// 设置验证服务器 URL
- (BOOL)Initialize:(id<JCIapProcessCtrlDelegate>) p_ProcessDelegate;

// 设置获取 Product Identifiers 列表的 URL 和当前游戏的标识字串
- (BOOL)SetGameID:(NSString *)p_pszGameID;

// 设置完成以后启动服务
//- (BOOL)Start;

- (void)Stop;

//IOS内购结果
- (void)OnIAPPurchaseResult:(NSString*)p_param;
//设置内购回调函数
- (void)SetIAPCallBack;
// 购买
// p_pProductIdentifier 产品标识
// p_iNum 购买数量
- (BOOL)addPayment:(NSString *)p_pProductIdentifier quantity:(NSInteger)p_iNum orderID:(NSString *)p_sOrderID callbackUrl:(NSString*)p_sUrl;

- (NSArray *)GetProductionList;

@end


