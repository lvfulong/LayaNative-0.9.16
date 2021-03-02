//
//  LayaBrowserDownloadMgr.cpp
//  proj.ios
//
//  Created by layabox_mac on 16/10/12.
//  Copyright © 2016年 layabox_mac. All rights reserved.
//

#include "layaWebviewDownloadMgr.h"
#include "URLSessionProtocol.h"
#include "resource/JCFileResManager.h"

void layaWebviewDownloadMgr::download(const char* p_pszURL, int p_nPriority,
                                      onProgressFunc p_ProgCb,
                                      onEndFunc p_CompleteCb,
                                      int p_nOptTimeout) {
    m_bCancelTask = false;
    if (0 != p_pszURL) {
        if (strlen(p_pszURL) <= 0) {
            return;
        }
        load(p_CompleteCb);
    }
}

bool layaWebviewDownloadMgr::onCacheHit(void* p_pRes, boost::weak_ptr<int> callbackref)
{
    if( !callbackref.lock() )
        return false;
    
    laya::JCResStateDispatcher* pRes = (laya::JCResStateDispatcher*)p_pRes;
    laya::JCFileRes* pFileRes = (laya::JCFileRes*)pRes;
    
    if( pFileRes->m_pBuffer.get()==NULL || pFileRes->m_nLength==0 ){
        return false;
    }
    
    if (pFileRes-> m_nLastAction != laya::JCFileRes::LOADFROMCACHE)
        return false;
    URLSessionProtocol* protocol = (__bridge URLSessionProtocol*)this->currentProtocol;
    [protocol onCacheHit:pFileRes->m_pBuffer.get() length:pFileRes->m_nLength];
    return true;
}
                    
void layaWebviewDownloadMgr::setCurrentProtocol(id protocol)
{
    this->currentProtocol = (__bridge void*)protocol;
}

void layaWebviewDownloadMgr::load(onEndFunc p_CompleteCb)
{
    URLSessionProtocol* protocol = (__bridge URLSessionProtocol*)this->currentProtocol;
    void* func = new OnEndFuncObject(p_CompleteCb);
    [protocol load:func];
}

void layaWebviewDownloadMgr::setOnHitCacheCallBack(laya::JCFileRes* pRes)
{
    m_CallbackRef.reset(new int(1));
    boost::weak_ptr<int> cbref(m_CallbackRef);
    pRes->setOnReadyCB( std::bind(&layaWebviewDownloadMgr::onCacheHit,this, std::placeholders::_1,cbref));
}

void OnEndFuncObject::call(const char* buff, int length, int statusCode)
{
    laya::JCBuffer buffer;
    buffer.create(length);
    memcpy(buffer.m_pPtr, buff, length);
    this->onEnd(buffer, "", "",0,statusCode,"");
}

