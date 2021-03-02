/**
@file			JCWordRes.h
@brief			
@author			James
@version		1.0
@date			2016_6_2
*/

#ifndef __JCWordRes_H__
#define __JCWordRes_H__

#include "JCInAtlasRes.h"
#include "../text/JCFontInfo.h"
#include <string.h>
#include "../../shapedText/JCTextShaper.h"
#include "../../FontRender/JCFreeTypeRender.h"
namespace laya
{
    /** 
     * @brief 
    */
    class JCFreeTypeFontRender;
    class JCAtlas;
    class JCAtlasManager;
    class JCWordRes : public JCInAtlasRes
    {
    public:

        /** @brief构造函数
        */
        JCWordRes(JCAtlasManager* pAtlasManager, JCFreeTypeFontRender* pFreeTypeRender, JCFontInfo* pFont, uint32_t nGlyphIndex, int nColor, uint32_t nScale, FT_Face pFTFace);

        /** @brief析构函数
        */
        virtual ~JCWordRes();

        /** @brief restoreRes恢复用的
         *  @return 成功或者失败
        */
        bool gpuRestoreRes(JCDisplayRes* pDisplayRes);

    public:

        JCAtlasManager*         m_pAtlasManager;
        JCFreeTypeFontRender*   m_pFreeTypeRender;
        JCFontInfo*             m_pFont;
        int                     m_nColor;
        std::string             m_sWord;
        Rectangle               m_kRectNoScaled;
        uint32_t                m_nScale;
        uint32_t                m_nGlyphIndex;
        JCGlyphInfo             m_glyphInfo;
        FT_Face                 m_pFTFace;
    };
}

#endif //__JCWordRes_H__

//-----------------------------END FILE--------------------------------
