/**
@file			JCAudioManager.h
@brief			
@author			dt
@version		1.0
@date			2014_11_26
*/

#ifndef __JCAudioManager_H__
#define __JCAudioManager_H__

//包含头文件
#ifdef ANDROID
	#include "android/JCAudioMp3Media.h"
#elif WIN32
	#include "windows/JCAudioMp3Player.h"	
#elif __APPLE__
    #include "ios/JCAudioMp3Player.h"
#endif

#include "resource/Audio/JCAudioWavPlayer.h"
#include <map>
#include <vector>
#include "resource/Audio/JCWaveInfo.h"
#include "buffer/buffer.h"
#include "resource/Audio/JCMp3Interface.h"

namespace laya
{
/** 
 * @brief 
*/
class JCAudioManager
{
public:

	/** @brief构造函数
	*/
	JCAudioManager( void );

	/** @brief析构函数
	*/
	~JCAudioManager( void );

	/*
	 * getInstace()
	*/
	static JCAudioManager* GetInstance( void );
	static void ClearAllWork();
	static void DelInstance( void );

public:

	//以下是mp3的接口
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	void createMp3player();
	//设置全局的静音
	void setMp3Mute( bool p_bMute );

	//获得全局的静音
	bool getMp3Mute( void );

	//全局音量
	void setMp3Volume( float p_nVolume );

	//获得音量
	float getMp3Volume( void );

	void playMp3( const char* p_sUrl,int p_nTimes,JCAudioInterface* p_pJSAudio );
	void delMp3Obj(JCAudioInterface* p_pJSAudio);	//如果js对象删除了，就要调用这个

	void pauseMp3();

	void stopMp3();
	
	void resumeMp3();

	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------
	//------------------------------------------------------------------------------

public:

	/** @brief 清空所有
	*/
	bool ClearAllAudioBufferPlay( void );

    /*
        停止单个音效
    */
    void stopWav( JCAudioInterface* p_pAudio );


	//设置全局的静音 音效
	void setWavMute( bool p_bMute );

	//获得全局的静音 音效
	bool getWavMute( void );

	//全局音量 音效
	void setWavVolume( float p_nVolume );

	//获得音量 音效
	float getWavVolume( void );

	/** @brief 播放声音
	 *  @param[in] 声音ID
	 */
	bool playWav( JCAudioInterface* p_pAudio,const std::string& p_sUrl );
	void delWav(JCAudioInterface* p_pAudio);

	/** @brief 添加资源
	 *  @return
	*/
	bool AddWaveInfo( const std::string& p_sUrl,unsigned char* p_pBuffer,int p_nSize,const char* p_sFilePath,void* p_pExternalMark,bool p_bIsOgg );
	bool AddWaveInfo( const std::string& p_sUrl,buffer& p_pBuffer,int p_nSize,void* p_pExternalMark,bool p_bIsOgg );

	JCWaveInfo* FindWaveInfo( const std::string& p_sUrl );

	void update();


public:

	JCMp3Interface*					m_pMp3Player;

	JCAudioWavPlayer*				m_pWavPlayer;				//播放wav

protected:

	//声音的全局参数设定
	bool							m_bMuteWav;					//音效 是否静音

	float							m_nVolumeWav;				//音效的 音量

	bool							m_bMuteMp3;					//MP3是否静音

	float							m_nVolumeMp3;				//MP3的音量

protected:

	static	JCAudioManager*			m_sAudioManager;			//静态的this指针

};

}

#endif //__JCAudioManager_H__

//-----------------------------END FILE--------------------------------
