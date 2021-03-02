#pragma once

#include <hb.h>
#include <hb-ft.h>
#include <unicode/unistr.h>
#include "JCShapeUtils.h"
#include <ft2build.h>
#include FT_FREETYPE_H

namespace laya
{
    class JCFontInfo;
    class JCFreeTypeFontRender;


    struct JCShapedGlyphEntry
    {
        uint32_t glyphIndex;
        int16_t xAdvance;
        int16_t yAdvance;
        int16_t xOffset;
        int16_t yOffset;
        int16_t height;
        TextDirection textDirection;
        bool bIsVisible; 
        std::shared_ptr<void>   pWordRes;
        FT_Face pFTFace;
        JCShapedGlyphEntry();
    };

    class JCTextShaper
    {
    public:
        JCTextShaper(JCFreeTypeFontRender* pFreeTypeFontRender);
        void shapeUnidirectionalText(const icu::UnicodeString& icuUTF16Text, JCFontInfo* fontInfo, std::vector<JCShapedGlyphEntry>& shapedGlyphEntrys) const;
        void shapeBidirectionalText(const icu::UnicodeString& icuUTF16Text, JCFontInfo* fontInfo, const TextDirection baseTextDirection, std::vector<JCShapedGlyphEntry>& shapedGlyphEntrys) const;
    private:
        void doHarfBuzzTextShaping(const icu::UnicodeString& icuUTF16Text, const int32_t textStart, const int32_t textLength, JCFontInfo* fontInfo, const TextDirection textDirection, std::vector<JCShapedGlyphEntry>& shapedGlyphEntrys) const;
        hb_font_t* createFont(const FT_Face& ftFace, const int32_t glyphFlags, const int32_t fontSize) const;
    private:
        JCFreeTypeFontRender* m_pFreeTypeFontRender;
    };
}