/**
@file			JCAudioManager.cpp
@brief
@author			dt
@version		1.0
@date			2014_11_26
*/

//包含头文件
#include "JCAudioManager.h"
#include "resource/Audio/JCWaveParser.h"
#include "util/Log.h"

namespace laya
{
//------------------------------------------------------------------------------
JCAudioManager* JCAudioManager::m_sAudioManager = NULL;
//------------------------------------------------------------------------------
JCAudioManager::JCAudioManager( void )
{
	m_bMuteMp3 = false;
	m_bMuteWav = false;
    //音量是从 0 到 -10000
	m_nVolumeMp3 = 1.0f;
	m_nVolumeWav = 1.0f;
	createMp3player();
	m_pWavPlayer = new JCAudioWavPlayer();
}
//------------------------------------------------------------------------------
JCAudioManager::~JCAudioManager( void )
{
	ClearAllAudioBufferPlay();
	if( m_pMp3Player != NULL )
	{
		delete m_pMp3Player;
		m_pMp3Player = NULL;
	}
	if( m_pWavPlayer != NULL )
	{
		m_pWavPlayer->ClearAllWaveInfo();
		delete m_pWavPlayer;
		m_pWavPlayer = NULL;
	}
}
//------------------------------------------------------------------------------
void JCAudioManager::update()
{
	m_pWavPlayer->checkWavePlayEnd();
}
//------------------------------------------------------------------------------
JCAudioManager* JCAudioManager::GetInstance( void )
{
	if( m_sAudioManager == NULL )
	{
		m_sAudioManager = new JCAudioManager();
	}
	return m_sAudioManager;
}
//------------------------------------------------------------------------------
void JCAudioManager::ClearAllWork(){
	if( m_sAudioManager != NULL )
	{
		m_sAudioManager->m_bMuteMp3 = false;
		m_sAudioManager->m_bMuteWav = false;
		JCAudioWavPlayer* pWavPlayer = m_sAudioManager->m_pWavPlayer;
		if( pWavPlayer != NULL )
		{
			for (int i = 0; i < MAX_OPENAL_SOURCE_NUM; i++)
			{
				if( pWavPlayer->m_vOpenALSource[i].m_bPlaying == true )
				{
					alSourceStop( pWavPlayer->m_vOpenALSource[i].m_nOpenALSouceID );
					pWavPlayer->m_vOpenALSource[i].m_pAudio = NULL;
					pWavPlayer->m_vOpenALSource[i].m_bPlaying = false;
				}
			}
			pWavPlayer->ClearAllWaveInfo();
		}
		m_sAudioManager->ClearAllAudioBufferPlay();
	}
}

void JCAudioManager::DelInstance( void )
{
	if( m_sAudioManager != NULL )
	{
		m_sAudioManager->m_pWavPlayer->ClearAllWaveInfo();
		m_sAudioManager->ClearAllAudioBufferPlay();

		delete m_sAudioManager;
		m_sAudioManager = NULL;
	}
}
//------------------------------------------------------------------------------
void JCAudioManager::createMp3player()
{
#ifdef WIN32
	m_pMp3Player = new JCAudioMp3Player();
#elif ANDROID
	m_pMp3Player = new JCAudioMp3Media();
#elif __APPLE__
    m_pMp3Player = new JCAudioMp3Player();
#endif
}
//------------------------------------------------------------------------------
void JCAudioManager::setMp3Mute( bool p_bMute )
{
	m_bMuteMp3 = p_bMute;
	if( m_pMp3Player != NULL )
	{
		m_pMp3Player->setMute( p_bMute );
	}
}
//------------------------------------------------------------------------------
bool JCAudioManager::getMp3Mute( void )
{
	return m_bMuteMp3;
}
//------------------------------------------------------------------------------
void JCAudioManager::setMp3Volume( float p_nVolume )
{
	//LOGE("setMp3Volume=========%f",p_nVolume);
	m_nVolumeMp3 = p_nVolume;
	if( m_pMp3Player != NULL )
	{
		m_pMp3Player->setVolume( p_nVolume );
	}
}
//------------------------------------------------------------------------------
float JCAudioManager::getMp3Volume( void )
{
	return m_nVolumeMp3;
}
//------------------------------------------------------------------------------
void JCAudioManager::playMp3( const char* p_sUrl,int p_nTimes,JCAudioInterface* p_pJSAudio )
{
	//每次播放mp3都是重新new
	if( m_pMp3Player == NULL )
	{
		createMp3player();
	}
	else
	{
		delete m_pMp3Player;
		createMp3player();
	}
    if( m_pMp3Player == NULL )return;
	m_pMp3Player->play( p_sUrl,p_nTimes,p_pJSAudio );
	if( m_bMuteMp3 )
	{
		m_pMp3Player->setMute(true);
	}
	else
	{
		m_pMp3Player->setVolume( (float)m_nVolumeMp3 );
	}
}

void JCAudioManager::delMp3Obj(JCAudioInterface* p_pJSAudio){
	if(m_pMp3Player)
		m_pMp3Player->delAudio(p_pJSAudio);
}

//------------------------------------------------------------------------------
void JCAudioManager::pauseMp3()
{
	if( m_pMp3Player != NULL )
	{
		m_pMp3Player->pause();
	}
}
//------------------------------------------------------------------------------
void JCAudioManager::stopMp3()
{
	if( m_pMp3Player != NULL )
	{
		m_pMp3Player->stop();
	}
}
//------------------------------------------------------------------------------
void JCAudioManager::resumeMp3()
{
	if( m_pMp3Player != NULL )
	{
		m_pMp3Player->resume();
	}
}
//------------------------------------------------------------------------------
void JCAudioManager::setWavMute( bool p_bMute )
{
	m_bMuteWav = p_bMute;	
	if( m_bMuteWav )
	{
		m_pWavPlayer->setVolume(0.0f);
	}
	else
	{
		m_pWavPlayer->setVolume( (float)m_nVolumeWav );
	}	
}
//------------------------------------------------------------------------------
bool JCAudioManager::getWavMute( void )
{
	return m_bMuteWav;
}
//------------------------------------------------------------------------------
void JCAudioManager::setWavVolume( float p_nVolume )
{
	m_nVolumeWav = p_nVolume;
	m_pWavPlayer->setVolume( p_nVolume );	
}
//------------------------------------------------------------------------------
float JCAudioManager::getWavVolume( void )
{
	return m_nVolumeWav;
}
//------------------------------------------------------------------------------
bool JCAudioManager::ClearAllAudioBufferPlay( void )
{
	return true;
}
//------------------------------------------------------------------------------
bool JCAudioManager::playWav( JCAudioInterface* p_pAudio,const std::string& p_sUrl  )
{
	if( m_bMuteWav == true ) return false;
	m_pWavPlayer->playAudio( p_pAudio,p_sUrl );
	if( m_bMuteWav )
	{
		m_pWavPlayer->setVolume( 0.0f );
	}
	else
	{
		m_pWavPlayer->setVolume( (float)m_nVolumeWav );
	}
	return true;
}
//------------------------------------------------------------------------------
void JCAudioManager::stopWav( JCAudioInterface* p_pAudio )
{
    if( p_pAudio )
    {
        m_pWavPlayer->stop( p_pAudio );
    }
}
//------------------------------------------------------------------------------
void JCAudioManager::delWav(JCAudioInterface* p_pAudio)
{
	m_pWavPlayer->delAudio(p_pAudio);
}
//------------------------------------------------------------------------------
bool JCAudioManager::AddWaveInfo( const std::string& p_sUrl,unsigned char* p_pBuffer,int p_nSize,const char* p_sFilePath,void* p_pExternalMark,bool p_bIsOgg )
{
	return m_pWavPlayer->AddWaveInfo( p_sUrl,p_pBuffer,p_nSize,p_sFilePath,p_pExternalMark,p_bIsOgg );
}
//------------------------------------------------------------------------------
bool JCAudioManager::AddWaveInfo( const std::string& p_sUrl,buffer& p_pBuffer,int p_nSize,void* p_pExternalMark,bool p_bIsOgg )
{
	return m_pWavPlayer->AddWaveInfo( p_sUrl,(unsigned char*)p_pBuffer.m_pPtr,p_nSize,NULL,p_pExternalMark,p_bIsOgg );
}
//------------------------------------------------------------------------------
JCWaveInfo* JCAudioManager::FindWaveInfo( const std::string& p_sUrl )
{
	return m_pWavPlayer->FindWaveInfo( p_sUrl );
}
//------------------------------------------------------------------------------

}

//-----------------------------END FILE--------------------------------
