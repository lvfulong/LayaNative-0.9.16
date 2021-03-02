#include "JCShapeUtils.h"
#include <assert.h>
#include <unicode/ucnv.h>
#include "../util/Log.h"
namespace FreeTypeUtils
{ 
}

namespace ShapeUtils
{  
    static UBiDiLevel getParagraphDirection(const TextDirection baseDirection)
    {
        assert(baseDirection != TextDirection::Mixed);
        return (baseDirection == TextDirection::LeftToRight) ? 0 : 1; // 0 = LTR, 1 = RTL
    }

    /*
    The base direction is the overall reading direction of the text (see ComputeBaseDirection).
    This will affect where some characters (such as brackets and quotes) are placed within the resultant FTextDirectionInfo data.
    */

    static TextDirection ICUToLAYA(const UBiDiDirection direction)
    {
        switch (direction)
        {
        case UBIDI_LTR:
            return TextDirection::LeftToRight;
        case UBIDI_RTL:
            return TextDirection::RightToLeft;
        case UBIDI_MIXED:
            return TextDirection::Mixed;
        default:
            break;
        }

        return TextDirection::LeftToRight;
    }

    TextDirection computeBaseDirection(const icu::UnicodeString& icuUTF16Text)
    {
        if (icuUTF16Text.length() == 0)
        {
            return TextDirection::LeftToRight;
        }

        const UBiDiDirection icuBaseDirection = ubidi_getBaseDirection(icuUTF16Text.getBuffer(), icuUTF16Text.length());

        return ICUToLAYA(icuBaseDirection);
    }


    TextDirection computeTextDirection(icu::UnicodeString icuUTF16Text, const TextDirection baseDirection, std::vector<TextDirectionInfo>& outTextDirectionInfos)
    {
        outTextDirectionInfos.clear();

        if (icuUTF16Text.length() == 0)
        {
            return TextDirection::LeftToRight;
        }

        UErrorCode icuStatus = U_ZERO_ERROR;
        UBiDi* icubidi = ubidi_openSized(icuUTF16Text.length(), 0, &icuStatus);
        if (icubidi && U_SUCCESS(icuStatus))
        {
            icuStatus = U_ZERO_ERROR;

            ubidi_setPara(icubidi, icuUTF16Text.getBuffer(), icuUTF16Text.length(), getParagraphDirection(baseDirection), nullptr, &icuStatus);

            if (U_SUCCESS(icuStatus))
            {
                const TextDirection returnDirection = ICUToLAYA(ubidi_getDirection(icubidi));

                const int32_t runCount = ubidi_countRuns(icubidi, &icuStatus);
                outTextDirectionInfos.resize(runCount);
                for (int32_t runIndex = 0; runIndex < runCount; ++runIndex)
                {
                    TextDirectionInfo& curTextDirectionInfo = outTextDirectionInfos[runIndex];

                    int32_t internalStartIndex = 0;
                    int32_t internalLength = 0;
                    curTextDirectionInfo.textDirection = ICUToLAYA(ubidi_getVisualRun(icubidi, runIndex, &internalStartIndex, &internalLength));
                    curTextDirectionInfo.startIndex = internalStartIndex;
                    curTextDirectionInfo.length = internalLength;
                }
                ubidi_close(icubidi);
                return returnDirection;
            }
            else
            {
                ubidi_close(icubidi);
                LOGE("Failed to set the string data on the ICU BiDi object (error code: %d). Text will assumed to be left-to-right", icuStatus);
            }
        }
        else
        {
            LOGE("Failed to create ICU BiDi object error code: %d. Text will assumed to be left-to-right", icuStatus);
        }

        return TextDirection::LeftToRight;
    }
}