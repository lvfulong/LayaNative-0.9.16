/**
@file			JCImageRW.h
@brief			
@author			hugao
@version		1.0
@date			2016_5_11
*/

#ifndef __JCImageRW_H__
#define __JCImageRW_H__

#include <memory>
#include <vector>
#include <functional>
#include "../3DRS/JCGL.h"
#include "../geom/Rectangle.h"
#define PNG_UINT32 unsigned int



namespace laya
{
	enum ImageType
    {
		ImgType_jpeg,
		ImgType_png,
		ImgType_gif,
        ImgType_pvrv2,
        ImgType_pvrv3,
        ImgType_etc,
        ImgType_tpg,
		ImgType_unk,
	};
    struct MipmapData
    {
        int m_nLength = 0;
        char* m_pData = nullptr;
    };
	struct ImageBaseInfo
    {
        int m_nWidth;
        int m_nHeight;
        int m_nBpp;
        int m_nFormat = GL_RGBA;
        bool m_bIsCompressed = false;
        int m_nLength = 0;
        float m_nTextureHeight = 0;
        float m_nTextureWidth = 0;
        float m_nOffsetX = 0;
        float m_nOffsetY = 0;
        ImageBaseInfo()
        {
        }
        
	};
    bool isCompressedFormat(int nFormat);

    int calcTextureSize(int nFormat, int nWidth, int nHeight);

	class BitmapData:public ImageBaseInfo
    {
	public:
		BitmapData();
		BitmapData(char* p_pData, int p_nWidth, int p_nHeight );
		BitmapData(int p_nWidth, int p_nHeight, bool p_bTranslate, int p_nFillColor );
        char* premultiplyAlphaData();
		void releaseData()
        { 
			if(m_pImageData)
            {
				delete [] m_pImageData;
				m_pImageData=0;
			}
            if (m_pUserData)
            {
                delete[] m_pUserData;
                m_pUserData = 0;
            }
            m_bPremultiplyAlpha = false;
		}
		char*	m_pImageData;
        bool m_bPremultiplyAlpha = false;
        char* m_pUserData;
	};

	//�����Ƕ���ʵ�����������á���Ϊʵ�ʶ����������ʱ����
	typedef std::function<void(BitmapData bmp)> imgDecodeCB;

    /** @brief �ں����ڲ���� p_pData ����ռ䡣ע����һά���飬���Ƕ�ά�� ����ֻ�������ͼƬ��û�л������ȹ��ܡ�
     *         ���ص��Ǻ����ڲ������ָ�롣��Ҫ�ⲿ�ͷ�
     *  @param[in] �ڴ�����
     *  @param[in] ����
     *  @param[in] ��õ���bitmap
     *  @return �Ƿ����ɹ�
    */
	bool loadImageMemSync( const char* p_pMem, int p_nLenth, BitmapData& p_bmp );

    /** @brief
         ֱ�Ӽ���һ��ͼƬ�ļ���
     *  @param[in] p_pszFile �����ļ���·��������·����
     *  @return 
            ����һ��Bitmap����ע����Ҫ�Լ��ֶ�ɾ�����е�ָ�롣
    */
    BitmapData loadLocalImageSync( const char* p_pszFile );


	void loadImageMemASync(std::shared_ptr<char> p_pBuff, int p_nLenth, imgDecodeCB p_CB);

	ImageType getImgType( const char* p_pMem, int p_nLength );

    /** @brief ���ͼƬ�Ļ�����Ϣ��Ҫ����٣�������ؽ���Ͳ�Ҫ���� ͬ������
     *  @param[in] �ڴ�����
     *  @param[in] �ڴ泤��
     *  @param[in] ͼƬ������Ϣ
     *  @return �Ƿ����ɹ�
    */
	bool getImageBaseInfo( const char* p_pMem, int p_nLength, ImageBaseInfo& p_Info );

	/** @brief ����png
	 *  @param[in] ͼƬ����
	 *  @param[in] w
	 *  @param[in] h
	 *  @param[in] �ļ�����
	 *  @return �Ƿ񱣴�ɹ�
	*/
	bool saveAsPng(const char* p_pData, int w, int h, const char* p_pszFile );

    /** @brief ����jpg
    *  @param[in] ͼƬ����
    *  @param[in] ͼƬ��Ϣ
    *  @param[in] �ļ�����
    *  @return �Ƿ񱣴�ɹ�
    */
	bool saveAsJpeg(const char* p_pData, ImageBaseInfo& p_Info, const char* p_pszFile );

    /** @brief ����bmp
    *  @param[in] ͼƬ����
    *  @param[in] ͼƬ��Ϣ
    *  @return �Ƿ񱣴�ɹ�
    */
	bool saveAsBmp(const char* p_pData, ImageBaseInfo& p_Info);

    /** @brief windw��ÿ����������һ����windh���ơ�
     *  @param[in] 
     *  @param[in]
     *  @param[in]
     *  @param[out] 
     *  @return  ���ص�dst���ں����ڲ������
    */
	bool downsampleBmp( BitmapData& src, BitmapData& dst, int windw, int windh, bool rbBorder );
	
    /** @brief pFrcation ��4��int��ʾ�Ŀ�����š�[����ӣ����ĸ���߷��ӣ��߷�ĸ]
     *  @param[in] 
     *  @param[in]
     *  @param[in]
     *  @param[out] 
     *  @return 
    */
	bool downsampleBmp( BitmapData& src, BitmapData& dst, int* pFrcation, bool rbBorder );

	void createGridBmp( BitmapData& out, int w, int h, int gridw);

    /** @brief
     *      ��һ���Ҷ�ͼת����ĳ����ɫ��������dst�С�
     *  @param[in] dst Ŀ��BitmapData��
     *  @param[in] nSx Ŀ��bmp�����x
     *  @param[in] nSy Ŀ��bmp�����y
     *  @param[in] pSrc  ԴͼƬ����һ���Ҷ�ͼ���Ҷ�ֵ��ʾ��ɫ����ǳ��
     *  @param[in] nSrcW  ԴͼƬ�Ŀ�
     *  @param[in] nSrcH  ԴͼƬ�ĸ�
     *  @param[in] nSrcColor  ��ɫ��
     *  @return void
    */
    void copy8BitBmp( BitmapData& dst, int nSx, int nSy, unsigned char* pSrc, int nSrcW, int nSrcH, int nSrcColor );

    void copy32BitBmp(BitmapData& dst, int nSx, int nSy,int nDstW, unsigned char* pSrc, int nSrcW, int nSrcH, int srcPitch);

    std::pair<unsigned char*, unsigned long> convertBitmapToJpeg(const char* p_pData, int nWidth, int nHeight, int nBpp);

    std::pair<unsigned char*, unsigned long> convertBitmapToPng(const char* p_pData, int nWidth, int nHeight, int nBitDepth);

}
//------------------------------------------------------------------------------


#endif //__JCImageRW_H__

//-----------------------------END FILE--------------------------------
