//
//  LayaWebViewCacheManager.cpp
//  proj.ios
//
//  Created by layabox_mac on 16/10/11.
//  Copyright © 2016年 layabox_mac. All rights reserved.
//

#include "layaWebviewCacheManager.h"
#import "URLSessionProtocol.h"

std::string gRedistPath = "";
//  std::string gResourcePath = "";
#ifdef __APPLE__
std::string gAssetRootPath = "";
#endif

layaWebviewCacheManager* layaWebviewCacheManager::m_pInstance = nullptr;

layaWebviewCacheManager::layaWebviewCacheManager()
{
}

layaWebviewCacheManager::~layaWebviewCacheManager()
{
}

layaWebviewCacheManager* layaWebviewCacheManager::getInstance(){
    if( m_pInstance == NULL){
        m_pInstance = new layaWebviewCacheManager();
    }
    return m_pInstance;
}

void layaWebviewCacheManager::delInstance(){
    if(m_pInstance){
        delete m_pInstance;
        m_pInstance = NULL;
    }
}

void layaWebviewCacheManager::setCurrentProtocol(id protocol)
{
    this->currentProtocol = (__bridge void*)protocol;
}

void layaWebviewCacheManager::onHitCache(const char* buff, int length)
{
    URLSessionProtocol* protocol = (__bridge URLSessionProtocol*)this->currentProtocol;
    [protocol onCacheHit:buff length:length];
}

void layaWebviewCacheManager::onMissCache(laya::JCDownloadMgr::onEndFunc onEnd)
{
    URLSessionProtocol* protocol = (__bridge URLSessionProtocol*)this->currentProtocol;
    ((laya::Laya8CacheData*)protocol.laya8CacheData)->onEnd = onEnd;
    [protocol load];
}
