#import "LayaPlatformFactory.h"

//-------------------------------------------------------------------------------
static LayaPlatformFactory* g_pPlatformFactory = nil;
//-------------------------------------------------------------------------------
@implementation LayaPlatformFactory
//-------------------------------------------------------------------------------
-(id) init
{
    self = [super init];
    return self;
}
//-------------------------------------------------------------------------------
-(id<LayaPlatformInterface>) CreateInterface:(NSString*)className
{
    Class pClass = NSClassFromString( className );
    id<LayaPlatformInterface> pInterface = [[pClass alloc]init];
    return pInterface;
}
//-------------------------------------------------------------------------------
+(id) GetInstance
{
    if( g_pPlatformFactory == nil )
    {
        g_pPlatformFactory = [[LayaPlatformFactory alloc] init];
    }
    return g_pPlatformFactory;
}
//-------------------------------------------------------------------------------
-(void) setCacheDir:(const std::string&) sDirPath
{
    //laya::JSMarket::ms_sCacheDir = sDirPath;
}
//-------------------------------------------------------------------------------
-(std::string) getLocalInfoFromCache
{
    //std::string sInfo = laya::JCScriptRuntime::s_JSRT->m_pMarket->getLocalInfo();
    //return sInfo;
    return "";
}
//-------------------------------------------------------------------------------
-(void)writeLocalInfoToCache:(const char*)sInfo
{
    //laya::JCScriptRuntime::s_JSRT->m_pMarket->writeLocalInfo(sInfo);
}
//-------------------------------------------------------------------------------

@end
