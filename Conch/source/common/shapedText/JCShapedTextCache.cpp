#include "JCShapedTextCache.h"
#include "../shapedText/JCShapeUtils.h"
#include "../util/Log.h"
#include "../../conch/JCSystemConfig.h"
#include <algorithm>
#include "../FontRender/JCFreeTypeRender.h"

namespace laya
{
    extern JCSystemConfig g_kSystemConfig;
    void JCShapedGlyphSequence::getMeasuredSize(int32_t& nWidth, int32_t& nHeight) const
    {
        if (m_nTextWidth == -1 && m_nTextHeight == -1)
        {
            nWidth = 0;
            nHeight = 0;
            for (std::vector<JCShapedGlyphEntry>::const_iterator itr = m_vShapedGlyphEntrys.begin(); itr != m_vShapedGlyphEntrys.end(); itr++)
            {
                nWidth += itr->xAdvance;
                nHeight = (std::max)(nHeight, (int32_t)itr->height);
            }
            m_nTextWidth = nWidth;
            m_nTextHeight = nHeight;
        }
        else
        {
            nWidth = m_nTextWidth;
            nHeight = m_nTextHeight;
        }
    }
    JCShapedTextCache::JCShapedTextCache(JCFreeTypeFontRender* pFreeTypeFontRender): 
        m_pTextShaper(new JCTextShaper(pFreeTypeFontRender))
        , m_pFreeTypeFontRender(pFreeTypeFontRender)
    {
        //LOGI("JCShapedTextCache %x", std::this_thread::get_id());
    }

    const JCShapedGlyphSequence& JCShapedTextCache::getShapedTextSequence(const std::string& utf8Text, JCFontInfo* pFontInfo, Direction nDirection)
    {
        //LOGI("getShapedTextSequence %x", std::this_thread::get_id());
        icu::UnicodeString icuUTF16Text = icu::UnicodeString::fromUTF8(utf8Text.c_str());
        ShapedGlyphSequenceKey key(icuUTF16Text, nDirection, pFontInfo);

        std::unique_lock<std::mutex> lock_a(m_Mutex, std::defer_lock);
        std::unique_lock<std::recursive_mutex> lock_b(m_pFreeTypeFontRender->m_kLoadGlyphLock, std::defer_lock);
        std::lock(lock_a, lock_b);
        {
            MapShapedGlyphSequence::iterator it = m_mapShapedGlyphSequence.find(key);
            if (it != m_mapShapedGlyphSequence.end())
            {
                return *it->second;
            }
            else
            {
                JCShapedGlyphSequence* pSequence = new JCShapedGlyphSequence();
#if 1
                if (g_kSystemConfig.m_nTextShapingMode == TextShapingMode::Simple)
                {
                    m_pTextShaper->shapeUnidirectionalText(icuUTF16Text, pFontInfo, pSequence->m_vShapedGlyphEntrys);
                    pSequence->m_nTextDirection = TextDirection::LeftToRight;
                }
                else
                {
                    TextDirection baseDirection = TextDirection::LeftToRight;
                    switch (nDirection)
                    {
                    case Direction::ELeftToRight:
                        baseDirection = TextDirection::LeftToRight;
                        break;
                    case Direction::ERightToLeft:
                        baseDirection = TextDirection::RightToLeft;
                        break;
                    case Direction::EAuto:
                        baseDirection = ShapeUtils::computeBaseDirection(icuUTF16Text);
                        break;
                    default:
                        LOGW("JCShapedTextCache::getShapedTextSequence invalid direction use TextDirection::LeftToRight");
                        break;
                    }
                    pSequence->m_nTextDirection = baseDirection;
                    m_pTextShaper->shapeBidirectionalText(icuUTF16Text, pFontInfo, baseDirection, pSequence->m_vShapedGlyphEntrys);
                }
#else
                m_pTextShaper->shapeUnidirectionalText(icuUTF16Text, pFontInfo, pSequence->m_vShapedGlyphEntrys);
                pSequence->m_nTextDirection = TextDirection::LeftToRight;
#endif
                m_mapShapedGlyphSequence.insert(std::make_pair(key, pSequence));
           
            return *pSequence; 
            }
        }
    }

    JCShapedTextCache::~JCShapedTextCache()
    {
        //LOGI("~JCShapedTextCache %x", std::this_thread::get_id());
        clear();
        if (m_pTextShaper != NULL)
        {
            delete m_pTextShaper;
            m_pTextShaper = NULL;
        }
    }

    void JCShapedTextCache::clear()
    {
        //LOGI("clear %x", std::this_thread::get_id());
        std::lock_guard<std::mutex> lock(m_Mutex);
        {
            for (MapShapedGlyphSequence::iterator iter = m_mapShapedGlyphSequence.begin(); iter != m_mapShapedGlyphSequence.end(); iter++)
            {
                JCShapedGlyphSequence* pShapedGlyphSequence = iter->second;
                if (pShapedGlyphSequence != NULL)
                {
                    delete pShapedGlyphSequence;
                    pShapedGlyphSequence = NULL;
                }
            }
            m_mapShapedGlyphSequence.clear();
        }
    }

    void JCShapedTextCache::garbageCollection()
    {
        //LOGI("garbageCollection %x", std::this_thread::get_id());
        //优化LRU算法？
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            uint32_t nSize = m_mapShapedGlyphSequence.size();
            if (nSize > g_kSystemConfig.m_nTextCacheNum)
            {
                uint32_t nGarbageNum = nSize / 2;
                LOGI(">>>>>>>>>>>>>JCShapedTextCache garbageCollection count %d garbage count %d", nSize, nGarbageNum);
                MapShapedGlyphSequence::iterator iter = m_mapShapedGlyphSequence.begin();
                int i = 0;
                while (i < nGarbageNum)
                {
                    if (iter->second != nullptr)
                    {
                        delete iter->second;
                    }
                    iter = m_mapShapedGlyphSequence.erase(iter);
                    i++;
                }
            }
        }
    }
}