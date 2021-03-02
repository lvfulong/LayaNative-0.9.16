﻿/**
@file            JCApp.cpp
@brief            
@author            GZK
@version        1.0
@date            2014_2_9
*/

#include "JCConchApp.h"
#include <Windows.h>
#include "winWindows.h"
#include "gles.h"
#include "shlobj.h"
//#include "FontRender/FreeTypeRender.h"
#include <util/Log.h>

extern int g_nInnerWidth;
extern int g_nInnerHeight;
namespace laya{

    void dispatchRunCmdEvtToConch(JCConch* pConch, JCEventEmitter::evtPtr evt){
        pConch->onRunCmdInMainThread(evt->m_nP1, evt->m_nP2, (int)(evt->getData()) );
    }

    void dispatchKeyUpEvtToConch(JCConch* pConch, JCEventEmitter::evtPtr evt ){
        /*
        if(evt->m_nP1=='K'){
            pConch->closeCurActiveProcess();
        }
        */
    }

    JCConchApp::JCConchApp(){
        m_bUseUpdate = false;
        m_pGLEnv = new GLEnvThread();
        m_pWindows = m_pGLEnv->m_pWindows;//  new winWindows();
        //m_pGLES = NULL;
        m_pConch = new JCConch(3);
        m_pConch->m_funcPostMsgToMainThread = std::bind(&JCConchApp::postCmdToWindow, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        m_pWindows->on(winWindows::Evt_WndClose, std::bind(&JCConchApp::exitApp,this));
        m_pWindows->on(winWindows::Evt_RunCmd, std::bind(dispatchRunCmdEvtToConch, m_pConch, std::placeholders::_1));
        m_pWindows->on(winWindows::Evt_KeyUp, std::bind(dispatchKeyUpEvtToConch, m_pConch, std::placeholders::_1));
        m_bExiting = false;
    }

    JCConchApp::~JCConchApp(){
        //if( m_pWindows )
        //    delete m_pWindows;
        if( m_pConch )
            delete m_pConch;
        m_pConch = NULL;
        if (m_pGLEnv) {
            delete m_pGLEnv;
        }
    }

    void JCConchApp::startApp(){
        m_pGLEnv->init( (JCIGLRender*)JCConch::s_pConchRender.get(), g_nInnerWidth,g_nInnerHeight );
        JCConch::s_pConchRender->m_pRenderThread = &m_pGLEnv->m_RenderThread;
        m_pConch->onAppStart();
        onResume();
    }

    char* JCConchApp::getFontfilePath(char* pFontfilePath){
        char filePath[MAX_PATH];
        SHGetSpecialFolderPath( m_pWindows->m_hWnd , filePath , CSIDL_FONTS , FALSE);
        int size= strlen(filePath);
        strcpy(pFontfilePath,filePath);
        //wcstombs(pFontfilePath, filePath, size+1);
        return pFontfilePath;
    }
    
    void JCConchApp::exitApp(){
        if(m_bExiting)
            return;
        m_bExiting = true;
        onDestroy();
        m_pGLEnv->stop();
    }

    void JCConchApp::onIdle(){
    }

    void JCConchApp::onPause(){
        m_pGLEnv->onPause();
    }

    void JCConchApp::onRenderThreadStop(JCEventEmitter::evtPtr evt){
        //TODO gles 要退出了。需要通知
    }

    void JCConchApp::onResume(){
        m_pGLEnv->onResume();
    }

    void JCConchApp::onDestroy(){
        if( m_pConch )
            m_pConch->onAppDestory();
    }

    void JCConchApp::onUpdate(float dt){
    }

    void JCConchApp::postMsgToWindow(unsigned int nMsg, unsigned int param1, unsigned long param2 ){
        PostMessage(g_hWnd, nMsg, param1,param2);
    }

    void JCConchApp::postCmdToWindow(unsigned int nCmd, unsigned int param1, unsigned long param2){
        PostMessage(g_hWnd, nCmd, param1,param2);
    }
}
