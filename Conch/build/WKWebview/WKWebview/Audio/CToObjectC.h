/**
@file			CToObjectC.h
@brief			
@author			wyw
@version		1.0
@date			2014_8_26
*/

#ifndef _CToObjectC_H_
#define _CToObjectC_H_

#include <stdio.h>
#include <string>
#include <vector>
#include <JavaScriptCore/JSBase.h>
#include <resource/Audio/JCAudioInterface.h>
#include <resource/Audio/JCWaveInfo.h>

//以下是Market相关的
//------------------------------------------------------------------------------
//获得marketName
std::string CToObjectCGetMarketName();
//获得支付的方式 0 为充值 1为购买道具
int CToObjectCGetPayType();
//充值方式 0为人民币充值传入元。 1为代币充值，充值传入元宝数量
int CToObjectCGetChargeType();
//获得登陆方式
int CToObjectCGetLoginType();
//获得平台的类型
int CToObjectCGetEnterPlatformType();
//平台初始化
void CToObjectCMarketInit( const char* jsonParam );
//平台登录
void CToObjectCMarketLogin( const char* jsonParam );
//平台切换用户
void CToObjectCMarketSwitchUser( const char* jsonParam );
//平台登录退出
void CToObjectCMarketLogout( const char* jsonParam );
//平台代币充值 sProjectName根据自己需要填写
void CToObjectCMarketUniPayForCoin( const char* jsonParam );
//社区
void CToObjectCMarketEnterPlatform( const char* jsonParam );
//退出社区
void CToObjectCMarketLeavePlatform( const char* jsonParam );
//进入游戏，或者进入大厅
void CToObjectCMarketOnGameEvent( const char* jsonParam );
//进入BBS
void CToObjectCMarketEnterBBS( const char* jsonParam );
//进入游戏反馈
void CToObjectCMarketEnterFeedback( const char* jsonParam );
//暂停的处理，当从后台转为前台的时候处理
void CToObjectCMarketOnPause( const char* jsonParam );
//退出的处理函数  如果没有处理一定返回false 否则无法退出
bool CToObjectCMarketOnExit( const char* jsonParam );
//充值的时候设置的信息
void CToObjectCMarketSetRechargeInfo( const char* jsonParam );
//账户管理
void CToObjectCMarketEnterAccountMgr( const char* jsonParam );
//购买道具
void CToObjectCMarketBuyProps( const char* jsonParam );
//分享
void CToObjectCMarketEnterShareAndFeed( const char* jsonParam );
//facebook的邀请
void CToObjectCMarketEnterInvite( const char* jsonParam );
//授权
void CToObjectCMarketAuthorize( const char* jsonParam );
//刷新token
void CToObjectCMarketRefreshToken( const char* jsonParam );
//获得好友列表
void CToObjectCMarketGetGameFriends( const char* jsonParam );
//发送桌面图标
void CToObjectCMarketSendToDesktop( const char* jsonParam );
//能否发送到桌面
int CToObjectCMarketCanSendToDesktop( const char* jsonParam );
//发送消息给平台
void CToObjectCMarketSendMessageToPlatform( const char* jsonParam );
//打开话题圈
void CToObjectCMarketOpenTopicCircle( const char* jsonParam );
//设置IOS内购结果
void CToObjectCsetIAPPurchaseResult( const char* jsonParam);
//设置IOS内购结果
void CToObjectCSetIAPCallBack();

//------------------------------------------------------------------------------
//以下是声音相关的
//------------------------------------------------------------------------------
void CToObjectCPlayMp3Audio( const char* p_sUrl,int p_nTimes );
void CToObjectCSetMp3Volume( float p_nVolume );
void CToObjectCStopMp3();
void CToObjectCResumeMp3();
void CToObjectCPauseMp3();

//------------------------------------------------------------------------------
//本地推送
void CToObjectCSetRepeatNotify( int p_nID,long p_nStartTime,int p_nRepeatType,const char* p_sTickerText,const char* p_sTitleText,const char* p_sDesc );
void CToObjectCSetOnceNotify( int p_nID,long p_nStartTime,const char* p_sTickerText,const char* p_sTitleText,const char* p_sDesc );
void CToObjectCDeleteOnceNotify( int p_nID );
void CToObjectCDeleteAllNotify();

//------------------------------------------------------------------------------
//以下是一些杂项
//------------------------------------------------------------------------------
//获得总的内存数
long CToObjectCGetTotalMem();
long CToObjectCGetUsedMem();
long CToObjectCGetAvalidMem();
float CToObjectCGetScreenInch();
void CToObjectCSetScreenOrientation( int p_nType );
long CToObjectCGetScreenOrientation();
int CToObjectCGetNetworkType();
std::string CToObjectCGetWifiMac();
std::string CToObjectCGetDeviceModel();
std::string CToObjectCGetDeviceInfo();
void CToObjectCShowLoadingView( bool p_bShow );
void CToObjectCShowWaiting( bool p_bShow );
void CToObjectCSetExternalLink( const char* p_sUrl );
void CToObjectCCloseExternalLink();
void CToObjectCSetScreenWakeLock( bool p_bWakeLock );
void CToObjectCShowAssistantTouch( bool p_bShow );
void CToObjectCShowToast( const char* p_sInfo );
void CToObjectCOpenAppStoreUrl( const char* p_sAppID );
bool CToObjectCGetIsInstalledWx();

#endif //_CToObjectC_H_

//-----------------------------END FILE--------------------------------