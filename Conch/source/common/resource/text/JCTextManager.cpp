/**
@file			JCTextManager.cpp
@brief			
@author			James
@version		1.0
@date			2016_6_2
*/

#include "JCTextManager.h"
#include "../../util/Log.h"

//------------------------------------------------------------------------------
namespace laya
{
    JCTextManager::JCTextManager(JCAtlasManager* pAtlasManager) : m_LRUCache(1024)
    {
        m_pAtlasManager = pAtlasManager;
    }
    JCTextManager::~JCTextManager()
    {
        clearAll();
    }
    void JCTextManager::setFreeTypeRender(JCFreeTypeFontRender* pFreeTypeRender)
    {
        m_pFreeTypeRender = pFreeTypeRender;
    }
    void JCTextManager::clearAll()
    {
        m_LRUCache.clear();
    }
    std::shared_ptr<JCWordRes> JCTextManager::getWordRes(uint32_t nGlyphIndex, JCFontInfo* pFontInfo, int nColor, uint32_t nScale, FT_Face pFTFace)
    {
        if (m_pFreeTypeRender == NULL)
        {
            LOGE("JCTextManager::getWordRes m_pFreeTypeRender == NULL");
            return NULL;
        }
        WordResKey key(pFontInfo->m_nFontId, nColor, nGlyphIndex, nScale, pFTFace);
		
        if (m_LRUCache.isCached(key))
        {
            return m_LRUCache.get(key);
        }
        else
		{
			std::shared_ptr<JCWordRes> value = std::make_shared<JCWordRes>(m_pAtlasManager, m_pFreeTypeRender, pFontInfo, nGlyphIndex, nColor, nScale, pFTFace);
            m_LRUCache.set(key, value);
            return value;
        }
    }   
}
//------------------------------------------------------------------------------


//-----------------------------END FILE--------------------------------
