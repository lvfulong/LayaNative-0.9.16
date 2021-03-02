#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>
#import <conchRuntime.h>

@interface ViewController : GLKViewController
{
@public
    
    GLKView*                    m_pGLKView;
    
    EAGLContext*                m_pGLContext;
    
    conchRuntime*               m_pConchRuntime;
    CADisplayLink*              m_displayLink;
}
+(ViewController*)GetIOSViewController;
-(id)init;
- (void)startDisplayLink;
- (void)handleDisplayLink:(CADisplayLink *)displayLink;
- (void)stopDisplayLink;
@end

