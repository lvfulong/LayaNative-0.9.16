/**
@file			JCGraphicsWordInfo.h
@brief			
@author			James
@version		1.0
@date			2016_8_2
*/

#ifndef __JCGraphicsWordInfo_H__
#define __JCGraphicsWordInfo_H__

#include <string>
#include <resource/text/JCTextManager.h>
#include <resource/text/JCFontManager.h>
#include <vector>
#include "shapedText/JCShapedTextCache.h"

namespace laya
{
    class JCHtml5Context;
    /** 
     * @brief 
    */
    class JCGraphicsWordInfo
    {
    public:

        /** @brief构造函数
        */
        JCGraphicsWordInfo(JCFontManager* pFontManager, JCTextManager* pTextManager, char* sText, const char* sFont, int nColor, Direction nDirection);

        /** @brief析构函数
        */
        ~JCGraphicsWordInfo();

        void setTextInfo( JCFontManager* pFontManager,JCTextManager* pTextManager, const char* sText, const char* sFont,int nColor );

        void modifyTextInfo(JCFontManager* pFontManager, JCTextManager* pTextManager, int nColor );

        void modifyTextInfo(JCFontManager* pFontManager, JCTextManager* pTextManager, const char* sText);

        JCShapedGlyphSequence& getWordRes(JCShapedTextCache* pShapedTextCache, JCHtml5Context* pRenderContext, JCFontManager* pFontManager, JCTextManager* pTextManager);

        void update(JCHtml5Context* pRenderContext, JCShapedTextCache* pShapedTextCache, JCTextManager* pTextManager, uint32_t nScale);

    private:

        void setTextInfo(JCFontManager* pFontManager, JCTextManager* pTextManager, const char* sText, JCFontInfo* pFont, int nColor);

    public:

        std::string                 m_sText;
        JCFontInfo*                 m_pFont;
        int                         m_nColor;

    private:
        JCShapedGlyphSequence       m_shapedGlyphSequence;
        bool                        m_bDirty;
        uint32_t                    m_nLastScale = 0;
        Direction                   m_nDirection;
    };
}

#endif //__JCGraphicsWordInfo_H__

//-----------------------------END FILE--------------------------------