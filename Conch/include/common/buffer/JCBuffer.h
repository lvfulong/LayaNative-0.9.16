﻿/**
@file			JCBuffer.h
@brief			
@author			James
@version		1.0
@date			2016_5_11
*/

#ifndef __JCBuffer_H__
#define __JCBuffer_H__

#include <memory>
namespace laya
{
	/** 
	 * @brief 内存快速的buffer操作
	*/
	class  JCMemClass
	{
	public:

        /** @brief 构造函数
         *  @param[in] 初始化size的大小
         *  @param[in] 当大小不够的时候，每次增加的大小
        */
        JCMemClass( unsigned int nSize,unsigned int nAdjustSize );

        /** @brief 析构函数
        */
		~JCMemClass();

        /** @brief 设置每次扩充buffer的size
         *  @param[in] 扩充的size
        */
		void setBufferExpandStep(int nAdjustSize);

        /** @brief 是否需要对其
         *  @param[in] 是否需要对其
        */
        void setAlign(bool bAlign);

        /** @brief 获得buffer
         *  @return 返回buffer
        */
		char* getBuffer()
        {
            return m_pBuffer;
        }

        /** @brief 获得读取到当前的位置
         *  @return 返回buffer的指针
        */
		char* getReadPtr()
        {
            if( m_nReadPos>= m_nDataSize ) 
                return 0; 
            else 
                return m_pBuffer+m_nReadPos;
        }

        /** @brief 设置buffer的大小。只分配空间，但是getDataSize依然是原来的值
         *  @param[in] 要设置的大小
        */
		void setBufferSize(int nBufferSize);


        /** @brief 读取buffer
        *  @param[in] 需要传入读取的长度
        *  @return 返回buffer指针
        */
        char* readBuffer(int nLength)
        {
            nLength = alignValue(nLength);
            char* pRet = m_pBuffer + m_nReadPos;
            m_nReadPos += nLength;
            return pRet;
        }

        /** @brief 从当前读指针返回一个对象
        */
		template<class T> 
		void popv( T& v)
        {
			v=*(T*)(m_pBuffer+m_nReadPos);
			m_nReadPos+=alignValue(sizeof(T));
		}

        /** @brief 从当前读指针返回一个对象指针。并且前进。（4对齐）
        */
		template<class T>
		T* popp()
        {
			T* ret = (T*)(m_pBuffer+m_nReadPos);
			m_nReadPos+=alignValue(sizeof(T));
			return ret;
		}

        /** @brief 获得数据长度
         *  @return 返回数据长度大小
        */
		unsigned int getDataSize()
        {
            return m_nDataSize;
        }

        /** @brief append buffer函数
         *  @param[in] buffer指针
         *  @param[in] buffer大小
        */
		void append( const void* pBuffer,int nSize );


        /** @brief 假装添加数据，其实就是修改数据大小。如果调用者希望直接针对指针修改数据，通过这个来通知buffer 数据已经改变了。
         *  @param[in] sz 大小，字节。
         *  @return 扩展前的位置。
        */
        void* appendEmpty(int nSize);

        /** @brief append 数据，比如一个结构体
         *  @param[in] 数据
        */
		template<class T>
		void append(T& v)
        {
			int size = alignValue(sizeof(T));
			expand(m_nDataSize+size-m_nBufSize);
			*(T*)(m_pBuffer+m_nDataSize)=v;
			m_nDataSize+=size;
			m_bChanged=true;
		}

        /** @brief append一个int值
         *  @param[in] int值
        */
		void append(int v)
        {
			expand(m_nDataSize+sizeof(int)-m_nBufSize);
			*(int*)(m_pBuffer+m_nDataSize)=v;
			m_nDataSize+=sizeof(int);
			m_bChanged=true;
		}

        /** @brief append一个long值
        *  @param[in] long值
        */
        void append(long v)
        {
            expand(m_nDataSize + sizeof(long) - m_nBufSize);
            *(long*)(m_pBuffer + m_nDataSize) = v;
            m_nDataSize += sizeof(long);
            m_bChanged = true;
        }

        /** @brief 清空数据，但是不删除buffer
         *  @return 
        */
		inline void clearData() 
        {
            m_nDataSize=m_nReadPos=0; 
        }

        /** @brief 是否为Emplty,如果没有数据，就认为是null
         *  @return  
        */
		inline bool isEmpty()
        {
            return m_nDataSize==0;
        }

        /** @brief 设置内容是否改变
         *  @param[in] 是否改变
        */
		inline void setChangedFlag(bool bChanged)
        {
            m_bChanged = bChanged;
        };

        /** @brief 获得内容是否改变了 
         *  @return 
        */
		inline bool getChangedFlag()
        { 
            return m_bChanged; 
        }

        /** @brief 设置读取read的pos
        *  @param[in] pos
        */
        inline void setReadPos(int nPos)
        {
            m_nReadPos = nPos;
        }

        /** @brief 设置写入的数据的位置，也就是相当于data的size
        *  @param[in] 输入size
        */
        inline void setWritePos(int nSize)
        {
            m_nDataSize = nSize;
        }

    private:

        /** @brief 返回bufferSize
        *  @return 返回buffer数据大小
        */
        unsigned int getBuffSize()
        {
            return m_nBufSize;
        }

        /** @brief 扩充buffer
        *  @param[in] 需要扩充的size
        */
        void expand(int nExpandSize);

        /** @brief 对其数据函数
        *  @param[in] 输入的值
        *  @return 获得对其后的值
        */
        inline int alignValue(int nValue)
        {
            return m_bAlign ? (nValue + 3) & 0xfffffffc : nValue;
        }

	private:
		char*			    m_pBuffer;          ///<buffer
		unsigned int 	    m_nBufSize;         ///<buffer的总大小，内存开辟的空间大小
		unsigned int 	    m_nDataSize;        ///<里面数据的总大小，buffer中的真实数据大小
		unsigned int	    m_nReadPos;         ///<当前读取的位置
		unsigned int	    m_nAdjustSize:16;   ///<每次扩容的时候，增加的大小
		bool			    m_bChanged:1;	    ///<内容是否发生了改变
		bool			    m_bAlign:1;		    ///<所有的操作都按照4对齐
	};

	class JCBuffer
    {
	public:
		enum encode
        {
			raw=0,		//原样。一般不是文本的。
			gbk=1,		//是文本的。是gbk编码的
			utf8=2,		//是文本的。是utf8编码的。
		};

        JCBuffer();

        JCBuffer(int p_nSize );

		//p_bManage = true 则buffer来释放指针
        JCBuffer( void* p_ptr, int p_nLength, bool p_bCpy, bool p_bManage);

		//把utf8的转换成某种编码的buffer
        JCBuffer( char* p_pStr, encode p_end );

		~JCBuffer();

		bool create( int p_nLen );

		void free();

		bool toString(encode p_enc);

	public:
		char*			m_pPtr;
		char*			m_pCurPtr;
		int				m_nLen:30;
		int				m_bAlign4:1;		//每次操作都按照4字节对齐？
		int				m_bNeedDel:1;
	};
	class JCSharedBuffer
    {
	public:
        JCSharedBuffer(){m_nLen=0;}
        JCSharedBuffer(JCSharedBuffer& buf)
        {
			m_pBuffer = buf.m_pBuffer;
			m_nLen = buf.m_nLen;
		}
		inline void create(int p_nLen )
        {
			m_pBuffer = std::shared_ptr<char>(new char[p_nLen],std::default_delete<char[]>());
			m_nLen = p_nLen;
		}
		std::shared_ptr<char>		m_pBuffer;//注意，必须是array类型的，构造的时候必须有第二个参数，std::default_delete<char[]>()
		int								m_nLen;
	};
};
//------------------------------------------------------------------------------


#endif //__JCBuffer_H__

//-----------------------------END FILE--------------------------------