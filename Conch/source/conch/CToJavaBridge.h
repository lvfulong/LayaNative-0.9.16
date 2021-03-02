/**
@file			CToJavaBridge.h
@brief			
@author			James
@version		1.0
@date			2016_5_19
*/
#ifndef __CToJavaBridge_H__
#define __CToJavaBridge_H__

#include <jni.h>
#include <string>
#include <vector>
#include <map>

/*
	根据名字来调用一个java函数。
	不见得效率高，但是会方便一些。
	TODO
		根据注册的时候的函数的描述来自动把参数从C转换成java类型的
		根据类型调用响应的函数
*/
enum JniParamType
{
	PT_bool,	//Z
	PT_byte,	//B
	PT_char,	//C
	PT_short,	//S
	PT_int,		//I
	PT_long,	//J
	PT_float,	//F
	PT_double,	//D
	PT_String,	//string
	PT_array,
	PT_object,
};
enum JniReturnType
{
	JRT_Int,
	JRT_string,
	JRT_object,
	JRT_void,
	JRT_Float,
};

bool paserParamDesc( const char* p_sDesc,std::vector<int>& p_nParamNum,int& p_nReturnType );

class CToJavaBridge
{
public:
	struct JavaMethod
    {
		jclass cls;
		jmethodID method;
		std::vector<int> paramInfo;
		int retureValueType;
		JavaMethod(){}
		JavaMethod(jclass p_cls, jmethodID p_method)
        {
			cls = p_cls;
			method = p_method;
		}
	};
    struct ThreadJNIData
    {
        JavaVM*		pJVM;
        JNIEnv*		pThreadJNI;
        ThreadJNIData()
        {
            pThreadJNI = 0;
            pJVM = 0;
        }
    };
    struct JavaRet 
    {
        enum RTType 
        {
            RT_Object = 0,
            RT_String,
            RT_Int,
            RT_Float,
            RT_Unk = 0xffffffff
        };
        JNIEnv* pJNI;
        RTType	retType;
        jobject	objRet;
        jstring strRet;
        int		intRet;
        float   floatRet;
        JavaRet()
        {
            pJNI = NULL;
            retType = RT_Unk;
            objRet = NULL;
            strRet = NULL;
            intRet = 0;
            floatRet = 0;
        }
        ~JavaRet() 
        {
            if (pJNI&& retType == RT_Object) 
            {
                pJNI->DeleteLocalRef(objRet);
            }
            if (pJNI&& retType == RT_String) 
            {
                pJNI->DeleteLocalRef(strRet);
            }
        }
    };
public:
    static std::string JavaClass;
    static CToJavaBridge* GetInstance();

    static void DelInstance();

	CToJavaBridge();

	~CToJavaBridge();

    /** @brief 内部不会保存这些字符串的指针
     *  @return 
    */
	jmethodID addStaticMethod( JNIEnv* p_Env, const char* p_sCls);
		
	ThreadJNIData* checkThreadJNI();
		

    /** @brief 返回的指针是重新分配内存的。
     *  @return 
    */
	int* getJavaIntArray(JNIEnv* p_pJNI, jobject p_obj);

	//p_nLen 是byte的个数
	int* getJavaIntArray(JNIEnv* p_pJNI, jobject p_obj, char* p_pBuff, int& p_nByteLen);
		
	std::string getJavaString( JNIEnv* p_pJNI, jstring p_sJString );
		
	//bool callStaticJavaMethod( int p_nJavaMethodName, std::vector<intptr_t>& p_Params, JavaRet& ret );
    bool callMethod(int objid, bool isSyn, const char* className, const char* methodName, const char* param, JavaRet& ret);
    bool callMethod(const char* className, const char* methodName, int n, JavaRet& ret);
    bool callMethod(const char* className, const char* methodName, float f, JavaRet& ret);
	bool callMethod(const char* className, const char* methodName, int x, int y, int w, int h, JavaRet& ret);
	bool callMethod(const char* className, const char* methodName, const char* s,int x, int y, int w, int h,int bKeyCloseView, JavaRet& ret);
	bool callMethod(const char* className, const char* methodName, float x,float y,JavaRet& ret);
    bool callMethod(const char* className, const char* methodName, bool b, JavaRet& ret);
    bool callMethod(const char* className, const char* methodName, JavaRet& ret,JavaRet::RTType p_eReturnType=JavaRet::RT_String);
    bool callMethod(const char* className, const char* methodName, int x, int y, JavaRet& ret);
    bool callMethod(const char* className, const char* methodName, const char* s1,const char* s2,JavaRet& ret);
    bool callMethod(const char* className, const char* methodName, const char* s1,const char* s2,const char* s3,JavaRet& ret);
    bool callMethod(const char* className, const char* methodName, const char*s, int n, JavaRet& ret);
	bool callMethod(const char* className, const char* methodName, const char*s, int n,int n1, JavaRet& ret);
    bool callMethod(const char* className, const char* methodName, int n1, int n2,const char*s1, const char*s2, const char*s3, JavaRet& ret);
    bool callMethod(const char* className, const char* methodName,int n1,int n2,int n3,const char*s1,const char*s2,const char*s3,JavaRet& ret);
    bool callMethod(const char* className, const char* methodName, const char*s, JavaRet& ret,JavaRet::RTType p_eReturnType = JavaRet::RT_String);
    void replace_all_distinct(std::string&   str, const   std::string&   old_value, const   std::string&   new_value);

public:
    //typedef std::map<int, JavaMethod>   JavaMethodMap;
   // JavaMethodMap		                m_StaticMethods;
    JavaVM*				                m_pJVM;
    JNIEnv*				                m_pDefJNIEnv;
    pthread_key_t 		                m_kThreadkeyJNI;	//TEST多个线程一起使用有没有问题
    jmethodID					        m_jMethodID;//JS调用c++再调用java
    jmethodID                           m_jNativeMethodStrId;//C++直接调用java
    jclass					            m_jClass;
};
//------------------------------------------------------------------------------


#endif //__CToJavaBridge_H__

//-----------------------------END FILE--------------------------------
