//
//  LayaWebViewCacheManager.hpp
//  proj.ios
//
//  Created by layabox_mac on 16/10/11.
//  Copyright © 2016年 layabox_mac. All rights reserved.
//

#ifndef layaWebviewCacheManager_hpp
#define layaWebviewCacheManager_hpp

#include <string>
#include <stdio.h>
#include "laya8cache/Laya8CacheManager.h"
#include "laya8cache/Laya8DownloadMgr.h"

class layaWebviewCacheManager:public laya::Laya8CacheManager
{
public:
    layaWebviewCacheManager();
    ~layaWebviewCacheManager();
    virtual void onHitCache(const char* buff, int length) override;
    virtual void onMissCache(laya::JCDownloadMgr::onEndFunc onEnd) override;
    void setCurrentProtocol(id protocol);
    static layaWebviewCacheManager* getInstance();
    static void delInstance();
private:
    static layaWebviewCacheManager* m_pInstance;
    void* currentProtocol;
};
#endif /* layaWebviewCacheManager_hpp */
