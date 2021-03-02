#ifdef _TEST_
/*
    ��Ⱦ���̵Ĳ��ԡ�
        ��Ⱦ�����ǿɲ�ֵ�

    ���������Ŀ
*/

#include "../JCTestManager.h"
#include <Windows.h>
#include "../../WindowsEnv/winWindows.h"
#include "../../WindowsEnv/gles.h"
#include <misc/JCWorkerThread.h>
#include <JCIGLRender.h>
#include <functional>

/*
    1.�������̴߳���һ��gles����������ԭʼ����Ⱦ
    2.����һ���¿����߳��д�һ��gles��Ⱦ
    3.����Ⱦ3D����
    4.�ܻ����Ⱦ���
*/

using namespace laya;

namespace renderFrameTest{
    void _1_testRender_GLTread(){
        class TestRender:public JCIGLRender{
        public:
            float   fR = 0.0f;
            int     nFrmNum = 0;
            std::function<void()> funcExit;
            TestRender(std::function<void()> f) {
                funcExit = f;
            }
            virtual void onGLReady(){
            }

            virtual void renderFrame(long p_nCurrentFrame,bool){
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                fR += 0.1f;
                if (fR > 1.0f)fR = 0.0f;
                glClearColor(fR,1.0f,1.0f,1.0f);
                nFrmNum++;
                if (nFrmNum > 100) {
                    funcExit();
                }
            }

            virtual void invalidGLRes() {
            }
            virtual void freeGLRes() {
            }
        };

        GLEnvThread gl;
        TestRender render([&gl](){ gl.m_pWindows->close(); });
        gl.init(&render,800,600);
        winWindows::handleMessage(NULL);
    }
    void testMain(){
        _1_testRender_GLTread();
    }

    ADDTESTCASE(testMain,"renderFrameTest");
}
#endif
