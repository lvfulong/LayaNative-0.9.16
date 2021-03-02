/**
@file			JCGraphicsWordInfo.cpp
@brief			
@author			James
@version		1.0
@date			2016_8_2
*/

#include "JCGraphicsWordInfo.h"
#include "../Html5Render/JCHtml5Context.h"
#include <util/JCCommonMethod.h>
#include <cmath>
#include <algorithm> 
#include "shapedText/JCShapeUtils.h"
#include "shapedText/JCTextShaper.h"
//------------------------------------------------------------------------------
namespace laya
{
    JCGraphicsWordInfo::JCGraphicsWordInfo(JCFontManager* pFontManager, JCTextManager* pTextManager, char* sText, const char* sFont, int nColor, Direction nDirection)
    {
        m_nDirection = nDirection;
        m_pFont = NULL;
        m_nColor = 0;
        setTextInfo(pFontManager, pTextManager, sText, sFont, nColor);
    }
    JCGraphicsWordInfo::~JCGraphicsWordInfo()
    {
        m_pFont = NULL;
        m_shapedGlyphSequence.clear();
    }
    void JCGraphicsWordInfo::setTextInfo(JCFontManager* pFontManager, JCTextManager* pTextManager, const char* sText, const char* sFont, int nColor)
    {
        JCFontInfo* pFont = pFontManager->getFontInfoFromText(sFont);
        setTextInfo( pFontManager,pTextManager,sText,pFont,nColor );
    }
    void JCGraphicsWordInfo::setTextInfo(JCFontManager* pFontManager, JCTextManager* pTextManager, const char* sText, JCFontInfo* pFont, int nColor)
    {
        m_nColor = nColor;
        m_pFont = pFont;
        m_sText = sText;
        m_bDirty = true;
    }
    void JCGraphicsWordInfo::update(JCHtml5Context* pRenderContext, JCShapedTextCache* pShapedTextCache, JCTextManager* pTextManager, uint32_t nScale)
    {
        m_shapedGlyphSequence.clear();

        int nLen = m_sText.length();
        if (nLen < 1)
            return;
       
        const JCShapedGlyphSequence& cachedSequence = pShapedTextCache->getShapedTextSequence(m_sText, m_pFont, m_nDirection);
        m_shapedGlyphSequence.m_vShapedGlyphEntrys.reserve(cachedSequence.m_vShapedGlyphEntrys.size());
        for (int i = 0, n = cachedSequence.m_vShapedGlyphEntrys.size(); i < n; i++)
        {
            const JCShapedGlyphEntry& entry = cachedSequence.m_vShapedGlyphEntrys[i];
            if (entry.bIsVisible)
            {
                std::shared_ptr<JCWordRes> pWordRes = pTextManager->getWordRes(entry.glyphIndex, m_pFont, m_nColor, nScale, entry.pFTFace);
                if (pWordRes->enable() == ENALBE_TYPE_FAIL)
                {
                    continue;
                }
                m_shapedGlyphSequence.m_nTextWidth += entry.xAdvance;
                m_shapedGlyphSequence.m_vShapedGlyphEntrys.emplace_back(entry);
                m_shapedGlyphSequence.m_vShapedGlyphEntrys.back().pWordRes = std::static_pointer_cast<void>(pWordRes);
            }
            else
            {
                m_shapedGlyphSequence.m_nTextWidth += entry.xAdvance;
                m_shapedGlyphSequence.m_vShapedGlyphEntrys.emplace_back(entry);
                m_shapedGlyphSequence.m_vShapedGlyphEntrys.back().pWordRes.reset();
            }
        }
        m_shapedGlyphSequence.m_nTextDirection = cachedSequence.m_nTextDirection;
    }
    void JCGraphicsWordInfo::modifyTextInfo(JCFontManager* pFontManager, JCTextManager* pTextManager, int nColor)
    {
        m_shapedGlyphSequence.clear();
        setTextInfo( pFontManager,pTextManager,m_sText.c_str(),m_pFont,nColor );
    }
    void JCGraphicsWordInfo::modifyTextInfo(JCFontManager* pFontManager, JCTextManager* pTextManager, const char* sText)
    {
        m_shapedGlyphSequence.clear();
        setTextInfo(pFontManager, pTextManager, sText, m_pFont, m_nColor);
    }
    JCShapedGlyphSequence& JCGraphicsWordInfo::getWordRes(JCShapedTextCache* pShapedTextCache, JCHtml5Context* pRenderContext, JCFontManager* pFontManager, JCTextManager* pTextManager)
    { 
        float sx = pRenderContext->m_pContextData->m_pCurrentContextData->kMatrix.getScaleX();
        float sy = pRenderContext->m_pContextData->m_pCurrentContextData->kMatrix.getScaleY();
        uint32_t currentScale = ceilf(std::max(1.0f, std::max(sx, sy)));
        bool scaleChanged = (m_nLastScale != currentScale);
        m_nLastScale = currentScale;
        if (m_bDirty || scaleChanged)
        {
            update(pRenderContext, pShapedTextCache, pTextManager, currentScale);
            m_bDirty = false;
        }
        return m_shapedGlyphSequence;
    }
}
//------------------------------------------------------------------------------


//-----------------------------END FILE--------------------------------

