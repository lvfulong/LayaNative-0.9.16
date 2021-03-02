#include "JCTextShaper.h"
#include <vector>
#include "JCShapeUtils.h"
#include "../resource/text/JCFontInfo.h"
#include "../FontRender/JCFreeTypeRender.h"
#include "../util/Log.h"
#include FT_ADVANCES_H
#include <algorithm>
#include <unicode/schriter.h>

namespace laya
{
    const uint32_t InvalidChar = 0xFFFD;
    JCShapedGlyphEntry::JCShapedGlyphEntry()
        : glyphIndex(0)
        , xAdvance(0)
        , yAdvance(0)
        , xOffset(0)
        , yOffset(0) 
        , height(0)
        , textDirection(TextDirection::LeftToRight)
        , bIsVisible(false)
        , pFTFace(nullptr)
        {
        }

    JCTextShaper::JCTextShaper(JCFreeTypeFontRender* pFreeTypeFontRender): m_pFreeTypeFontRender(pFreeTypeFontRender)
    {

    }
    void JCTextShaper::shapeBidirectionalText(const icu::UnicodeString& icuUTF16Text, JCFontInfo* fontInfo, const TextDirection baseTextDirection, std::vector<JCShapedGlyphEntry>& shapedGlyphEntrys) const
    {
        std::vector<TextDirectionInfo> textDirectionInfos;
        TextDirection textDirection = ShapeUtils::computeTextDirection(icuUTF16Text, baseTextDirection, textDirectionInfos);
        for (const TextDirectionInfo& info : textDirectionInfos)
        {
            doHarfBuzzTextShaping(icuUTF16Text, info.startIndex, info.length, fontInfo, info.textDirection, shapedGlyphEntrys);
        }
    }
    void JCTextShaper::shapeUnidirectionalText(const icu::UnicodeString& icuUTF16Text, JCFontInfo* fontInfo, std::vector<JCShapedGlyphEntry>& shapedGlyphEntrys) const
    {
        //std::lock_guard<std::recursive_mutex> lockGuard(m_pFreeTypeFontRender->m_kLoadGlyphLock);
        m_pFreeTypeFontRender->setFont(fontInfo->m_sFamily);
       
        
       
        shapedGlyphEntrys.clear();
        shapedGlyphEntrys.reserve(icuUTF16Text.countChar32());
        icu::StringCharacterIterator itr(icuUTF16Text);
        for (UChar32 curChar = itr.first32(); curChar != icu::CharacterIterator::DONE; curChar = itr.next32())
        { 
            FT_Face ftFace = m_pFreeTypeFontRender->getCurrentFTFace();
            FT_UInt glyphIndex = 0;
            if (ftFace)
            {
                glyphIndex = FT_Get_Char_Index(ftFace, curChar);
            }
            if (glyphIndex == 0)
            {
                glyphIndex = FT_Get_Char_Index(m_pFreeTypeFontRender->m_pDefaultFT->face, curChar); 
                if (ftFace && curChar != 0 && glyphIndex == 0)
                {
                    glyphIndex = FT_Get_Char_Index(ftFace, InvalidChar);
                   
                }
                else
                {
                    ftFace = m_pFreeTypeFontRender->m_pDefaultFT->face;
                }
            }

            FT_Error error = FT_Set_Char_Size(ftFace, 0, FreeTypeUtils::convertPixelTo26Dot6<FT_F26Dot6>(fontInfo->m_nFontSize), 72, 72);
            FT_Set_Transform(ftFace, nullptr, nullptr);
            if (error != 0)
            {
                LOGW("JCTextShaper::shapeUnidirectionalText error FT_Set_Char_Size %d", error);
                continue;
            }

            FT_Fixed  advance;
            error = FT_Get_Advance(ftFace, glyphIndex, FT_LOAD_COMPUTE_METRICS, &advance);
            if (error != 0)
            {
                LOGW("shapeUnidirectionalText error FT_Get_Advance %d", error);
                continue;
            }
          
            JCShapedGlyphEntry shapedGlyphEntry;
            shapedGlyphEntry.glyphIndex = glyphIndex;
            shapedGlyphEntry.xAdvance = FreeTypeUtils::convert26Dot6ToRoundedPixel<int16_t>((advance + (1 << 9)) >> 10);;
            shapedGlyphEntry.yAdvance = 0;
            shapedGlyphEntry.xOffset = 0;
            shapedGlyphEntry.yOffset = 0;   
            shapedGlyphEntry.height = (ftFace->size->metrics.ascender - ftFace->size->metrics.descender) >> 6;
            shapedGlyphEntry.bIsVisible = (curChar != ' ');
            shapedGlyphEntry.textDirection = LeftToRight;
            shapedGlyphEntry.pFTFace = ftFace;
            shapedGlyphEntrys.emplace_back(shapedGlyphEntry);
        }
    }
    struct SequenceEntry
    {
        int32_t startIndex;
        int32_t length;
        hb_script_t harfBuzzScript;

        SequenceEntry(const int32_t startIndex, const int32_t length, const hb_script_t harfBuzzScript): startIndex(startIndex), length(length), harfBuzzScript(harfBuzzScript)
        {
        }
    };
    void JCTextShaper::doHarfBuzzTextShaping(const icu::UnicodeString& icuUTF16Text, const int32_t textStart, const int32_t textLength, JCFontInfo* fontInfo, const TextDirection textDirection, std::vector<JCShapedGlyphEntry>& shapedGlyphEntrys) const
    {
        //std::lock_guard<std::recursive_mutex> lockGuard(m_pFreeTypeFontRender->m_kLoadGlyphLock);
        hb_unicode_funcs_t* unicodeFuncs = hb_unicode_funcs_get_default();
        m_pFreeTypeFontRender->setFont(fontInfo->m_sFamily);
        FT_Face ftFace = m_pFreeTypeFontRender->getCurrentFTFace();
        if (ftFace == nullptr)
        {
            LOGI("JCTextShaper error find a null ftFace");
            return;
        }

        hb_buffer_t* hbBuffer = hb_buffer_create();
        int32_t glyphFlags = FT_LOAD_NO_AUTOHINT | FT_LOAD_NO_HINTING;
        hb_font_t* hbFont = createFont(ftFace, glyphFlags, fontInfo->m_nFontSize);
        auto shapeText = [&](hb_script_t harfBuzzScript, const uint16_t* buffer, int textLength, unsigned int itemOffset, int itemLength)
        {
            hb_buffer_set_cluster_level(hbBuffer, HB_BUFFER_CLUSTER_LEVEL_MONOTONE_GRAPHEMES);
            hb_buffer_set_direction(hbBuffer, (textDirection == TextDirection::LeftToRight) ? HB_DIRECTION_LTR : HB_DIRECTION_RTL);
            hb_buffer_set_script(hbBuffer, harfBuzzScript);
            hb_buffer_add_utf16(hbBuffer, buffer, textLength, itemOffset, itemLength);
            hb_shape(hbFont, hbBuffer, NULL, 0);

            unsigned int harfBuzzGlyphCount = 0;
            hb_glyph_info_t* harfBuzzGlyphInfos = hb_buffer_get_glyph_infos(hbBuffer, &harfBuzzGlyphCount);
            hb_glyph_position_t* harfBuzzGlyphPositions = hb_buffer_get_glyph_positions(hbBuffer, &harfBuzzGlyphCount);


            const int32_t initGlyphEntryIndex = shapedGlyphEntrys.size();
            shapedGlyphEntrys.reserve(shapedGlyphEntrys.size() + harfBuzzGlyphCount);
            for (uint32_t harfBuzzGlyphIndex = 0; harfBuzzGlyphIndex < harfBuzzGlyphCount; ++harfBuzzGlyphIndex)
            {
                const hb_glyph_info_t& harfBuzzGlyphInfo = harfBuzzGlyphInfos[harfBuzzGlyphIndex];
                const hb_glyph_position_t& harfBuzzGlyphPosition = harfBuzzGlyphPositions[harfBuzzGlyphIndex];

                const int32_t currentCharIndex = static_cast<int32_t>(harfBuzzGlyphInfo.cluster);
                const char32_t curChar = icuUTF16Text.char32At(currentCharIndex);

                JCShapedGlyphEntry shapedGlyphEntry;
                shapedGlyphEntry.glyphIndex = harfBuzzGlyphInfo.codepoint;
                shapedGlyphEntry.xAdvance = FreeTypeUtils::convert26Dot6ToRoundedPixel<int16_t>(harfBuzzGlyphPosition.x_advance);
                shapedGlyphEntry.yAdvance = FreeTypeUtils::convert26Dot6ToRoundedPixel<int16_t>(harfBuzzGlyphPosition.y_advance);
                shapedGlyphEntry.xOffset = FreeTypeUtils::convert26Dot6ToRoundedPixel<int16_t>(harfBuzzGlyphPosition.x_offset);
                shapedGlyphEntry.yOffset = FreeTypeUtils::convert26Dot6ToRoundedPixel<int16_t>(harfBuzzGlyphPosition.y_offset);
                shapedGlyphEntry.height = (ftFace->size->metrics.ascender - ftFace->size->metrics.descender) >> 6;
                shapedGlyphEntry.bIsVisible = (curChar != ' ');
                shapedGlyphEntry.textDirection = textDirection;
                shapedGlyphEntry.pFTFace = ftFace;
                shapedGlyphEntrys.emplace_back(shapedGlyphEntry);
            }
            hb_buffer_clear_contents(hbBuffer);
        };

        std::vector<SequenceEntry> vecSequenceEntrys;
        vecSequenceEntrys.reserve(16);
        int32_t splitStartIndex = textStart;
        int32_t runningTextIndex = textStart;
        hb_script_t runningHarfBuzzScript = _HB_SCRIPT_MAX_VALUE;

        auto startNewPendingTextSequence = [&](const hb_script_t harfBuzzScript)
        {
            splitStartIndex = runningTextIndex;
            runningHarfBuzzScript = harfBuzzScript;
        };

        auto appendPendingTextToSequence = [&]()
        {
            if (runningHarfBuzzScript != _HB_SCRIPT_MAX_VALUE)
            {
                vecSequenceEntrys.emplace_back(SequenceEntry(splitStartIndex, runningTextIndex - splitStartIndex, runningHarfBuzzScript));
                runningHarfBuzzScript = _HB_SCRIPT_MAX_VALUE;
            }
        };

        auto isSpecialCharacter = [](const hb_script_t harfBuzzScript) -> bool
        {
            // Characters in the common, inherited, and unknown scripts are allowed (and in the case of inherited, required) to merge with the script 
            // of the character(s) that preceded them. This also helps to minimize shaping batches, as spaces are within the common script.
            return harfBuzzScript == HB_SCRIPT_COMMON || harfBuzzScript == HB_SCRIPT_INHERITED || harfBuzzScript == HB_SCRIPT_UNKNOWN;
        };
        const int32_t textEndIndex = textStart + textLength;
        for (; runningTextIndex < textEndIndex; ++runningTextIndex)
        {
            //UTF16 双字节这样结果也是对的 hb_buffer_add_utf16 hb_glyph_info_t.cluster
            const UChar32 curChar = icuUTF16Text.char32At(runningTextIndex);
            const hb_script_t charHarfBuzzScript = hb_unicode_script(unicodeFuncs, curChar);
            if (runningHarfBuzzScript == _HB_SCRIPT_MAX_VALUE || runningHarfBuzzScript != charHarfBuzzScript)
            {
                if (runningHarfBuzzScript == _HB_SCRIPT_MAX_VALUE)
                {
                    startNewPendingTextSequence(charHarfBuzzScript);
                }
                else if (!isSpecialCharacter(charHarfBuzzScript))
                {
                    if (isSpecialCharacter(runningHarfBuzzScript))
                    {
                        runningHarfBuzzScript = charHarfBuzzScript;
                    }
                    else
                    {
                        appendPendingTextToSequence();
                        startNewPendingTextSequence(charHarfBuzzScript);
                    }
                }
            }
        }
        appendPendingTextToSequence();
        for (std::vector<SequenceEntry>::iterator it = vecSequenceEntrys.begin(); it != vecSequenceEntrys.end(); it++)
        {
            shapeText(it->harfBuzzScript, (const uint16_t *)(icuUTF16Text.getBuffer()), icuUTF16Text.length(), it->startIndex, it->length);
        }
        hb_font_destroy(hbFont);
        hb_buffer_destroy(hbBuffer);
    }
    hb_font_t* JCTextShaper::createFont(const FT_Face& ftFace, const int32_t glyphFlags, const int32_t fontSize) const
    {
        FT_Error error = FT_Set_Char_Size(ftFace, 0, FreeTypeUtils::convertPixelTo26Dot6<FT_F26Dot6>(fontSize), 72, 72);
        FT_Set_Transform(ftFace, nullptr, nullptr);
        if (error != 0)
        {
            LOGW("JCTextShaper::createFont error FT_Set_Char_Size %d", error);
        }
        
        hb_font_t* hbFont = hb_ft_font_create(ftFace, nullptr);
        hb_ft_font_set_load_flags(hbFont, glyphFlags);
        return hbFont;
    }
}
