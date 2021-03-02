#import <UIKit/UIKit.h>
#import "wx/WXApi.h"
#import <TencentOpenAPI/TencentOAuth.h>
#import <TencentOpenAPI/QQApiInterface.h>
@interface AppDelegate : UIResponder <UIApplicationDelegate,WXApiDelegate>
{
@public
    UIBackgroundTaskIdentifier m_kBackgroundTask;
}
@property (strong, nonatomic) UIWindow *window;
@end
