#ifndef V8_DEBUG_AGENT_H_
#define V8_DEBUG_AGENT_H_
#ifdef JS_V8
#include <v8.h>
#include <v8-debug.h>
#include <misc/boostSemaphore.h>
#include "V8HeapProfiler.h"
#include "V8CpuProfile.h"
#include "V8Console.h"
#include <thread>
#include <mutex>
#define ASSERT(condition)      ((void) 0)

namespace laya {
	class V8Socket;
	class DebuggerAgentSession;
	class JSThread;
	// Debugger agent which starts a socket listener on the debugger port and
	// handles connection from a remote debugger.
	class DebuggerAgent {
	public:
		enum Proto_MsgType {
			Request=0,
			Response=1,
			Event=2
		};
		enum Proto_Event {
			Console_messageAdded = 100,
			Perf_BaseInfo = 200,
		};

		DebuggerAgent(const char* name, int port);//���ֻ�ǲ���Socket����Ϣ�շ����֣�ֱ��New DebuggerAgent(const char* name, int port)
		//���������������Ҫ��ChormeDebugAPI��ʼ����
		~DebuggerAgent();

		void Shutdown();
		void WaitUntilListening();
		v8::Isolate* isolate() { return isolate_; }
		void enableJSDebug();
		/**
		* ����js�߳��ˣ�����һ���µ�jsid���Ժ�js��ص���Ϣ����ʹ�����jsid��
		* �ṩһ��������ϣ��js�߳���ѭ���е�������
		*/
		typedef void(*JSDebugger_OnJSUpdate)();
		typedef void(*JSDebugger_DebuggerMessage)(const v8::Debug::Message& message);
		void onJSStart(JSThread* pJSThread);
		/**
		* js�߳̽����ˣ���ǰ��jsid��ʧЧ�ˣ��Ժ���յ��Ĵ�id����Ϣ�����ԡ�
		*/
		void onJSExit();
		void setMessageHandler();
		void DebuggerMessage(const v8::Debug::Message& message);
		void Run();
		void onjsupdate();
		//������������log
		void sendToDbgConsole(char* pMsg, const char* src, int line, int colum, const char* type);
		//����һ���������������seq����Ҫ���á�
		void sendToDbg(char* pJSON);
		void sendToDbg_command(int seq, const char* cmd, const char* arguments);
		void sendToDbg_event(int seq, const char* event, const char* body);
		void sendToDbg_resinfo(const char* resname, const char* event);
		void sendPerfInfo(int* pData, int nDataLen);
	private:
		void CreateSession(V8Socket* socket);

		void CloseSession();
		void OnSessionClosed(DebuggerAgentSession* session);

		v8::Isolate* isolate_;
		std::string name_;
		///*v8::base::SmartArrayPointer<const char>*/const char* name_;  // Name of the embedding application.
		int port_;  // Port to use for the agent.
		V8Socket* server_;  // Server socket for listen/accept.
		bool terminate_;  // Termination flag.
		std::mutex session_access_;  // Mutex guarging access to session_.
		semaphore terminate_now_;  // Semaphore to signal termination.
		semaphore listening_;
		friend class DebuggerAgentSession;
		bool	    v8debugEnabled_;	//�Ѿ�����v8������
		JSThread*	pJSThread_;
	public:
		DebuggerAgentSession*       session_;  // Current active session if any.
		std::thread*		        m_pAgentThread;
		static DebuggerAgent*		s_Instance;
		std::string				    m_strBreakEvt;	//�����break�¼�����ʹû��sessionҲҪ���������һ����session�������ˣ��ͷ�������
		static int g_nSeqNum;
		//DISALLOW_COPY_AND_ASSIGN(DebuggerAgent);
	public:
		static const char* const kContentLength;

		static char* ReceiveMessage(const V8Socket* conn);
		static bool SendConnectMessage(const V8Socket* conn, const char* embedding_host);
		//static bool SendMessage(const V8Socket* conn, const uint16_t* message, int msgLen);
		//�@���l�͵���utf8��
		static bool SendDbgMessage(const V8Socket* conn, const char* message, int msgLen);
		static bool SendDbgMessage(const V8Socket* conn, const v8::Handle<v8::String> message);
		static int ReceiveAll(const V8Socket* conn, char* data, int len);
	};


	// Debugger agent session. The session receives requests from the remote
	// debugger and sends debugger events/responses to the remote debugger.
	class DebuggerAgentSession {//: public v8::base::Thread {
	public:
		struct BinMsgHead {
			enum DataType {
				JSON,
				BIN
			};
			unsigned int flag:32;//0xfedc0123
			unsigned int version : 8;
			unsigned int session : 8;		//no use
			unsigned int type : 1;			//0 json, 1. bin
			unsigned int padd : 15;
			unsigned int payloadlen : 32;
		};
	public:
		DebuggerAgentSession(DebuggerAgent* agent, V8Socket* client);
		~DebuggerAgentSession();
		///void DebuggerMessage(v8::internal::Vector<uint16_t> message);
		void Shutdown();

		void DebuggerMessage(const char* message, int msgLen);
		void Run();
		void onjsupdate();
		void onjsexit();
		void sendToDbgConsole(char* pMsg, const char* src, int line, int colum, const char* type);
		void sendToDbg(char* pJSON);
		void sendToDbg_command(int seq, const char* cmd, const char* arguments);
		void sendToDbg_event(int seq, const char* event, const char* body);
		void sendToDbg_resinfo(const char* resname, const char* event);
		void sendPerfInfo(int* pData, int nDataLen);
		bool SendDbgMessage(const char* data, int datalen, BinMsgHead::DataType type);
	private:
		DebuggerAgent* agent_;
		V8Socket* client_;
		V8HeapProfiler*		heap_profile_;
		V8CpuProfile*		cpu_profile_;
		V8Console*			console;
	public:
		std::thread*		m_pSessionThread;
		//DISALLOW_COPY_AND_ASSIGN(DebuggerAgentSession);
	};
}
#endif
#endif  // V8_DEBUG_AGENT_H_
