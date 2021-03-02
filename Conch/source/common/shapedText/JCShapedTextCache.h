#pragma once

#include <vector>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "../shapedText/JCTextShaper.h"
#include "../resource/text/JCFontInfo.h"
#include <unordered_map>  
#include <functional>
#include <unicode/unistr.h>
#include "../util/JCCommonMethod.h"
#include <functional>
#include <mutex>
namespace laya
{
    class JCTextShaper;
    class JCFreeTypeFontRender;

    class JCShapedGlyphSequence
    {
    public:
        TextDirection m_nTextDirection = TextDirection::LeftToRight;
        mutable int32_t m_nTextWidth = -1;
        mutable int32_t m_nTextHeight = -1;
        void clear() { m_nTextDirection = TextDirection::LeftToRight; m_vShapedGlyphEntrys.clear(); m_nTextWidth = -1; m_nTextHeight = -1; }
        void getMeasuredSize(int32_t& nWidth, int32_t& nHeight) const;
        std::vector<JCShapedGlyphEntry> m_vShapedGlyphEntrys;
    };


    struct ShapedGlyphSequenceKey
    {
        icu::UnicodeString m_utf16Text;
        JCFontInfo* m_pFontInfo;
        Direction m_nDirection;
        ShapedGlyphSequenceKey(const icu::UnicodeString& utf16Text, Direction nDirection, JCFontInfo* pFontInfo)
            : m_utf16Text(utf16Text), m_nDirection(nDirection), m_pFontInfo(pFontInfo)
        {}
        bool operator==(const ShapedGlyphSequenceKey& ins) const
        {
            return (m_pFontInfo->m_nFontId == ins.m_pFontInfo->m_nFontId) && (m_nDirection == ins.m_nDirection) && (m_utf16Text == ins.m_utf16Text);
        }
    };

    struct ShapedGlyphSequenceKey_Hash
    {
        size_t operator()(const ShapedGlyphSequenceKey& ins) const
        {
            size_t seed = 0;   
            hash_combine(seed, ins.m_utf16Text.hashCode());
            hash_combine(seed, std::hash<int>{}(ins.m_pFontInfo->m_nFontId));
            hash_combine(seed, std::hash<int>{}(ins.m_nDirection));
            return seed;
        }
    };


    class JCShapedTextCache
    {
    public:

        JCShapedTextCache(JCFreeTypeFontRender* pFreeTypeFontRender);

        ~JCShapedTextCache();

        const JCShapedGlyphSequence& getShapedTextSequence(const std::string& utf8Text, JCFontInfo* pFontInfo, Direction nDirection);

        void clear();

        void garbageCollection();

    private:

        typedef std::unordered_map<ShapedGlyphSequenceKey, JCShapedGlyphSequence*, ShapedGlyphSequenceKey_Hash> MapShapedGlyphSequence;

        JCTextShaper* m_pTextShaper;

        MapShapedGlyphSequence m_mapShapedGlyphSequence;

        std::mutex m_Mutex;

        JCFreeTypeFontRender* m_pFreeTypeFontRender;
    };
}