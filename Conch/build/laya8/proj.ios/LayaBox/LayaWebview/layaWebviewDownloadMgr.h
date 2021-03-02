//
//  LayaBrowserDownloadMgr.hpp
//  proj.ios
//
//  Created by layabox_mac on 16/10/12.
//  Copyright © 2016年 layabox_mac. All rights reserved.
//

#ifndef layaWebviewDownloadMgr_hpp
#define layaWebviewDownloadMgr_hpp

#include "resource/JCFileResManager.h"
#include "downloadMgr/JCDownloadMgr.h"

class layaWebviewDownloadMgr :public laya::JCDownloadMgr {
public:
        virtual void download(const char* p_pszURL, int p_nPriority,
                          onProgressFunc p_ProgCb,
                          onEndFunc p_CompleteCb,
                          int p_nOptTimeout);
    void setCurrentProtocol(id protocol);
    bool onCacheHit(void* p_pRes, boost::weak_ptr<int> callbackref);
    void setOnHitCacheCallBack(laya::JCFileRes* pRes);
private:
    void load(onEndFunc p_CompleteCb);
    void* currentProtocol;
    boost::shared_ptr<int>	m_CallbackRef;
};
struct OnEndFuncObject
{
    OnEndFuncObject(laya::JCDownloadMgr::onEndFunc p_CompleteCb):onEnd(p_CompleteCb)
    {
    }
    laya::JCDownloadMgr::onEndFunc onEnd;
    void call(const char* buff, int length, int statusCode);
};
#endif /* layaWebviewDownloadMgr_hpp */
