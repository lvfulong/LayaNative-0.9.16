#import <Foundation/Foundation.h>
#import <StoreKit/StoreKit.h>

@protocol JCIapProcessCtrlDelegate <NSObject>
@required
- (void)onSuccessPurchase:(NSString *)p_pProductionIdentifier quantity:(NSInteger)p_iNumber orderID:(NSString *)p_pszOrderID;
- (void)onFailedPurchase:(NSString *)p_pProductionIdentifier quantity:(NSInteger)p_iNumber orderID:(NSString *)p_pszOrderID result:(int)result;
@end

@interface IAPManager : NSObject<SKProductsRequestDelegate,SKPaymentTransactionObserver>
+ (IAPManager*) getInstance;
-(void)recharge:(NSString*) jsonParam;
- (id)initWithGameID:(NSString*)gameID andDelegate:(id<JCIapProcessCtrlDelegate>)delegate;
@end


