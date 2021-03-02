/**
@file			JCTextManager.h
@brief			
@author			James
@version		1.0
@date			2016_6_2
*/

#ifndef __JCTextManager_H__
#define __JCTextManager_H__

#include "../DisplayRes/JCWordRes.h"
#include <unordered_map>
#include <string>  
#include <iostream>  
#include <functional>
#include "../../util/JCLRUCache.h"
#include "../../util/JCCommonMethod.h"
namespace laya
{
    /** 
     * @brief 
    */
    class JCAtlasManager;
    class JCFreeTypeFontRender;




    struct WordResKey
    {
        int m_nFontId;
        int m_nColor;
        uint32_t m_nGlyphIndex;
        uint32_t m_nScale;
        FT_Face m_pFTFace;
        WordResKey(int nFontId, int nColor, uint32_t nGlyphIndex, uint32_t nScale, FT_Face pFTFace)
            : m_nFontId(nFontId), m_nColor(nColor), m_nGlyphIndex(nGlyphIndex), m_nScale(nScale), m_pFTFace(pFTFace)
        {}
        bool operator==(const WordResKey& ins) const
        {
            return  m_nFontId == ins.m_nFontId &&
                m_nColor == ins.m_nColor &&
                m_nGlyphIndex == ins.m_nGlyphIndex &&
                m_nScale == ins.m_nScale &&
                m_pFTFace == ins.m_pFTFace;
        }
    };
    inline bool operator< (const WordResKey& left, const WordResKey& right)
    {
        if (left.m_nFontId != right.m_nFontId)
        {
            return left.m_nFontId < right.m_nFontId;
        }

        if (left.m_nColor != right.m_nColor)
        {
            return left.m_nColor < right.m_nColor;
        }

        if (left.m_nGlyphIndex != right.m_nGlyphIndex)
        {
            return left.m_nGlyphIndex < right.m_nGlyphIndex;
        }

        if (left.m_nScale != right.m_nScale)
        {
            return left.m_nScale < right.m_nScale;
        }

        if (left.m_pFTFace != right.m_pFTFace)
        {
            return left.m_pFTFace < right.m_pFTFace;
        }

        return false;
    }
    struct WordResKey_Hash
    {
        size_t operator()(const WordResKey& ins) const
        {
            size_t seed = 0;
            hash_combine(seed, std::hash<int>{}(ins.m_nFontId));
            hash_combine(seed, std::hash<int>{}(ins.m_nColor));
            hash_combine(seed, std::hash<uint32_t>{}(ins.m_nGlyphIndex));
			hash_combine(seed, std::hash<uint32_t>{}(ins.m_nScale));
            hash_combine(seed, std::hash<FT_Face>{}(ins.m_pFTFace));
            return seed;
        }
    };

    struct WordResKey_Cmp
    {
        bool operator()(const WordResKey &left, const WordResKey &right) const
        {
            if (left.m_nFontId != right.m_nFontId)
            {
                return left.m_nFontId < right.m_nFontId;
            }

            if (left.m_nColor != right.m_nColor)
            {
                return left.m_nColor < right.m_nColor;
            }

            if (left.m_nGlyphIndex != right.m_nGlyphIndex)
            {
                return left.m_nGlyphIndex < right.m_nGlyphIndex;
            }

            if (left.m_nScale != right.m_nScale)
            {
                return left.m_nScale < right.m_nScale;
            }

            return false;
        }
    };

    class JCTextManager
    {
    public:

        /** @brief构造函数
        */
        JCTextManager(JCAtlasManager* pAtlasManager);

        /** @brief析构函数
        */
        ~JCTextManager();

        /** @brief 获得wodRes，先去map中查找，如果有直接返回，如果没有再创建新的
         *  @param[in] FonfInfo
         *  @param[in] 文字的颜色
         *  @param[in] 单个文字
         *  @return wordRes指针
        */
        std::shared_ptr<JCWordRes> getWordRes(uint32_t nGlyphIndex, JCFontInfo* pFontInfo, int nColor, uint32_t nScale, FT_Face pFTFace);

        void setFreeTypeRender(JCFreeTypeFontRender* pFreeTypeRender);

        void clearAll();

    public:
//#ifndef ANDROID
        LRUCache<WordResKey, std::shared_ptr<JCWordRes>, WordResKey_Hash> m_LRUCache;
//#else
//        LRUCache<WordResKey, std::shared_ptr<JCWordRes>, WordResKey_Cmp> m_LRUCache;
//#endif
        JCAtlasManager*         m_pAtlasManager;
        JCFreeTypeFontRender*   m_pFreeTypeRender;
    };
}

#endif //__JCTextManager_H__

//-----------------------------END FILE--------------------------------
