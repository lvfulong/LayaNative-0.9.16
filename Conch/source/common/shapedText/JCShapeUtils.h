#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H
#include <math.h>
#include <unicode/utypes.h>
#include <unicode/unistr.h>
#include <unicode/coll.h>
#include <unicode/sortkey.h>
#include <unicode/ubidi.h>
#include <vector>
#include <stdint.h>

namespace laya
{
    enum Direction
    {
        ELeftToRight = 0,
        ERightToLeft,
        EAuto,
    };


    enum TextDirection
    {
        LeftToRight = 0,
        RightToLeft,
        Mixed,
    };

    struct TextDirectionInfo
    {
        int32_t startIndex;
        int32_t length;
        TextDirection textDirection;
    };
}
namespace ShapeUtils
{
    using namespace laya;

    TextDirection computeBaseDirection(const icu::UnicodeString& icuUTF16Text);

    TextDirection computeTextDirection(icu::UnicodeString icuUTF16Text, const TextDirection baseDirection, std::vector<TextDirectionInfo>& outTextDirectionInfos);
}
namespace FreeTypeUtils
{
    inline int32_t roundToInt(float f)
    {
#ifdef WIN32
        // Note: the x2 is to workaround the rounding-to-nearest-even-number issue when the fraction is .5
        return _mm_cvt_ss2si(_mm_set_ss(f + f + 0.5f)) >> 1;
#else
        return (int32_t)(floorf(f + 0.5f));
#endif
    }
    /** Convert the given value from 26.6 space into rounded pixel space */
    template <typename TRetType, typename TParamType>
    inline TRetType convert26Dot6ToRoundedPixel(TParamType InValue)
    {
        return static_cast<TRetType>(roundToInt(InValue / 64.0f));
    }

    template <typename TRetType, typename TParamType>
    inline TRetType convertPixelTo26Dot6(TParamType InValue)
    {
        return static_cast<TRetType>(InValue * 64);
    }
    /** Convert the given value from pixel space into 16.16 space */
    template <typename TRetType, typename TParamType>
    inline TRetType convertPixelTo16Dot16(TParamType InValue)
    {
        return static_cast<TRetType>(InValue * 65536);
    }
}