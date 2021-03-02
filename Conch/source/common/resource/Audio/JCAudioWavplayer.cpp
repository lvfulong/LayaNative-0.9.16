/**
@file			JCAudioWavPlayer.cpp
@brief
@author			James
@version		1.0
@date			2014_4_22
*/

#include "JCAudioWavPlayer.h"
#include "../../util/JCCommonMethod.h"
#include "../../util/Log.h"
#include "JCWaveParser.h"
#include "JCOggParser.h"

//------------------------------------------------------------------------------
namespace laya
{
//------------------------------------------------------------------------------
JCAudioWavPlayer::JCAudioWavPlayer()
{
    m_nCurrentIndex = 0;
    m_pOpenALSource.reserve(128);
	m_bStop=false;
	m_pDevice = alcOpenDevice(NULL);
	m_pContext = alcCreateContext( m_pDevice, NULL );
	alcMakeContextCurrent( m_pContext );
    createOpenALSource();
}
void JCAudioWavPlayer::createOpenALSource()
{
    int m_nALCount = m_pOpenALSource.size();
    m_pOpenALSource.resize(m_nALCount + OPENAL_SOURCE_NUM);
    ALuint pOpenALBuffer[OPENAL_SOURCE_NUM];
    alGenBuffers(OPENAL_SOURCE_NUM, pOpenALBuffer);
    ALuint pOpenALSouceID[OPENAL_SOURCE_NUM];
    alGenSources(OPENAL_SOURCE_NUM, pOpenALSouceID);
    for (int i = 0; i < OPENAL_SOURCE_NUM; i++)
    {
        m_pOpenALSource[m_nALCount + i] = new OpenALSourceInfo;
        m_pOpenALSource[m_nALCount+i]->m_nOpenALSouceID = pOpenALSouceID[i];
        m_pOpenALSource[m_nALCount + i]->m_nBufferID = 0;
        for (int j = 0; j < 3; j++)
        {
            m_pOpenALSource[m_nALCount + i]->m_vSourcePos[j] = 0;
            m_pOpenALSource[m_nALCount + i]->m_vSourceVel[j] = 0;
        }
        m_pOpenALSource[m_nALCount + i]->m_bPlaying = false;
        m_pOpenALSource[m_nALCount + i]->m_pAudio = NULL;
    }
    LOGI("createOpenALSource current num=%d", m_pOpenALSource.size());
}
//------------------------------------------------------------------------------
JCAudioWavPlayer::~JCAudioWavPlayer()
{
	Release();
}
//------------------------------------------------------------------------------
OpenALSourceInfo* JCAudioWavPlayer::getOpenALSource()
{
    int m_nALCount = m_pOpenALSource.size();
    bool bFind = false;
    OpenALSourceInfo* pInfo = NULL;
	for( int i = 0; i < m_nALCount; i++ )
	{
        int n = (m_nCurrentIndex + i) % m_nALCount;
		pInfo= m_pOpenALSource[n];
		if( pInfo->m_bPlaying == false )
		{
            m_nCurrentIndex = n + 1;
            bFind = true;
            break;
		}
	}
    if (bFind==false)
    {
        int nLastCount = m_nALCount;
        createOpenALSource();
        pInfo = m_pOpenALSource[nLastCount];
        m_nCurrentIndex = nLastCount;
    }
	return pInfo;
}
//------------------------------------------------------------------------------
void JCAudioWavPlayer::checkWavePlayEnd()
{
    int m_nALCount = m_pOpenALSource.size();
	ALint nState = 0;
	for ( int i = 0; i < m_nALCount; i++)
	{
		if( m_pOpenALSource[i]->m_bPlaying == true )
		{
			alGetSourcei(m_pOpenALSource[i]->m_nOpenALSouceID, AL_SOURCE_STATE, &nState );
			if ( nState == AL_STOPPED )
			{
                releaseOpenAL(m_pOpenALSource[i]);
                m_pOpenALSource[i]->m_bPlaying = false;
				if(m_pOpenALSource[i]->m_pAudio != NULL )
				{
                    m_pOpenALSource[i]->m_pAudio->onPlayEnd();
                    m_pOpenALSource[i]->m_pAudio = NULL;
				}
			}
		}
	}
}
//------------------------------------------------------------------------------
OpenALSourceInfo* JCAudioWavPlayer::playAudio( JCAudioInterface* p_pAudio,const std::string& p_sSrc )
{
	MapWaveInfoIter iter = m_vWaveInfos.find( p_sSrc );
	if( iter != m_vWaveInfos.end() )
	{
		JCWaveInfo* pInfo = iter->second;
		if( pInfo != NULL )
		{
            WAVE_FORMAT* pFormat= &(pInfo->m_kFmtBlock.wavFormat);
			return playAudioFromBuffer( p_pAudio,(char*)(pInfo->m_pData),pInfo->m_nRealDataSize, pFormat->dwSamplesPerSec, pFormat->wBitsPerSample, pFormat->wChannels );
		}
	}
    return NULL;
}
void JCAudioWavPlayer::delAudio(JCAudioInterface* p_pAudio)
{
    int m_nALCount = m_pOpenALSource.size();
	for ( int i = 0; i < m_nALCount; i++){
		if( m_pOpenALSource[i]->m_pAudio==p_pAudio){
			m_pOpenALSource[i]->m_pAudio = NULL;
		}
	}
}
//------------------------------------------------------------------------------
void JCAudioWavPlayer::releaseOpenAL( OpenALSourceInfo* pOpenALInfo )
{
	alSourceUnqueueBuffers( pOpenALInfo->m_nOpenALSouceID, 1, &(pOpenALInfo->m_nBufferID) );
	alDeleteBuffers( 1, &pOpenALInfo->m_nBufferID );
}
//------------------------------------------------------------------------------
OpenALSourceInfo* JCAudioWavPlayer::playAudioFromBuffer( JCAudioInterface* p_pAudio,const char* p_pBuffer,unsigned int p_nBufferSize,
                    int p_nRate,int nBitsPerSample, int nChannels)
{
	OpenALSourceInfo* pOpenALInfo = getOpenALSource();
	alSourceStop( pOpenALInfo->m_nOpenALSouceID );
	releaseOpenAL( pOpenALInfo );
	alSourcef ( pOpenALInfo->m_nOpenALSouceID, AL_PITCH, 1.0 );
	alSourcef ( pOpenALInfo->m_nOpenALSouceID, AL_GAIN, 1.0 );
	alSourcei ( pOpenALInfo->m_nOpenALSouceID, AL_LOOPING, 0 );
	//������
	//alSourcefv( pOpenALInfo->m_nOpenALSouceID, AL_VELOCITY, pOpenALInfo->m_vSourceVel );
	//alSourcefv( pOpenALInfo->m_nOpenALSouceID, AL_POSITION, pOpenALInfo->m_vSourcePos );

	//bind buffer
	ALuint nBufferID = 0;
	alGenBuffers( 1,&nBufferID );
    int nFormat = AL_FORMAT_MONO16;
    switch (nChannels)
    {
    case 1:
        switch (nBitsPerSample)
        {
        case 8:
            nFormat = AL_FORMAT_MONO8;
            break;
        case 16:
            nFormat = AL_FORMAT_MONO16;
            break;
        default:
            LOGE("JCAudioWavPlayer::playAudioFromBuffer sound bitsPerSample error,only support 8 or 16");
            nFormat = AL_FORMAT_MONO16;
            break;
        }
        break;
    case 2:
        switch (nBitsPerSample)
        {
        case 8:
            nFormat = AL_FORMAT_STEREO8;
            break;
        case 16:
            nFormat = AL_FORMAT_STEREO16;
            break;
        default:
            LOGE("JCAudioWavPlayer::playAudioFromBuffer sound bitsPerSample error,only support 8 or 16");
            nFormat = AL_FORMAT_STEREO16;
            break;
        }
        break;
    default:
        LOGE("JCAudioWavPlayer::playAudioFromBuffer sound channel error");
        switch (nBitsPerSample)
        {
        case 8:
            nFormat = AL_FORMAT_MONO8;
            break;
        case 16:
            nFormat = AL_FORMAT_MONO16;
            break;
        default:
            LOGE("JCAudioWavPlayer::playAudioFromBuffer sound bitsPerSample error,only support 8 or 16");
            nFormat = AL_FORMAT_MONO16;
            break;
        }
        break;
    }
    alBufferData(nBufferID, nFormat, p_pBuffer, p_nBufferSize, p_nRate);
	alSourceQueueBuffers( pOpenALInfo->m_nOpenALSouceID, 1, &nBufferID );
	pOpenALInfo->m_nBufferID = nBufferID;
	//play
	alSourcePlay( pOpenALInfo->m_nOpenALSouceID );
	//������Ϣ
    pOpenALInfo->m_pAudio = p_pAudio;
	pOpenALInfo->m_bPlaying = true;
    return pOpenALInfo;
}
//------------------------------------------------------------------------------
void JCAudioWavPlayer::stopAll()
{
    int m_nALCount = m_pOpenALSource.size();
	for (int i = 0; i < m_nALCount; i++)
	{
		if( m_pOpenALSource[i]->m_bPlaying == true )
		{
			alSourceStop(m_pOpenALSource[i]->m_nOpenALSouceID );
			m_pOpenALSource[i]->m_bPlaying = false;
		}
	}
}
//------------------------------------------------------------------------------
void JCAudioWavPlayer::setAllVolume( float p_nVolume )
{
    int m_nALCount = m_pOpenALSource.size();
	for ( int i = 0; i < m_nALCount; i++ )
	{
		alSourcef ( m_pOpenALSource[i]->m_nOpenALSouceID, AL_GAIN, p_nVolume);
	}
}
//------------------------------------------------------------------------------
void JCAudioWavPlayer::stop(OpenALSourceInfo* pOpenALInfo)
{
    if (pOpenALInfo->m_bPlaying == true)
    {
        alSourceStop(pOpenALInfo->m_nOpenALSouceID);
        pOpenALInfo->m_bPlaying = false;
        pOpenALInfo->m_pAudio = NULL;
    }
}
//------------------------------------------------------------------------------
void JCAudioWavPlayer::setVolume(OpenALSourceInfo* pOpenALInfo,float p_nVolume)
{
    alSourcef(pOpenALInfo->m_nOpenALSouceID, AL_GAIN, p_nVolume);
}
//------------------------------------------------------------------------------
void JCAudioWavPlayer::Release()
{
    int m_nALCount = m_pOpenALSource.size();
	for (int i = 0; i < m_nALCount; i++)
	{
		alDeleteBuffers( 1, &m_pOpenALSource[i]->m_nBufferID );
		alDeleteSources( 1, &m_pOpenALSource[i]->m_nOpenALSouceID );
        delete m_pOpenALSource[i];
	}
    if (m_pContext != NULL) 
    { 
        alcDestroyContext( m_pContext );
        m_pContext=NULL; 
    } 
    if ( m_pDevice !=NULL ) 
    { 
        alcCloseDevice(m_pDevice); 
        m_pDevice=NULL; 
    } 
	m_bStop=true;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool JCAudioWavPlayer::AddWaveInfo( const std::string& p_sUrl,unsigned char* p_pBuffer,int p_nSize,const char* p_sFilePath,void* p_pExternalMark,bool p_bIsOgg )
{
	JCWaveInfo* pInfo = FindWaveInfo( p_sUrl );
	if( pInfo == NULL )
	{
		if( p_bIsOgg == false )
		{
			pInfo = JCWaveParser::GetInstance()->GetWaveInfoFromBuffer( p_pBuffer,p_nSize );
		}
		else
		{
            pInfo = JCOggParser::GetInstance()->GetWaveInfo( p_sFilePath,p_pBuffer,p_nSize );
		}
		if( pInfo != NULL )
		{
			pInfo->m_pExternalMark = p_pExternalMark;
			m_vWaveInfos[ p_sUrl ] = pInfo;
		}
		else
		{
			LOGE( "JCAudioWavPlayer::AddWaveInfo wave paser err" );
		}
	}
	return true;
}
//------------------------------------------------------------------------------
JCWaveInfo* JCAudioWavPlayer::FindWaveInfo( const std::string& p_sUrl )
{
	MapWaveInfoIter iter = m_vWaveInfos.find( p_sUrl );
	if( iter != m_vWaveInfos.end() )
	{
		return iter->second;
	}
	return NULL;
}
//------------------------------------------------------------------------------
bool JCAudioWavPlayer::ClearAllWaveInfo( void )
{
	for( MapWaveInfoIter iter = m_vWaveInfos.begin(); iter != m_vWaveInfos.end(); iter++ )
	{
		JCWaveInfo* pInfo = NULL;
		pInfo = iter->second;
		if( pInfo != NULL )
		{
			delete pInfo;
			pInfo = NULL;
		}
	}
	m_vWaveInfos.clear();
	return true;
}
//------------------------------------------------------------------------------
}

//-----------------------------END FILE--------------------------------
