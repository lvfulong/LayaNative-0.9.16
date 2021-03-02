#import "JCIapProcessCtrl.h"
#import "LayaPlatformCallback.h"

#define __SAVE_RECEIPT_ARRAY_KEY_NAME   @"JCMicroClientWaitVerifyRecipts"
#define __MAX_GAME_ID_SIZE     32
#define __RESULT_CODE_SUBMIT_LATER  6

//-------------------------------------------------------------------------------
@implementation PurchaseData
-(id)init
{
    nCount = 1;
    strOrderId = nil;
    strBackUrl = nil;
    return self;
}
@end
//-------------------------------------------------------------------------------
SKPaymentTransaction *gPendingTransaction = nil;
JCIapProcessCtrl* g_pIapProcessCtrl = nil;
@implementation JCIapProcessCtrl
{
    NSMutableArray	*m_pCompleted;  // 完成的内购事务队列
	NSMutableArray	*m_pFailed;     // 失败的内购事务队列
	NSMutableArray	*m_pRestored;   // 恢复的内购事务队列
    
    id<JCIapProcessCtrlDelegate> m_ProcessDelegate; // 内购代理对象
    
    NSString *m_szGameAppID;
    
    BOOL m_bRunning;        // 是否已经开始运行
    BOOL m_bAddedObserver;  // 是否已经加入了内购事务观察器
    BOOL m_bInVerifyReceipt;    // 是否正在进行内购事务的服务器验证
    
    NSArray *m_ProductionArray; // 产品(SKProduction *)列表
    
    NSTimer *m_pTimerReconnection;  // 重连定时器

    NSString *m_strPlateform;
    
    NSMutableData *m_pRecvData;
    
    NSArray*    m_pProducts;        //如果请求产品信息失败，则需要保存在这里，以便重新请求
    
    PurchaseData* m_pPurchaseData;  // 购买的数据
}

+(JCIapProcessCtrl*)getInstance
{
    if( g_pIapProcessCtrl == nil )
    {
        g_pIapProcessCtrl = [[JCIapProcessCtrl alloc]init];
    }
    return g_pIapProcessCtrl;
}

-(id)init
{
	if( self = [super init] )
    {
		m_pCompleted    = [[NSMutableArray alloc] init];
		m_pFailed       = [[NSMutableArray alloc] init];
		m_pRestored     = [[NSMutableArray alloc] init];
        m_szGameAppID = nil;
        m_ProcessDelegate = nil;
        m_bRunning = NO;
        m_bAddedObserver = NO;
        m_bInVerifyReceipt = NO;
        m_ProductionArray = nil;
        m_pTimerReconnection = nil;
        m_pRecvData = nil;
        m_pProducts = nil;
        m_pPurchaseData = nil;
        m_strPlateform = [NSString stringWithFormat:@"%@,%@,%@", [[UIDevice currentDevice] model], [[UIDevice currentDevice] systemName],[[UIDevice currentDevice] systemVersion]];
        NSLog(@"plateform info: %@\n", m_strPlateform);
	}
	return self;
}
-(void)dealloc
{
    [self Stop];
    [[SKPaymentQueue defaultQueue] removeTransactionObserver:self];
}
+ (NSString *)GetLocalizedPriceString:(SKProduct *)p_pProduction
{
    NSNumberFormatter *numberFormatter = [[NSNumberFormatter alloc] init];
    [numberFormatter setFormatterBehavior:NSNumberFormatterBehavior10_4];
    [numberFormatter setNumberStyle:NSNumberFormatterCurrencyStyle];
    [numberFormatter setLocale:p_pProduction.priceLocale];
    NSString *formattedString = [numberFormatter stringFromNumber:p_pProduction.price];
    return formattedString;
}
+ (NSString *)GetTransactionStatusString:(SKPaymentTransactionState)iStatus
{
    switch (iStatus)
    {
        case SKPaymentTransactionStatePurchased:
            return @"Success";
            
        case SKPaymentTransactionStateFailed:
            return @"Failed";
            
        case SKPaymentTransactionStateRestored:
            return @"Restored";
            
        case SKPaymentTransactionStatePurchasing:
            return @"Pending";
            
        default:
            return @"Unknown";
    }
}
+ (id)__GetTopDataInArray:(NSString *)p_pszKeyName
{
    NSArray *pReceiptArray = [[NSUserDefaults standardUserDefaults] arrayForKey:p_pszKeyName];
    if( nil == pReceiptArray || 0 == [pReceiptArray count] )
        return nil;
    else
    {
        return[pReceiptArray objectAtIndex:0];
    }
}
+ (void)__AppandDataToArrayTail:(id)p_pData keyname:(NSString *)p_pszKeyName
{
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    NSArray *pArray = [defaults arrayForKey:p_pszKeyName];
    NSInteger iSize = 1;
    if( nil != pArray )
        iSize += [pArray count];
    NSMutableArray *pActiveArray = [NSMutableArray arrayWithCapacity:iSize];
    if( nil != pArray )
        [pActiveArray addObjectsFromArray:pArray];
    [pActiveArray addObject:p_pData];
    [defaults setObject:(NSArray *)pActiveArray forKey:p_pszKeyName];
    [defaults synchronize];
}
+ (void)__RemoveTopDataFromArray:(NSString *)p_pszKeyName
{
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    NSArray *pArray = [defaults arrayForKey:p_pszKeyName];
    if( nil == pArray )
        return;
    NSMutableArray *pActiveArray = nil;
    if( [pArray count] > 1 )
    {
        pActiveArray = [NSMutableArray arrayWithCapacity:[pArray count]];
        [pActiveArray addObjectsFromArray:pArray];
        [pActiveArray removeObjectAtIndex:0];
        if( 0 == [pActiveArray count] )
            pActiveArray = nil;
    }
    [defaults setObject:(NSArray *)pActiveArray forKey:p_pszKeyName];
    [defaults synchronize];
}
+ (void)__UpdateTopDataByArray:(id)p_pData keyname:(NSString *)p_pszKeyName
{
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    NSArray *pArray = [defaults arrayForKey:p_pszKeyName];
    if( nil == pArray )
        return;
    NSMutableArray *pActiveArray = nil;
    if( [pArray count] >= 1 )
    {
        pActiveArray = [NSMutableArray arrayWithCapacity:[pArray count]];
        [pActiveArray addObjectsFromArray:pArray];
        [pActiveArray removeObjectAtIndex:0];
        [pActiveArray insertObject:p_pData atIndex:0];
    }
    [defaults setObject:(NSArray *)pActiveArray forKey:p_pszKeyName];
    [defaults synchronize];
}
+ (void)__ClearArrayData:(NSString *)p_pszKeyName
{
    [[NSUserDefaults standardUserDefaults] removeObjectForKey:p_pszKeyName];
    [[NSUserDefaults standardUserDefaults] synchronize];
}
// 是否有一个正在处理的交易数据
+ (BOOL)HaveReciptDataInProcess
{
    return (nil != [JCIapProcessCtrl __GetTopDataInArray:__SAVE_RECEIPT_ARRAY_KEY_NAME]);
}
// 删除队列顶端的一个数据
+ (void)RemoveCurrentReceipt
{
    [JCIapProcessCtrl __RemoveTopDataFromArray:__SAVE_RECEIPT_ARRAY_KEY_NAME];
}
///
+ (BOOL)CanMakePayment
{
    // Determine whether payments can be processed.
    if( [SKPaymentQueue canMakePayments] )
    {
        NSLog(@"payments can be processed");
        return YES;
    }
    else
    {
        NSLog(@"payments can NOT be processed");
        return NO;
    }
}
// 设置代理
- (BOOL)Initialize:(id<JCIapProcessCtrlDelegate>) p_ProcessDelegate
{
    m_ProcessDelegate = p_ProcessDelegate;
    return YES;
}
- (BOOL)SetGameID:(NSString *)p_pszGameID
{
    if( nil == p_pszGameID || p_pszGameID.length > __MAX_GAME_ID_SIZE )
    {
        NSLog(@"IapProcessCtrl::SetGameID failed\n");
    }
    m_szGameAppID = [NSString stringWithString:p_pszGameID];
    return [self Start];
}
- (BOOL)Start
{
    if( nil == m_ProcessDelegate )
    {
        NSLog(@"IapProcessCtrl::Start failed, need SetDelegate\n");
        return NO;
    }
    m_bRunning = YES;
    return YES;
}
- (void)Stop
{
    m_ProductionArray = nil;
    m_bRunning = NO;
}
- (NSArray *)GetProductionList
{
    return m_ProductionArray;
}
// 根据 production ID 向 app store 查询产品的详细数据信息
- (BOOL)QueryProductionData:(NSArray *)p_pIdentifiers
{
    if( nil == p_pIdentifiers || 0 == [p_pIdentifiers count] )
    {
        NSLog(@"QueryProductionData param error!\n");
        return NO;
    }
    else
    {
        NSSet *setProductions = [NSSet setWithArray:p_pIdentifiers];
        NSLog(@"+++开始向苹果请求产品信息");
        // 根据 production id 列表向 app store 请求产品的详细信息
        // 这个请求是异步的
        SKProductsRequest *_request= [[SKProductsRequest alloc] initWithProductIdentifiers:setProductions];
        _request.delegate = self;
        [_request start];   // ARC 不需要自己管理 _request 的生存周期了
        NSLog(@"Started production request\n");
        return YES;
    }
}
// 定时器，重新发起查询产品 ID
- (void)OnTimerRequeryProductionIdentifiers:(NSTimer *)p_pTimer
{
}
// SKProductsRequestDelegate 代理接口
- (void)productsRequest:(SKProductsRequest *)request didReceiveResponse:(SKProductsResponse *)response
{
    NSLog(@"+++++苹果返回产品信息。");
    m_pProducts = nil;  //不用了
    // 要保存产品信息列表
    // 依据产品信息列表构建 UI
    m_ProductionArray = response.products;
    
    NSMutableString *resOutput = [[NSMutableString alloc] init];
    SKProduct *_production;
    int n = [m_ProductionArray count];
    
    [resOutput appendString:@"IAP Response Info\n"];
    
     // 遍历产品详细信息列表
    for( int i = 0; i < n; ++i )
    {
        _production = [m_ProductionArray objectAtIndex:i];
        [resOutput appendFormat:@"%@,%@,%@,%@,%@\n",
        _production.localizedTitle,         // 本地化标题
        _production.localizedDescription,   // 本地化描述
        _production.price,                  // 价格
        _production.priceLocale,            // 地域
        _production.productIdentifier];     // 产品标识符
    }
     
    // 错误的产品 ID 回馈
    for( NSString *invalidProductId in response.invalidProductIdentifiers )
    {
        [resOutput appendFormat:@"Invalid product id: %@\n",invalidProductId];
    }
    NSLog(@"%@",resOutput);
    
    int nProductionSize = [m_ProductionArray count];
    if( nProductionSize > 0 )
    {
        SKProduct *pProduction;
        for( int i = 0; i < n; ++i )
        {
            pProduction = [m_ProductionArray objectAtIndex:i];
        }
        SKPayment *pPayment = nil;
        int p_iNum = m_pPurchaseData->nCount;
        if( nil != pProduction )
        {
            SKMutablePayment *paymentmulti = [SKMutablePayment paymentWithProduct:pProduction];
            if( nil == paymentmulti )
            {
                NSLog(@"SKMutablePayment paymentWithProduct FAILED");
                return;
            }
            
            NSMutableDictionary* dict = [[NSMutableDictionary alloc] init];
            [dict setObject:m_pPurchaseData->strOrderId forKey:@"order_id"];
            [dict setObject:m_pPurchaseData->strBackUrl forKey:@"backurl"];
            [dict setObject:m_szGameAppID forKey:@"gameid"];
            NSData* pData = [NSKeyedArchiver archivedDataWithRootObject:dict];
            
            paymentmulti.requestData = pData;
            paymentmulti.quantity = p_iNum;
            pPayment = paymentmulti;
        }
        
        if( nil == pPayment )
        {
            NSLog(@"before addPayment, nil == pPayment\n");
            return;
        }
        
        [[SKPaymentQueue defaultQueue] addPayment:pPayment];
        m_pPurchaseData = nil;
    }
}
-(void)request:(SKRequest *)request didFailWithError:(NSError *)error
{
    NSLog(@"+++请求产品信息出错，重试");
    if( m_pProducts!=nil )
    {
        [self QueryProductionData:m_pProducts];
    }
}
// SKPaymentTransactionObserver 代理接口
// 当事务队列发生变化时被调用（增加或者事务状态变化）
- (void)paymentQueue:(SKPaymentQueue *)queue updatedTransactions:(NSArray *)transactions
{
	for( SKPaymentTransaction *transaction in transactions )
    {
        switch (transaction.transactionState)
        {
            case SKPaymentTransactionStatePurchased:
            {
                NSLog(@">>>>>>>paymentQueue>SKPaymentTransactionStatePurchased");
                gPendingTransaction = transaction;
                [self OnTransactionSuccess:transaction];
                // Asynchronous.  Remove a finished (i.e. failed or completed) transaction from the queue.
                // Attempting to finish a purchasing transaction will throw an exception.
                // [[SKPaymentQueue defaultQueue] finishTransaction: transaction];
                break;
            }
                
            case SKPaymentTransactionStateFailed:
                switch (transaction.error.code)
                {
                    case SKErrorUnknown:
                        NSLog(@"SKErrorUnknown");
                        break;
                    case SKErrorClientInvalid:
                        NSLog(@"SKErrorClientInvalid");
                        break;
                    case SKErrorPaymentCancelled:                        
                        NSLog(@"SKErrorPaymentCancelled");
                        break;
                    case SKErrorPaymentInvalid:
                        NSLog(@"SKErrorPaymentInvalid");
                        break;
                    case SKErrorPaymentNotAllowed:
                        NSLog(@"SKErrorPaymentNotAllowed");
                        break;
                    default:
                        NSLog(@"No Match Found for error");
                        break;
                }
                
                NSLog(@"transaction.error.code %@",[transaction.error description]);
                [self OnTransactionFailed:transaction];
                [[SKPaymentQueue defaultQueue] finishTransaction: transaction];
                break;
                
            case SKPaymentTransactionStateRestored:
                NSLog(@">>>>>>>paymentQueue>SKPaymentTransactionStateRestored");
                gPendingTransaction = transaction;
                [self OnTransactionRestore:transaction];
                //[[SKPaymentQueue defaultQueue] finishTransaction: transaction];
                break;

            case SKPaymentTransactionStatePurchasing:
                break;
                
            default:
                break;
        }
    }
}
- (void) OnTransactionSuccess: (SKPaymentTransaction *)transaction
{
    NSLog(@">>>>>>>>>OnTransactionSuccess");
    [self DoVerifyTransaction:transaction];
}
- (void) OnTransactionFailed: (SKPaymentTransaction *)transaction
{
    NSMutableDictionary *paymentmulti =(NSMutableDictionary *)[NSKeyedUnarchiver unarchiveObjectWithData:transaction.payment.requestData];
    NSString* _order_id = [paymentmulti objectForKey:@"order_id"];
    
    [m_ProcessDelegate onFailedPurchase:transaction.payment.productIdentifier quantity:transaction.payment.quantity orderID:_order_id];
}
- (void) OnTransactionRestore: (SKPaymentTransaction *)transaction
{
    //[JCIapProcessCtrl UpdateCurrentReceiptData:transaction];
    NSLog(@">>>>>>>>>OnTransactionRestore");
    [self DoVerifyTransaction:transaction];
}
// 定时器，重新发起查询产品 ID
- (void)OnTimerRequeryVerifyReceipt:(NSTimer *)p_pTimer
{
   
}
- (void)SetIAPCallBack
{    
    if(m_bAddedObserver == NO)
    {
        m_bAddedObserver = YES;
        [[SKPaymentQueue defaultQueue] addTransactionObserver:self];
    }
}
- (void)OnIAPPurchaseResult:(NSString*)p_param
{
    if(gPendingTransaction == nil)
        return;
    NSLog(@">>OnIAPPurchaseResult param=%@",p_param);
    NSData* pJsonData = [p_param dataUsingEncoding:NSUTF8StringEncoding];
    NSError* pError;
    NSDictionary* pJson = [NSJSONSerialization JSONObjectWithData:pJsonData options:kNilOptions error:&pError];
    if( !pError )
    {
        NSString* nsQuantity = [pJson objectForKey:@"quantity"];
        NSString* nsOrderID = [pJson objectForKey:@"orderID"];
        NSString* resultCode = [pJson objectForKey:@"code"];
        int nCode = [resultCode intValue];
        
        if( nCode == 0 ) // 成功
            [m_ProcessDelegate onSuccessPurchase:gPendingTransaction.payment.productIdentifier quantity:[nsQuantity intValue] orderID:nsOrderID];
        else
            [m_ProcessDelegate onFailedPurchase:gPendingTransaction.payment.productIdentifier quantity:[nsQuantity intValue] orderID:nsOrderID];
    }
    else
    {
        [m_ProcessDelegate onFailedPurchase:gPendingTransaction.payment.productIdentifier quantity:0 orderID:@"null"];
    }
    [[SKPaymentQueue defaultQueue] finishTransaction: gPendingTransaction];
    gPendingTransaction = nil;
}
-(void)storeFinishTransaction
{
    [[SKPaymentQueue defaultQueue] finishTransaction: gPendingTransaction];
    gPendingTransaction = nil;
}
// 向验证服务器发起验证请求
- (void)DoVerifyTransaction: (SKPaymentTransaction *)transaction
{
    if( transaction != nil )
    {
        if(NULL==transaction.payment.requestData)
        {
            NSLog(@">>>>>>>>storeFinishTransaction  requestData is null");
            [self storeFinishTransaction];
            return;
        }
        NSMutableDictionary *paymentmulti =(NSMutableDictionary *)[NSKeyedUnarchiver unarchiveObjectWithData:transaction.payment.requestData];//
        NSString* _order_id = [paymentmulti objectForKey:@"order_id"];
        NSString* _backurl = [paymentmulti objectForKey:@"backurl"];
        NSString* _gameid = [paymentmulti objectForKey:@"gameid"];
        NSLog(@">>>>>order:%@backurl:%@gameid:%@",_order_id,_backurl,_gameid);
        
        NSString* pszBase64 = [transaction.transactionReceipt base64Encoding];
        if(NULL==_order_id||NULL==_backurl||NULL==_gameid||NULL==m_strPlateform||NULL==transaction.payment.productIdentifier||NULL==transaction.transactionIdentifier)
        {
            NSLog(@">>>>>>>>storeFinishTransaction _backurl is NULL");
            [self storeFinishTransaction];
            return;
        }
        
        NSMutableDictionary* dict = [[NSMutableDictionary alloc] init];
        
        [dict setObject:_backurl forKey:@"backurl"];
        [dict setObject:pszBase64 forKey:@"receipt_data"];
        [dict setObject:m_strPlateform forKey:@"plateform"];
        [dict setObject:_order_id forKey:@"order_id"];
        [dict setObject:_gameid forKey:@"game_id"];
        [dict setObject:transaction.payment.productIdentifier forKey:@"production_id"];
        [dict setObject:transaction.transactionIdentifier forKey:@"transaction_id"];
        [dict setObject:[NSString stringWithFormat: @"%d", transaction.payment.quantity] forKey:@"production_amount"];
        
        if([NSJSONSerialization isValidJSONObject:dict]){
            NSError *error;
            NSData *jsonData = [NSJSONSerialization dataWithJSONObject:dict options:NSJSONWritingPrettyPrinted error:&error];
            NSString *buffer =[[NSString alloc] initWithData:jsonData encoding:NSUTF8StringEncoding];
            
            [[LayaPlatformCallback GetInstance] LP_IAPStoreCallBack:buffer];
        }else{
            NSLog(@"An error happened while serializing the JSON data.");
        }
    }
}
// 购买
- (BOOL)addPayment:(NSString *)p_pProductIdentifier quantity:(NSInteger)p_iNum orderID:(NSString *)p_sOrderID callbackUrl:(NSString*)p_sUrl
{
    if([JCIapProcessCtrl CanMakePayment] == NO)
    {
        [m_ProcessDelegate onFailedPurchase:p_pProductIdentifier quantity:p_iNum orderID:p_sOrderID];
        return NO;
    }
    if( !m_bRunning || nil == p_sUrl || m_pPurchaseData != nil)
    {
        if(m_pPurchaseData != nil){
            NSLog(@"++当前有支付请求，orderid=%@\n",m_pPurchaseData->strOrderId);
        }
        return NO;
    }
    
    if( p_iNum <= 0 || nil == p_pProductIdentifier || nil == p_sOrderID )
    {
        return NO;
    }
    
    NSArray* transactions = [SKPaymentQueue defaultQueue].transactions;
    if (transactions.count > 0)
    {
        //检测是否有未完成的交易
        SKPaymentTransaction* transaction = [transactions firstObject];
        if (transaction.transactionState == SKPaymentTransactionStatePurchased)
        {
            [self OnTransactionSuccess:transaction];
            return true;
        }  
    }
    if( m_pPurchaseData == nil )
        m_pPurchaseData = [[PurchaseData alloc] init];
    m_pPurchaseData->nCount = p_iNum;
    m_pPurchaseData->strOrderId = p_sOrderID;
    m_pPurchaseData->strBackUrl = p_sUrl;
    m_pProducts = [[NSArray alloc] initWithObjects:p_pProductIdentifier, nil];
    [self QueryProductionData:m_pProducts];
    return true;
}

@end
