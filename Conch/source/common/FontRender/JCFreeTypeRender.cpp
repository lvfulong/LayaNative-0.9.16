/**
@file			JCFreeTypeRender.cpp
@brief			
@author			James
@version		1.0
@date			2016_5_11
*/

#include "JCFreeTypeRender.h"
#include "../util/Log.h"
#include "../fileSystem/JCFileSystem.h"
#include "../util/JCLayaUrl.h"
#include <thread>
#include "../util/JCCommonMethod.h"
#include <assert.h>
#include "../shapedText/JCShapeUtils.h"
#include FT_STROKER_H

#ifdef _MSC_VER
#define MIN __min
#define MAX __max
#else
#define MIN std::min
#define MAX std::max
#endif
namespace laya
{
    unsigned short JCFreeTypeFontRender::m_pWordBmpBorder[4 * (MAX_FONT_SIZE + TEXT_SIZE_ALLOWANCE) * (MAX_FONT_SIZE + TEXT_SIZE_ALLOWANCE)];
    char JCFreeTypeFontRender::m_pWordBuff[4 * (MAX_FONT_SIZE + TEXT_SIZE_ALLOWANCE) * (MAX_FONT_SIZE + TEXT_SIZE_ALLOWANCE)];
    JCFreeTypeFontRender::JCFreeTypeFontRender()
    {
        m_pFTlibrary = NULL;
        m_nFontSizeW = 0;
        m_nFontSizeH = 0;
        m_nDevDPIX = 72;
        m_nDevDPIY = 72;
        m_fItalicsValue = 0;
        m_pDefaultFT = NULL;
        m_pCurrentFT = NULL;
        m_pIOSFTInterface = NULL;
        FT_Init_FreeType(&m_pFTlibrary);
    }
    JCFreeTypeFontRender::~JCFreeTypeFontRender()
    {
        clearCustomFont();
        clearDefaultFont();
        FT_Done_FreeType(m_pFTlibrary);
        m_pFTlibrary = NULL;
    }
    void JCFreeTypeFontRender::clearCustomFont()
    {
        for (MapFTFace::iterator iter = m_vCustomMadeFT.begin(); iter != m_vCustomMadeFT.end(); iter++)
        {
            delete iter->second;
        }
        m_vCustomMadeFT.clear();
    }
    void JCFreeTypeFontRender::clearDefaultFont()
    {
        if (m_pDefaultFT)
        {
            delete m_pDefaultFT;
            m_pDefaultFT = NULL;
        }
    }
    bool JCFreeTypeFontRender::initDefaultFont(const char* sDefaultTTFs)
    {        
        if(strlen(sDefaultTTFs) > 0)
        {
            FT_Face pFTFace = getFTFaceFromFile(sDefaultTTFs);
            if ( pFTFace == NULL)
            {
                return false;
            }
            else
            {
                m_pDefaultFT = new FTFaceRecord(pFTFace);
                m_pCurrentFT = pFTFace;
            }
        }
        else
        {
        #ifdef __APPLE__
            std::string sPath = getIOSFontTTFPath();
            FT_Face pFTFace = getFTFaceFromFile(sPath.c_str());
            if (pFTFace == NULL)
            {
                sPath = writeIOSFontTTF();
                pFTFace = getFTFaceFromFile(sPath.c_str());
                if (pFTFace == NULL)
                {
                    return false;
                }
                else
                {
                    m_pDefaultFT = new FTFaceRecord(pFTFace);
                    m_pCurrentFT = pFTFace;
                }
            }
            else
            {
                m_pDefaultFT = new FTFaceRecord(pFTFace);
                m_pCurrentFT = pFTFace;
            }
        #endif
        }
        return true;
    }
    bool JCFreeTypeFontRender::initDefaultFont(char* pBuffer, int nBuferLen)
    {
        clearDefaultFont();
        //arrayBuffer will be GC , so copy
        char* copyBuffer = new char[nBuferLen];
        memcpy(copyBuffer, pBuffer, nBuferLen);
        FT_Face pFTFace = getFTFaceFromBuffer(copyBuffer, nBuferLen);
        if (pFTFace == NULL)
        {
            delete[] copyBuffer;
            return false;
        }
        else
        {
            m_pDefaultFT = new FTFaceRecord(pFTFace, copyBuffer);
            m_pCurrentFT = pFTFace;
        }
        return true;
    }
    FT_Face JCFreeTypeFontRender::getFTFaceFromBuffer(char* pBuffer, int nBuferLen)
    {
        FT_Error nError = 0;
        FT_Face pFTFace = NULL;
        nError = FT_New_Memory_Face(m_pFTlibrary, (const FT_Byte*)pBuffer, nBuferLen, 0, &pFTFace);
        if (nError == FT_Err_Unknown_File_Format) 
        {
            return NULL;
        }
        else if (nError)
        {
            return NULL;
        }
        FT_Select_Charmap(pFTFace, FT_ENCODING_UNICODE);
        return pFTFace;
    }
    FT_Face JCFreeTypeFontRender::getFTFaceFromFile(const char* sFileName)
    {
        FT_Face pFTFace = NULL;
        int nError = FT_New_Face(m_pFTlibrary, sFileName, 0, &pFTFace);
        if (nError)
        {
            pFTFace = NULL;
        }
        FT_Select_Charmap(pFTFace, FT_ENCODING_UNICODE);
        return pFTFace;
    }
    bool JCFreeTypeFontRender::setFont(const char* sFontName)
    {
        if (m_pDefaultFT == NULL)
        {
            LOGW("default font not set")
            return false;
        }

        if (strcmp(sFontName, "default") == 0)
        {
            m_pCurrentFT = m_pDefaultFT->face;
            return true;
        }
        m_pCurrentFT = NULL;
        MapFTFace::iterator iter = m_vCustomMadeFT.find(sFontName);
        if (iter != m_vCustomMadeFT.end())
        {
            m_pCurrentFT = iter->second->face;
            return true;
        }
        //m_pCurrentFT = m_pDefaultFT->face;
        return false;
    }
	void JCFreeTypeFontRender::setItalics(float fValue)
    {
        if (m_fItalicsValue == fValue)return;
		m_fItalicsValue = fValue;
        FT_Matrix kMatrix;
        kMatrix.xx = 0x10000L;
        kMatrix.xy = (FT_Fixed)(m_fItalicsValue * 0x10000L);
        kMatrix.yx = 0;
        kMatrix.yy = 0x10000L;
        if (m_pCurrentFT)
        {
            FT_Set_Transform(m_pCurrentFT, &kMatrix, 0);
        }
        if (m_pDefaultFT->face)
        {
            FT_Set_Transform(m_pDefaultFT->face, &kMatrix, 0);
        }
	}
	void JCFreeTypeFontRender::getBitmapData(int nUnicode, unsigned char*& pBitmap, int& nWidth, int& nHeight, int& nDataW, int& nDataH, int& nDataLeft, int& nDataTop, int& nUnderlineTop, int& nUnderlineHeight)
    {
        FT_Face pFTFace = m_pCurrentFT;
        FT_UInt nFoundIndex = 0;
        if (pFTFace)
        {
            nFoundIndex = FT_Get_Char_Index(pFTFace, nUnicode);
        }
        if (nFoundIndex == 0)
        {
            nFoundIndex = FT_Get_Char_Index(m_pDefaultFT->face, nUnicode);
            if (nFoundIndex > 0)
            {
                pFTFace = m_pDefaultFT->face;
            }
        }
		if(nFoundIndex == 0)
        {
			LOGW("JCFreeTypeFontRender::getBitmapData Cannot find the specified character in all fonts:%x",nUnicode);
            pBitmap =NULL;
			nWidth=nHeight=nDataW=nDataH=nDataLeft=nDataTop=0;
			return;
		}

        FT_Error kFTError = FT_Load_Glyph(pFTFace, nFoundIndex, FT_LOAD_RENDER | FT_LOAD_DEFAULT | FT_LOAD_NO_BITMAP);
        if (kFTError == 36)
        {
            LOGE("JCFreeTypeFontRender::getBitmapData Not set font size yet");
            pBitmap = NULL;
            nWidth = nHeight = nDataW = nDataH = nDataLeft = nDataTop = 0;
        }
        FT_GlyphSlot pGlyph = pFTFace->glyph;
		if( pGlyph->format != FT_GLYPH_FORMAT_BITMAP )
        {
            FT_Render_Glyph(pGlyph, FT_RENDER_MODE_NORMAL);	
		}
		FT_Bitmap& kBitmap = pGlyph->bitmap;
		nDataLeft= pGlyph->bitmap_left;
		nDataTop = (pFTFace->size->metrics.ascender>>6) - pGlyph->bitmap_top;
		nWidth = pGlyph->advance.x >> 6;//pGlyph->bitmap_left+ kBitmap.width;
		nHeight = (pFTFace->size->metrics.ascender- pFTFace->size->metrics.descender)>>6;
		nDataW = kBitmap.width;
		nDataH = kBitmap.rows;
        //pFTFace->size->metrics.ascender;
        //pFTFace->size->metrics.descender;
  
        //下划线
        nUnderlineHeight = static_cast<int16_t>(FreeTypeUtils::convert26Dot6ToRoundedPixel<int32_t>(FT_MulFix(pFTFace->underline_thickness, pFTFace->size->metrics.y_scale)));
        nUnderlineHeight = nUnderlineHeight > 0 ? nUnderlineHeight : 1;
        int16_t OutUnderlinePos = static_cast<int16_t>(FreeTypeUtils::convert26Dot6ToRoundedPixel<int32_t>(FT_MulFix(pFTFace->underline_position, pFTFace->size->metrics.y_scale)));
        nUnderlineTop = static_cast<int16_t>(FreeTypeUtils::convert26Dot6ToRoundedPixel<int32_t>(pFTFace->size->metrics.ascender)) - OutUnderlinePos - (int)(0.5f * nUnderlineTop);

        pBitmap = new unsigned char [kBitmap.rows*kBitmap.width];
		if(kBitmap.width== kBitmap.pitch)
        {
			memcpy(pBitmap, kBitmap.buffer, kBitmap.rows*kBitmap.width);
		}
        else
        {
			for( int y=0; y<kBitmap.rows; y++)
            {
				memcpy(&pBitmap[y*kBitmap.width], &kBitmap.buffer[y*kBitmap.pitch], kBitmap.width);
			}
		}
	}
    bool JCFreeTypeFontRender::setFontFaceFromUrl(const char* sFontFamily, const char* sTTFFileName)
    {
        m_kLoadGlyphLock.lock();
        bool bRet = false;
        FT_Face pFTFace = getFTFaceFromFile(sTTFFileName);
        if (pFTFace)
        {
            MapFTFace::iterator iter = m_vCustomMadeFT.find(sFontFamily);
            if (iter!= m_vCustomMadeFT.end())
            {
                delete iter->second;
            }
            m_vCustomMadeFT[sFontFamily] = new FTFaceRecord(pFTFace);
            bRet = true;
        }
        m_kLoadGlyphLock.unlock();
        return bRet;
    }
    bool JCFreeTypeFontRender::setFontFaceFromBuffer(const char* sFontFamily, char* pBuffer, int nBuferLen)
    {
        m_kLoadGlyphLock.lock();
        bool bRet = false;
        //arrayBuffer will be GC , so copy
        char* copyBuffer = new char[nBuferLen];
        memcpy(copyBuffer, pBuffer, nBuferLen);
        FT_Face pFTFace = getFTFaceFromBuffer(copyBuffer, nBuferLen);
        if (pFTFace)
        {
            MapFTFace::iterator iter = m_vCustomMadeFT.find(sFontFamily);
            if (iter != m_vCustomMadeFT.end())
            {
                delete iter->second;
            }
            m_vCustomMadeFT[sFontFamily] = new FTFaceRecord(pFTFace, copyBuffer);
            bRet = true;
        }
        m_kLoadGlyphLock.unlock();
        return bRet;
    }
    bool JCFreeTypeFontRender::removeFont(const char* sFontFamily)
    {
        m_kLoadGlyphLock.lock();
        if (strcmp(sFontFamily, "default") == 0)
        {
            return false;
        }
        bool bRet = false;
        MapFTFace::iterator iter = m_vCustomMadeFT.find(sFontFamily);
        if( iter != m_vCustomMadeFT.end() )
        {
            if (m_pCurrentFT == iter->second->face)
            {
                m_pCurrentFT = m_pDefaultFT->face;
            }
            delete iter->second;
            iter = m_vCustomMadeFT.erase(iter);
            bRet = true;
        }
        m_kLoadGlyphLock.unlock();
        return bRet;
    }
    void JCFreeTypeFontRender::setIOSFTInterface(JCIOSFTInterface* pIOSFTInterface)
    {
        m_pIOSFTInterface = pIOSFTInterface;
    }

    bool JCFreeTypeFontRender::getTextBitmapData(char* str, BitmapData* bitmapData, int nColor, JCFontInfo* fontInfo)
    {
        unsigned char firstv = str[0];
        if (firstv == '\r' || firstv == '\n')
        {
            bitmapData->m_nWidth = bitmapData->m_nHeight = 0;
            return true;
        }
        int nInBuffLen = bitmapData->m_nWidth*bitmapData->m_nHeight;
        int nTextWidth = 0;
        int nTextHeight = 0;
        int dataw = 0, datah = 0;
        int datal = 0, datat = 0;
        unsigned char* pBmp = NULL;
        static unsigned short ucStr[8];
        int nUnderlineTop = 0;
        int nUnderlineHeight = 0;
        int nResultLen = UTF8StrToUnicodeStr((unsigned char*)str, ucStr, 8);
        if (nResultLen)
        {
            int nWord = ucStr[0];
            m_kLoadGlyphLock.lock();
            setFont(fontInfo->m_sFamily);
            setFontSize(fontInfo->m_nFontSize, fontInfo->m_nFontSize, 1);
            getBitmapData(nWord, pBmp, nTextWidth, nTextHeight, dataw, datah, datal, datat, nUnderlineTop, nUnderlineHeight);
            m_kLoadGlyphLock.unlock();
        }
        datal = datal > 0 ? datal : 0;
        datat = datat > 0 ? datat : 0;
        if (nTextWidth<dataw + datal)
        {
            nTextWidth = dataw + datal;
        }
        if (nTextHeight<datah + datat)
        {
            nTextHeight = datah + datat;
        }
        int nBorderSize = fontInfo->m_nBorderSize > TEXT_SIZE_ALLOWANCE / 2 ? TEXT_SIZE_ALLOWANCE / 2 : fontInfo->m_nBorderSize;
        nTextWidth += nBorderSize * 2;
        nTextHeight += nBorderSize * 2;
        if (nTextHeight*nTextWidth>nInBuffLen)
        {
            LOGW("JCFreeTypeFontRender::getTextBitmapData the buffersize is insufficient.");
            delete pBmp;
            return false;
        }
        unsigned int* pBuff = (unsigned int*)bitmapData->m_pImageData;//  new unsigned int [nTextWidth*nTextHeight];
        memset(pBuff, 0, nTextWidth*nTextHeight * 4);//TODO 为了效率这个能去掉么
        if (nBorderSize > 0)
        {
            //这里先把描边绘制好
            int nW = dataw + nBorderSize * 2;
            int nH = datah + nBorderSize * 2;
            unsigned short* pBorderBmp = m_pWordBmpBorder;
            memset(pBorderBmp, 0, nW * nH * sizeof(uint64_t));
            for (int y = 0; y < datah; y++)
            {
                for (int x = 0; x < dataw; x++)
                {
                    unsigned char bmpColor = pBmp[y*dataw + x];
                    //8个方向扩充
                    pBorderBmp[y * nW + x] += bmpColor;//左上
                    pBorderBmp[y * nW + x + nBorderSize] += bmpColor;//上
                    pBorderBmp[y * nW + x + nBorderSize * 2] += bmpColor;//右上
                    pBorderBmp[(y + nBorderSize)*nW + x] += bmpColor;//左中
                    pBorderBmp[(y + nBorderSize)*nW + x + nBorderSize * 2] += bmpColor;//右中
                    pBorderBmp[(y + nBorderSize * 2)*nW + x] += bmpColor;//左下
                    pBorderBmp[(y + nBorderSize * 2)*nW + x + nBorderSize] += bmpColor;//下
                    pBorderBmp[(y + nBorderSize * 2)*nW + x + nBorderSize * 2] += bmpColor;//右下
                }
            }
            {
                int nBB = fontInfo->m_nBorderColor & 0xff;
                int nBG = (fontInfo->m_nBorderColor >> 8) & 0xff;
                int nBR = (fontInfo->m_nBorderColor >> 16) & 0xff;
                for (int y = 0; y < nH; y++)
                {
                    for (int x = 0; x < nW; x++)
                    {
                        uint64_t bmpColor = pBorderBmp[x + y*nW];
                        bmpColor = bmpColor > 255 ? 255 : bmpColor;
                        float fAlpha = bmpColor / 255.0f;
                        int b = nBB * fAlpha;
                        int g = nBG * fAlpha;
                        int r = nBR * fAlpha;
                        pBuff[x + datal + (y + datat)*nTextWidth] = (bmpColor << 24) + (b << 16) + (g << 8) + r;
                    }
                }
            }

            //文字本身的颜色
            int nTB = nColor & 255;
            int nTG = (nColor >> 8) & 255;
            int nTR = (nColor >> 16) & 255;
            //文字描边的颜色
            int nBB = fontInfo->m_nBorderColor & 0xff;
            int nBG = (fontInfo->m_nBorderColor >> 8) & 0xff;
            int nBR = (fontInfo->m_nBorderColor >> 16) & 0xff;
            for (int y = 0; y < datah; y++)
            {
                for (int x = 0; x < dataw; x++)
                {
                    int alpha = (int)(pBmp[x + y*dataw]);
                    float fAlpha = alpha / 255.0f;
                    int nPos = (y + datat + nBorderSize) * nTextWidth + x + nBorderSize + datal;
                    if (alpha > 0)
                    {
                        int r = (int)(nTR * fAlpha + nBR * (1 - fAlpha));
                        int g = (int)(nTG * fAlpha + nBG * (1 - fAlpha));
                        int b = (int)(nTB * fAlpha + nBB * (1 - fAlpha));
                        pBuff[nPos] = (255 << 24) + (b << 16) + (g << 8) + r;
                    }
                }
            }
            //下划线
            if (fontInfo->m_nDecorationLine == 1)
            {
                int nLineColor = fontInfo->m_nDecorationColor;
                unsigned char* pLineColor = (unsigned char*)&nLineColor;
                std::swap(pLineColor[0], pLineColor[2]);
                int nLineTop = nUnderlineTop + nBorderSize*2;
                for (int y = 0; y < nUnderlineHeight; y++) {
                    for (int x = 0; x < nTextWidth; x++) {
                        pBuff[x + (y + nLineTop)*nTextWidth] = *(int*)(pLineColor);
                    }
                }
                if (nLineTop + nUnderlineHeight > nTextHeight)
                {
                    nTextHeight = nUnderlineTop + nUnderlineHeight;
                }
            }
        }
        else
        {
            int nTB = nColor & 255;
            int nTG = (nColor >> 8) & 255;
            int nTR = (nColor >> 16) & 255;
            for (int y = 0; y < datah; y++) {
                for (int x = 0; x < dataw; x++) {
                    unsigned char a = pBmp[x + y*dataw];
                    float fAlpha = a / 255.0f;
                    int b = nTB * fAlpha;
                    int g = nTG * fAlpha;
                    int r = nTR * fAlpha;
                    pBuff[x + datal + (y + datat)*nTextWidth] = (a << 24) + (b << 16) + (g << 8) + r;
                }
            }
            //下划线
            if (fontInfo->m_nDecorationLine == 1)
            {
                int nLineColor = fontInfo->m_nDecorationColor;
                unsigned char* pLineColor = (unsigned char*)&nLineColor;
                std::swap(pLineColor[0], pLineColor[2]);
                for (int y = 0; y < nUnderlineHeight; y++) {
                    for (int x = 0; x < nTextWidth; x++) {
                        pBuff[x + (y + nUnderlineTop)*nTextWidth] = *(int*)(pLineColor);
                    }
                }
                if (nUnderlineTop + nUnderlineHeight > nTextHeight)
                {
                    nTextHeight = nUnderlineTop + nUnderlineHeight;
                }
            }
        }
        bitmapData->m_nWidth = nTextWidth;
        bitmapData->m_nHeight = nTextHeight;
        bitmapData->m_bPremultiplyAlpha = true;//防止多次预乘
        delete[] pBmp;
        pBmp = NULL;
        return true;
    }
    std::string JCFreeTypeFontRender::getIOSFontTTFPath()
    {
        return m_pIOSFTInterface->getIOSFontTTFPath();
    }
    std::string JCFreeTypeFontRender::writeIOSFontTTF()
    {
        return m_pIOSFTInterface->writeIOSFontTTF();
    }
    FT_Face JCFreeTypeFontRender::getCurrentFTFace()
    {
        //assert(m_pCurrentFT != NULL && "font not set");
        return m_pCurrentFT;
    }

    struct Vec2
    {
        Vec2() { }
        Vec2(float a, float b)
            : x(a), y(b) { }

        float x, y;
    };
    struct Rect
    {
        Rect() { }
        Rect(float left, float top, float right, float bottom)
            : xmin(left), xmax(right), ymin(top), ymax(bottom) { }

        void Include(const Vec2 &r)
        {
            xmin = MIN(xmin, r.x);
            ymin = MIN(ymin, r.y);
            xmax = MAX(xmax, r.x);
            ymax = MAX(ymax, r.y);
        }

        float Width() const { return xmax - xmin + 1; }
        float Height() const { return ymax - ymin + 1; }

        float xmin, xmax, ymin, ymax;
    };
    // A horizontal pixel span generated by the FreeType renderer.
    struct Span
    {
        Span() { }
        Span(int _x, int _y, int _width, int _coverage)
            : x(_x), y(_y), width(_width), coverage(_coverage) { }

        int x, y, width, coverage;
    };

    typedef std::vector<Span> Spans;
    // Each time the renderer calls us back we just push another span entry on our list.

    void RasterCallback(const int y, const int count, const FT_Span * const spans, void * const user)
    {
        Spans *sptr = (Spans *)user;
        for (int i = 0; i < count; ++i)
            sptr->push_back(Span(spans[i].x, y, spans[i].len, spans[i].coverage));
    }
    bool JCFreeTypeFontRender::loadGlyphData(uint32_t nGlyphIndex, BitmapData* bitmapData, int nColor, JCFontInfo* fontInfo, uint32_t nScale, JCGlyphInfo& glyphInfo, FT_Face pFTFace)
    {
        /************************/
        std::lock_guard<std::recursive_mutex> lockGuard(m_kLoadGlyphLock);
        if (nScale != 1)//有縮放
        {
            //setFont(fontInfo->m_sFamily);
            setFontSize(pFTFace, fontInfo->m_nFontSize, fontInfo->m_nFontSize, 1);

            FT_Error kFTError = FT_Load_Glyph(pFTFace, nGlyphIndex, FT_LOAD_RENDER);
            if (kFTError != 0)
            {
                pFTFace = m_pDefaultFT->face;
                kFTError = FT_Load_Glyph(pFTFace, nGlyphIndex, FT_LOAD_RENDER);
                if (kFTError != 0)
                {
                    LOGE("JCFreeTypeFontRender::loadGlyphData error %d", kFTError);
                    bitmapData->m_nWidth = bitmapData->m_nHeight = 0;
                    return false;
                }
            }

            FT_GlyphSlot pGlyph = pFTFace->glyph;
            glyphInfo.xBearing = pGlyph->bitmap_left;
            glyphInfo.yBearing = pGlyph->bitmap_top;
            glyphInfo.ascender = pFTFace->size->metrics.ascender >> 6;
            glyphInfo.descender = pFTFace->size->metrics.descender >> 6;
            glyphInfo.widthNoScale = pGlyph->bitmap.width;
            glyphInfo.heightNoScale = pGlyph->bitmap.rows;
            glyphInfo.underlinePos = static_cast<int16_t>(FreeTypeUtils::convert26Dot6ToRoundedPixel<int32_t>(FT_MulFix(pFTFace->underline_position, pFTFace->size->metrics.y_scale)));
            glyphInfo.underlineThickness = static_cast<int16_t>(FreeTypeUtils::convert26Dot6ToRoundedPixel<int32_t>(FT_MulFix(pFTFace->underline_thickness, pFTFace->size->metrics.y_scale)));
        }

        /************************/
        int outlineWidth = fontInfo->m_nBorderSize > 0 ? 1 : fontInfo->m_nBorderSize;

        //setFont(fontInfo->m_sFamily);
        setFontSize(pFTFace, fontInfo->m_nFontSize, fontInfo->m_nFontSize, nScale);

        FT_Error kFTError = FT_Load_Glyph(pFTFace, nGlyphIndex, outlineWidth > 0 ? FT_LOAD_NO_BITMAP : FT_LOAD_RENDER);
        if (kFTError != 0)
        {
            pFTFace = m_pDefaultFT->face;
            kFTError = FT_Load_Glyph(pFTFace, nGlyphIndex, outlineWidth > 0 ? FT_LOAD_NO_BITMAP : FT_LOAD_RENDER);
            if (kFTError != 0)
            {
                LOGE("JCFreeTypeFontRender::loadGlyphData error %d", kFTError);
                bitmapData->m_nWidth = bitmapData->m_nHeight = 0;
                 return false;
            }
        }

        // Need an outline for this to work.
        if (pFTFace->glyph->format == FT_GLYPH_FORMAT_OUTLINE)
        {
            // Render the basic glyph to a span list.
            Spans spans;
            // Set up the raster parameters and render the outline.
            FT_Raster_Params params;
            memset(&params, 0, sizeof(params));
            params.flags = FT_RASTER_FLAG_AA | FT_RASTER_FLAG_DIRECT;
            params.gray_spans = RasterCallback;
            params.user = &spans;

            FT_Outline_Render(m_pFTlibrary, &pFTFace->glyph->outline, &params);

            // Next we need the spans for the outline.
            Spans outlineSpans;

            // Set up a stroker.
            FT_Stroker stroker;
            FT_Stroker_New(m_pFTlibrary, &stroker);
            FT_Stroker_Set(stroker, (int)(outlineWidth * 64), FT_STROKER_LINECAP_ROUND, FT_STROKER_LINEJOIN_ROUND, 0);

            FT_Glyph glyph;
            if (FT_Get_Glyph(pFTFace->glyph, &glyph) == 0)
            {
                FT_Glyph_StrokeBorder(&glyph, stroker, 0, 1);
                // Again, this needs to be an outline to work.
                if (glyph->format == FT_GLYPH_FORMAT_OUTLINE)
                {
                    // Render the outline spans to the span list
                    FT_Outline *o = &reinterpret_cast<FT_OutlineGlyph>(glyph)->outline;

                    FT_Raster_Params params;
                    memset(&params, 0, sizeof(params));
                    params.flags = FT_RASTER_FLAG_AA | FT_RASTER_FLAG_DIRECT;
                    params.gray_spans = RasterCallback;
                    params.user = &outlineSpans;

                    FT_Outline_Render(m_pFTlibrary, o, &params);
                }
                FT_GlyphSlot pGlyph = pFTFace->glyph;
                // Note: in order to render the stroke properly AND to get proper measurements this must be done after rendering the stroke
                if (nScale == 1 && pGlyph->format != FT_GLYPH_FORMAT_BITMAP)
                {
                    FT_Render_Glyph(pGlyph, FT_RENDER_MODE_NORMAL);
                }
                // Clean up afterwards.
                FT_Stroker_Done(stroker);
                FT_Done_Glyph(glyph);

                // Now we need to put it all together.
                if (!spans.empty())
                {
                    // Figure out what the bounding rect is for both the span lists.
                    Rect rect(spans.front().x, spans.front().y, spans.front().x, spans.front().y);
                    for (Spans::iterator s = spans.begin(); s != spans.end(); ++s)
                    {
                        rect.Include(Vec2(s->x, s->y));
                        rect.Include(Vec2(s->x + s->width - 1, s->y));
                    }
                    for (Spans::iterator s = outlineSpans.begin(); s != outlineSpans.end(); ++s)
                    {
                        rect.Include(Vec2(s->x, s->y));
                        rect.Include(Vec2(s->x + s->width - 1, s->y));
                    }

                    // Get some metrics of our image.
                    int imgWidth = rect.Width();
                    int imgHeight = rect.Height();
                    int imgSize = imgWidth * imgHeight;
   
                    if (nScale == 1)//無縮放
                    {
                        glyphInfo.xBearing = pGlyph->bitmap_left;
                        glyphInfo.yBearing = pGlyph->bitmap_top;
                        glyphInfo.ascender = pFTFace->size->metrics.ascender >> 6;
                        glyphInfo.descender = pFTFace->size->metrics.descender >> 6;
                        glyphInfo.widthNoScale = imgWidth;
                        glyphInfo.heightNoScale = imgHeight;
                        glyphInfo.underlinePos = static_cast<int16_t>(FreeTypeUtils::convert26Dot6ToRoundedPixel<int32_t>(FT_MulFix(pFTFace->underline_position, pFTFace->size->metrics.y_scale)));
                        glyphInfo.underlineThickness = static_cast<int16_t>(FreeTypeUtils::convert26Dot6ToRoundedPixel<int32_t>(FT_MulFix(pFTFace->underline_thickness, pFTFace->size->metrics.y_scale)));
                    }

                    int nInBuffLen = bitmapData->m_nWidth * bitmapData->m_nHeight;
                    if (imgSize > nInBuffLen)
                    {
                        LOGW("JCFreeTypeFontRender::loadGlyphData the buffersize is insufficient. Text is too big");
                        bitmapData->m_nWidth = bitmapData->m_nHeight = 0;
                        return false;
                    }

                    uint32_t* pBuff = (uint32_t*)bitmapData->m_pImageData;
                    memset(pBuff, 0, imgSize * 4);

                    int outlineBlue = fontInfo->m_nBorderColor & 0xff;
                    int outlineGreen = (fontInfo->m_nBorderColor >> 8) & 0xff;
                    int outlineRed = (fontInfo->m_nBorderColor >> 16) & 0xff;
 
                    // Loop over the outline spans and just draw them into the image.
                    for (Spans::iterator s = outlineSpans.begin(); s != outlineSpans.end(); ++s)
                    {
                        for (int w = 0; w < s->width; ++w)
                        {
                            pBuff[(int)((imgHeight - 1 - (s->y - rect.ymin)) * imgWidth + s->x - rect.xmin + w)] = (s->coverage << 24) + (outlineBlue << 16) + (outlineGreen << 8) + outlineRed;
                        }
                    }

                    uint8_t srcB = nColor & 0xff;
                    uint8_t srcG = (nColor >> 8) & 0xff;
                    uint8_t srcR = (nColor >> 16) & 0xff;

                    // Then loop over the regular glyph spans and blend them into the image.
                    for (Spans::iterator s = spans.begin(); s != spans.end(); ++s)
                    {
                        for (int w = 0; w < s->width; ++w)
                        {
                            int index = (int)((imgHeight - 1 - (s->y - rect.ymin)) * imgWidth + s->x - rect.xmin + w);
                            uint32_t dst = pBuff[index];

                            uint8_t srcA = s->coverage;

                            uint8_t dstA = (dst >> 24) & 0xff;
                            uint8_t dstR = (dst >> 16) & 0xff;
                            uint8_t dstG = (dst >> 8) & 0xff;
                            uint8_t dstB = dst & 0xff;
                               
                            uint8_t r = (dstR + ((srcR - dstR) * srcA) / 255.0f);
                            uint8_t g = (dstG + ((srcG - dstG) * srcA) / 255.0f);
                            uint8_t b = (dstB + ((srcB - dstB) * srcA) / 255.0f);
                            uint8_t a = MIN(255, dstA + srcA);

                            float fa = a / 255.0f;
                            uint8_t fb = b * fa;
                            uint8_t fg = g * fa;
                            uint8_t fr = r * fa;
                            pBuff[index] = (a << 24) + (fb << 16) + (fg << 8) + fr;
                        }
                    }
                    bitmapData->m_nWidth = imgWidth;
                    bitmapData->m_nHeight = imgHeight;
                    bitmapData->m_bPremultiplyAlpha = true;//防止多次预乘
                    return true;
                }
                else
                {
                    bitmapData->m_nWidth = bitmapData->m_nHeight = 0;
                    return false;
                }
            }
        }
        else
        {
            FT_GlyphSlot pGlyph = pFTFace->glyph;
            FT_Bitmap& kBitmap = pGlyph->bitmap;
            int nTextWidth = kBitmap.width;
            int nTextHeight = kBitmap.rows;

            if (nScale == 1)//無縮放
            {
                glyphInfo.xBearing = pGlyph->bitmap_left;
                glyphInfo.yBearing = pGlyph->bitmap_top;
                glyphInfo.ascender = pFTFace->size->metrics.ascender >> 6;
                glyphInfo.descender = pFTFace->size->metrics.descender >> 6;
                glyphInfo.widthNoScale = pGlyph->bitmap.width;
                glyphInfo.heightNoScale = pGlyph->bitmap.rows;
                glyphInfo.underlinePos = static_cast<int16_t>(FreeTypeUtils::convert26Dot6ToRoundedPixel<int32_t>(FT_MulFix(pFTFace->underline_position, pFTFace->size->metrics.y_scale)));
                glyphInfo.underlineThickness = static_cast<int16_t>(FreeTypeUtils::convert26Dot6ToRoundedPixel<int32_t>(FT_MulFix(pFTFace->underline_thickness, pFTFace->size->metrics.y_scale)));
            }

            int nInBuffLen = bitmapData->m_nWidth * bitmapData->m_nHeight;
            if (nTextHeight * nTextWidth > nInBuffLen)
            {
                LOGW("JCFreeTypeFontRender::loadGlyphData the buffersize is insufficient. Text is too big");
                bitmapData->m_nWidth = bitmapData->m_nHeight = 0;
                return false;
            }

            uint32_t* pBuff = (uint32_t*)bitmapData->m_pImageData;
            memset(pBuff, 0, nTextWidth * nTextHeight * 4);

            uint8_t nTB = nColor & 0xff;
            uint8_t nTG = (nColor >> 8) & 0xff;
            uint8_t nTR = (nColor >> 16) & 0xff;
            for (int y = 0; y < nTextHeight; y++)
            {
                for (int x = 0; x < nTextWidth; x++)
                {
                    uint8_t a = kBitmap.buffer[x + y * kBitmap.pitch];
                    float fAlpha = a / 255.0f;
                    uint8_t b = nTB * fAlpha;
                    uint8_t g = nTG * fAlpha;
                    uint8_t r = nTR * fAlpha;
                    pBuff[x + y * nTextWidth] = (a << 24) + (b << 16) + (g << 8) + r;
                }
            }
            bitmapData->m_nWidth = nTextWidth;
            bitmapData->m_nHeight = nTextHeight;
            bitmapData->m_bPremultiplyAlpha = true;//防止多次预乘
        }
        return true;
    }
    void JCFreeTypeFontRender::setFontSize(FT_Face pFTFace, int nWidth, int nHeight, uint32_t nScale)
    {
        int nW = nWidth << 6;
        int nH = nHeight << 6;

        FT_Set_Char_Size(pFTFace, nW, nH, m_nDevDPIX, m_nDevDPIY);
 
        if (nScale != 1)
        {
            FT_Matrix ScaleMatrix;
            ScaleMatrix.xy = 0;
            ScaleMatrix.xx = FreeTypeUtils::convertPixelTo16Dot16<FT_Fixed>(nScale);
            ScaleMatrix.yy = FreeTypeUtils::convertPixelTo16Dot16<FT_Fixed>(nScale);
            ScaleMatrix.yx = 0;

            FT_Set_Transform(pFTFace, &ScaleMatrix, nullptr);

        }
        else
        {
            FT_Set_Transform(pFTFace, nullptr, nullptr);
        }
    }
    void JCFreeTypeFontRender::setFontSize(int nWidth, int nHeight, uint32_t nScale)
    {
        int nW = nWidth << 6;
        int nH = nHeight << 6;
        if (m_pCurrentFT)
        {
            FT_Set_Char_Size(m_pCurrentFT, nW, nH, m_nDevDPIX, m_nDevDPIY);
        }
        if (m_pDefaultFT->face)
        {
            FT_Set_Char_Size(m_pDefaultFT->face, nW, nH, m_nDevDPIX, m_nDevDPIY);
        }
        if (nScale != 1)
        {
            FT_Matrix ScaleMatrix;
            ScaleMatrix.xy = 0;
            ScaleMatrix.xx = FreeTypeUtils::convertPixelTo16Dot16<FT_Fixed>(nScale);
            ScaleMatrix.yy = FreeTypeUtils::convertPixelTo16Dot16<FT_Fixed>(nScale);
            ScaleMatrix.yx = 0;
            if (m_pCurrentFT)
            {
                FT_Set_Transform(m_pCurrentFT, &ScaleMatrix, nullptr);
            }
            if (m_pDefaultFT->face)
            {
                FT_Set_Transform(m_pDefaultFT->face, &ScaleMatrix, nullptr);
            }
        }
        else
        {
            if (m_pCurrentFT)
            {
                FT_Set_Transform(m_pCurrentFT, nullptr, nullptr);
            }
            if (m_pDefaultFT->face)
            {
                FT_Set_Transform(m_pDefaultFT->face, nullptr, nullptr);
            }
        }
    }
    void JCFreeTypeFontRender::measureText(char* p_sWord, JCFontInfo* p_pFontInfo, int& p_nW, int& p_nH)
    {
        if (!p_sWord)
        {
            p_nW = p_nH = 0;
            return;
        }
        int pA = 0;
        static unsigned short ucStr[8];
        int nResultLen = UTF8StrToUnicodeStr((unsigned char*)p_sWord, ucStr, 8);
        if (nResultLen > 1)
        {
            measureTexts(p_sWord, p_pFontInfo, p_nW, p_nH);
            return;
        }
        if (nResultLen)
        {
            int nWord = ucStr[0];
            m_kLoadGlyphLock.lock();
            setFont(p_pFontInfo->m_sFamily);
            setFontSize(p_pFontInfo->m_nFontSize, 0, 1);
            getMetric(nWord, p_nW, p_nH);
            m_kLoadGlyphLock.unlock();
        }
    }
    void JCFreeTypeFontRender::measureTexts(char* p_sWord, JCFontInfo* p_pFontInfo, int& p_nW, int& p_nH)
    {
        if (!p_sWord)
        {
            p_nW = p_nH = 0;
            return;
        }
        int pA = 0;
        int nWord;
        int wordlen = strlen(p_sWord);
        m_kLoadGlyphLock.lock();
        setFont(p_pFontInfo->m_sFamily);
        setFontSize(p_pFontInfo->m_nFontSize, 0, 1);
        int maxHeight = 0;
        int width = 0;
        while (true)
        {
            int ulen = UTF8ToUnicode((unsigned char*)p_sWord, &nWord);
            wordlen -= ulen;
            getMetric(nWord, p_nW, p_nH);
            pA += p_nW;
            width += p_nW;
            if (p_nH > maxHeight) maxHeight = p_nH;
            if (wordlen <= 0)
                break;
            p_sWord += ulen;
        }
        p_nW = width;
        p_nH = maxHeight;
        m_kLoadGlyphLock.unlock();
    }
    void JCFreeTypeFontRender::getMetric(int nUnicode, int& nWidth, int& nHeight)
    {
        FT_UInt nFoundIndex = 0;
        FT_Face pFTFace = m_pCurrentFT;
        //先去看看用户设置的字体中是否存在
        if (pFTFace)
        {
            nFoundIndex = FT_Get_Char_Index(pFTFace, nUnicode);
        }
        if (nFoundIndex == 0)
        {
            pFTFace = m_pDefaultFT->face;
            nFoundIndex = FT_Get_Char_Index(m_pDefaultFT->face, nUnicode);
        }
        if (nFoundIndex == 0)
        {
            LOGW("JCFreeTypeFontRender::getMetric Cannot find the specified character in all fonts:%x", nUnicode);
            nWidth = nHeight = 0;
            return;
        }
        FT_Error kFTError = FT_Load_Glyph(pFTFace, nFoundIndex, FT_LOAD_RENDER | FT_LOAD_DEFAULT | FT_LOAD_NO_BITMAP);
        if (kFTError == 36)
        {
            LOGE("JCFreeTypeFontRender::getMetric Not set font size yet");
            nWidth = nHeight = 0;
        }
        FT_GlyphSlot pGlyph = pFTFace->glyph;
        FT_Bitmap& kBitmap = pGlyph->bitmap;
        nWidth = pGlyph->bitmap_left + kBitmap.width;
        nHeight = (pFTFace->size->metrics.ascender - pFTFace->size->metrics.descender) >> 6;
    }
}
//------------------------------------------------------------------------------


//-----------------------------END FILE--------------------------------
