/**
@file			JCFileSource.h
@brief			
@author			James
@version		1.0
@date			2016_5_11
*/

#ifndef __JCFileSource_H__
#define __JCFileSource_H__

namespace laya
{
	class JCFileSource
	{
	public:
        virtual ~JCFileSource() {};
        /** @brief ֱ���жϰ����Ƿ��������ļ�
         *  @param[in] �ļ�����
         *  @return ����true����false
        */
		virtual bool isFileExist( const char* sFileName )=0;


        /** @brief ���fileSize
         *  @param[in] �ļ�����
         *  @return ����size
        */
        virtual unsigned int getFileSize(const char* sFileName)=0;

        /** @brief ����һ�οռ䣬����ָ�룬Ϊ�˷����ڴ�ʹ�õ�
         *  @param[in] 
         *  @param[in]
         *  @return 
        */
		typedef unsigned char* (*ALLOCMEM)(int nBufferSize,void* pBuffer );

        /** @brief ��ȡ�ļ�����
         *  @param[in] �ļ�����
         *  @param[in] alloc�ĺ���ָ�룬�������Լ������������
         *  @param[in] ��õ�buffer
         *  @param[out] ���ص�buffersize
         *  @return �Ƿ�ɹ�
        */
		virtual bool loadFileContent( const char* sFileName, ALLOCMEM pAllocFunc, void* pUserData, int& nBuffSize ) = 0;


        /** @brief ��ȡ�ļ�����
        *  @param[in] �ļ�����
        *  @param[in] ��õ�buffer
        *  @param[out] ���ص�buffersize
        *  @return �Ƿ�ɹ�
        */
        virtual bool loadFileContent(const char* sFileName, char*& pBuffer, int& nBuffSize) = 0;
	};
}
//------------------------------------------------------------------------------


#endif //__JCFileSource_H__

//-----------------------------END FILE--------------------------------
