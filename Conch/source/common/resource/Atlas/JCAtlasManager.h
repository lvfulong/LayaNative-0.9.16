/**
@file			JCAtlasManager.h
@brief			
@author			James
@version		1.0
@date			2016_5_26
*/

#ifndef __JCAtlasManager_H__
#define __JCAtlasManager_H__

#include <vector>
#include "JCAtlas.h"
#include "../../imageLib/JCImageRW.h"

namespace laya
{
    class JCAtlasManager
    {
    public:

        JCAtlasManager( int nWidth, int nHeight, float nGridSize,int nMaxTexNum );

	    ~JCAtlasManager();

	    bool setAtlasParam( int nWidth, int nHeight, float nGridSize,int nMaxTexNum );

	    bool Initialize();

	    void setInAltasResValues( JCInAtlasRes* pInAltasRes,int nOffsetX,int nOffsetY,int nAtlasNum, BitmapData* pBitmapData);

	    bool pushData( JCInAtlasRes* pInAltasRes,BitmapData* pBitmapData );

	    bool toGarbageCollection();

	    void freeGLResource();

        void resetAtlas();

    public:

        int                     m_nMaxAtlasNum;
        int                     m_nWidth;
        int                     m_nHeight;
        float                   m_nGridSize;
        int                     m_nGridNumX;
        int                     m_nGridNumY;
        bool                    m_bInit;
        int                     m_nCurAtlasIndex;
	    std::vector<JCAtlas*>   m_vAtlasArray;
	    bool                    m_bNeedUploadToGPU;
	    bool                    m_bSetAtlasParam;//�Ƿ�������޸Ĳ����������ͼ�ϼ��Ŀ��ߡ��������������ֻ���ڿ�ʼ��ʱ�����ʹ�ã���Ϸ���ǲ����Ե�

    };
}
//------------------------------------------------------------------------------


#endif //__JCAtlasManager_H__

//-----------------------------END FILE--------------------------------