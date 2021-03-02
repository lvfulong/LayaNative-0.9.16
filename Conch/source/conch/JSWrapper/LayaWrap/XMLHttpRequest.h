
#ifndef _LAYA_XMLHTTPREQUEST_H__
#define _LAYA_XMLHTTPREQUEST_H__

#include <string>
#include <map>
#include "buffer/JCBuffer.h"
#include "../JSInterface/JSInterface.h"

namespace laya{

	class Curl;
	class JCDownloadMgr;
    class IConchThreadCmdMgr;
	class XMLHttpRequest:public JsObjBase , public JSObjNode {
	public:
		static JsObjClassInfo JSCLSINFO;
		typedef std::string String;

		enum ErrorCode{
			SUCCESS,
			SYNTAX_ERR,
			INVALID_STATE_ERR,
		};
	 // These exact numeric values are important because JS expects them.
		enum State {
			UNSENT = 0,
			OPENED = 1,
			HEADERS_RECEIVED = 2,
			LOADING = 3,
			DONE = 4
		};
    
		enum ResponseTypeCode {
			ResponseTypeDefault,
			ResponseTypeText,
			ResponseTypeJSON,
			ResponseTypeDocument,

			// Binary format
			ResponseTypeBlob,
			ResponseTypeArrayBuffer
		};
		static const ResponseTypeCode FirstBinaryResponseType = ResponseTypeBlob;

	public:
		XMLHttpRequest();
        ~XMLHttpRequest();

		State readyState() const;

		//����
		void open(const char* method, const char* p_url, bool async);

		void changeState(State newState);

		void postString(const char* p_pszUrl, const char* p_pszString, JSValueAsParam p_funOnOK, JSValueAsParam p_funOnErr);

		void abort();

		void setRequestHeader(const char* name, const char* value );

		void setRequestHeaderInternal(const std::string& name, const std::string& value);

		std::string getRequestHeader(const std::string& name) const;

		void callReadyStateChangeListener();

		int getStatus(){
			return m_state;
		}

		int getReadyState(){
			return m_state;
		}

		char* getStatusText(){
			return "not implement";
		}

		char* getResponseURL(){
			return (char*)m_url.c_str();
		}

		int get_responseTypeCode() const { return (int)m_responseTypeCode; }
        void set_responseTypeCode(int c) { m_responseTypeCode = (ResponseTypeCode)c; }

        int get_timeout();
        void set_timeout(int tm);

		bool getUpload(){
			return false;
		}

		bool isBin(){
			return (m_responseTypeCode==ResponseTypeArrayBuffer || m_responseTypeCode==ResponseTypeBlob);
		}

		void set_onreadystatechange(JSValueAsParam pObj);
		void _changeState(int p_nState);

		inline void postData(JCDownloadMgr* p_pDownloadMgr, const char* p_pszURL, char* p_pData, int p_nLen );
		//��Ϊ������v8ԭ���ķ�����js����ת��JsObjHandle2���Էֳ���������
        void setPostCB(JSValueAsParam p_onOK, JSValueAsParam p_onError);

		void JsPostData( const char* p_sUrl, JSValueAsParam arg1 );
        //�ص�Ҳ���� setPostCB���õĺ���
        void getData(const char* p_sUrl);
		
        static void exportJS();

	private:
		State m_state;
		bool m_uploadComplete;
		bool m_error;
		ErrorCode	m_ErrorCode;
		std::string m_method;
		std::string m_url;
		bool m_async;
		typedef std::map<std::string,std::string> HTTPHeaderMap;
		HTTPHeaderMap	m_requestHeaders;
		ResponseTypeCode m_responseTypeCode;
		std::function<void(JCBuffer&,const std::string&, const std::string&,
            int, int, const std::string&)>		m_funcPostComplete;
        int m_nTimeout = 0;
	public:
		enum{onstatchgid,thisid,oncompleteid,onerrid};
		JsObjHandle		m_funcOnStateChg;
		JsObjHandle		m_This;		// this �� JS ����
		JsObjHandle		m_jsfunPostComplete;
		JsObjHandle		m_jsfunPostError;
		std::shared_ptr<int>	m_CallbackRef;
        IConchThreadCmdMgr*                 m_pCmdPoster;
    };
}

#endif
