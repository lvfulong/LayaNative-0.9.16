/**
@file			JCFreeTypeRender.h
@brief			
@author			James
@version		1.0
@date			2016_5_11
*/

#ifndef __JCFreeTypeRender_H__
#define __JCFreeTypeRender_H__

#include <vector>
#include<unordered_map>
#include <string>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "../resource/JCResource.h"
#include "../buffer/JCBuffer.h"
#include <thread>
#include <mutex>
#include "../resource/text/JCFontInfo.h"
#include "../imageLib/JCImageRW.h"
#include "../JCIOSFTInterface.h"
#include <math.h>

namespace laya
{
    struct JCGlyphInfo
    {
        int32_t xBearing = 0;
        int32_t yBearing = 0;
        int32_t ascender = 0;
        int32_t descender = 0;
        int32_t widthNoScale = 0;
        int32_t heightNoScale = 0;
        int16_t underlinePos = 0;
        int16_t underlineThickness = 0;
    };
    #define TEXT_SIZE_ALLOWANCE 8
    #define MAX_FONT_SIZE 256
	class JCFreeTypeFontRender
    {
    public:

        struct FTFaceRecord
        {
            FT_Face face;
            char* buffer;
            FTFaceRecord(FT_Face face = nullptr, char* buffer = nullptr)
                :face(face), buffer(buffer)
            {}
            ~FTFaceRecord()
            {
                if (face)
                {
                    FT_Done_Face(face);
                    face = NULL;
                }
                if (buffer)
                {
                    delete[] buffer;
                    buffer = nullptr;
                }
            }
        };
        typedef std::unordered_map<std::string, FTFaceRecord*> MapFTFace;

	public:
        
        JCFreeTypeFontRender();

		~JCFreeTypeFontRender();

        FT_Face getCurrentFTFace();

        /** @brief 初始化默认字体，传入的参数中间用 \4 分割
         *  @return 返回成功或者失败
        */
		bool initDefaultFont( const char* sDefaultTTFs );


        /** @brief 初始化默认字体buffer 和 size
        *  @return 返回成功或者失败
        */
        bool initDefaultFont(char* pBuffer, int nBuferLen);

        /** @brief 设置IOSFT的Interface 因为是从IOS层传过来的
         *  @param[in] JCIOSFTInterface实例
         *  @return 
        */
        void setIOSFTInterface(JCIOSFTInterface* pIOSFTInterface);

        /** @brief 设置字体从FileName中读取
         *  @param[in] 字体名字
         *  @param[in] ttf的路径
         *  @return 是否读取成功
        */
        bool setFontFaceFromUrl(const char* sFontFamily, const char* sTTFFileName);

        /** @brief 设置字体从buffer中读取
         *  @param[in] 字体的名字
         *  @param[in] buffer
         *  @param[in] bufferLength
         *  @return 是否读取成功
        */
        bool setFontFaceFromBuffer(const char* sFontFamily, char* pBuffer, int nBuferLen );

        /** @brief 移除字体
         *  @param[in] 字体的名字
         *  @return 
        */
        bool removeFont( const char* sFontFamily );

        /** @brief bitmapData 提供buffer和buffer的大小。 返回的时候，bitmapData中的大小已经被改成了实际的大小。
        *  @param[in] 字符
        *  @param[in] bitmapdata
        *  @param[in] colkor
        *  @param[in] 文字信息
        *  @param[out] 如果buffer大小不足，则返回false
        *  @return
        */
        bool getTextBitmapData(char* str, BitmapData* bitmapData, int nColor, JCFontInfo* p_pFontInfo);


        /** @brief 清空用户自己传入的文字
        *  @return
        */
        void clearCustomFont();

        bool loadGlyphData(uint32_t nGlyphIndex, BitmapData* bitmapData, int nColor, JCFontInfo* fontInfo, uint32_t nScale, JCGlyphInfo& glyphInfo, FT_Face pFTFace);
        
        /** @brief 设置字体
        *  @param[in] 字体的名字
        *  @return 是否设置成功
        */
        bool setFont(const char* sFontName);

        void measureText(char* p_sWord, JCFontInfo* p_pFontInfo, int& p_nW, int& p_nH);

        void getMetric(int nUnicode, int& nWidth, int& nHeight);

        void measureTexts(char* p_sWord, JCFontInfo* p_pFontInfo, int& p_nW, int& p_nH);

        void setFontSize(FT_Face pFTFace, int nWidth, int nHeight, uint32_t nScale);
    private:

        /** @brief 设置斜体的值
        *  @param[in] 值越大越斜
        *  @return
        */
        void setItalics(float nValue);

        /** @brief 获得bitmapData从freetype中
        *  @param[in] unicode文字
        *  @param[out] 获得的bitmap,这个bitmap需要外面删除的
        *  @param[out] 宽度
        *  @param[out] 高度
        *  @param[out] 数据的宽度度
        *  @param[out] 数据的高度
        *  @param[out] 左上角的距离
        *  @param[out] 左上角的距离
        *  @return
        */
        void getBitmapData(int nUnicode, unsigned char*& pBitmap, int& nWidth, int& nHeight, int& nDataW, int& nDataH, int& nDataLeft, int& nDataTop, int& nUnderlineTop, int& nUnderlineHeight);

        /** @brief 通过buffer获得FT_face
         *  @param[in] buffer
         *  @param[in] bufferSize
         *  @return FT_Face
        */
        FT_Face getFTFaceFromBuffer(char* pBuffer, int nBuferLen);


        /** @brief 通过文件名字获得FT
        *  @param[in] 文件名字
        *  @return FT_Face
        */
        FT_Face getFTFaceFromFile(const char* sFileName);


        /** @brief 清空默认的font
        */
        void clearDefaultFont();


        /** @brief 获得IOS写入的font路径
         *  @return 返回路径
        */
        std::string getIOSFontTTFPath();

        /** @brief 把ios的font写入文件
         *  @return 返回路径
        */
        std::string writeIOSFontTTF();

        void setFontSize(int nWidth, int nHeight, uint32_t nScale);

      

    public:

        static unsigned short                       m_pWordBmpBorder[4 * (MAX_FONT_SIZE + TEXT_SIZE_ALLOWANCE) * (MAX_FONT_SIZE + TEXT_SIZE_ALLOWANCE)];
        static char                                 m_pWordBuff[4 * (MAX_FONT_SIZE + TEXT_SIZE_ALLOWANCE) * (MAX_FONT_SIZE + TEXT_SIZE_ALLOWANCE)];
        std::recursive_mutex	                    m_kLoadGlyphLock;               ///<锁

    protected:
        FT_Library		                            m_pFTlibrary;                   ///<FreeType需要的
        int				                            m_nFontSizeW;                   ///<字体大小
        int                                         m_nFontSizeH;	                ///<字体大小
        int				                            m_nDevDPIX;                     ///<dpi
        int                                         m_nDevDPIY;                     ///<dpi
        float			                            m_fItalicsValue;                ///<斜体度
        FTFaceRecord*                               m_pDefaultFT;	                ///<默认的字体库
        friend class JCTextShaper;
    private:
        FT_Face                                     m_pCurrentFT;                   ///<当前的字体库
        MapFTFace                                   m_vCustomMadeFT;                ///<用户定制的字体
        JCIOSFTInterface*                           m_pIOSFTInterface;              ///<为了IOS使用的
	};
}
//------------------------------------------------------------------------------


#endif //__JCFreeTypeRender_H__

//-----------------------------END FILE--------------------------------