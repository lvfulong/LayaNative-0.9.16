#ifdef JS_JSC
#include "JSCArrayBuffer.h"
#include <JavaScriptCore/JSTypedArray.h>
#include "JSCProxyTLS.h"
#include "util/Log.h"
#include <vector>
#include "JSCProxyType.h"
#include "JSConchConfig.h"
#include "EJConvertTypedArray.h"
namespace laya {
    
	//TODO 这里没有考虑多线程的问题，如果js要支持多线程的话，需要修改。
	struct JsStrBuff {
		char* _buff;
		int _len;
		static std::vector<JsStrBuff> jsstrbuffs;
		static int curIdx;
		JsStrBuff() {
			_buff = NULL;
			_len = 0;
		}

		static JsStrBuff& getAData() {
			if (JsStrBuff::curIdx >= JsStrBuff::jsstrbuffs.size()) {
				JsStrBuff::jsstrbuffs.push_back(JsStrBuff());
				JsStrBuff::curIdx++;
				return JsStrBuff::jsstrbuffs.back();
			}
			else {
				return JsStrBuff::jsstrbuffs[JsStrBuff::curIdx++];
			}
		}
	};
	std::vector<JsStrBuff> JsStrBuff::jsstrbuffs;
	int JsStrBuff::curIdx = 0;

	void resetJsStrBuf() {
		JsStrBuff::curIdx = 0;
	}
 
    char* JsCharToC(JSStringRef str) {
        int len = 0;
        if (str == nullptr)
            return nullptr;//return "";
        len = JSStringGetMaximumUTF8CStringSize(str);
        if (len <= 0)
            return nullptr;//return "";
        JsStrBuff& curdata = JsStrBuff::getAData();
        char*& tocharBuf = curdata._buff;
        int& tocharBufLen = curdata._len;
        
        //tocharBuf= new char[len + 1];
        if (len > tocharBufLen) {
            tocharBufLen = len;
            if (tocharBuf != NULL)
                delete[] tocharBuf;
            tocharBuf = new char[len+1];
        }
        else {
            //如果占用空间太大，也要重新分配
            if (tocharBufLen > 1024 ) {
                tocharBufLen = len;
                if (tocharBuf != NULL)
                    delete[] tocharBuf;
                tocharBuf = new char[len+1];
            }
        }
        JSStringGetUTF8CString(str,tocharBuf,len);
        return tocharBuf;
    }
    char* JsCharToC(JSValueRef p_vl) {
        JSContextRef ctx = __TlsData::GetInstance()->GetCurContext();
        JSStringRef str = JSValueToStringCopy(ctx, p_vl, nullptr);
        if (str == nullptr)
            return nullptr;//return "";
        char* ret = JsCharToC(str);
        JSStringRelease(str);
        return ret;
    }
}

namespace laya{
    
    static void deallocator(void* bytes, void* deallocatorContext)
    {
        if (bytes){
            delete (char*)bytes;
        }
    }
    
	JSValueRef  createJSAB(char* pData, int len)
	{
        JSContextRef ctx = __TlsData::GetInstance()->GetCurContext();
        if (isSupportTypedArrayAPI())
        {
            char* pBuffer = new char[len];
            memcpy(pBuffer, pData, len);
            return JSObjectMakeArrayBufferWithBytesNoCopy(ctx, pBuffer, len, deallocator, pBuffer, NULL);
        }
        else
        {
            JSValueRef ab = JSObjectMakeTypedArrayEJ(ctx, _kJSTypedArrayTypeArrayBuffer, len);
            JSObjectRef arrayObj = JSValueToObject(ctx,ab,NULL);
            JSObjectSetTypedArrayData(ctx, arrayObj, (const uint8_t *)pData, len);
            return ab;
        }
	}
    JSValueRef  createJSABAligned(char* pData, int len)
    {
        int asz = (len+3) & 0xfffffffc;
        JSContextRef ctx = __TlsData::GetInstance()->GetCurContext();
        char* pBuffer = new char[asz];
        memcpy(pBuffer, pData, len);
        for( int i = len; i < asz; i++ )
        {
            pBuffer[i] = 0;
        }
        if (isSupportTypedArrayAPI())
        {
            return JSObjectMakeArrayBufferWithBytesNoCopy(ctx, pBuffer, asz, deallocator, pBuffer, NULL);
        }
        else
        {
            JSValueRef ab = JSObjectMakeTypedArrayEJ(ctx, _kJSTypedArrayTypeArrayBuffer, asz);
            JSObjectSetTypedArrayData(ctx, (JSObjectRef)ab, (const uint8_t *)pBuffer, asz);
            delete[] pBuffer;//optmize
            return ab;
        }
    }
	bool extractJSAB(JSValueRef  ab, JsArrayBufferData& data)
	{
        JSContextRef ctx = __TlsData::GetInstance()->GetCurContext();
        JSObjectRef arrayObj = JSValueToObject(ctx,ab,NULL);
        data.data = NULL;
        data.len = 0;
        if (isSupportTypedArrayAPI())
        {
            JSTypedArrayType arrayType = JSValueGetTypedArrayType(ctx, ab, NULL);
            if (arrayType == kJSTypedArrayTypeNone){
                return false;
            }
            else if (arrayType == kJSTypedArrayTypeArrayBuffer){
                data.data = (char*)JSObjectGetArrayBufferBytesPtr(ctx, arrayObj, NULL);
                data.len = (int)JSObjectGetArrayBufferByteLength(ctx, arrayObj, NULL);
                return true;
            }
            else {
                data.data = (char*)JSObjectGetTypedArrayBytesPtr(ctx, arrayObj, NULL) + JSObjectGetTypedArrayByteOffset(ctx, arrayObj, NULL);
                data.len = (int)JSObjectGetTypedArrayByteLength(ctx, arrayObj, NULL);
                return true;
            }
        }
        else
        {
            return JSObjectGetTypedArrayData(ctx, arrayObj, data.data, data.len);
        }
	}
}
#endif
